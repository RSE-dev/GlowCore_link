# NastySaturator F405 Firmware

Firmware for the GlowCore Audio / RSE Audio **NastySaturator** hardware platform based on the STM32F405 family.

This repository is intended to be the source of truth for the embedded firmware used by the NastySaturator device and for further assisted development through GitHub/Codex workflows.

## Project status

Initial firmware import is complete. The repository is being prepared for structured development, review, and future feature work.

Current known target:

- MCU family: STM32F405
- Board/project name: NastySaturator_F405
- Device type: stereo tube/transistor/pentode saturator controller
- Repository visibility: private

Hardware details, exact pinout, and build instructions should be kept synchronized with the actual STM32Cube project files and schematics.

## What this firmware controls

The NastySaturator firmware is expected to handle low-level control and device state for a digitally controlled analog saturation unit, including features such as:

- input/output gain control;
- dry/wet or processed mix control;
- transistor/pentode bias or blend control;
- EQ or bypass-related controls;
- USB or serial communication with host software;
- device identification and status reporting;
- future motorized/servo-driven or GPIO-controlled hardware functions where applicable.

The exact feature list must follow the implemented firmware and hardware revision.

## Repository layout

The repository was imported from a local firmware working directory. Typical STM32 firmware projects may contain files and folders such as:

```text
Core/
Drivers/
Middlewares/
Startup/
*.ioc
```

Generated build output, temporary files, IDE cache files, object files, binaries, and local machine settings should not be committed.

## Build environment

Recommended baseline tools:

- STM32CubeIDE or compatible STM32CubeMX-generated project workflow;
- ARM GCC toolchain matching the project configuration;
- ST-LINK or another compatible debug/programming interface;
- Git for version control.

Until the build procedure is fully documented, use the original STM32CubeIDE project settings as the authoritative build configuration.

## Building

Open the project in STM32CubeIDE and build the active firmware configuration.

Suggested local workflow:

```powershell
git pull
# open/build in STM32CubeIDE
git status
```

Build artifacts such as `.elf`, `.hex`, `.bin`, `.map`, `.o`, and dependency files must remain untracked unless there is a specific release reason to store them separately.

## Flashing and debugging

Use the debug/programming setup defined by the hardware revision. Typical options include:

- ST-LINK via SWD;
- STM32CubeProgrammer;
- STM32CubeIDE debug configuration.

Before flashing unknown hardware, verify:

- board revision;
- power supply voltages;
- SWD pinout;
- boot pin state;
- connected analog hardware safety conditions.

## Communication protocol

The firmware should keep the host communication protocol stable and documented. For NastySaturator-related control IDs, keep numeric values backward-compatible once released.

Expected saturator-related controls include, but are not limited to:

| Control | Purpose |
| --- | --- |
| `SAT_GAIN_IN` | Input gain |
| `SAT_MIX` | Dry/wet or processed mix |
| `SAT_GAIN_OUT` | Output gain |
| `SAT_TR_BIAS` | Transistor stage bias |
| `SAT_PNT_BIAS` | Pentode stage bias |
| `SAT_TR_PNT_MIX` | Transistor/pentode blend |
| `SAT_PREEQ_ON` | Pre-EQ enable |
| `SAT_POSTEQ_ON` | Post-EQ enable |
| `SAT_BYPASS` | Hardware or logical bypass |

If protocol definitions are implemented in headers, update this README whenever public IDs or scaling rules change.

## Development workflow

Use branches for non-trivial changes:

```powershell
git checkout main
git pull
git checkout -b feature/short-description
```

After changes:

```powershell
git status
git add <files>
git commit -m "Describe the firmware change"
git push -u origin feature/short-description
```

Open a pull request or request review before merging into `main`.

## Working with Codex / AI agents

When using Codex or another AI coding assistant:

1. Treat GitHub as the current source of truth.
2. Read the latest version of any file before modifying it.
3. Prefer small, reviewable commits.
4. Do not invent hardware pin assignments or protocol IDs without checking project files or schematics.
5. Keep generated output out of the repository.
6. For complete file rewrites, provide the full resulting file content.

## Safety notes

This firmware controls real analog audio hardware. Changes may affect relays, bias networks, gain stages, servos, power sequencing, or other hardware-connected functions.

Before testing firmware on real hardware:

- confirm safe default GPIO states;
- check that outputs do not momentarily enable unsafe analog states during reset/startup;
- verify boot pin behavior;
- test with current-limited supplies where appropriate;
- avoid connecting valuable audio equipment until the control behavior is verified.

## License

License status is not finalized in this repository. Add a `LICENSE` file before distributing the firmware outside the private development workflow.
