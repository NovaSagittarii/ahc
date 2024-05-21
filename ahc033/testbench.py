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

T = 10000
penalty = 0
for _ in range(T):
    a = gen_input(n)
    s, y = solve_multi(n, a)
    penalty += s
    if s >= 1000: # adversial test case found
        print(n)
        for row in a: 
            print(" ".join(str(s) for s in row))
        print(y)
        print()
        break

print("prev", 14396)
print("curr", penalty / T * 50)