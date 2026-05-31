# Contributing to NastySaturator F405 Firmware

These guidelines describe how to work on the NastySaturator firmware repository safely and consistently.

## Source of truth

The GitHub repository is the source of truth for firmware development. Before editing any file, pull or inspect the current version from GitHub.

For local work:

```powershell
git checkout main
git pull
```

For AI/Codex-assisted work, always read the current file contents before proposing or applying changes. Do not rely on stale local context.

## Branching model

Use `main` for reviewed and accepted firmware state.

Use short-lived feature branches for changes:

```powershell
git checkout -b feature/soft-servo-pwm
```

Recommended branch prefixes:

- `feature/` for new functionality;
- `fix/` for bug fixes;
- `docs/` for documentation-only changes;
- `refactor/` for internal cleanup without behavior changes;
- `test/` for test or verification-related work.

## Commit style

Keep commits focused and reviewable.

Good examples:

```text
Add software servo PWM scheduler
Fix CDC packet checksum validation
Document NastySaturator control IDs
```

Avoid vague messages such as:

```text
Update files
Fix stuff
Changes
```

## What should be committed

Commit source files, project configuration, protocol headers, documentation, and files required to rebuild the firmware.

Usually commit:

- `Core/`
- `Drivers/` when generated or versioned as part of the project
- `Middlewares/` when required by the project
- `Startup/`
- STM32Cube project files such as `.ioc`, `.project`, `.cproject`, `.mxproject`
- documentation files such as `README.md`, `CONTRIBUTING.md`, and design notes

Usually do not commit:

- build output directories such as `Debug/`, `Release/`, `build/`
- object files and dependency files
- generated binaries unless explicitly needed for a release
- user-local IDE settings
- temporary files and logs

## Firmware safety rules

This firmware may control real hardware connected to analog audio circuits. Treat GPIO, PWM, DAC, relay, bias, and power-control changes as hardware-affecting changes.

Before merging such changes, verify:

1. reset/default pin states;
2. startup sequencing;
3. safe behavior before peripheral initialization;
4. behavior after communication loss or command timeout;
5. interaction with boot pins and debug pins;
6. current and voltage limits of connected circuits.

For any new output pin assignment, document:

- MCU pin name;
- signal name;
- active level;
- reset/default state;
- connected hardware;
- whether the signal is safety-critical.

## Coding style

Prefer clear, explicit embedded C code over overly compact abstractions.

General rules:

- keep interrupt handlers short;
- avoid blocking delays in time-critical code;
- avoid long sections with global interrupts disabled;
- use named constants instead of magic numbers;
- keep protocol numeric IDs stable once released;
- document units for timing, voltage, gain, and scaling values;
- validate host commands before applying them to hardware.

## STM32Cube-generated code

When editing STM32Cube-generated projects:

- keep user code inside `USER CODE BEGIN` / `USER CODE END` sections where applicable;
- avoid manual edits to generated regions unless there is a clear reason;
- mention regenerated CubeMX changes in the commit message or PR description;
- check diffs carefully after regenerating code.

## Testing before merge

For firmware behavior changes, perform the most relevant checks available:

- build succeeds without new warnings;
- firmware flashes successfully;
- device boots reliably;
- USB/CDC or host communication still works;
- affected controls respond correctly;
- unsafe outputs remain inactive during reset and boot;
- timing-sensitive signals are checked with an oscilloscope or logic analyzer when relevant.

For servo/PWM work, verify:

- frame period is approximately 20 ms;
- pulse width range is clamped to the intended limits;
- jitter is acceptable;
- all outputs return to a safe state on startup/reset;
- servo power supply ground is common with MCU ground.

## Pull request checklist

Before requesting review or merging:

- [ ] The branch is based on the latest `main`.
- [ ] The change is limited to one logical topic.
- [ ] The firmware builds.
- [ ] Hardware-affecting behavior is described.
- [ ] New constants, pins, and protocol values are documented.
- [ ] Build artifacts and local files are not committed.
- [ ] README or design notes are updated if behavior changed.

## AI/Codex contribution rules

When using AI assistance:

- provide the current relevant files or allow the assistant to fetch them from GitHub;
- require full replacement file contents for complete rewrites;
- prefer small steps and reviewable changes;
- do not accept changes that invent hardware details without verification;
- review all generated code manually before flashing hardware.

## Reporting issues

When documenting a bug or requested change, include:

- hardware revision if known;
- firmware branch/commit;
- expected behavior;
- actual behavior;
- reproduction steps;
- logs, serial output, scope screenshots, or measurements where useful.
