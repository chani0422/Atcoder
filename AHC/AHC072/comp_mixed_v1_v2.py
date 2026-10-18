#!/usr/bin/env python3
"""AHC072 mixed_v1/mixed_v2の固定コードを逐次比較。自動的な解答改良は行わない。

標準: python3 comp_mixed_v1_v2.py
公式採点必須: python3 comp_mixed_v1_v2.py --judge official
検証器のみの単体テスト: python3 comp_mixed_v1_v2.py --self-test
Rustがない環境では --judge auto がPython移植検証器へ明示的に切り替わる。
"""
import argparse
from collections import Counter, deque
import csv
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys
import threading
import time

VERSIONS = ('mixed_v1', 'mixed_v2')
DIRECTIONS = {'U': (-1, 0), 'D': (1, 0), 'L': (0, -1), 'R': (0, 1)}
LOG_FIELDS = ('baseline_T', 'grouped_T', 'search_T', 'routes_T', 'pool_T',
              'route_trials', 'target_trials', 'target_improvements',
              'jump_trials', 'jump_improvements', 'local_rewrites',
              'pre_mixed_T', 'mixed_saved_steps', 'mixed_proposals', 'mixed_tried',
              'mixed_reordered', 'mixed_searches', 'mixed_routes_found', 'mixed_accepted',
              'mixed_selected_pairs', 'mixed_carry_moves', 'mixed_invalid_routes',
              'mixed_suffix_rejected', 'mixed_fallback', 'mixed_ms', 'elapsed_ms')
FIELDS = ('status', 'T', 'E', 'score', 'seconds', 'detail') + LOG_FIELDS


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def parse_input(text):
    parts = text.split()
    n, k = map(int, parts[:2])
    grid = parts[2:]
    if len(grid) != n or any(len(row) != n for row in grid):
        raise ValueError('入力の盤面サイズが不正です')
    floor = [ch != '#' for row in grid for ch in row]
    stacks, nests = [[] for _ in floor], [-1] * len(floor)
    for p, ch in enumerate(''.join(grid)):
        if 'a' <= ch <= 'l':
            stacks[p] = [ord(ch) - ord('a')]
        elif 'A' <= ch <= 'L':
            nests[p] = ord(ch) - ord('A')
    return n, k, grid, floor, stacks, nests


def apply_operation(n, floor, stacks, nests, op):
    """添付 tools/src/lib.rs の State::apply を直接対応させた独立検証器。"""
    r, c, leave, direction, length = op
    if not (0 <= r < n and 0 <= c < n and 0 <= leave < 8
            and direction in DIRECTIONS and 1 <= length <= 8):
        raise ValueError('Output value out of range')
    source = r * n + c
    height = len(stacks[source])
    if not floor[source] or leave >= height:
        raise ValueError('Source is empty or k is not below height')
    if length > leave + 1:
        raise ValueError('Jump length exceeds k + 1')
    dr, dc = DIRECTIONS[direction]
    for step in range(1, length + 1):
        nr, nc = r + dr * step, c + dc * step
        if not (0 <= nr < n and 0 <= nc < n):
            raise ValueError('Jump leaves board')
        destination = nr * n + nc
        if not floor[destination]:
            raise ValueError('Jump crosses a wall')
    if len(stacks[destination]) + height - leave > 8:
        raise ValueError('Landing height exceeds 8 before receiving')
    block = stacks[source][leave:]
    del stacks[source][leave:]
    stacks[destination].extend(reversed(block))
    for p in (source, destination):
        while stacks[p] and nests[p] >= 0 and stacks[p][-1] == nests[p]:
            stacks[p].pop()


def score_port(input_text, output_text):
    n, _, _, floor, stacks, nests = parse_input(input_text)
    tokens = output_text.split()
    if len(tokens) % 5:
        raise ValueError('Unexpected EOF in output')
    turns = len(tokens) // 5
    if turns > 100000:
        raise ValueError('Too many operations')
    for index in range(turns):
        values = tokens[5 * index:5 * index + 5]
        if not all(re.fullmatch(r'\+?[0-9]+', values[j]) for j in (0, 1, 2, 4)):
            raise ValueError(f'Operation {index + 1}: invalid unsigned integer')
        op = (int(values[0]), int(values[1]), int(values[2]), values[3], int(values[4]))
        try:
            apply_operation(n, floor, stacks, nests, op)
        except ValueError as exc:
            raise ValueError(f'Operation {index + 1}: {exc}') from exc
    remaining = sum(map(len, stacks))
    return turns, remaining, turns + 100000 * remaining


def judge_self_test():
    n = 4
    floor, nests = [True] * 16, [-1] * 16
    stacks = [[] for _ in floor]
    stacks[0] = [0, 1, 2]
    stacks[1] = [3]
    apply_operation(n, floor, stacks, nests, (0, 0, 0, 'R', 1))
    assert stacks[0] == [] and stacks[1] == [3, 2, 1, 0]
    stacks = [[] for _ in floor]
    stacks[0], nests[0], nests[3] = [0, 0, 1], 0, 1
    apply_operation(n, floor, stacks, nests, (0, 0, 2, 'R', 3))
    assert sum(map(len, stacks)) == 0  # 出発点と着地点の両方で帰巣

    def must_reject(st, fl, ns, op, message):
        before = [x[:] for x in st]
        try:
            apply_operation(n, fl, st, ns, op)
        except ValueError as exc:
            assert message in str(exc), str(exc)
            assert before == st  # 不正操作は状態を変更しない
        else:
            raise AssertionError('illegal operation accepted')

    stacks = [[] for _ in floor]
    stacks[0], stacks[1] = [0] * 8, [0]
    ns = [-1] * 16
    ns[1] = 0
    must_reject(stacks, floor, ns, (0, 0, 0, 'R', 1), 'before receiving')
    stacks[0], stacks[1] = [0, 1], []
    wall = floor[:]
    wall[1] = False
    must_reject(stacks, wall, nests, (0, 0, 1, 'R', 2), 'wall')
    must_reject(stacks, floor, nests, (0, 0, 0, 'R', 2), 'k + 1')
    must_reject(stacks, floor, nests, (0, 0, 1, 'U', 1), 'board')
    must_reject(stacks, floor, nests, (1, 1, 0, 'R', 1), 'empty')
    must_reject(stacks, floor, nests, (0, 0, 1, 'X', 1), 'range')
    assert score_port('2 1\naA\n..\n', '0 0 0 R 1\n') == (1, 0, 1)
    assert score_port('2 1\naA\n..\n', '') == (0, 1, 100000)
    print('検証器単体テスト: 10項目 OK（ソルバーは未実行）', flush=True)


def input_features(text):
    n, k, grid, floor, stacks, nests = parse_input(text)
    counts = Counter(st[0] for st in stacks if st)
    goals = {color: p for p, color in enumerate(nests) if color >= 0}
    solo = 0
    for color, goal in goals.items():
        distances = {goal: 0}
        queue = deque([goal])
        while queue:
            p = queue.popleft()
            for dr, dc in DIRECTIONS.values():
                nr, nc = p // n + dr, p % n + dc
                q = nr * n + nc
                if 0 <= nr < n and 0 <= nc < n and floor[q] and q not in distances:
                    distances[q] = distances[p] + 1
                    queue.append(q)
        solo += sum(distances[p] for p, st in enumerate(stacks) if st == [color])
    slimes, floors = sum(counts.values()), sum(floor)
    return dict(N=n, K=k, slimes=slimes, walls=n*n-floors,
                wall_ratio=round((n*n-floors)/(n*n), 6),
                slime_density=round(slimes/(floors-k), 6),
                largest_color=max(counts.values()), solo_distance=solo)


def run_solver(binary, input_path, work, timeout):
    """逐次実行。wait(timeout)の最大50msポーリング誤差を避け、終了時に測る。"""
    expired = threading.Event()
    with input_path.open('rb') as src, (work / 'output.txt').open('wb') as out, \
            (work / 'solver.log').open('wb') as log:
        started = time.perf_counter()
        process = subprocess.Popen([str(binary)], stdin=src, stdout=out, stderr=log, cwd=work)

        def kill_late():
            if process.poll() is None:
                expired.set()
                try:
                    process.kill()
                except ProcessLookupError:
                    pass

        timer = threading.Timer(max(0, timeout - (time.perf_counter() - started)), kill_late)
        timer.daemon = True
        timer.start()
        try:
            returncode = process.wait()
            seconds = time.perf_counter() - started
        finally:
            timer.cancel()
            timer.join()
    return returncode, seconds, expired.is_set() or seconds > timeout


def run_case(binary, visualizer, input_path, work, timeout):
    work.mkdir(parents=True)
    record = {key: '' for key in FIELDS}
    code, seconds, late = run_solver(binary, input_path, work, timeout)
    record['seconds'] = round(seconds, 6)
    solver_log = (work / 'solver.log').read_text(errors='replace')
    for key in LOG_FIELDS:
        match = re.search(r'\b' + key + r'=([0-9.]+)', solver_log)
        if match:
            record[key] = float(match[1]) if key.endswith('_ms') else int(match[1])
    if late:
        record.update(status='TIMEOUT', detail=f'wall={seconds:.6f}s limit={timeout:g}s')
        return record
    if code != 0:
        record.update(status='RUNTIME_ERROR', detail=f'exit={code}')
        return record
    try:
        turns, remaining, score = score_port(input_path.read_text(), (work/'output.txt').read_text())
    except (ValueError, UnicodeError) as exc:
        record.update(status='INVALID_OUTPUT', detail=str(exc))
        return record
    record.update(T=turns, E=remaining, score=score)
    if record['pre_mixed_T'] != '' and record['pre_mixed_T'] - turns != record['mixed_saved_steps']:
        record.update(status='AUDIT_ERROR', detail='追加処理前後の操作数ログが一致しません')
        return record
    (work/'port_score.json').write_text(json.dumps(dict(T=turns, E=remaining, score=score)) + '\n')
    if visualizer:
        try:
            result = subprocess.run([str(visualizer), str(input_path), str(work/'output.txt'), '--no-vis'],
                                    cwd=work, capture_output=True, text=True, timeout=30)
            judge_log = result.stdout + result.stderr
            (work/'vis.log').write_text(judge_log)
            matches = re.findall(r'^\s*Score\s*=\s*(\d+)\s*$', judge_log, re.MULTILINE)
            if result.returncode or len(matches) != 1 or int(matches[0]) != score:
                record.update(status='JUDGE_MISMATCH', detail='公式visと移植検証器が不一致。vis.log参照')
                return record
        except subprocess.TimeoutExpired:
            record.update(status='JUDGE_TIMEOUT')
            return record
    record['status'] = 'OK' if remaining == 0 else 'REMAINING'
    return record


def summarize(records, args, run_dir, judge_mode):
    total = len(records)
    summary = ['比較結果', f'対象: {args.start:04d}.txt～{args.end:04d}.txt（{total}件）',
               f'採点: {judge_mode}', f'実時間上限: {args.timeout:g}秒（起動・終了込み、逐次実行）']
    metrics = {'cases': total, 'judge': judge_mode, 'versions': {}}
    for version in VERSIONS:
        rows = [row[version] for row in records]
        ok = sum(row['status'] == 'OK' for row in rows)
        remaining = sum(row['status'] == 'REMAINING' for row in rows)
        errors = total - ok - remaining
        maximum = max(row['seconds'] for row in rows)
        max_cases = [rec['case'] for rec in records if rec[version]['seconds'] == maximum]
        all_sum = sum(row['T'] for row in rows) if ok == total else None
        metrics['versions'][version] = dict(home=ok, remaining=remaining, errors=errors,
                                           total_T=all_sum, max_seconds=maximum, max_time_cases=max_cases)
        summary.append(f'{version}: 全員帰巣 {ok}/{total}、残存あり {remaining}件、実行・採点エラー '
                       f'{errors}件、合計操作数 {all_sum}、最大実時間 {maximum:.6f}秒（{",".join(max_cases)}）')
    pairs = [r for r in records if all(r[v]['status'] == 'OK' for v in VERSIONS)]
    summary.append(f'操作数の比較対象: 両版とも正常に全員帰巣した {len(pairs)}/{total}件')
    if pairs:
        before, after = [sum(r[v]['T'] for r in pairs) for v in VERSIONS]
        deltas = [(r['case'], r[VERSIONS[0]]['T']-r[VERSIONS[1]]['T']) for r in pairs]
        wins, ties, losses = (sum(d > 0 for _, d in deltas), sum(d == 0 for _, d in deltas),
                              sum(d < 0 for _, d in deltas))
        best, worst = max(0, max(d for _, d in deltas)), max(0, max(-d for _, d in deltas))
        best_cases = [c for c, d in deltas if d == best] if best else []
        worst_cases = [c for c, d in deltas if d == -worst] if worst else []
        rate = 100 * (before-after) / before if before else 0
        metrics['paired'] = dict(count=len(pairs), baseline_T=before, candidate_T=after,
                                 saved=before-after, reduction_percent=rate, wins=wins, ties=ties,
                                 losses=losses, max_gain=best, max_loss=worst,
                                 max_gain_cases=best_cases, max_loss_cases=worst_cases)
        summary += [f'合計操作数: {VERSIONS[0]}={before}, {VERSIONS[1]}={after}',
                    f'削減率: {rate:.4f}%', f'改善 {wins}件 / 同点 {ties}件 / 悪化 {losses}件',
                    f'最大改善 {best}手（{",".join(best_cases) or "なし"}） / '
                    f'最大悪化 {worst}手（{",".join(worst_cases) or "なし"}）']
        if len(pairs) == total and total == 99 and args.start == 1 and args.end == 99:
            summary.append(f'過去のmixed_v1報告値28280との参考差分: {28280-after}手（別環境・別実行）')
    metrics['added_pass_by_version'] = {}
    for version in VERSIONS:
        audited = [r for r in records if r[version]['status'] == 'OK'
                   and r[version]['mixed_saved_steps'] != '']
        if not audited:
            continue
        saved = sum(r[version]['mixed_saved_steps'] for r in audited)
        affected = [r['case'] for r in audited if r[version]['mixed_saved_steps'] > 0]
        pre_total = sum(r[version]['pre_mixed_T'] for r in audited)
        post_total = sum(r[version]['T'] for r in audited)
        assert pre_total - post_total == saved
        metrics['added_pass_by_version'][version] = dict(
            count=len(audited), before_T=pre_total, after_T=post_total,
            saved=saved, improved_cases=affected,
            max_milliseconds=max(r[version]['mixed_ms'] for r in audited))
        summary.append(f'{version}の混載処理: {pre_total} → {post_total}手、'
                       f'{saved}手削減、効果あり {len(affected)}/{len(audited)}件')
    summary += ['削減率は操作数の比較であり、順位表の相対スコアではありません。',
                '時間依存探索のため同じソースでも探索回数と操作数が変動します。',
                f'CSV: {run_dir / "comparison.csv"}', f'詳細ログと出力: {run_dir / "cases"}']
    (run_dir/'summary.json').write_text(json.dumps(metrics, ensure_ascii=False, indent=2)+'\n')
    (run_dir/'summary.txt').write_text('\n'.join(summary)+'\n')
    print('\n'+'\n'.join(summary), flush=True)
    return 0 if len(pairs) == total else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--start', type=int, default=1)
    parser.add_argument('--end', type=int, default=99)
    parser.add_argument('--timeout', type=float, default=2.0)
    parser.add_argument('--judge', choices=('auto', 'official', 'port'), default='auto')
    parser.add_argument('--self-test', action='store_true')
    args = parser.parse_args()
    if args.self_test:
        judge_self_test()
        return 0
    if not 1 <= args.start <= args.end <= 9999 or args.timeout <= 0:
        parser.error('範囲またはtimeoutが不正です')
    root = Path(__file__).resolve().parent
    inputs = [root/'tools/in'/f'{i:04d}.txt' for i in range(args.start, args.end+1)]
    required = inputs + [root/f'{version}.cpp' for version in VERSIONS]
    for path in required:
        if not path.is_file():
            parser.error(f'必要なファイルがありません: {path}')
    visualizer = root/'tools/target/release/vis'
    available = visualizer.is_file() and os.access(visualizer, os.X_OK)
    if args.judge == 'official' and not available:
        parser.error('公式visがありません。toolsで cargo build --release --bin vis を実行してください')
    if args.judge == 'port' or not available:
        visualizer = None
    judge_mode = '公式vis + Python移植検証器（一致を必須とする）' if visualizer else 'Python移植検証器のみ（公式バイナリ未実行）'
    print('採点: '+judge_mode, flush=True)
    compiler = shutil.which('g++')
    if not compiler:
        parser.error('g++が見つかりません')
    run_dir = root/'comparison_results'/datetime.now().strftime('%Y%m%d_%H%M%S_%f')
    snapshots = run_dir/'sources'
    snapshots.mkdir(parents=True)
    input_dir = run_dir/'inputs'
    input_dir.mkdir()
    manifest = dict(timestamp_utc=datetime.now(timezone.utc).isoformat(),
                    platform=platform.platform(), python=sys.version,
                    compiler=subprocess.check_output([compiler, '--version'], text=True).splitlines()[0],
                    flags=['-std=c++17', '-O2', '-Wall', '-Wextra'], timeout=args.timeout,
                    timing='Popen + blocking wait + watchdog; launch included; sequential',
                    order=list(VERSIONS), judge=judge_mode, sources={}, inputs={})
    for input_path in inputs:
        shutil.copyfile(input_path, input_dir/input_path.name)
        manifest['inputs'][input_path.name] = digest(input_path)
    binaries = {}
    print(f'記録先: {run_dir}', flush=True)
    for version in VERSIONS:
        source = snapshots/f'{version}.cpp'
        shutil.copyfile(root/source.name, source)
        manifest['sources'][source.name] = digest(source)
        binary = run_dir/version
        print(f'{version}: コンパイル中', flush=True)
        with (run_dir/f'compile_{version}.log').open('wb') as log:
            subprocess.run([compiler, *manifest['flags'], str(source), '-o', str(binary)],
                           stdout=log, stderr=subprocess.STDOUT, timeout=60, check=True)
        binaries[version] = binary
    shutil.copyfile(Path(__file__), snapshots/Path(__file__).name)
    manifest['sources'][Path(__file__).name] = digest(Path(__file__))
    official_source = root/'tools/src/lib.rs'
    if official_source.is_file():
        shutil.copyfile(official_source, snapshots/'official_lib.rs')
        manifest['official_source_sha256'] = digest(official_source)
    if visualizer:
        manifest['official_binary_sha256'] = digest(visualizer)
    (run_dir/'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2)+'\n')
    records = []
    with (run_dir/'comparison.csv').open('w', newline='', encoding='utf-8-sig') as stream:
        feature_names = list(input_features(inputs[0].read_text()))
        fields = ['case', *feature_names] + [f'{v}_{key}' for v in VERSIONS for key in FIELDS] + ['saved_steps']
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        for index, original_path in enumerate(inputs, 1):
            input_path = input_dir/original_path.name
            case = input_path.stem
            features = input_features(input_path.read_text())
            pair, row = {'case': case, **features}, {'case': case, **features, 'saved_steps': ''}
            for version in VERSIONS:
                record = run_case(binaries[version], visualizer, input_path,
                                  run_dir/'cases'/case/version, args.timeout)
                pair[version] = record
                row.update({f'{version}_{key}': value for key, value in record.items()})
            complete = all(pair[v]['status'] == 'OK' for v in VERSIONS)
            if complete:
                row['saved_steps'] = pair[VERSIONS[0]]['T']-pair[VERSIONS[1]]['T']
            writer.writerow(row)
            stream.flush()
            records.append(pair)
            if index % 10 == 0 or index == len(inputs) or not complete:
                details = ' / '.join(f'{v}: {pair[v]["status"]} T={pair[v]["T"]} '
                                     f'sec={pair[v]["seconds"]}' for v in VERSIONS)
                print(f'[{index}/{len(inputs)}] {case} {details}', flush=True)
    return summarize(records, args, run_dir, judge_mode)


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, subprocess.SubprocessError) as exc:
        print(f'比較を完了できません: {exc}', file=sys.stderr)
        sys.exit(1)
