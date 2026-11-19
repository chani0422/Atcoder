
#!/usr/bin/env python3
"""AtCoder ランダムテスト
使い方: python3 stress.py [回数] [--seed N] [--timeout 秒数]
"""
import argparse
import random
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent

# ===== ターミナルの色設定 =====
RESET = '\033[0m'
GREEN = '\033[92m'   # 明るい緑
YELLOW = '\033[93m'  # 明るい黄色
RED = '\033[91m'     # 明るい赤
CYAN = '\033[96m'    # 明るい水色


def colored(text, color):
    return f'{color}{text}{RESET}'


def run(command, data=None, timeout=2.0):
    return subprocess.run(
        command,
        input=data,
        capture_output=True,
        cwd=ROOT,
        timeout=timeout
    )


def compile_cpp(src, output, extra):
    src_path = ROOT / src
    out_path = ROOT / output

    # 更新されていなければコンパイルを省略
    if out_path.exists():
        if src_path.stat().st_mtime <= out_path.stat().st_mtime:
            print(f'コンパイル省略: {src}')
            return

    command = [
        'g++', '-std=gnu++23', '-O2', '-Wall', '-Wextra',
        *extra, str(src_path), '-o', str(out_path)
    ]

    print(f'コンパイル中: {src}', flush=True)
    p = subprocess.run(command, cwd=ROOT)

    if p.returncode:
        print(colored(f'コンパイルに失敗しました: {src}', RED))
        sys.exit(p.returncode)


def save_failure(seed, case_num, input_data, fast, naive, reason):
    dest = ROOT / 'failed'
    dest.mkdir(exist_ok=True)

    # 失敗したテストケースを保存
    (dest / 'input.txt').write_bytes(input_data)
    (dest / 'main.out').write_bytes(fast.stdout if fast else b'')
    (dest / 'naive.out').write_bytes(naive.stdout if naive else b'')
    (dest / 'main.err').write_bytes(fast.stderr if fast else b'')
    (dest / 'naive.err').write_bytes(naive.stderr if naive else b'')
    (dest / 'seed.txt').write_text(f'{seed}\n')
    (dest / 'reason.txt').write_text(
        f'ケース番号: {case_num}\n'
        f'seed: {seed}\n'
        f'原因: {reason}\n',
        encoding='utf-8'
    )


    print(colored(f'ケース番号: {case_num}', CYAN))
    print(f'seed: {seed}')

    print('入力:')
    print(input_data.decode(errors='replace'))

    if fast is not None:
        print('main.cpp の出力:')
        print(fast.stdout.decode(errors='replace'))

    if naive is not None:
        print('naive.cpp の出力:')
        print(naive.stdout.decode(errors='replace'))

    print(colored(f'原因: {reason}', YELLOW))
    print('失敗したケースを failed/ に保存しました')
    print(f'再現コマンド: python3 stress.py 1 --seed {seed}')


def main():
    ap = argparse.ArgumentParser(
        description='AtCoder ランダムテスト'
    )

    ap.add_argument(
        'count', type=int, nargs='?', default=10000,
        help='テスト回数（デフォルト: 10000）'
    )
    ap.add_argument(
        '--seed', type=int, default=None,
        help='最初の乱数seed'
    )
    ap.add_argument(
        '--timeout', type=float, default=2.0,
        help='実行時間制限（秒）'
    )
    ap.add_argument(
        '--include', action='append', default=[],
        help='追加するg++オプション'
    )

    args = ap.parse_args()

    if args.count <= 0 or args.timeout <= 0:
        ap.error('テスト回数と制限時間は正の数にしてください')

    # C++をコンパイル
    compile_cpp('main.cpp', 'main_test', args.include)
    compile_cpp('naive.cpp', 'naive_test', args.include)

    first_seed = (
        args.seed if args.seed is not None
        else random.SystemRandom().randrange(0, 2**63)
    )

    print(f'{args.count} ケースのランテスを開始')
    print(f'初期seed: {first_seed}')

    for i in range(args.count):
        case_num = i + 1
        seed = first_seed + i

        try:
            # ランダムな入力を生成
            gen = run(
                [sys.executable, str(ROOT / 'generate.py'), str(seed)],
                timeout=args.timeout
            )

            if gen.returncode != 0:
                print(colored(f'GEN {case_num}', RED))
                print(f'入力生成に失敗しました（seed: {seed}）')
                print(gen.stderr.decode(errors='replace'))
                sys.exit(2)

            inp = gen.stdout

            # main.cppを実行
            try:
                fast = run(
                    [str(ROOT / 'main_test')],
                    inp,
                    args.timeout
                )
            except subprocess.TimeoutExpired:
                print(colored(f'TLE {case_num}', RED))
                save_failure(
                    seed, case_num, inp,
                    None, None, 'main.cpp が制限時間を超えました'
                )
                sys.exit(1)

            # naive.cppを実行
            try:
                slow = run(
                    [str(ROOT / 'naive_test')],
                    inp,
                    args.timeout
                )
            except subprocess.TimeoutExpired:
                print(colored(f'NAIVE_TLE {case_num}', RED))
                save_failure(
                    seed, case_num, inp,
                    fast, None, 'naive.cpp が制限時間を超えました'
                )
                sys.exit(1)

            # 実行時エラーを確認
            if fast.returncode != 0:
                print(colored(f'RE {case_num}', RED))
                save_failure(
                    seed, case_num, inp,
                    fast, slow,
                    f'main.cpp が異常終了しました（終了コード: {fast.returncode}）'
                )
                sys.exit(1)

            if slow.returncode != 0:
                print(colored(f'NAIVE_RE {case_num}', RED))
                save_failure(
                    seed, case_num, inp,
                    fast, slow,
                    f'naive.cpp が異常終了しました（終了コード: {slow.returncode}）'
                )
                sys.exit(1)

            # 出力を比較
            if fast.stdout.split() != slow.stdout.split():
                print(colored(f'WA {case_num}', YELLOW))
                save_failure(
                    seed, case_num, inp,
                    fast, slow, '出力が一致しません'
                )
                sys.exit(1)

            # 正解
            print(colored(f'AC {case_num}', GREEN), flush=True)

        except KeyboardInterrupt:
            print(f'\nランテスを中断しました（完了: {i} ケース）')
            sys.exit(130)

    # 全ケース正解
    print(colored(f'全{args.count}ケース AC！', GREEN))


if __name__ == '__main__':
    main()
