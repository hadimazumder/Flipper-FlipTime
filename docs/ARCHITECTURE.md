# Architecture

Flip Time is intentionally implemented as one standalone FAP with no background process.

## Runtime model

`flip_time_app()` allocates one `FlipTimeApp` structure containing:

- GUI viewport and input queue
- Current screen and selection state
- Stopwatch tick counters
- One countdown timer
- Notification service handle

The event loop waits up to 50 ms for input, checks the welcome timeout and timer deadline, then requests a viewport update. This interval provides a responsive tenths-of-a-second stopwatch without busy-waiting.

## Screens

The `Screen` enum drives a small state machine:

1. `ScreenWelcome`
2. `ScreenHome`
3. `ScreenStopwatch`
4. `ScreenTimers`
5. `ScreenTimerEdit`
6. `ScreenTimerDone`

Drawing is read-only. Input handlers perform transitions and update state.

## Stopwatch

The stopwatch stores accumulated ticks and the tick value at the latest start. Pausing adds the current interval to accumulated ticks. Rendering converts ticks to milliseconds and displays tenths.

## Timer

The timer stores:

- Configured duration in seconds
- Remaining ticks while stopped or paused
- Absolute end tick while running
- Running state

Signed subtraction is used when comparing the current tick to the deadline, preserving correct behavior when the 32-bit tick counter wraps.

## Notifications

When the deadline is reached, the timer stops, the completion screen opens, and a short sound/vibration/backlight sequence is sent through the standard notification service. No custom hardware drivers are used.

## Lifecycle

All state is in RAM. Exiting frees the viewport, message queue, and app allocation, and closes service records. Nothing remains running after exit, and no files or firmware settings are modified.
