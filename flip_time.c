// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Hadi

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view_port.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define WELCOME_MS 1600U

typedef enum {
    ScreenWelcome,
    ScreenHome,
    ScreenStopwatch,
    ScreenTimers,
    ScreenTimerEdit,
    ScreenTimerDone,
} Screen;

typedef struct {
    uint32_t duration_sec;
    uint32_t remaining_ticks;
    uint32_t end_tick;
    bool running;
} FlipTimer;

typedef struct {
    FuriMessageQueue* queue;
    ViewPort* viewport;
    NotificationApp* notification;
    Screen screen;
    bool running;
    uint8_t selected;
    uint8_t field;
    uint32_t welcome_tick;
    uint32_t stopwatch_elapsed_ticks;
    uint32_t stopwatch_started_tick;
    bool stopwatch_running;
    FlipTimer timer;
} FlipTimeApp;

static const NotificationSequence sequence_timer_done = {
    &message_force_speaker_volume_setting_1f,
    &message_force_vibro_setting_on,
    &message_display_backlight_on,
    &message_vibro_on,
    &message_note_c7,
    &message_delay_250,
    &message_note_e7,
    &message_delay_250,
    &message_note_g7,
    &message_delay_250,
    &message_sound_off,
    &message_vibro_off,
    NULL,
};

static uint32_t ticks_to_ms(uint32_t ticks) {
    return (uint32_t)(((uint64_t)ticks * 1000U) / furi_kernel_get_tick_frequency());
}

static void format_time(uint32_t total_sec, char* text, size_t size) {
    uint32_t hours = total_sec / 3600U;
    uint32_t minutes = (total_sec / 60U) % 60U;
    uint32_t seconds = total_sec % 60U;
    snprintf(text, size, "%02lu:%02lu:%02lu", hours, minutes, seconds);
}

static void draw_header(Canvas* canvas, const char* title) {
    canvas_draw_box(canvas, 0, 0, 128, 12);
    canvas_set_color(canvas, ColorWhite);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 4, 6, AlignLeft, AlignCenter, title);
    canvas_set_color(canvas, ColorBlack);
}

static void draw_row(
    Canvas* canvas,
    uint8_t y,
    bool selected,
    const char* name,
    const char* value) {
    if(selected) {
        canvas_draw_rbox(canvas, 2, y, 124, 19, 4);
        canvas_set_color(canvas, ColorWhite);
    } else {
        canvas_draw_rframe(canvas, 2, y, 124, 19, 4);
    }
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 8, y + 10, AlignLeft, AlignCenter, name);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 120, y + 10, AlignRight, AlignCenter, value);
    canvas_set_color(canvas, ColorBlack);
}

static uint32_t stopwatch_ms(const FlipTimeApp* app, uint32_t now) {
    uint32_t ticks = app->stopwatch_elapsed_ticks;
    if(app->stopwatch_running) ticks += now - app->stopwatch_started_tick;
    return ticks_to_ms(ticks);
}

static uint32_t timer_remaining(const FlipTimer* timer, uint32_t now) {
    uint32_t remaining = timer->remaining_ticks;
    if(timer->running) {
        if((int32_t)(now - timer->end_tick) >= 0) return 0;
        remaining = timer->end_tick - now;
    }
    return (remaining + furi_kernel_get_tick_frequency() - 1U) /
           furi_kernel_get_tick_frequency();
}

static void draw_welcome(Canvas* canvas) {
    canvas_draw_rbox(canvas, 9, 8, 110, 48, 6);
    canvas_set_color(canvas, ColorWhite);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 19, AlignCenter, AlignCenter, "Welcome");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 35, AlignCenter, AlignCenter, "FlipTime");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 49, AlignCenter, AlignCenter, "by Hadi");
    canvas_set_color(canvas, ColorBlack);
}

static void draw_home(Canvas* canvas, FlipTimeApp* app, uint32_t now) {
    draw_header(canvas, "Flip Time");
    char value[22];
    uint32_t sw_ms = stopwatch_ms(app, now);
    snprintf(
        value,
        sizeof(value),
        "%02lu:%02lu.%01lu",
        (sw_ms / 60000U) % 100U,
        (sw_ms / 1000U) % 60U,
        (sw_ms / 100U) % 10U);
    draw_row(canvas, 16, app->selected == 0, "Stopwatch", value);
    format_time(timer_remaining(&app->timer, now), value, sizeof(value));
    draw_row(canvas, 40, app->selected == 1, "Timer", value);
}

static void draw_stopwatch(Canvas* canvas, FlipTimeApp* app, uint32_t now) {
    draw_header(canvas, "Stopwatch");
    uint32_t ms = stopwatch_ms(app, now);
    char value[24];
    snprintf(
        value,
        sizeof(value),
        "%02lu:%02lu:%02lu.%01lu",
        ms / 3600000U,
        (ms / 60000U) % 60U,
        (ms / 1000U) % 60U,
        (ms / 100U) % 10U);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 31, AlignCenter, AlignCenter, value);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas,
        64,
        44,
        AlignCenter,
        AlignCenter,
        app->stopwatch_running ? "RUNNING" : (ms ? "PAUSED" : "READY"));
    elements_button_center(canvas, app->stopwatch_running ? "Pause" : "Start");
    elements_button_right(canvas, "Reset");
}

static void draw_timer(Canvas* canvas, FlipTimeApp* app, uint32_t now) {
    draw_header(canvas, "Timer");
    char value[20];
    format_time(timer_remaining(&app->timer, now), value, sizeof(value));
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 31, AlignCenter, AlignCenter, value);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas,
        64,
        44,
        AlignCenter,
        AlignCenter,
        app->timer.running ? "RUNNING" :
        (app->timer.remaining_ticks < furi_ms_to_ticks(app->timer.duration_sec * 1000U) ?
             "PAUSED" : "READY"));
    elements_button_left(canvas, "Reset");
    elements_button_center(canvas, app->timer.running ? "Pause" : "Start");
    elements_button_right(canvas, "Set");
}

static void draw_timer_edit(Canvas* canvas, FlipTimeApp* app) {
    FlipTimer* timer = &app->timer;
    draw_header(canvas, "Set Timer");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 19, AlignCenter, AlignCenter, "L/R field   U/D value");
    uint32_t values[3] = {
        timer->duration_sec / 3600U,
        (timer->duration_sec / 60U) % 60U,
        timer->duration_sec % 60U};
    const char* names[3] = {"HOUR", "MIN", "SEC"};
    for(uint8_t i = 0; i < 3; i++) {
        uint8_t x = 8 + i * 40;
        if(app->field == i) {
            canvas_draw_rbox(canvas, x, 25, 33, 27, 3);
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_draw_rframe(canvas, x, 25, 33, 27, 3);
        }
        char value[5];
        snprintf(value, sizeof(value), "%02lu", values[i]);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, x + 16, 33, AlignCenter, AlignCenter, names[i]);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, x + 16, 45, AlignCenter, AlignCenter, value);
        canvas_set_color(canvas, ColorBlack);
    }
    elements_button_center(canvas, "Save");
}

static void draw_timer_done(Canvas* canvas, FlipTimeApp* app) {
    UNUSED(app);
    canvas_draw_rbox(canvas, 6, 7, 116, 49, 6);
    canvas_set_color(canvas, ColorWhite);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 19, AlignCenter, AlignCenter, "TIME IS UP!");
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 35, AlignCenter, AlignCenter, "00:00:00");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 48, AlignCenter, AlignCenter, "Press OK to dismiss");
    canvas_set_color(canvas, ColorBlack);
}

static void draw_callback(Canvas* canvas, void* context) {
    FlipTimeApp* app = context;
    uint32_t now = furi_get_tick();
    canvas_clear(canvas);
    switch(app->screen) {
    case ScreenWelcome: draw_welcome(canvas); break;
    case ScreenHome: draw_home(canvas, app, now); break;
    case ScreenStopwatch: draw_stopwatch(canvas, app, now); break;
    case ScreenTimers: draw_timer(canvas, app, now); break;
    case ScreenTimerEdit: draw_timer_edit(canvas, app); break;
    case ScreenTimerDone: draw_timer_done(canvas, app); break;
    }
}

static void input_callback(InputEvent* event, void* context) {
    FlipTimeApp* app = context;
    furi_message_queue_put(app->queue, event, 0);
}

static void change_timer_field(FlipTimeApp* app, int8_t delta) {
    FlipTimer* timer = &app->timer;
    int32_t h = timer->duration_sec / 3600U;
    int32_t m = (timer->duration_sec / 60U) % 60U;
    int32_t s = timer->duration_sec % 60U;
    if(app->field == 0) h = (h + 24 + delta) % 24;
    else if(app->field == 1) m = (m + 60 + delta) % 60;
    else s = (s + 60 + delta) % 60;
    timer->duration_sec = h * 3600U + m * 60U + s;
    if(timer->duration_sec == 0) timer->duration_sec = 1;
    timer->remaining_ticks = furi_ms_to_ticks(timer->duration_sec * 1000U);
}

static void handle_input(FlipTimeApp* app, const InputEvent* event) {
    if(event->type != InputTypeShort && event->type != InputTypeRepeat) return;

    if(app->screen == ScreenWelcome) {
        app->screen = ScreenHome;
        return;
    }
    if(app->screen == ScreenTimerDone) {
        if(event->key == InputKeyOk || event->key == InputKeyBack) {
            notification_message_block(app->notification, &sequence_empty);
            app->screen = ScreenTimers;
            app->selected = 1;
        }
        return;
    }
    if(app->screen == ScreenHome) {
        if(event->key == InputKeyBack) app->running = false;
        else if(event->key == InputKeyUp || event->key == InputKeyDown)
            app->selected = 1U - app->selected;
        else if(event->key == InputKeyOk) {
            app->screen = app->selected == 0 ? ScreenStopwatch : ScreenTimers;
            app->selected = 0;
        }
    } else if(app->screen == ScreenStopwatch) {
        if(event->key == InputKeyBack) {
            app->screen = ScreenHome;
            app->selected = 0;
        } else if(event->key == InputKeyOk) {
            uint32_t now = furi_get_tick();
            if(app->stopwatch_running) {
                app->stopwatch_elapsed_ticks += now - app->stopwatch_started_tick;
                app->stopwatch_running = false;
            } else {
                app->stopwatch_started_tick = now;
                app->stopwatch_running = true;
            }
        } else if(event->key == InputKeyRight) {
            app->stopwatch_running = false;
            app->stopwatch_elapsed_ticks = 0;
        }
    } else if(app->screen == ScreenTimers) {
        if(event->key == InputKeyBack) {
            app->screen = ScreenHome;
            app->selected = 1;
        } else if(event->key == InputKeyLeft) {
            app->timer.running = false;
            app->timer.remaining_ticks =
                furi_ms_to_ticks(app->timer.duration_sec * 1000U);
        } else if(event->key == InputKeyRight && !app->timer.running) {
            app->screen = ScreenTimerEdit;
            app->field = 0;
        } else if(event->key == InputKeyOk) {
            uint32_t now = furi_get_tick();
            if(app->timer.running) {
                if((int32_t)(now - app->timer.end_tick) >= 0) {
                    app->timer.remaining_ticks = 0;
                    app->timer.running = false;
                    app->screen = ScreenTimerDone;
                    notification_message(app->notification, &sequence_timer_done);
                } else {
                    app->timer.remaining_ticks = app->timer.end_tick - now;
                    app->timer.running = false;
                }
            } else {
                if(app->timer.remaining_ticks == 0) {
                    app->timer.remaining_ticks =
                        furi_ms_to_ticks(app->timer.duration_sec * 1000U);
                }
                app->timer.end_tick = now + app->timer.remaining_ticks;
                app->timer.running = true;
            }
        }
    } else if(app->screen == ScreenTimerEdit) {
        if(event->key == InputKeyBack || event->key == InputKeyOk) {
            app->screen = ScreenTimers;
        } else if(event->key == InputKeyLeft) {
            app->field = (app->field + 2U) % 3U;
        } else if(event->key == InputKeyRight) {
            app->field = (app->field + 1U) % 3U;
        } else if(event->key == InputKeyUp) {
            change_timer_field(app, 1);
        } else if(event->key == InputKeyDown) {
            change_timer_field(app, -1);
        }
    }
}

static void check_timer(FlipTimeApp* app) {
    uint32_t now = furi_get_tick();
    if(app->timer.running && (int32_t)(now - app->timer.end_tick) >= 0) {
        app->timer.running = false;
        app->timer.remaining_ticks = 0;
        app->screen = ScreenTimerDone;
        notification_message(app->notification, &sequence_timer_done);
    }
}

int32_t flip_time_app(void* context) {
    UNUSED(context);
    FlipTimeApp* app = malloc(sizeof(FlipTimeApp));
    memset(app, 0, sizeof(*app));
    app->running = true;
    app->screen = ScreenWelcome;
    app->welcome_tick = furi_get_tick();
    app->timer.duration_sec = 300U;
    app->timer.remaining_ticks = furi_ms_to_ticks(300U * 1000U);

    app->queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->viewport = view_port_alloc();
    view_port_draw_callback_set(app->viewport, draw_callback, app);
    view_port_input_callback_set(app->viewport, input_callback, app);
    Gui* gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    gui_add_view_port(gui, app->viewport, GuiLayerFullscreen);

    while(app->running) {
        InputEvent event;
        if(furi_message_queue_get(app->queue, &event, 50) == FuriStatusOk) {
            handle_input(app, &event);
        }
        if(app->screen == ScreenWelcome &&
           furi_get_tick() - app->welcome_tick >= furi_ms_to_ticks(WELCOME_MS)) {
            app->screen = ScreenHome;
        }
        check_timer(app);
        view_port_update(app->viewport);
    }

    notification_message_block(app->notification, &sequence_empty);
    gui_remove_view_port(gui, app->viewport);
    view_port_free(app->viewport);
    furi_message_queue_free(app->queue);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    free(app);
    return 0;
}
