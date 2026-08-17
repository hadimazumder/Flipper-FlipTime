# Flip Time by Hadi — Standalone Edition

A standalone Flipper Zero app with:

- Stopwatch with tenths-of-a-second display
- One countdown timer with the same simple start/pause/reset workflow
- Editable timer duration up to 23:59:59
- Timer completion sound, vibration, and on-screen alert
- Opening pop-up: “Welcome FlipTime by Hadi”
- No alarms, background service, or firmware modification

The stopwatch and timer operate only while Flip Time is open. Closing the app stops and clears them.

## Install

Upload this file:

```text
FlipTime.fap
```

Using qFlipper:

1. Connect the Flipper Zero by USB.
2. Open qFlipper and its SD card file browser.
3. Open `apps/Tools` on the SD card.
4. Upload `FlipTime.fap`.
5. Open **Apps → Tools → Flip Time** on the Flipper.

The included FAP was compiled against Momentum API 87.1. If an API mismatch appears, update Momentum or rebuild against your installed Momentum version.

## Controls

### Home
- Up/Down: select Stopwatch or Timer
- OK: open
- Back: exit

### Stopwatch
- OK: start/pause
- Right: reset
- Back: home

### Timer
- OK: start/pause
- Left: reset to the configured duration
- Right: set duration while stopped
- Back: home

### Set Timer
- Left/Right: select hours, minutes, or seconds
- Up/Down: change the selected value
- OK: save
- Back: return

Press OK or Back to dismiss the completed-timer alert.

## Build from source

Copy this folder to `applications_user/flip_time` inside Momentum Firmware and run:

```bash
./fbt build APPSRC=applications_user/flip_time
```

The resulting app is generated at:

```text
build/f7-firmware-C/.extapps/flip_time.fap
```

No custom firmware build or flash is required.
