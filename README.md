# Flip Time

**Flip Time by Hadi** is an open-source, standalone time utility for Flipper Zero running Momentum Firmware.

It provides a precise stopwatch and one straightforward countdown timer without alarms, background services, persistent files, or firmware modifications.

## Features

- Stopwatch with a tenths-of-a-second display
- Start, pause, and reset controls
- One countdown timer
- Configurable duration from 1 second to 23:59:59
- Timer start, pause, reset, and edit controls
- Sound, vibration, backlight, and on-screen completion alert
- Opening pop-up: **“Welcome FlipTime by Hadi”**
- Clean 128×64 monochrome interface
- Standalone FAP installation
- No background activity after exiting
- No SD-card data or firmware settings modified

## Requirements

- Flipper Zero
- microSD card
- Momentum Firmware compatible with the FAP's SDK/API version

The bundled `FlipTime.fap` was built against **Momentum API 87.1**. If the Flipper reports an API mismatch, update Momentum or rebuild Flip Time against the SDK matching your installed firmware.

## Install the ready-made FAP

The file to install is:

```text
FlipTime.fap
```

### qFlipper

1. Connect the Flipper Zero over USB.
2. Open qFlipper and its SD-card file browser.
3. Open `apps/Tools`.
4. Upload `FlipTime.fap`.
5. On the Flipper, open **Apps → Tools → Flip Time**.

To update, replace the old FAP with the new file.

## Controls

### Home

| Button | Action |
|---|---|
| Up/Down | Select Stopwatch or Timer |
| OK | Open selection |
| Back | Exit Flip Time |

### Stopwatch

| Button | Action |
|---|---|
| OK | Start or pause |
| Right | Reset |
| Back | Return home |

### Timer

| Button | Action |
|---|---|
| OK | Start or pause |
| Left | Reset to configured duration |
| Right | Set duration while stopped |
| Back | Return home |

### Set Timer

| Button | Action |
|---|---|
| Left/Right | Select hours, minutes, or seconds |
| Up/Down | Change selected value |
| OK | Save and return |
| Back | Return |

Press **OK** or **Back** to dismiss the completion alert.

## Build with uFBT

Install and configure uFBT for Momentum:

```bash
python3 -m pip install --upgrade ufbt
ufbt update --index-url https://up.momentum-fw.dev/firmware/directory.json --channel release
```

Clone the repository and build from its root:

```bash
git clone <repository-url>
cd flip-time
ufbt
```

The compiled FAP will be generated under `dist/`.

## Build inside Momentum Firmware

Copy the repository folder to:

```text
Momentum-Firmware/applications_user/flip_time
```

Then run from the Momentum Firmware root:

```bash
./fbt build APPSRC=applications_user/flip_time
```

The result will be generated at:

```text
build/f7-firmware-C/.extapps/flip_time.fap
```

## Project structure

```text
.
├── .github/                 GitHub Actions and contribution templates
├── docs/                    Architecture and release documentation
├── application.fam         Flipper application manifest
├── flip_time.c             Complete application source
├── flip_time_10px.png      Application icon
├── FlipTime.fap            Ready-made Momentum build
├── CHANGELOG.md             Version history
├── CODE_OF_CONDUCT.md       Community standards
├── CONTRIBUTING.md          Contribution guide
├── LICENSE                  MIT License
├── SECURITY.md              Vulnerability reporting policy
└── README.md                Project documentation
```

## Design scope

Flip Time is deliberately simple. The timer and stopwatch run only while the app is open. Alarm and background-service features are outside the project's scope because they require firmware integration and complicate installation.

See [Architecture](docs/ARCHITECTURE.md) for implementation details.

## Contributing

Contributions are welcome. Read [CONTRIBUTING.md](CONTRIBUTING.md) before opening a pull request. Use the issue templates for bugs and feature proposals, and report vulnerabilities according to [SECURITY.md](SECURITY.md).

## License

Flip Time is released under the [MIT License](LICENSE). Copyright © 2026 Hadi.

Momentum Firmware and Flipper Zero are separate projects and retain their respective trademarks and licenses. This repository is not affiliated with or endorsed by Flipper Devices Inc.
