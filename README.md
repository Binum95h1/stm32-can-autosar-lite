# Design and Evaluation of a Multi-ECU CAN Communication System at the Register Level Based on a Simplified AUTOSAR Classic Architecture

A three-ECU embedded firmware project comparing a bare-metal implementation with a manually implemented, reduced AUTOSAR Classic-inspired architecture. Both versions implement the same application roles and use register-level STM32 bxCAN drivers.

The repository contains the current Main, ABS and MPU6050 Airbag firmware, reproducible PC host tests, build instructions and results regenerated from the included source. The current firmware snapshot targets **STM32F103C8 / Cortex-M3**, including the Main ECU.

## Current implementation

| ECU | Role implemented in the source | Firmware variants |
| --- | --- | --- |
| Main | Read driver inputs, process Airbag and ABS messages, send control commands, update LCD/UART status | `main_baremetal`, `main_autosar` |
| ABS | Process motor commands, control direction and PWM, calculate encoder-based wheel speed, report status and handle command timeout | `abs_baremetal`, `abs_autosar` |
| Airbag | Read MPU6050 acceleration over I2C, detect and latch a collision indication, control a buzzer and send status | `airbag_baremetal`, `airbag_autosar` |

The Airbag ECU is an educational collision-indication node; its actuator in this project is a buzzer.

Implemented firmware features include:

- Register-level CAN initialization, bit timing, receive filters, transmit mailboxes, receive interrupt handling and error-status access.
- Classic CAN configuration at 500 kbit/s, with standard identifiers.
- Application message counters, transmit retry handling and diagnostic messages.
- Cooperative cyclic execution with millisecond time services.
- Matching application roles in bare-metal and AUTOSAR-lite variants.
- Native C host tests with mocked hardware, time and CAN driver boundaries.

The reduced architecture is referred to as **AUTOSAR-lite** throughout this repository. Its RTE, communication interfaces and BSW/MCAL modules are handwritten for this project. This repository does not claim full AUTOSAR Classic compliance.

## Architecture

```text
Bare-metal                      AUTOSAR-lite
Application / SWC               Application / SWC
    |                               |
Bare CAN adapter                RTE
    |                               |
    |                           Communication interface
    |                               |
    |                           PduR -> CanIf
    |                               |
Register-level CAN driver       Register-level CAN driver
    |                               |
STM32 bxCAN                     STM32 bxCAN
```

The other firmware modules provide platform types, configuration, time services, diagnostics and ECU-specific hardware interfaces. AUTOSAR-lite projects additionally contain startup and mode-management modules.

## CAN message map

| Standard CAN ID | Direction | Purpose |
| --- | --- | --- |
| `0x111` | Airbag -> Main | Collision indication and sequence counter |
| `0x121` | Main -> ABS | Motor/control command |
| `0x122` | ABS -> Main | Motor and wheel-speed feedback |
| `0x211` | Main diagnostic output | CAN diagnostic status |
| `0x212` | ABS diagnostic output | CAN diagnostic status |
| `0x213` | Airbag diagnostic output | CAN diagnostic status |

Payload packing and decoding are defined by the checked-in `Bare` and `EcuAbstraction/ComIf*` modules. Timing values and ECU-specific settings are in each project's `Config/Project_Cfg.h` and SWC configuration files.

## Repository layout

```text
benchmark/
  main_baremetal/        Main ECU, bare-metal
  main_autosar/          Main ECU, AUTOSAR-lite
  abs_baremetal/         ABS ECU, bare-metal
  abs_autosar/           ABS ECU, AUTOSAR-lite
  airbag_baremetal/      Airbag ECU, bare-metal
  airbag_autosar/        Airbag ECU, AUTOSAR-lite
  tools/
    all3_ecu_host_test/  Current-source native C regression tests
    measurement/        CAN traffic model and UART measurement-log parser
tools/
  build_firmware.py     Build all six firmware projects without GNU Make
docs/
  TESTING.md            Validation scope, commands and known limitations
  SOURCE_SELECTION.md   Included source and excluded historical material
  source_manifest.json  SHA-256 hashes of the unchanged firmware files
evidence/
  firmware_build/       Fresh ARM build summaries and logs
  host_tests/           Fresh PC test summaries, cases, traces and source hashes
```

## Build the STM32 firmware

Requirements: Python 3.10 or newer and an Arm GNU Toolchain containing `arm-none-eabi-gcc`, `arm-none-eabi-size` and `arm-none-eabi-objcopy`.

From the repository root:

```sh
python tools/build_firmware.py
```

To build a single project:

```sh
python tools/build_firmware.py --project main_autosar
```

If ARM GCC is outside `PATH`, pass `--cc` with its full executable path. The helper reads the source lists and flags from the checked-in Makefiles; it supports the simple variable and wildcard syntax used by these six projects.

Each project also retains its original Makefile. With GNU Make and the ARM toolchain available, for example:

```sh
make -C benchmark/main_autosar
```

Build outputs are generated inside the project folders and ignored by Git. Build logs and the size summary are written under `benchmark/results/firmware_build/`.

## Run the software tests

Requirements: Python 3.10 or newer and a **native PC GCC** compiler, such as MinGW-w64 GCC on Windows. The ARM cross-compiler is used for firmware builds; native GCC is used for PC execution.

From the repository root:

```sh
python benchmark/tools/all3_ecu_host_test/run_all_3ecus.py --root benchmark
```

Use `--cc` to select a native compiler explicitly, or `--verbose` to print every case. Results and source hashes are regenerated under `benchmark/results/all3_ecu_host_test/`.

## Validation snapshot

All **six ARM firmware projects built successfully** from the source included here. The native C regression run produced:

| ECU | PASS | REVIEW | FAIL | Bare-metal / AUTOSAR-lite output parity |
| --- | ---: | ---: | ---: | ---: |
| Main | 19 | 0 | 0 | 19/19 |
| ABS | 16 | 1 | 0 | 17/17 |
| Airbag | 31 | 0 | 0 | 31/31 |
| Total | 66 | 1 | 0 | 67/67 |

The ABS `first_seq_nonzero` case is **REVIEW**: the first received sequence value of 7 is counted as seven missing messages. The expected startup convention needs to be agreed before treating this case as a pass.

Matching outputs demonstrate parity for the tested scenarios; they do not establish correctness for untested behavior. The PC tests exercise application and selected communication/interface code with mocked peripherals. They do not execute STM32 peripheral registers.

### Firmware section-size comparison

The following values were regenerated with the checked-in `-O0` build configuration. Flash is `text + data`; static RAM is `data + bss`, as reported by `arm-none-eabi-size`.

| ECU | Bare-metal Flash (B) | AUTOSAR-lite Flash (B) | Bare-metal static RAM (B) | AUTOSAR-lite static RAM (B) |
| --- | ---: | ---: | ---: | ---: |
| Main F103 | 11,120 | 12,887 | 504 | 520 |
| ABS F103 | 11,664 | 13,000 | 1,920 | 1,936 |
| Airbag F103 | 9,246 | 10,239 | 348 | 368 |

These are compiler/linker section totals, not measured runtime memory peaks. They exclude runtime stack/heap demand and are specific to the compiler and flags recorded in `evidence/firmware_build/`.

## Evaluation tools

`benchmark/tools/measurement/can_busload_model.py` estimates classic CAN serialization and priority contention from an input trace. Its built-in arithmetic/arbitration self-check can be run with:

```sh
python benchmark/tools/measurement/can_busload_model.py --self-test
```

This is a traffic model with synthetic background frames, not a bxCAN hardware emulator. A successful self-check does not establish an ECU latency result.

`parse_bench_uart.py` parses complete `[BENCH]` records from a supplied UART log. It is included as a preparation tool; this repository does not include a fresh hardware timing capture or the separate profiling firmware snapshot.

## Validation boundaries

The evidence in this snapshot covers ARM compilation/linking and separate ECU logic tests on a PC. It does not establish physical CAN communication, bit timing on an actual bus, electrical behavior, hardware Bus-Off recovery, MCU execution time, DWT timing, jitter, WCET or end-to-end deadlines. The separate ECU test suite is not a current three-ECU closed-loop integration test.

See [Testing and validation](docs/TESTING.md) for commands, trace locations and the remaining hardware work.
