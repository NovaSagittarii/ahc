import random

coords = [(i, j) for i in range(5) for j in range(5)]


def count_inversions(a: "list[int]"):
    inversions = 0
    for i in range(len(a)):
        for j in range(i + 1, len(a)):
            if a[i] > a[j]:
                inversions += 1
    return inversions


def solve(n: int, a: "list[list[int]]", strategy: int = 0):
    # b = [[x // n for x in row] for row in a]
    curr = [0 for i in range(n)]
    grid = [[-1 for _ in range(n)] for _ in range(n)]
    cgrid = [[0 for _ in range(n)] for _ in range(n)]
    output = [[] for _ in range(n)]
    dispatch_count = [0]
    cranes = []

    def process_insert():
        for i, x in enumerate(curr):
            if x == n:
                continue
            if cgrid[i][0]:
                continue
            if grid[i][0] == -1:
                grid[i][0] = a[i][x]
                curr[i] += 1

    def pad():
        longest = max(len(crane.seq) for crane in cranes)
        for crane in cranes:
            crane.pad(longest)

    def clear_residue():
        for i in range(n):
            for j in range(n):
                cgrid[i][j] %= 2

    class Crane:
        def __init__(self, x: int, y: int):
            self.big: bool = y == 0
            self.x = x
            self.y = y
            self.id = y
            self.hold: int = -1
            self.seq: str = ""
            cgrid[y][x] = 1
            self.dead = False

        def dx(self, n: int):
            cgrid[self.y][self.x] = 2
            self.x += n
            self.seq += ("R" if n > 0 else "L") * abs(n)
            cgrid[self.y][self.x] = 1

        def dy(self, n: int):
            cgrid[self.y][self.x] = 2
            self.y += n
            self.seq += ("D" if n > 0 else "U") * abs(n)
            cgrid[self.y][self.x] = 1

        def nop(self, n: int = 1):
            self.seq += "." * n

        def pad(self, n: int = 1):
            self.nop(max(0, n - len(self.seq)))

        def take(self):
            if self.hold != -1:
                return self.nop()
            below = grid[self.y][self.x]
            if below != -1:
                self.seq += "P"
                self.hold = below
                grid[self.y][self.x] = -1

        def drop(self):
            if self.hold == -1:
                return self.nop()
            if self.x == n - 1:
                output[self.y].append(self.hold)
                dispatch_count[0] += 1
            else:
                grid[self.y][self.x] = self.hold
            self.seq += "Q"
            self.hold = -1

        def can_go(self, x: int, y: int):
            if x < 0 or x >= n or y < 0 or y >= n:
                return False
            if cgrid[y][x]:
                return False
            if not self.big and self.hold != -1 and grid[y][x] != -1:
                return False
            return True

        def step_x(self, dx: int):
            if dx == 0:
                return False
            dx = max(-1, min(1, dx))
            # grid[self.y][self.x + dx] == -1 or
            if self.can_go(self.x + dx, self.y):
                self.dx(dx)
                # process_insert()
                return True
            else:
                return False

        def step_y(self, dy: int):
            if dy == 0:
                return False
            dy = max(-1, min(1, dy))
            if self.can_go(self.x, self.y + dy):
                self.dy(dy)
                # process_insert()
                return True
            else:
                return False

        def struggle(self):
            dx, dy = ((1, 0), (0, 1), (-1, 0), (0, -1), (0, 0))[random.randint(0, 4)]
            self.goto(self.x + dx, self.y + dy, False)

        def goto(self, x: int, y: int, can_struggle: bool = True):
            if self.dead:
                return False
            dx = x - self.x
            dy = y - self.y
            # print(f"goto {x},{y}")
            if not self.big:
                can_struggle = False
            if dx or dy:  # go closer
                if self.x < self.y - 2 + self.id:
                    if not self.step_x(dx):
                        if not self.step_y(dy):
                            if can_struggle:
                                self.struggle()
                else:
                    if not self.step_y(dy):
                        if not self.step_x(dx):
                            if can_struggle:
                                self.struggle()
                return False
            else:  # you're there
                return True

        def bomb(self):
            if self.dead:
                return self.nop()
            self.seq += "B"
            self.dead = True
            cgrid[self.y][self.x] = 0

        def step(self):
            if self.dead:
                return
            if not self.big and curr[self.id] >= n - 1 and self.hold == -1:
                self.bomb()
            if self.hold == -1:  # find *something*
                if self.big and cranes[1].dead:
                    # find something good to take?
                    good = []
                    for x, y in coords:
                        v = grid[y][x]
                        if v == -1:
                            continue
                        order = v % 5
                        col = v // 5
                        if order == len(output[col]):
                            good.append((x, y))
                    if not len(good):
                        # find something else to pop out
                        good = [(0, i) for i in range(5) if grid[i][0] != -1]
                    if len(good):
                        x, y = good[0]
                        # print(x, y, grid[y][x])
                        if self.goto(x, y):
                            self.take()
                        return
                # candidates = sorted(
                #     [
                #         (abs(i - self.y), 0, self.y + 0 * i)
                #         for i in range(n)
                #         if grid[i][0] != -1
                #     ]
                # )
                candidates = [(0, 0, self.y)] 
                # if grid[self.y][0] == -1: candidates = []
                if len(candidates):
                    if self.goto(*candidates[0][1:]):
                        if not self.big and curr[self.id] == n - 1:
                            self.bomb()
                        else:
                            self.take()
                else:
                    self.bomb()
                # elif self.
            else:  # move it over
                if self.big and cranes[1].dead: # todo: Strategy Phases
                    if self.hold % n == len(output[self.hold // n]):
                        # print("ORDERED!", self.hold)
                        # ordered item!
                        if self.goto(n - 1, self.hold // n):
                            self.drop()
                    else:  # unordered (just drop it off anywhere)
                        # print("UNORDER", self.hold)
                        nearest = min(
                            *((x, y) for x, y in coords if grid[y][x] == -1 and x > 0),
                            key=lambda xy: abs(xy[0] - self.x) + abs(xy[1] - self.y)
                        )
                        if self.goto(*nearest):
                            self.drop()

                else:
                    if self.goto(n - curr[self.id], self.id):
                        self.drop()

    cranes = [Crane(0, i) for i in range(n)]

    for c in cranes:
        c.dx(1)
    clear_residue()
    # cranes[1].bomb()
    # cranes[2].bomb()
    # cranes[3].bomb()
    # cranes[4].bomb()

    # while min(min(*x) for x in curr) != n:
    for t in range(1000):
        process_insert()
        for c in cranes:
            c.step()
        pad()
        clear_residue()
        if dispatch_count[0] >= n*n: break

    M0 = len(cranes[0].seq)
    M1 = sum(
        count_inversions([x for x in row if i == x // n])
        for i, row in enumerate(output)
    )
    M2 = sum(sum(i != x // n for x in row) for i, row in enumerate(output))
    M3 = sum(n - x for x in curr)
    score = M0 + 100 * M1 + 10000 * M2 + 1000000 * M3
    return (score, "\n".join(crane.seq for crane in cranes))


def rep(x: int, n: int):
    return [x for i in range(n)]


def solve_multi(n: int, a: "list[list[int]]"):
    best = 1e9
    ans = ""
    for i in range(1):
        score, res = solve(n, a)
        if score < best:
            best = score
            ans = res
    return (best, ans)


if __name__ == "__main__":
    n = int(input())
    a = [[int(x) for x in input().strip().split()] for _ in range(n)]
    score, ans = solve_multi(n, a)
    print(score)
    print(ans)
