#!/usr/bin/env python3
"""AHC072: targeted_v6版とtargeted_v7版を実行・公式採点する。コードの自動改良は行わない。"""
import argparse
import csv
from datetime import datetime
import hashlib
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time

VERSIONS = {
    'targeted_v6': 'targeted_v6.cpp',
    'targeted_v7': 'targeted_v7.cpp',
}


def decode_score(judge_log, operations, initial_slimes):
    matches = re.findall(r'^\s*Score\s*=\s*(\d+)\s*$', judge_log, re.MULTILINE)
    if len(matches) != 1:
        raise ValueError('公式ツールのScoreを一意に読み取れません。vis.logを確認してください。')
    score = int(matches[0])
    if score <= 0:
        raise ValueError('Scoreが0以下です。不正出力などの可能性があります。')
    if not 0 <= operations <= 100000:
        raise ValueError('操作数が範囲外です。')
    penalty = score - operations
    if penalty < 0 or penalty % 100000:
        raise ValueError('公式Scoreと操作数が得点式に一致しません。')
    remaining = penalty // 100000
    if remaining > initial_slimes:
        raise ValueError('残存数が初期匹数を超えています。')
    return score, remaining


def run_case(binary, visualizer, input_path, work, timeout):
    work.mkdir(parents=True)
    output = work / 'output.txt'
    record = {'status': '', 'T': '', 'E': '', 'score': '', 'seconds': '', 'detail': ''}
    started = time.perf_counter()
    try:
        with input_path.open('rb') as src, output.open('wb') as dst, \
                (work / 'solver.log').open('wb') as log:
            result = subprocess.run([str(binary)], stdin=src, stdout=dst,
                                    stderr=log, cwd=work, timeout=timeout)
        record['seconds'] = round(time.perf_counter() - started, 6)
    except subprocess.TimeoutExpired:
        record.update(status='TIMEOUT', seconds=round(time.perf_counter() - started, 6))
        return record
    if result.returncode != 0:
        record.update(status='RUNTIME_ERROR', detail=f'exit={result.returncode}')
        return record

    record['T'] = sum(bool(line.strip()) for line in output.read_text().splitlines())
    # 空の出力を誤って成功扱いしないよう、公式採点と得点式の両方を確認する。
    try:
        with (work / 'vis.log').open('wb') as log:
            judge = subprocess.run([str(visualizer), str(input_path), str(output)],
                                   stdout=log, stderr=subprocess.STDOUT, cwd=work, timeout=30)
    except subprocess.TimeoutExpired:
        record.update(status='VIS_TIMEOUT')
        return record
    if judge.returncode != 0:
        record.update(status='VIS_ERROR', detail=f'exit={judge.returncode}')
        return record
    initial_slimes = sum('a' <= ch <= 'l' for ch in input_path.read_text())
    try:
        score, remaining = decode_score((work / 'vis.log').read_text(errors='replace'),
                                        record['T'], initial_slimes)
    except ValueError as exc:
        record.update(status='BAD_SCORE', detail=str(exc))
        return record
    record.update(score=score, E=remaining, status='OK' if remaining == 0 else 'REMAINING')
    return record


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--start', type=int, default=1, help='開始ファイル番号。標準は1')
    parser.add_argument('--end', type=int, default=99, help='終了ファイル番号。標準は99')
    parser.add_argument('--timeout', type=float, default=2.0, help='解答1回の実時間上限（秒）')
    args = parser.parse_args()
    if not 1 <= args.start <= args.end <= 9999 or args.timeout <= 0:
        parser.error('1 <= start <= end <= 9999、timeout > 0 にしてください。')

    root = Path(__file__).resolve().parent
    visualizer = root / 'tools/target/release/vis'
    inputs = [root / 'tools/in' / f'{i:04d}.txt' for i in range(args.start, args.end + 1)]
    required = [root / name for name in VERSIONS.values()] + [visualizer] + inputs
    missing = [str(path) for path in required if not path.is_file()]
    if missing:
        print('必要なファイルがありません:\n' + '\n'.join(missing[:10]), file=sys.stderr)
        return 1
    if not os.access(visualizer, os.X_OK):
        print('公式visを実行できません。toolsで cargo build --release を確認してください。', file=sys.stderr)
        return 1
    compiler = shutil.which('g++')
    if compiler is None:
        print('g++が見つかりません。WSLのターミナルで実行してください。', file=sys.stderr)
        return 1

    # 毎回別のフォルダを作り、解答・出力・過去の結果を保持する。
    stamp = datetime.now().strftime('%Y%m%d_%H%M%S_%f')
    run_dir = root / 'comparison_results' / stamp
    snapshots = run_dir / 'sources'
    snapshots.mkdir(parents=True)
    print(f'記録先: {run_dir}', flush=True)
    binaries = {}
    fingerprints = []
    for version, name in VERSIONS.items():
        snapshot = snapshots / name
        shutil.copyfile(root / name, snapshot)
        digest = hashlib.sha256(snapshot.read_bytes()).hexdigest()
        fingerprints.append(f'{name}: sha256={digest}')
        binary = run_dir / version
        print(f'{version}: コンパイル中', flush=True)
        with (run_dir / f'compile_{version}.log').open('wb') as log:
            try:
                build = subprocess.run([compiler, '-std=c++17', '-O2', '-Wall', '-Wextra',
                                        str(snapshot), '-o', str(binary)],
                                       stdout=log, stderr=subprocess.STDOUT, timeout=60)
            except subprocess.TimeoutExpired:
                print(f'コンパイルが時間切れです: {version}', file=sys.stderr)
                return 1
        if build.returncode != 0:
            print((run_dir / f'compile_{version}.log').read_text(errors='replace'), file=sys.stderr)
            return 1
        binaries[version] = binary
    (run_dir / 'sources.txt').write_text('\n'.join(fingerprints) + '\n')

    records = []
    fields = ['case'] + [f'{v}_{field}' for v in VERSIONS
                        for field in ('status', 'T', 'E', 'score', 'seconds', 'detail')] + ['saved_steps']
    csv_path = run_dir / 'comparison.csv'
    with csv_path.open('w', newline='', encoding='utf-8-sig') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        for index, input_path in enumerate(inputs, 1):
            case = input_path.stem
            pair = {'case': case}
            row = {'case': case, 'saved_steps': ''}
            for version, binary in binaries.items():
                work = run_dir / 'cases' / case / version
                record = run_case(binary, visualizer, input_path, work, args.timeout)
                pair[version] = record
                row.update({f'{version}_{key}': value for key, value in record.items()})
            complete = all(pair[v]['status'] == 'OK' for v in VERSIONS)
            if complete:
                row['saved_steps'] = pair['targeted_v6']['T'] - pair['targeted_v7']['T']
            writer.writerow(row)
            stream.flush()
            records.append(pair)
            if index % 10 == 0 or index == len(inputs) or not complete:
                details = ' / '.join(f"{v}: {pair[v]['status']} T={pair[v]['T']} E={pair[v]['E']}"
                                     for v in VERSIONS)
                print(f'[{index}/{len(inputs)}] {case}  {details}', flush=True)

    summary = ['比較結果', f'対象: {args.start:04d}.txt～{args.end:04d}.txt（{len(inputs)}件）',
               f'解答の実時間上限: {args.timeout:g}秒（起動時間を含む手元での測定）']
    for version in VERSIONS:
        all_rows = [row[version] for row in records]
        ok = sum(row['status'] == 'OK' for row in all_rows)
        remaining = sum(row['status'] == 'REMAINING' for row in all_rows)
        errors = len(all_rows) - ok - remaining
        maximum = max(float(row['seconds']) for row in all_rows)
        summary.append(f'{version}: 全員帰巣 {ok}/{len(inputs)}、残存あり {remaining}件、'
                       f'実行・採点エラー {errors}件、最大実時間 {maximum:.4f}秒')
    pairs = [row for row in records if all(row[v]['status'] == 'OK' for v in VERSIONS)]
    summary.append(f'操作数の比較対象: 両版とも全員帰巣した {len(pairs)}/{len(inputs)}件')
    if pairs:
        before = sum(row['targeted_v6']['T'] for row in pairs)
        after = sum(row['targeted_v7']['T'] for row in pairs)
        wins = sum(row['targeted_v7']['T'] < row['targeted_v6']['T'] for row in pairs)
        ties = sum(row['targeted_v7']['T'] == row['targeted_v6']['T'] for row in pairs)
        losses = len(pairs) - wins - ties
        summary.extend([f'合計操作数: targeted_v6={before}, targeted_v7={after}',
                        f'合計操作数の削減率: {(before - after) / before * 100:.2f}%',
                        f'改善 {wins}件 / 同点 {ties}件 / 悪化 {losses}件'])
    summary.extend(['削減率は手元の操作数比較であり、順位表の相対スコアではありません。',
                    f'CSV: {csv_path}', f'詳細ログと出力: {run_dir / "cases"}'])
    text = '\n'.join(summary)
    (run_dir / 'summary.txt').write_text(text + '\n', encoding='utf-8')
    print('\n' + text)
    return 0 if len(pairs) == len(inputs) else 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, UnicodeError) as exc:
        print(f'ファイルまたは実行環境のエラー: {exc}', file=sys.stderr)
        sys.exit(1)
