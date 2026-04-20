# Agent Instructions for stm32f411_cli

## Project type
- STM32F411-based embedded firmware project using STM32CubeMX-generated code.
- Build system is CMake with Ninja and an ARM GCC toolchain.
- Application logic lives under `MyApp/`; generated HAL and startup code is under `Core/`, `Drivers/`, and `cmake/stm32cubemx`.

## Important files
- `CMakeLists.txt` — root build definition.
- `CMakePresets.json` — supported configure/build presets (`Debug`, `Release`).
- `cmake/gcc-arm-none-eabi.cmake` — ARM GCC toolchain configuration.
- `startup_stm32f411xe.s` — ARM Cortex-M4 startup assembly.
- `STM32F411XX_FLASH.ld` / `STM32F411RETX_FLASH.ld` — linker scripts.
- `Core/Src/main.c` — application entry point.

## Build instructions
- Use CMake presets:
  - `cmake --preset Debug`
  - `cmake --build build/Debug`
- The project is intended to use Ninja and the toolchain configured in `CMakePresets.json`.

## Agent behavior
- Prefer editing user application code in `MyApp/`.
- Avoid changing STM32CubeMX-generated files unless the user explicitly asks.
- When suggesting fixes, keep the Cortex-M4/embedded context in mind:
  - limited heap/stack
  - interrupt handlers and HAL callbacks
  - `main()` in `Core/Src/main.c`
  - `target_include_directories`, `target_compile_definitions`, `target_link_libraries` in CMake

## Notes for code changes
- This is a bare-metal embedded project, not a desktop or server app.
- Serial/UART, DMA, ADC, and FreeRTOS configuration typically follow STM32 HAL patterns.
- Be cautious of generated macros and weak symbol overrides in HAL startup files.

## What not to assume
- There is no documentation file like `README.md`; use code and CMake setup as the source of truth.
- Do not assume standard Linux/Windows runtime libraries are available; this is `arm-none-eabi` firmware.
