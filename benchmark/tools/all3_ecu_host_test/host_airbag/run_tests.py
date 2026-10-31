from pathlib import Path
import subprocess, shutil, os, json, csv, sys
import argparse
parser=argparse.ArgumentParser()
parser.add_argument('--benchmark-root',required=True)
parser.add_argument('--out',required=True)
args=parser.parse_args()
base=Path(args.benchmark_root).resolve()
results=Path(args.out).resolve();results.mkdir(parents=True,exist_ok=True)
cc=os.getenv('HOST_CC') or next((x for x in ('gcc','clang','cc') if shutil.which(x)),None)
if not cc:
    print('ERROR: Need native C compiler (gcc/clang/cc). ARM arm-none-eabi-gcc cannot execute Windows host tests.')
    sys.exit(2)
compiled={}
for name in ('airbag_baremetal','airbag_autosar'):
    project=base/name
    include=['Base','Config','Swc','Mcal','Services','EcuAbstraction','App']+(['Bare'] if 'bare' in name else ['Bsw','Rte'])
    flags=['-std=c11','-O0','-Wall','-Wextra','-Werror']+['-I'+str(project/i) for i in include]
    sources=[Path(__file__).parent/'mock.c',Path(__file__).parent/'runner.c',project/'Swc/AirbagS.c',project/'EcuAbstraction/Mpu6050If.c']
    if name=='airbag_baremetal':
        sources.append(project/'Bare/BareCan.c')
    else:
        flags += ['-DTEST_AUTOSAR']
        sources += [project/x for x in ('Rte/Rte_Airbag.c','EcuAbstraction/ComIf.c','Bsw/PduR.c','Bsw/PduR_PBcfg.c','Bsw/CanIf.c','Bsw/CanIf_PBcfg.c')]
        # RTE uses vsnprintf, printf from native C lib.
        sources.append(project/'Services/SchM.c')
    executable=results/(name+('.exe' if os.name=='nt' else ''))
    cmd=[cc,*flags,*map(str,sources),'-o',str(executable)]
    print('Compile:',name,flush=True)
    subprocess.run(cmd,check=True)
    compiled[name]=executable
scenarios=['normal','threshold_below','threshold_above','negative_impact','diagonal_above','diagonal_below','z_axis_impact','latch','sensor_fault','sensor_recovery','post_collision_sensor_fault','tx_busy_retry','tx_busy_twice','diag_before_100','diag_at_100','diag_repeated','tx_seq_increment','tx_seq_wrap','heartbeat_latched','sensor_error_timing','report_every_10ms','sensor_init_ok','sensor_init_bad_id','mg_conversion','axes_big_endian','fault_pulse_on','fault_pulse_off','fault_pulse_restart','fault_recover_early','crash_after_fault','fault_after_crash']
def status_frames(data):return [f for f in data['frames'] if f['id']==0x111]
def diag_frames(data):return [f for f in data['frames'] if f['id']==0x213]
def expected(case,o):
    status=status_frames(o); diag=diag_frames(o)
    if case in ('normal','threshold_below','diagonal_below'): return all(f['data'][0]==0 for f in status),'No collision'
    if case in ('threshold_above','negative_impact','diagonal_above','z_axis_impact','latch','heartbeat_latched','post_collision_sensor_fault'):return all(f['data'][0]==1 for f in status) and o['buzzer']==1,'Collision latches'
    if case=='sensor_fault':return o['delay_calls']==0 and o['max_delay']==0 and o['buzzer']==1 and status[0]['data'][0]==0,'Sensor fault: nonblocking warning ON'
    if case=='sensor_recovery':return o['delay_calls']==0 and len(status)==2 and o['buzzer']==0,'Recovery from sensor read fault'
    if case=='tx_busy_retry':return len(status)==2 and status[0]['ok']==0 and status[1]['data'][1]==0 and status[1]['ok']==1,'Sequence retries on busy'
    if case=='tx_busy_twice':return len(status)==3 and status[-1]['data'][1]==0 and status[-1]['ok']==1,'Sequence preserved after two TX fails'
    if case=='diag_before_100':return len(diag)==0,'No premature diagnostic'
    if case=='diag_at_100':return len(diag)==1 and diag[0]['data']==[2,3,4,0,0,0,0,1],'Diagnostic payload'
    if case=='diag_repeated':return len(diag)==2 and [f['t'] for f in diag]==[100,200],'Diagnostic period 100ms'
    if case=='tx_seq_increment':return [f['data'][1] for f in status]==[0,1,2],'Monotonic successful TX seq'
    if case=='tx_seq_wrap':return len(status)==257 and status[255]['data'][1]==255 and status[256]['data'][1]==0,'8-bit counter wraps'
    if case=='sensor_error_timing':return o['delay_calls']==0 and o['max_delay']==0,'Runtime sensor fault path no longer uses blocking delay'
    if case=='sensor_init_ok':return o['cfg_writes']==4 and o['config']==0x03 and o['accel_config']==0x10 and o['max_delay']==10,'WHOAMI/config init'
    if case in ('mg_conversion','axes_big_endian'):return True,'MPU6050 raw conversion/endian checked by executable'
    if case=='sensor_init_bad_id':return o['cfg_writes']==0 and o['delay_calls']==0,'Reject invalid WHO_AM_I'
    if case=='fault_pulse_on':return o['max_delay']==0 and o['buzzer']==1 and o['buzzer_edges']==1,'0-29ms buzzer ON'
    if case=='fault_pulse_off':return o['max_delay']==0 and o['buzzer']==0 and o['buzzer_edges']==2,'>=30ms buzzer OFF'
    if case=='fault_pulse_restart':return o['max_delay']==0 and o['buzzer']==1 and o['buzzer_edges']==3,'100ms buzzer ON again'
    if case=='fault_recover_early':return o['max_delay']==0 and o['buzzer']==0 and o['buzzer_edges']==2,'Recovery immediate OFF'
    if case=='crash_after_fault':return o['max_delay']==0 and o['buzzer']==1 and all(f['data'][0]==1 for f in status[1:]),'Crash after transient fault latches'
    if case=='fault_after_crash':return o['max_delay']==0 and o['buzzer']==1 and all(f['data'][0]==1 for f in status),'Crash latch priority over fault'
    if case=='report_every_10ms':return [f['t'] for f in status]==[0,10,20,30],'CAN send called every 10ms'
    return False,'Missing assertion'
rows=[]
for case in scenarios:
    outputs={}
    for name,exe in compiled.items():
        run=subprocess.run([str(exe),case],capture_output=True,text=True)
        if run.returncode!=0: raise RuntimeError(f'{name} {case}: {run.stderr}')
        outputs[name]=json.loads(run.stdout)
        (results/f'{case}_{name}.json').write_text(json.dumps(outputs[name],indent=2),encoding='utf-8')
    parity=outputs['airbag_baremetal']==outputs['airbag_autosar']
    check,why=expected(case,outputs['airbag_baremetal'])
    status='PASS' if parity and check else 'FAIL'
    print(f'{case:30s} {status}')
    rows.append(dict(case=case,status=status,parity=parity,expected=why))
with (results/'airbag_host_results.csv').open('w',encoding='utf-8',newline='') as f:
    w=csv.DictWriter(f,fieldnames=rows[0].keys());w.writeheader();w.writerows(rows)
print('SUMMARY',*(f'{sum(r["status"]==s for r in rows)} {s}' for s in ('PASS','FAIL','REVIEW')))
if any(r['status']=='FAIL' for r in rows):sys.exit(1)
