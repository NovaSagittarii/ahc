n = int(input())
a = [list(map(int, input().strip().split())) for _ in range(n)]

# find the highest
def find_highest():
    res = (-1, -1)
    best = -200
    for i, row in enumerate(a):
        for j, x in enumerate(row):
            if x > best:
                res = (i, j)
                best = x
    return res

def find_nearby_hi(x, y):
    res = (-1, -1)
    best = 2000
    for i, row in enumerate(a):
        for j, x in enumerate(row):
            if x > 0:
                d = abs(i - x) + abs (j - y)
                if d < best:
                    best = d
                    res = (i, j)
    return res

def find_nearby_lo(x, y):
    res = (-1, -1)
    best = 2000
    for i, row in enumerate(a):
        for j, x in enumerate(row):
            if x < 0:
                d = abs(i - x) + abs (j - y)
                if d < best:
                    best = d
                    res = (i, j)
    return res

curr = 0
i = 0
j = 0

def level():
    global i
    global j
    global curr
    if a[i][j] == 0: return
    elif a[i][j] < 0: # fill
        w = min(abs(a[i][j]), curr)
        if curr > 0:
            curr -= w
            a[i][j] += w
            print(-w) # unload
    else: # flatten
        w = a[i][j]
        curr += w
        a[i][j] = 0
        print(f"+{w}") # pick up

def dist(x, y):
    global i
    global j
    return abs(x-i) + abs(y-j)

def go_towards(x, y):
    if x == -1: return False
    global i
    global j
    if i < x: # too high, go down
        print("D")
        i += 1
    elif i > x: # too low, go up
        print("U")
        i -= 1
    else:
        if j < y: # too left, go right
            print("R")
            j += 1
        elif j > y: # too right, go left
            print("L")
            j -= 1
    return True

state = 0
collect_time = 0
while find_nearby_lo(0, 0)[0] != -1 or curr:
    level()
    if state == 0:
        if go_towards(*find_highest()):
            state = 1 # go do a bit of spreading
            collect_time = 0
        else:
            state = 2 # there are no more
    elif state == 1:
        if collect_time <= 30 and dist(*find_nearby_hi(i, j)) <= 5:
            if go_towards(*find_nearby_hi(i, j)):
                collect_time += 1
            else:
                state = 2 # give up
        else:
            state = 2
    elif state == 2:
        if curr > 0:
            go_towards(*find_nearby_lo(i, j))
        else:
            state = 0
