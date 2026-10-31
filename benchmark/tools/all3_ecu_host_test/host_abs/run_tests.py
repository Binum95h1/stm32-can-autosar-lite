#!/usr/bin/env python3
"""Compile and execute ORIGINAL ABS C sources with host-only hardware stubs.
No third-party Python packages required. Needs a native gcc compiler.
"""
import argparse
import csv
from datetime import datetime, timezone
import json
from pathlib import Path
import shutil
import subprocess
import sys

HERE = Path(__file__).resolve().parent
BASE = HERE.parent
OUTPUT_DIR = HERE / "reports"
BUILD_DIR = HERE / "build"
SCENARIOS = [
    ("normal", "ADC=0 => STOP", "normal"),
    ("forward", "ADC=4095 => FORWARD, PWM=8000, encoder+", "normal"),
    ("reverse", "Direction=1 => REVERSE", "normal"),
    ("brake", "Brake=1 => PWM=0, state BRAKE", "normal"),
    ("wrong_id", "Reject frame outside CAN ID 0x121", "normal"),
    ("short_dlc", "Reject command with DLC<4", "normal"),
    ("deadband", "ADC<=10 should be STOP with no direction", "requirement"),
    ("feedback_cycle", "Feedback should transmit every 10ms", "requirement"),
    ("timeout_boundary", "Loss of MAIN command => safe by <=300ms", "requirement"),
    ("recovery", "Command after timeout restores run", "normal"),
    ("encoder", "Software motor model produces speed via original encoder algorithm", "normal"),
    ("diag", "0x212 encodes a mocked error-status structure", "normal"),
    ("sequence_gap", "Sequence 0->2 logs 1 missing message", "normal"),
    ("first_seq_nonzero", "Starting with sequence 7 counts 7 messages as lost", "observation"),
    ("tx_failure", "CAN Write failure does not advance feedback sequence", "normal"),
    ("motor_stall", "Stalled motor enters FAULT at 500ms", "normal"),
    ("latest_command", "Two RX commands before tick: latest command wins", "normal"),
]

def include_dirs(variant):
    base = BASE / ("abs_baremetal" if variant == "bare" else "abs_autosar")
    folders = ["Base", "Mcal", "Services", "EcuAbstraction", "Swc", "Config"]
    folders += ["Bare"] if variant == "bare" else ["Bsw", "Rte"]
    return base, [str(base / name) for name in folders]

def compile_variant(variant, compiler):
    base, includes = include_dirs(variant)
    source_files = [HERE / "harness.c", HERE / "sim.c", base / "Swc" / "AbsNodeS.c", base / "EcuAbstraction" / "MotorIf.c"]
    if variant == "bare":
        source_files += [base / "Bare" / "BareCan.c",base / "Bare" / "Can_Upper_Bare.c"]
    else:
        source_files += [base / sub for sub in ["Rte/Rte_Abs.c", "EcuAbstraction/ComIf.c", "Bsw/PduR.c", "Bsw/PduR_PBcfg.c", "Bsw/CanIf.c", "Bsw/CanIf_PBcfg.c", "Services/SchM.c"]]
    BUILD_DIR.mkdir(parents=True,exist_ok=True)
    out = BUILD_DIR / ("test_" + variant + (".exe" if sys.platform == "win32" else ""))
    cmd = [compiler,"-std=c11","-O0","-g","-Wall","-Wextra"]
    if variant == "bare":cmd.append("-DHOST_BARE")
    for x in [str(HERE),*includes]:cmd.extend(["-I",x])
    cmd.extend(map(str,source_files))
    cmd += ["-lm","-o",str(out)]
    subprocess.run(cmd,check=True)
    return out

def execute(exe,scenario):
    result = subprocess.run([str(exe),scenario],capture_output=True,text=True,check=True,timeout=10)
    return json.loads(result.stdout)

def check(s, data):
    frames = data['frames']; last_state = data['last_feedback_state']; wheel = data['last_feedback_wheels']
    zero_dir = data['dir'] == [[0,0],[0,0]]
    if s == "normal":return data['pwm']==[0,0] and last_state==0 and zero_dir
    if s == "forward":return data['pwm']==[8000,8000] and data['dir']==[[1,0],[1,0]] and last_state==1 and min(wheel)>0
    if s == "reverse":return data['pwm'][0]>5000 and data['dir']==[[0,1],[0,1]] and last_state==2 and min(wheel)>0
    if s == "brake":return data['pwm']==[0,0] and zero_dir and last_state==3
    if s in ('wrong_id','short_dlc'):return data['pwm']==[0,0] and last_state==0
    if s == "deadband":return data['pwm']==[0,0] and last_state==0 and zero_dir
    if s == "feedback_cycle":return data['frames_t10']>0
    if s == "timeout_boundary":return data['pwm_t300']==0
    if s == "recovery":return data['pwm_t310']==0 and data['pwm'][0]>0 and last_state==1
    if s == "encoder":return min(data['encoder'])>0 and min(wheel)>0
    if s == "diag":return data['diag_count']==1 and data['last_diag_boff']==1 and any(f['id']==0x212 and f['data']==[3,5,2,1,0,1,0,1] for f in frames)
    if s == "sequence_gap":return data['last_rx_lost']==1
    if s == "first_seq_nonzero":return None
    if s == "tx_failure":return data['tx_dropped']==1 and data['feedback_first_ms']>0 and data['feedback_count']>=1 and any(f['id']==0x122 and f['data'][3]==0 for f in frames)
    if s == "motor_stall":return data['pwm']==[0,0] and last_state==4
    if s == "latest_command":return data['pwm']==[0,0] and last_state==3
    raise ValueError(s)

def summarize(s,d):
    if s=='deadband':return f"pwm={d['pwm'][0]}, motor_state={d['last_feedback_state']}, direction={d['dir'][0]}"
    if s=='feedback_cycle':return f"TX @10ms={d['frames_t10']}; first feedback @ {d['feedback_first_ms']}ms"
    if s=='timeout_boundary':return f"PWM @300ms={d['pwm_t300']}; PWM @310ms={d['pwm_t310']}"
    if s=='first_seq_nonzero':return f"initial sequence=7, logged lost={d['last_rx_lost']} (review expected behavior)"
    if s=='diag':return f"0x212 frames={d['diag_count']}, BOFF={d['last_diag_boff']} (mocked)"
    if s=='sequence_gap':return f"sequence 0->2, logged lost={d['last_rx_lost']}"
    if s=='tx_failure':return f"mock failed CAN Write={d['tx_dropped']}, first successful feedback @ {d['feedback_first_ms']}ms"
    return f"PWM={d['pwm'][0]}, state={d['last_feedback_state']}, W1/W2={d['last_feedback_wheels']}, TX={d['feedback_count']}"

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--no-build',action='store_true',help='Use existing host executables')
    parser.add_argument('--cc',default='gcc',help='Native C compiler name/path (not arm-none-eabi-gcc)')
    parser.add_argument('--benchmark-root', type=Path, required=True)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    global BASE, OUTPUT_DIR, BUILD_DIR
    BASE=args.benchmark_root.resolve()
    OUTPUT_DIR=args.out.resolve()
    BUILD_DIR=OUTPUT_DIR / "bin"
    if not args.no_build and shutil.which(args.cc) is None:
        print(f"Native compiler '{args.cc}' is missing. Use WSL/Linux with gcc or install MinGW-w64 (NOT arm-none-eabi-gcc).",file=sys.stderr)
        return 2
    exe={}
    for variant in ('bare','autosar'):
        exe[variant]=compile_variant(variant,args.cc) if not args.no_build else BUILD_DIR/('test_'+variant+('.exe' if sys.platform=='win32' else ''))
        if not exe[variant].is_file():raise FileNotFoundError(exe[variant])
    report_dir=OUTPUT_DIR; traces=report_dir/'traces'
    traces.mkdir(parents=True,exist_ok=True)
    rows=[];all_data={};counts={'PASS':0,'FAIL':0,'REVIEW':0,'PARITY_FAIL':0}
    for scenario, expectation, category in SCENARIOS:
        pair={variant:execute(exe[variant],scenario) for variant in exe}
        for variant,d in pair.items():
            (traces/f'{scenario}_{variant}.json').write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
        a=dict(pair['bare']);b=dict(pair['autosar']);a.pop('variant');b.pop('variant')
        parity=a==b
        verdicts=[check(scenario,pair[v]) for v in ('bare','autosar')]
        verdict='REVIEW' if all(v is None for v in verdicts) else ('PASS' if all(v is True for v in verdicts) else 'FAIL')
        if not parity:counts['PARITY_FAIL']+=1
        counts[verdict]+=1
        rows.append({'scenario':scenario,'requirement':expectation,'category':category,'bare':verdict if verdict=='REVIEW' else ('PASS' if verdicts[0] else 'FAIL'),'autosar':verdict if verdict=='REVIEW' else ('PASS' if verdicts[1] else 'FAIL'),'parity':'PASS' if parity else 'FAIL','observed':summarize(scenario,pair['bare'])})
        all_data[scenario]=pair
        print(f"{scenario:<21} {verdict:<7} parity={'PASS' if parity else 'FAIL'} | {summarize(scenario,pair['bare'])}")
    with (report_dir/'results.csv').open('w',newline='',encoding='utf-8-sig') as fp:
        wr=csv.DictWriter(fp,fieldnames=list(rows[0]));wr.writeheader();wr.writerows(rows)
    json_data={'generated_utc':datetime.now(timezone.utc).isoformat(),'summary':counts,'cases':rows,'results':all_data,'scope':'Actual AbsNodeS, BareCan or RTE/ComIf/PduR/CanIf, MotorIf compiled on native host; mocked hardware/time/Can Driver'}
    (report_dir/'results.json').write_text(json.dumps(json_data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    md=['# ABS software-first functional test report','',f'Generated (UTC): {json_data["generated_utc"]}','',f'**Summary:** {counts["PASS"]} PASS / {counts["FAIL"]} FAIL / {counts["REVIEW"]} REVIEW; parity failures {counts["PARITY_FAIL"]} of {len(rows)}.','', '| Test case | Bare | AUTOSAR-lite | Parity | Observed |','|---|---|---|---|---|']
    for r in rows:md.append(f"| `{r['scenario']}` | {r['bare']} | {r['autosar']} | {r['parity']} | {r['observed']} |")
    md += ['', '## Ghi chu kiem thu',
           '', 'Ket qua duoc tao lai bang C source cua 2 firmware ABS hien tai, dung GCC native va mock ngoai vi.',
           '', 'Kich ban `first_seq_nonzero` duoc danh dau REVIEW: frame dau tien co sequence 7 duoc dem la 7 frame thieu. Can thong nhat quy uoc sequence dau tien.',
           '', 'Khong ket luan latency, jitter, Bus-Off, PWM/encoder vat ly tu host test.']
    (report_dir/'REPORT.md').write_text('\n'.join(md),encoding='utf-8')
    print(f"\nSummary: {counts}; reports: {report_dir}")
    return 1 if counts['FAIL'] or counts['PARITY_FAIL'] else 0

if __name__=='__main__':
    sys.exit(main())
