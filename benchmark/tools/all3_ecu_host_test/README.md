# Current-source three-ECU host tests

This suite compiles and runs the actual selected application and communication C sources from the six sibling firmware folders, using PC-native hardware mocks.

Run from the repository root:

```sh
python benchmark/tools/all3_ecu_host_test/run_all_3ecus.py --root benchmark
```

Use `--verbose` to print all cases and `--cc` to specify a native GCC/Clang executable. Use a PC-native compiler, not `arm-none-eabi-gcc`.

The suite runs 19 Main, 17 ABS and 31 Airbag MPU6050 scenarios per architecture pair. It writes summary/case CSV files, source hashes, per-ECU logs and detailed harness outputs under `benchmark/results/all3_ecu_host_test/`.

The current saved run has 66 PASS, 1 REVIEW and no FAIL. ABS `first_seq_nonzero` is REVIEW because the first sequence value of 7 is counted as seven missing messages. All 67 scenario pairs have matching observable outputs.

Hardware and time are mocked. This suite does not execute peripheral registers, measure physical CAN or establish real-time deadlines. It tests the ECUs separately and does not rerun the historical closed-loop three-ECU integration test.
