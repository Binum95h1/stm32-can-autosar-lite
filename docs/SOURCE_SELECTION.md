# Source selection

The project title is an English translation of the title in the supplied research document. That document was used only for the title. Implementation descriptions and validation results come from the current code and newly executed software checks.

## Included

- All `.c`, `.h`, startup `.s`, linker `.ld` and Makefile files from the six current `benchmark/{main,abs,airbag}_{baremetal,autosar}` firmware projects.
- The self-contained `tools/all3_ecu_host_test` native test suite for the current Main F103, ABS F103 and MPU6050 Airbag firmware.
- The existing CAN bus-load estimation script and UART benchmark-log parser.
- English repository documentation, a convenience build helper, a Git ignore file and newly regenerated validation evidence.

Firmware files are copied unchanged. The package intentionally retains the original folder layout so source lists, includes and test-runner paths remain valid.

## Excluded from this initial repository snapshot

- Compiled objects, firmware ELF/BIN/MAP files, PC executables and shared libraries. These are reproducible build outputs.
- ZIP archives, backup copies, editor settings with machine-specific paths and temporary files.
- The `reference`, `mpu6050_port` and `profiling` source snapshots. These are separate historical, migration or instrumentation variants and are not needed to reproduce the selected current firmware.
- Historical result folders and previous resource tables. The evidence here is regenerated from the current source rather than mixing board/sensor revisions.
- The older `can_closedloop` native testbed, whose Airbag build still references `Lis331hhIf.c`. The selected current Airbag uses `Mpu6050If.c`; including that older tool as a working integration test would misrepresent its compatibility.
- The complete research document, personal/student metadata and proposal-only objectives.

The original project directory is not changed by preparing this repository.
