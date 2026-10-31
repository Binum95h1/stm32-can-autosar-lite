#!/usr/bin/env python3
"""Build the six checked-in firmware projects without requiring GNU Make.

Read source lists and flags from their Makefiles. Only the simple variable and
wildcard expressions used by these six Makefiles are supported.
"""
import argparse
import csv
import glob
import re
import shlex
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PROJECTS = ('main_baremetal', 'main_autosar', 'abs_baremetal', 'abs_autosar', 'airbag_baremetal', 'airbag_autosar')


def settings(project):
    text = (project / 'Makefile').read_text(encoding='utf-8').replace('\\\n', ' ')
    variables = dict(re.findall(r'^([A-Za-z_]+)\s*:?=\s*(.*)$', text, re.M))

    def expand(value, depth=0):
        if depth > 12:
            raise ValueError('Recursive Makefile variables')
        value = re.sub(r'\$\(wildcard ([^)]+)\)', lambda m: ' '.join(sorted(str(Path(p).relative_to(project)).replace('\\', '/') for p in glob.glob(str(project / m[1])))), value)
        value = re.sub(r'\$\(([A-Za-z_]+)\)', lambda m: expand(variables[m[1]], depth + 1), value)
        if '$(' in value:
            raise ValueError('Unsupported Makefile expression: ' + value)
        return value

    return {key: shlex.split(expand(variables[key])) for key in ('PROJECT', 'CPU', 'CFLAGS', 'INCLUDES', 'LDFLAGS', 'SRCS')}


def build(project_name, compiler, logs):
    project = ROOT / 'benchmark' / project_name
    cfg = settings(project)
    prefix = str(Path(compiler).parent / 'arm-none-eabi-')
    suffix = '.exe' if compiler.lower().endswith('.exe') else ''
    size = prefix + 'size' + suffix
    objcopy = prefix + 'objcopy' + suffix
    output = cfg['PROJECT'][0]
    log = logs / (project_name + '.log')
    objects = []
    with log.open('w', encoding='utf-8') as stream:
        def run(command):
            stream.write('$ ' + shlex.join(command) + '\n')
            stream.flush()
            proc = subprocess.run(command, cwd=project, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
            stream.write(proc.stdout)
            stream.flush()
            if proc.returncode:
                raise RuntimeError(f'{project_name}: build failed; see {log}')
            return proc.stdout
        for source in cfg['SRCS']:
            obj = str(Path(source).with_suffix('.o')).replace('\\', '/')
            run([compiler, *cfg['CFLAGS'], *cfg['INCLUDES'], '-c', source, '-o', obj])
            objects.append(obj)
        startup = list(project.glob('startup_*.s'))
        if len(startup) != 1:
            raise RuntimeError('Expected one startup assembly file in ' + project_name)
        startup_obj = startup[0].with_suffix('.o').name
        run([compiler, *cfg['CPU'], '-c', startup[0].name, '-o', startup_obj])
        objects.append(startup_obj)
        run([compiler, *cfg['CFLAGS'], *objects, *cfg['LDFLAGS'], '-o', output + '.elf'])
        size_output = run([size, output + '.elf'])
        run([objcopy, '-O', 'binary', output + '.elf', output + '.bin'])
    fields = size_output.strip().splitlines()[-1].split()
    text, data, bss = map(int, fields[:3])
    return {'project': project_name, 'status': 'PASS', 'text_bytes': text, 'data_bytes': data, 'bss_bytes': bss, 'flash_text_plus_data_bytes': text + data, 'ram_data_plus_bss_bytes': data + bss, 'optimization': '-O0', 'log': str(log.relative_to(ROOT)).replace('\\', '/')}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--project', choices=PROJECTS, action='append', help='Build one or more projects; defaults to all six.')
    parser.add_argument('--cc', default='arm-none-eabi-gcc', help='ARM GCC executable or full path')
    args = parser.parse_args()
    compiler = shutil.which(args.cc)
    if not compiler:
        raise SystemExit('ARM GCC not found. Add its bin folder to PATH or pass --cc.')
    logs = ROOT / 'benchmark/results/firmware_build'
    logs.mkdir(parents=True, exist_ok=True)
    rows = []
    version = subprocess.check_output([compiler, '--version'], text=True, encoding='utf-8', errors='replace').splitlines()[0]
    (logs / 'compiler.txt').write_text(version + '\n', encoding='utf-8')
    for project in args.project or PROJECTS:
        row = build(project, compiler, logs)
        rows.append(row)
        print(f"{project}: PASS | Flash text+data={row['flash_text_plus_data_bytes']} B | RAM data+bss={row['ram_data_plus_bss_bytes']} B", flush=True)
    with (logs / 'build_summary.csv').open('w', newline='', encoding='utf-8') as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
