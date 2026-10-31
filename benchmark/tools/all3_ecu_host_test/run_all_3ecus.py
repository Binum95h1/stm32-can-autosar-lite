#!/usr/bin/env python3
"""Run ACTUAL Main / ABS / MPU6050 Airbag C host tests for both architectures.
Does not edit any firmware, does not use historical results as fresh evidence.
"""
import argparse
import csv
import hashlib
import json
import os
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

DIR = Path(__file__).resolve().parent
SPEC = [
    ('MAIN', 'host_main/run_main_19.py', 'main', 'main_host_results.csv', 19),
    ('ABS', 'host_abs/run_tests.py', 'abs', 'results.csv', 17),
    ('AIRBAG_MPU6050', 'host_airbag/run_tests.py', 'airbag', 'airbag_host_results.csv', 31),
]

def native_cc(preferred=None):
    if preferred:
        found = shutil.which(preferred)
        if not found and Path(preferred).is_file():
            found = str(Path(preferred).resolve())
        if not found:
            raise RuntimeError(f"Native compiler not found: {preferred}")
        return found
    for name in ('gcc', 'clang'):
        found = shutil.which(name)
        if found and 'arm-none-eabi' not in found.lower():
            return found
    for loc in (r'C:\msys64\ucrt64\bin', r'C:\msys64\mingw64\bin', r'C:\MinGW\bin',r'C:\Program Files\LLVM\bin'):
        for exe in ('gcc.exe','clang.exe'):
            candidate = Path(loc) / exe
            if candidate.is_file():
                os.environ['PATH'] = str(candidate.parent) + os.pathsep + os.environ.get('PATH','')
                return str(candidate)
    raise RuntimeError('Missing native GCC/Clang on PC. arm-none-eabi-gcc is NOT a host compiler. Install MinGW-w64 GCC (e.g. MSYS2 UCRT64).')

def check_sources(root):
    expected={
        'main_baremetal':['Swc/MainNodeS.c','Swc/MainNodeS_Data.c'],
        'main_autosar':['Swc/MainNodeS.c','Swc/MainNodeS_Data.c'],
        'abs_baremetal':['Swc/AbsNodeS.c','EcuAbstraction/MotorIf.c'],
        'abs_autosar':['Swc/AbsNodeS.c','EcuAbstraction/MotorIf.c'],
        'airbag_baremetal':['Swc/AirbagS.c','EcuAbstraction/Mpu6050If.c'],
        'airbag_autosar':['Swc/AirbagS.c','EcuAbstraction/Mpu6050If.c'],
    }
    for project, files in expected.items():
        for f in files:
            if not (root/project/f).is_file():
                raise FileNotFoundError(f'Missing CURRENT source: {root/project/f}. Have you replaced LIS331HH with MPU6050?')
    manifest=[]
    for project in expected:
        for path in sorted((root/project).rglob('*')):
            if path.is_file() and path.suffix.lower() in ('.c','.h'):
                manifest.append({'path':str(path.relative_to(root)).replace('\\','/'), 'sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
    return manifest

def load_result(label, path, expected_n):
    with path.open('r',encoding='utf-8-sig',newline='') as f:
        lines=list(csv.DictReader(f))
    if len(lines)!=expected_n:
        raise RuntimeError(f'{label}: expected {expected_n} test scenarios, got {len(lines)}')
    normalized=[]
    for row in lines:
        if label=='MAIN':
            name=row['scenario']; status=row['result']; bare=row['bare']; auto=row['autosar']
            parity = str(row['equal_outputs']).lower() == 'true'
        elif label=='ABS':
            name=row['scenario'];bare=row['bare'];auto=row['autosar'];parity=row['parity']=='PASS'
            status='REVIEW' if bare=='REVIEW' and auto=='REVIEW' else ('PASS' if bare=='PASS' and auto=='PASS' and parity else 'FAIL')
        else:
            name=row['case'];status=row['status'];parity=str(row['parity']).lower()=='true';bare=status;auto=status
        if not parity or status not in ('PASS','REVIEW'):
            status='FAIL'
        normalized.append({'ECU':label,'Case':name,'Bare':bare,'AUTOSAR_lite':auto,'Parity':'PASS' if parity else 'FAIL','Result':status,'Provenance':'LIVE_NATIVE_HOST_SOURCE'})
    return normalized

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--root',type=Path,required=True,help='benchmark folder containing 6 CURRENT ECU firmware directories')
    ap.add_argument('--cc',help='Native gcc/clang (optional); not arm-none-eabi-gcc')
    ap.add_argument('--verbose',action='store_true',help='Show every case in terminal (otherwise show compact summary)')
    args=ap.parse_args()
    root=args.root.expanduser().resolve()
    if not root.is_dir():
        raise FileNotFoundError(root)
    compiler=native_cc(args.cc)
    if 'arm-none-eabi' in compiler.lower():
        raise ValueError('DO NOT use ARM cross-compiler for PC tests.')
    manifest=check_sources(root)
    out=root/'results'/'all3_ecu_host_test'
    out.mkdir(parents=True,exist_ok=True)
    generated=datetime.now(timezone.utc).isoformat()
    all_rows=[]; summary=[]
    print('=====================================================================')
    print('3 ECU C HOST TEST | STM32F103C8T6 | CURRENT SOURCES')
    print('Main + ABS + Airbag MPU-6050 | Bare-metal vs AUTOSAR-lite')
    print('Root: ',root)
    print('Native compiler:',compiler)
    print('========================================================================',flush=True)
    for label, relative, output_folder, csvname, expected in SPEC:
        dst=out/output_folder;dst.mkdir(exist_ok=True)
        script=DIR/relative
        if label=='MAIN':
            cmd=[sys.executable,str(script),'--root',str(root),'--out',str(dst),'--cc',compiler]
        elif label=='ABS':
            cmd=[sys.executable,str(script),'--benchmark-root',str(root),'--out',str(dst),'--cc',compiler]
        else:
            cmd=[sys.executable,str(script),'--benchmark-root',str(root),'--out',str(dst)]
        print(f'\n[{label}] COMPILE CURRENT C SOURCE + RUN {expected} SCENARIOS...',flush=True)
        env=os.environ.copy();env['HOST_CC']=compiler
        with (dst/'run.log').open('w',encoding='utf-8') as logfile:
            process=subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8',errors='replace',env=env)
            for line in process.stdout:
                logfile.write(line)
                if args.verbose or any(s in line for s in ('SUMMARY','Summary:','BUILD:','Compile:','BUILD FAILED','ERROR:')):
                    print(line.rstrip(),flush=True)
            exitcode=process.wait()
        if exitcode:
            raise RuntimeError(f'{label} FAILED (exit {exitcode}). View: {dst/"run.log"}')
        rows=load_result(label,dst/csvname,expected)
        all_rows+=rows
        n_pass=sum(row['Result']=='PASS' for row in rows)
        n_review=sum(row['Result']=='REVIEW' for row in rows)
        n_fail=sum(row['Result']=='FAIL' for row in rows)
        parity=sum(row['Parity']=='PASS' for row in rows)
        summary.append({'ECU':label,'PASS':n_pass,'REVIEW':n_review,'FAIL':n_fail,'TOTAL':len(rows),'PARITY':f'{parity}/{len(rows)}'})
    totals={col:sum(s[col] for s in summary) for col in ('PASS','REVIEW','FAIL','TOTAL')}
    print('\n========================================================================')
    print('SLIDE EVIDENCE | 3 ECU HOST TEST | Bare-metal vs AUTOSAR-lite')
    print('========================================================================')
    print(f'{"ECU":18} {"PASS":>7} {"REVIEW":>8} {"FAIL":>7} {"TOTAL":>7} {"PARITY":>9}')
    for s in summary:
        print(f'{s["ECU"]:18} {s["PASS"]:7} {s["REVIEW"]:8} {s["FAIL"]:7} {s["TOTAL"]:7} {s["PARITY"]:>9}')
    print('-'*70)
    print(f'{"TOTAL":18} {totals["PASS"]:7} {totals["REVIEW"]:8} {totals["FAIL"]:7} {totals["TOTAL"]:7} {"67/67":>9}' if all(s['PARITY'].split('/')[0]==s['PARITY'].split('/')[1] for s in summary) else 'PARITY FAILED')
    print('NOTE: ABS first_seq_nonzero is REVIEW, not PASS.')
    print('SCOPE: C logic on PC with mocked peripherals; no physical CAN or real-time latency.')
    print('NOTE: Old 13/13 3-ECU integration result is NOT rerun by this script.')
    with (out/'all3_cases.csv').open('w',encoding='utf-8-sig',newline='') as f:
        wr=csv.DictWriter(f,fieldnames=list(all_rows[0]));wr.writeheader();wr.writerows(all_rows)
    with (out/'all3_summary.csv').open('w',encoding='utf-8-sig',newline='') as f:
        wr=csv.DictWriter(f,fieldnames=list(summary[0]));wr.writeheader();wr.writerows(summary)
    (out/'source_sha256.json').write_text(json.dumps({'generated_utc':generated,'native_compiler':compiler,'firmware_files':manifest},indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print('CSV summary: ',out/'all3_summary.csv')
    print('CSV details: ',out/'all3_cases.csv')
    print('Source hashes:',out/'source_sha256.json')
    if totals['FAIL']:
        return 1
    print('[OK] CURRENT SOURCE HOST TEST COMPLETED')
    return 0

if __name__=='__main__':
    try:sys.exit(main())
    except Exception as e:
        print('ERROR:',e,file=sys.stderr)
        sys.exit(1)
