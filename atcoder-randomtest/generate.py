import random
import sys

# The argument fixes the seed so failing cases are reproducible.
seed = int(sys.argv[1])
rng = random.Random(seed)

# Sample: N and N integers. Change to fit the problem constraints.
n = rng.randint(1, 12)
a = [rng.randint(-20, 20) for _ in range(n)]
print(n)
print(*a)
