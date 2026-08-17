# Contributing to Flip Time

Thank you for helping improve Flip Time.

## Ways to contribute

- Report reproducible bugs
- Suggest focused UI or accessibility improvements
- Improve documentation
- Test on different Momentum releases
- Submit small, reviewable code changes

## Development setup

1. Install Python 3 and uFBT:

   ```bash
   python3 -m pip install --upgrade ufbt
   ```

2. Configure the Momentum release SDK:

   ```bash
   ufbt update --index-url https://up.momentum-fw.dev/firmware/directory.json --channel release
   ```

3. Clone the repository and build from its root:

   ```bash
   git clone <your-fork-url>
   cd flip-time
   ufbt
   ```

4. The generated FAP will be placed in `dist/`.

## Pull requests

- Create a branch from the default branch.
- Keep each pull request focused on one change.
- Follow the existing C style and use descriptive names.
- Build without errors before opening a pull request.
- Test navigation, Back-button behavior, stopwatch controls, timer editing, timer completion, and app exit on a real Flipper Zero when possible.
- Update `README.md` and `CHANGELOG.md` when behavior changes.
- Do not add unrelated generated files or firmware binaries.

## Reporting bugs

Please include:

- Momentum version and API version
- Flip Time version
- Exact reproduction steps
- Expected and actual behavior
- Whether the issue occurs after reinstalling the FAP
- Photos, video, or logs when useful

## Coding principles

- Keep memory usage predictable.
- Do not add background services.
- Do not modify firmware-wide settings.
- Preserve simple D-pad navigation.
- Avoid blocking the GUI thread.
- Check timer arithmetic for tick-counter wraparound.

By contributing, you agree that your contribution is licensed under the repository's MIT License.
