"""Run one fixed validation batch without changing either solution."""
from pathlib import Path
import hashlib
import json
import re
import subprocess
import sys
import time

from check_grouped import grid_text, verify


ROOT = Path(__file__).resolve().parent
WORKSPACE = ROOT.parent
DESTINATION = ROOT / 'targeted_results'
DESTINATION.mkdir(exist_ok=True)


def feature_case():
    n, k = 12, 4
    floor = {(r, c) for r in range(n) for c in range(n)}
    nests = [(0, 0), (11, 11), (11, 0), (6, 6)]
    slimes = [(0, 11, 0)]
    slimes += [(r, 10, 0) for r in range(1, 8)]
    slimes += [(0, 10, 1), (10, 0, 2), (6, 7, 3)]
    return grid_text(n, k, floor, nests, slimes)


stored = json.loads((ROOT / 'routes_results/results.json').read_text())
cases = [(row['case'], (ROOT / 'routes_results' / f"{row['case']}.in").read_text())
         for row in stored]
cases.append(('feature_launchpad', feature_case()))

sources = ['ahc072_pool.cpp', 'ahc072_targeted.cpp', 'compare_ahc072_targeted.py']
before = {name: hashlib.sha256((WORKSPACE / name).read_bytes()).hexdigest()
          for name in sources}
(DESTINATION / 'source_hashes.json').write_text(json.dumps(before, indent=2))

binary = Path(sys.argv[1]).resolve()
results = []
for name, data in cases:
    (DESTINATION / f'{name}.in').write_text(data)
    started = time.perf_counter()
    try:
        run = subprocess.run([str(binary)], input=data, text=True,
                             capture_output=True, timeout=2.0, check=False)
        elapsed = time.perf_counter() - started
        (DESTINATION / f'{name}.out').write_text(run.stdout)
        (DESTINATION / f'{name}.log').write_text(run.stderr)
        assert run.returncode == 0, (run.returncode, run.stderr)
        metrics = verify(data, run.stdout)
        routes_match = re.search(r'routes_T=(\d+)', run.stderr)
        launchpad_match = re.search(r'launchpad_reference_T=(\d+)', run.stderr)
        pool_match = re.search(r'pool_T=(\d+)', run.stderr)
        targeted_match = re.search(r'targeted_T=(\d+)', run.stderr)
        jumps_match = re.search(r'jumps=(\d+)', run.stderr)
        groups_match = re.search(r'groups_of_8=(\d+)', run.stderr)
        assert (routes_match and launchpad_match and pool_match and targeted_match
                and jumps_match and groups_match)
        routes_t = int(routes_match.group(1))
        launchpad_t = int(launchpad_match.group(1))
        pool_t = int(pool_match.group(1))
        targeted_t = int(targeted_match.group(1))
        assert metrics['T'] == targeted_t
        assert targeted_t <= pool_t <= launchpad_t <= routes_t
        result = {
            'case': name,
            'valid': True,
            'routes_T': routes_t,
            'launchpad_T': launchpad_t,
            'pool_T': pool_t,
            **metrics,
            'saved': pool_t - targeted_t,
            'jumps': int(jumps_match.group(1)),
            'groups_of_8': int(groups_match.group(1)),
            'seconds': round(elapsed, 4),
        }
    except Exception as exc:
        result = {'case': name, 'valid': False, 'error': str(exc)}
    results.append(result)
    print(json.dumps(result), flush=True)

(DESTINATION / 'results.json').write_text(json.dumps(results, indent=2))
after = {name: hashlib.sha256((WORKSPACE / name).read_bytes()).hexdigest()
         for name in sources}
assert before == after, 'A source file changed during validation'
assert all(row['valid'] for row in results)
assert sum(row['jumps'] for row in results) > 0, 'No two-cell jump was exercised'
raise SystemExit(0)
