# Testing and validation

## Firmware builds

Run `python tools/build_firmware.py` from the repository root. This compiles all C sources and startup assembly, links an ELF with each project's `linker.ld`, prints section sizes and generates a binary image. All six projects passed this process for the published snapshot.

The firmware source, Makefiles, startup assembly and linker scripts were copied unchanged from the six current project directories. `docs/source_manifest.json` records their SHA-256 values. The convenience build helper is an addition for repository portability.

The builds use `-O0 -g3` and Cortex-M3 flags from the original Makefiles. Compare Flash/static-RAM values only for the same compiler and options. The linker targets 64 KB Flash and 20 KB RAM; a successful link is not a measurement of stack headroom during execution.

Fresh build logs are generated under `benchmark/results/firmware_build/`. The checked-in `evidence/firmware_build/` directory is a saved validation snapshot. Its log paths are normalized to `$REPO_ROOT` for portability.

## Native host regression tests

```sh
python benchmark/tools/all3_ecu_host_test/run_all_3ecus.py --root benchmark --verbose
```

The runner compiles two native executables per ECU from current application and communication sources plus test harnesses. Each scenario is executed for both architectures, and the runner compares their observable outputs.

| ECU | Scenarios | Coverage examples |
| --- | ---: | --- |
| Main | 19 | Normal commands, brake/reverse, emergency/recovery, incoming feedback, sequence handling, short/unknown frames, transmit busy/retry |
| ABS | 17 | Motor commands, PWM/deadband, feedback cadence, timeout, sequence gaps, transmit failure and a mocked motor stall |
| Airbag MPU6050 | 31 | Acceleration threshold and axes, collision latch, sensor initialization/fault/recovery, nonblocking buzzer warning, transmit retries and sequence wrap |

Results: 66 PASS, 1 REVIEW, 0 FAIL; all 67 scenarios had matching outputs between the two architecture variants. The REVIEW case is the ABS initial-sequence convention (`first_seq_nonzero`). It remains visible in the summary and has not been converted to PASS.

The saved evidence includes the case tables, JSON traces produced by the individual harnesses, per-ECU run logs and firmware source hashes. It was regenerated against the source packaged in this repository, rather than copied from an older validation archive. Read `evidence/tool_versions.txt` for the validation toolchain versions.

## What is mocked

Host tests replace the hardware boundary: peripheral access, CAN driver behavior, timers, sensor I/O, LCD/UART and motor/encoder inputs as required by each harness. The tests execute actual selected C application and communication code, but they do not run `Mcal/Can.c` against STM32 registers.

Mocked CAN error status and simulated time do not measure real arbitration, retransmission, ACK behavior, Bus-Off, sensor timing or interrupt latency. Matching outputs across two implementations is a parity observation, not proof that both implementations satisfy every requirement.

## CAN traffic model

```sh
python benchmark/tools/measurement/can_busload_model.py --self-test
python benchmark/tools/measurement/can_busload_model.py --help
```

The built-in self-check passed for the published snapshot. It checks the nominal bit budget and lower-ID priority ordering on a small model input. No new bus-load performance result is claimed from that self-check.

The optional analysis command accepts a JSON trace containing a `trace` list of frames with `t` in milliseconds, an integer `id`, and a `data` byte list. The model assumes a serial, non-preemptive bus, quantized release timestamps and synthetic competing traffic. Its estimates must be labelled as model results.

## Remaining validation

- Run the selected firmware on the target STM32 boards with CAN transceivers.
- Verify actual bus bit timing, filters, payloads and message counters.
- Test physical error handling and recovery.
- Capture execution time, jitter and end-to-end latency on hardware.
- Rebuild an integrated three-ECU testbed around the current F103 Main and MPU6050 Airbag sources before claiming current closed-loop integration results.

## Reproducing on Windows

Use a native MinGW-w64 GCC for host tests and ARM GCC for firmware builds. Pass compiler paths with `--cc` when needed. If a compiler cannot handle the directory name or path length, extract the repository under a short ASCII path before running the commands. Generated outputs are ignored by Git; saved evidence is stored separately.
