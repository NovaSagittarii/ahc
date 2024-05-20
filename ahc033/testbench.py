import random
from main import solve_multi

n = 5
random.seed(0)

def gen_input(n: int):
    p = list(range(n**2))
    random.shuffle(p)

    a = [[] for i in range(n)]
    for i in range(n):
        a[i] = p[i * n : (i + 1) * n]
    
    return a

T = 50
penalty = 0
for _ in range(T):
    a = gen_input(n)
    s, y = solve_multi(n, a)
    penalty += s

print("prev", 102750)
print("curr", penalty)