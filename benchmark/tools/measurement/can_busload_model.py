#!/usr/bin/env python3
"""Offline CAN 2.0A contention estimate using previously captured 3-ECU software traces.
This is NOT a bxCAN hardware emulator or a real-time ECU execution-time benchmark.
No external Python dependencies.
"""
import argparse
import csv
import heapq
import json
import math
from pathlib import Path

DEFAULT_LEVELS = (0,20,40,60,80)

def nominal_bits(dlc):
    if not (0 <= dlc <= 8):
        raise ValueError(f'CAN classic DLC must be 0..8: {dlc}')
    # Standard 11-bit CAN2.0A data frame: SOF + arbitration + control +
    # data + CRC + CRC delimiter + ACK + EOF + intermission = 47+8*DLC.
    return 47 + 8 * dlc

def estimate_bits(dlc, stuffing):
    nominal = nominal_bits(dlc)
    if stuffing == 'nominal':
        return nominal
    # Conservative coarse bound, stuffing affects SOF through CRC sequence.
    return nominal + (33 + 8*dlc)//4

def percentile(nums, pct):
    if not nums:
        return 0.0
    a = sorted(nums)
    return float(a[max(0, math.ceil(pct * len(a))-1)])

def load_trace(path):
    raw = json.loads(Path(path).read_text(encoding='utf-8'))
    if not isinstance(raw, dict) or not isinstance(raw.get('trace'), list):
        raise ValueError('Expected {"trace": [{"t":millis,"id":int,"data":[...]},...]}')
    frames = []
    for idx, f in enumerate(raw['trace']):
        can_id = int(f['id'])
        if not (0 <= can_id < 0x800):
            raise ValueError(f'Only standard 11-bit CAN IDs supported: {can_id}')
        dlc = len(f['data'])
        nominal_bits(dlc)
        frames.append({'t_us':int(round(float(f['t']) * 1000)), 'id':can_id,
                       'dlc':dlc, 'sender':f.get('sender','ECU'),
                       'delivered':bool(f.get('delivered',1)), 'idx':idx, 'background':False})
    return frames, raw.get('summary', {})

def run_model(frames, bitrate, window_us, stuffing):
    """Non-preemptive serial bus; among released frames choose lower CAN ID first.
    This is coarse because release timestamps are quantized in source trace (ms).
    """
    indexed = sorted(frames, key=lambda e:(e['t_us'], e['idx']))
    pending=[]
    now=0.0
    pos=0
    rows=[]
    while pos < len(indexed) or pending:
        if not pending and pos < len(indexed):
            now=max(now, indexed[pos]['t_us'])
        while pos < len(indexed) and indexed[pos]['t_us'] <= now + 1e-7:
            f=indexed[pos]
            heapq.heappush(pending,(f['id'],f['idx'],f))
            pos+=1
        if not pending:
            continue
        _, _, f=heapq.heappop(pending)
        bits=estimate_bits(f['dlc'],stuffing)
        tx_us=(bits/bitrate) * 1_000_000.0
        start=now
        finish=start+tx_us
        wait=max(0.0,start-f['t_us'])
        rows.append({'time_ms':f['t_us']/1000, 'id':f['id'], 'background':f['background'],
                     'wait_us':wait,'wire_us':tx_us,'complete_us':finish-f['t_us'],
                     'delivered_in_source_trace':f['delivered']})
        now=finish
    busy_us=sum(f['wire_us'] for f in rows)
    primary=[f for f in rows if not f['background']]
    return {'offered_bus_load_pct':100*busy_us/window_us,
            'total_frames':len(rows),'real_trace_frames':len(primary),
            'wire_busy_us':round(busy_us,3),
            'priority_wait_mean_us':round(sum(f['wait_us'] for f in primary)/len(primary),3) if primary else 0,
            'priority_wait_p95_us':round(percentile([f['wait_us'] for f in primary],0.95),3),
            'priority_wait_max_us':round(max((f['wait_us'] for f in primary),default=0),3),
            'primary_complete_p95_us':round(percentile([f['complete_us'] for f in primary],0.95),3),
            'source_delivered_count':sum(f['delivered_in_source_trace'] for f in primary),
            'source_intentional_drop_count':sum(not f['delivered_in_source_trace'] for f in primary),
            'trace_start_ms':min((f['time_ms'] for f in rows if not f['background']),default=0),
            'model_backlog_after_window':bool(now > window_us),
            'by_id':{f'0x{x:03X}':{
                'count':len([f for f in primary if f['id']==x]),
                'p95_wait_us':round(percentile([f['wait_us'] for f in primary if f['id']==x],0.95),3),
                'max_wait_us':round(max((f['wait_us'] for f in primary if f['id']==x),default=0),3)}
                for x in sorted({f['id'] for f in primary})}}

def synthesize_background(frames, load_pct, bitrate, window_us, stuffing, background_id):
    budget_us=max(0.0,(load_pct/100.0)*window_us)
    already_us=sum(estimate_bits(f['dlc'],stuffing)*1_000_000.0/bitrate for f in frames)
    frame_us=estimate_bits(8,stuffing)*1_000_000.0/bitrate
    n=max(0,int((budget_us-already_us)/frame_us))
    if n>100_000:
        raise ValueError('Too many synthetic background frames for current window')
    # Evenly spaced arrivals, never claim these frames occurred in a real trace.
    background=[]
    for i in range(n):
        background.append({'t_us':int(i*window_us/max(1,n)), 'id':background_id,
                           'dlc':8,'idx':len(frames)+i, 'sender':'SYNTHETIC',
                           'delivered':True,'background':True})
    return background

def analyze(args):
    original,meta=load_trace(args.trace)
    if not original:
        raise ValueError('Trace is empty')
    duration_ms=args.window_ms or max(f['t_us'] for f in original)/1000+10
    duration_us=round(duration_ms*1000)
    if any(f['t_us'] > duration_us for f in original):
        raise ValueError('Window shorter than existing trace')
    runs=[]
    for target in args.loads:
        background=synthesize_background(original,target,args.bitrate,duration_us,args.stuffing,args.background_id)
        summary=run_model(original+background,args.bitrate,duration_us,args.stuffing)
        summary.update({'scenario':meta.get('case',Path(args.trace).stem),
                        'variant':meta.get('variant','unknown'),
                        'target_load_pct':target,
                        'synthetic_frame_count':len(background),
                        'bitrate_bps':args.bitrate,
                        'model':'CAN2.0A_11bit_serial_priority_queue',
                        'stuffing_assumption':args.stuffing,
                        'window_ms':duration_ms,
                        'trace_is_ms_quantized':True})
        runs.append(summary)
    output=Path(args.output)
    output.mkdir(parents=True,exist_ok=True)
    (output/'busload_summary.json').write_text(json.dumps(runs,indent=2,ensure_ascii=False),encoding='utf-8')
    fields=['scenario','variant','target_load_pct','offered_bus_load_pct','synthetic_frame_count',
            'real_trace_frames','priority_wait_mean_us','priority_wait_p95_us',
            'priority_wait_max_us','primary_complete_p95_us','model_backlog_after_window',
            'source_intentional_drop_count','bitrate_bps','stuffing_assumption']
    with (output/'busload_summary.csv').open('w',newline='',encoding='utf-8-sig') as f:
        w=csv.DictWriter(f,fieldnames=fields,extrasaction='ignore')
        w.writeheader();w.writerows(runs)
    print('MODEL ONLY: synthetic background traffic + timestamp-quantized trace.')
    for r in runs:
        print(f"{r['target_load_pct']:>3.0f}% target | {r['offered_bus_load_pct']:6.2f}% estimated | "
              f"P95 priority wait={r['priority_wait_p95_us']:.1f} us | "
              f"trace frames={r['real_trace_frames']} | added={r['synthetic_frame_count']}")
    print(f'Saved {output}/busload_summary.csv and busload_summary.json')

def self_test():
    assert nominal_bits(0)==47 and nominal_bits(8)==111
    f=[{'t_us':0,'id':0x121,'dlc':5,'idx':0,'background':False,'delivered':True},
       {'t_us':0,'id':0x111,'dlc':2,'idx':1,'background':False,'delivered':True}]
    s=run_model(f,500000,10000,'nominal')
    assert s['real_trace_frames']==2
    assert s['by_id']['0x111']['max_wait_us']==0.0
    assert s['by_id']['0x121']['max_wait_us'] > 0.0
    assert abs(s['offered_bus_load_pct']-((47+40+47+16)*2/10000*100)) < 0.01
    print('SELF TEST PASS: nominal bit budget and 11-bit arbitration ordering')

if __name__=='__main__':
    pa=argparse.ArgumentParser(description=__doc__)
    pa.add_argument('--trace',type=Path)
    pa.add_argument('--output',type=Path,default=Path('results/busload'))
    pa.add_argument('--window-ms',type=float,default=None)
    pa.add_argument('--bitrate',type=int,default=500000)
    pa.add_argument('--stuffing',choices=('nominal','upper'),default='nominal')
    pa.add_argument('--loads',nargs='+',type=float,default=list(DEFAULT_LEVELS))
    pa.add_argument('--background-id',type=lambda s:int(s,0),default=0x080,
                    help='11-bit ID of simulated competing frames; default high priority 0x080')
    pa.add_argument('--self-test',action='store_true')
    a=pa.parse_args()
    if a.self_test:
        self_test()
    else:
        if not a.trace: pa.error('--trace is required except in self-test mode')
        if not (0<=a.background_id<0x800):pa.error('Background ID must be 11-bit')
        if a.bitrate<=0:pa.error('Bitrate must be >0')
        if any(x<0 or x>=100 for x in a.loads):pa.error('Loads must be 0..99%')
        analyze(a)
