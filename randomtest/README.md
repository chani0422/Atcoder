# AtCoder Random Test (WSL Ubuntu / C++23)

This template follows AtCoder's "three separate files" differential-testing approach.

## Start

Open this folder in a WSL terminal and run:

```bash
python3 stress.py 10000
```

No third-party Python packages required. Requires `g++` and `python3`.

- `main.cpp`: Your solution (as submitted to AtCoder)
- `naive.cpp`: Simple reference solution
- `generate.py`: Generate valid random input for the task
- `stress.py`: Compile, generate, compare, and stop on mismatch

`main.cpp`, `naive.cpp`, and `generate.py` **currently implement an example task**: maximum contiguous subarray sum. Replace all three for the actual problem.

## Options

```bash
python3 stress.py 100000                    # 100,000 cases
python3 stress.py 1 --seed 123456          # Reproduce exactly one test
python3 stress.py 10000 --timeout 5        # Change per-process time limit
python3 stress.py 10000 --include=-I/home/USER/projects/kyopro/ac-library
```

Use `--include=...` only if your code includes `<atcoder/all>` and `g++` cannot find it. Replace USER with the actual username. Existing C++ `#if !__INCLUDE_LEVEL__ / #include __FILE__` templates should work as long as all dependencies are available.

## On a failure

The tester stops and writes `failed/input.txt`, `failed/main.out`, `failed/naive.out`, `failed/main.err`, `failed/naive.err`, and `failed/seed.txt`.

Reproduce the failure with `python3 stress.py 1 --seed <seed>` or run your executable directly:

```bash
./main_test < failed/input.txt
./naive_test < failed/input.txt
```

**Notes:** Output is compared as whitespace-separated tokens, so formatting-only whitespace differences are ignored. This template assumes one uniquely determined answer, not special judges or interactive problems. A passed random test does not prove correctness. Ensure generated inputs satisfy all constraints, especially edge cases. `--timeout` applies to each generator/solver process, not to the whole run.
