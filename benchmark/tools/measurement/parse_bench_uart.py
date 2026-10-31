#!/usr/bin/env python3
"""Read UART logs produced by BenchProbe.h and create a small CSV.
Never invent samples; only parse real [BENCH] lines from STM32 output.
"""
import argparse
import csv
import re
from pathlib import Path

HEAD=re.compile(r'\[BENCH\] ECU=(\S+) CLK_HZ=(\d+) TARGET_MS=(\d+) SAMPLES=(\d+)')
TASK=re.compile(r'\[BENCH\] TASK_US min=(\d+) avg=(\d+) p95=(\d+) max=(\d+) missed=(\d+)')
JIT=re.compile(r'\[BENCH\] START_JITTER_US min=(\d+) avg=(\d+) p95=(\d+) max=(\d+)')

def parse(content):
    result=[]
    current=None
    for line in content.splitlines():
        m=HEAD.search(line)
        if m:
            current=dict(zip(['ECU','CLK_HZ','TARGET_MS','SAMPLES'],m.groups()))
        m=TASK.search(line)
        if m and current:
            current.update(dict(zip(['task_min_us','task_avg_us','task_p95_us','task_max_us','task_missed'],m.groups())))
        m=JIT.search(line)
        if m and current:
            current.update(dict(zip(['jitter_min_us','jitter_avg_us','jitter_p95_us','jitter_max_us'],m.groups())))
            if 'task_min_us' in current:
                result.append(current)
            current=None
    return result

if __name__=='__main__':
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument('--log',required=True,type=Path)
    a.add_argument('--output',required=True,type=Path)
    args=a.parse_args()
    parsed=parse(args.log.read_text(encoding='utf-8',errors='replace'))
    if not parsed:
        raise SystemExit('No complete [BENCH] record found. Need all 3 UART lines for an ECU.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    fields=['ECU','CLK_HZ','TARGET_MS','SAMPLES','task_min_us','task_avg_us','task_p95_us','task_max_us','task_missed','jitter_min_us','jitter_avg_us','jitter_p95_us','jitter_max_us']
    with args.output.open('w',newline='',encoding='utf-8-sig') as f:
        w=csv.DictWriter(f,fieldnames=fields)
        w.writeheader();w.writerows(parsed)
    for x in parsed: print(x['ECU'], 'P95(task)=',x['task_p95_us'],'us;', 'missed=',x['task_missed'])
    print('Wrote',args.output)
