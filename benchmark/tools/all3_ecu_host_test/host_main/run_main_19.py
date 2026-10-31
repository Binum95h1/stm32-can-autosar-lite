#!/usr/bin/env python3
"""Host regression tests using the CURRENT Main F103 source in benchmark/.
Requires Windows-native GCC (MinGW-w64 or equivalent), not arm-none-eabi-gcc.
"""
import argparse, csv, os, shutil, subprocess, sys
from pathlib import Path

SCENARIOS = [
    "normal", "brake", "reverse", "emergency", "recovery", "abs_rx",
    "abs_gap", "airbag_gap", "ignore_unknown", "ignore_short",
    "tx_busy_retry", "diagnostic", "adc_mask", "concurrent_rx",
    "seq_wrap", "seq_duplicate", "seq_stale", "airbag_duplicate",
    "airbag_stale"
]
VARIANTS = ("main_baremetal", "main_autosar")
INCLUDES = ("Base", "Config", "Services", "Bsw", "Mcal",
            "EcuAbstraction", "Rte", "Swc", "App", "Bare")

def run():
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True, help="benchmark folder containing main_* source")
    parser.add_argument("--cc", default=os.environ.get("HOST_CC", "gcc"), help="host C compiler executable")
    parser.add_argument("--out", type=Path, help="output folder for fresh test results")
    args = parser.parse_args()
    root = args.root.expanduser().resolve()
    host = Path(__file__).resolve().parent
    out = args.out.resolve() if args.out else (root / "results" / "main_f103_host_test")
    binary = out / "bin"
    for name in VARIANTS:
        for f in ("Swc/MainNodeS.c", "Swc/MainNodeS_Data.c"):
            if not (root / name / f).is_file():
                print(f"ERROR: Missing CURRENT source: {root / name / f}", file=sys.stderr)
                return 2
    compiler = shutil.which(args.cc)
    if not compiler:
        print("ERROR: Windows-native gcc was not found. ARM arm-none-eabi-gcc cannot execute host tests on Windows.", file=sys.stderr)
        print("Install MinGW-w64 GCC (e.g. MSYS2 UCRT64), or pass --cc C:\\msys64\\ucrt64\\bin\\gcc.exe", file=sys.stderr)
        return 2
    binary.mkdir(parents=True, exist_ok=True)
    print("CURRENT MAIN ECU STM32F103C8T6 SOURCE | NATIVE GCC HOST TEST", flush=True)
    print("SOURCE:", root, flush=True)
    print("COMPILER:", compiler, flush=True)
    compiled = {}
    for name in VARIANTS:
        project = root / name
        cmd = [compiler, "-std=c11", "-O0", "-Wall", "-Wextra", "-Werror"]
        cmd.extend("-I" + str(project / d) for d in INCLUDES if (project / d).is_dir())
        cmd += [str(host / "mock.c"), str(host / "runner.c"),
                str(project / "Swc/MainNodeS.c"), str(project / "Swc/MainNodeS_Data.c"),
                str(project / "Services/SchM.c")]
        if name == "main_baremetal":
            cmd.extend(str(project / rel) for rel in ("Bare/BareCan.c", "Bare/Can_Upper_Bare.c"))
        else:
            cmd.append("-DTEST_AUTOSAR")
            cmd.extend(str(project / rel) for rel in (
                "Rte/Rte_Main.c", "EcuAbstraction/ComIf.c", "Bsw/PduR.c",
                "Bsw/PduR_PBcfg.c", "Bsw/CanIf.c", "Bsw/CanIf_PBcfg.c", "Bsw/Can_Upper_Autosar.c"
            ))
        exe = binary / ("host_" + name + (".exe" if os.name == "nt" else ""))
        cmd += ["-o", str(exe)]
        print("BUILD:", name, flush=True)
        try:
            subprocess.run(cmd, check=True)
        except (subprocess.CalledProcessError, FileNotFoundError) as err:
            print(f"BUILD FAILED: {name}: {err}", file=sys.stderr)
            return 1
        compiled[name] = exe
    print("-" * 80)
    print(f"{'SCENARIO':23s} {'BARE':8s} {'AUTOSAR':8s} {'PARITY':8s} RESULT")
    rows = []
    for scenario in SCENARIOS:
        results, stdout, stderr = {}, {}, {}
        for name, exe in compiled.items():
            proc = subprocess.run([str(exe), scenario], capture_output=True, text=True, errors="replace")
            results[name] = "PASS" if proc.returncode == 0 else "FAIL"
            stdout[name], stderr[name] = proc.stdout, proc.stderr
            (out / (scenario + "_" + name + ".log")).write_text(
                f"returncode={proc.returncode}\n" + proc.stdout + proc.stderr, encoding="utf-8")
        parity = (stdout["main_baremetal"] == stdout["main_autosar"]
                  and all(x == "PASS" for x in results.values()))
        status = "PASS" if parity else "FAIL"
        rows.append({"scenario": scenario, "result": status,
                     "bare": results["main_baremetal"], "autosar": results["main_autosar"],
                     "equal_outputs": parity, "bare_error": stderr["main_baremetal"].strip(),
                     "auto_error": stderr["main_autosar"].strip()})
        print(f"{scenario:23s} {results['main_baremetal']:8s} {results['main_autosar']:8s} {str(parity):8s} {status}", flush=True)
    csvpath = out / "main_host_results.csv"
    with csvpath.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)
    n = sum(r["result"] == "PASS" for r in rows)
    print("-" * 80)
    print(f"SUMMARY: {n}/{len(rows)} PASS | {len(rows)-n} FAIL | Bare-metal vs AUTOSAR-lite parity")
    print("CSV:", csvpath)
    print("NOTE: Host mock test on PC; NOT a physical STM32/CAN timing test.")
    return 0 if n == len(rows) else 1

if __name__ == "__main__":
    sys.exit(run())
