#!/usr/bin/env python3
"""AtCoder differential random tester. Usage: python3 stress.py [count] [--seed N] [--timeout SEC]"""
import argparse
import random
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent


def run(command, data=None, timeout=2.0):
    return subprocess.run(command, input=data, capture_output=True, cwd=ROOT, timeout=timeout)


def compile_cpp(src, output, extra):
    command = ['g++', '-std=gnu++23', '-O2', '-Wall', '-Wextra', *extra, str(ROOT / src), '-o', str(ROOT / output)]
    print('Compiling:', src, flush=True)
    p = subprocess.run(command, cwd=ROOT)
    if p.returncode:
        sys.exit(p.returncode)


def save_failure(seed, case_num, input_data, fast, naive, reason):
    dest = ROOT / 'failed'
    dest.mkdir(exist_ok=True)
    (dest / 'input.txt').write_bytes(input_data)
    (dest / 'main.out').write_bytes(fast.stdout if fast else b'')
    (dest / 'naive.out').write_bytes(naive.stdout if naive else b'')
    (dest / 'main.err').write_bytes(fast.stderr if fast else b'')
    (dest / 'naive.err').write_bytes(naive.stderr if naive else b'')
    (dest / 'seed.txt').write_text(f'{seed}\n')
    print(f'\nFAIL at test #{case_num}: {reason}')
    print(f'Seed: {seed}')
    print('Input:')
    print(input_data.decode(errors='replace'))
    if fast is not None:
        print('main output:', fast.stdout.decode(errors='replace'))
    if naive is not None:
        print('naive output:', naive.stdout.decode(errors='replace'))
    print('Saved in failed/ (input.txt, outputs, errors, seed.txt)')
    print(f'Reproduce: python3 stress.py 1 --seed {seed}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('count', type=int, nargs='?', default=10000, help='number of tests')
    ap.add_argument('--seed', type=int, default=None, help='first generator seed')
    ap.add_argument('--timeout', type=float, default=2.0, help='seconds for each executable')
    ap.add_argument('--include', action='append', default=[], help='additional g++ flag, e.g. --include=-I/path/to/ac-library')
    args = ap.parse_args()
    if args.count <= 0 or args.timeout <= 0:
        ap.error('count and timeout must be positive')
    compile_cpp('main.cpp', 'main_test', args.include)
    compile_cpp('naive.cpp', 'naive_test', args.include)
    first_seed = args.seed if args.seed is not None else random.SystemRandom().randrange(0, 2**63)
    print(f'Testing {args.count} cases (initial seed: {first_seed})')
    for i in range(args.count):
        seed = first_seed + i
        try:
            gen = run([sys.executable, str(ROOT / 'generate.py'), str(seed)], timeout=args.timeout)
            if gen.returncode != 0:
                print(f'Generator error at seed={seed}:\n{gen.stderr.decode(errors="replace")}')
                sys.exit(2)
            inp = gen.stdout
            try:
                fast = run([str(ROOT / 'main_test')], inp, args.timeout)
            except subprocess.TimeoutExpired:
                save_failure(seed, i + 1, inp, None, None, 'main timeout')
                sys.exit(1)
            try:
                slow = run([str(ROOT / 'naive_test')], inp, args.timeout)
            except subprocess.TimeoutExpired:
                save_failure(seed, i + 1, inp, fast, None, 'naive timeout')
                sys.exit(1)
            if fast.returncode or slow.returncode:
                save_failure(seed, i + 1, inp, fast, slow, f'RE (main={fast.returncode}, naive={slow.returncode})')
                sys.exit(1)
            if fast.stdout.split() != slow.stdout.split():
                save_failure(seed, i + 1, inp, fast, slow, 'WA / output mismatch')
                sys.exit(1)
        except KeyboardInterrupt:
            print(f'\nStopped after {i} cases.')
            sys.exit(130)
        if (i + 1) % 1000 == 0:
            print(f'OK: {i + 1} cases', flush=True)
    print(f'ALL OK: {args.count} cases')


if __name__ == '__main__':
    main()
