import random
import sys

# The argument fixes the seed so failing cases are reproducible.
seed = int(sys.argv[1])
rng = random.Random(seed)

# Sample: N and N integers. Change to fit the problem constraints.
n = rng.randint(0, 4)
k = rng.randint(0, 4)
a = [rng.randint(1, 9) for _ in range(n)]
# b = [rng.randint(-9, 9) for _ in range(n)]
print(n)
print(k)
print(*a)
# print(*b)
