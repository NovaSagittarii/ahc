# AHC038 - Tree Robot Arm

I am starting to see a pattern with the 10-day vs 4-hour AHCs and I don't
like what I'm seeing. D:

Important constraints:

$$
\begin{align*}
N &\in [15, 30]
\\ M &\in \left[\frac{1}{10}N^2, \frac{5}{10}N^2\right]=[22,450]
\\ V &\in [5, 15]
\end{align*}
$$

- Choose the starting place
- Build the tree
- Coordinate the tree

## $\text{0. Naive}$

Well, you could build the $V=1$ tree, and you'd take $O(NM)$ turns, which
is pretty bad. In the worst case of moving a $15\times30$ block of takoyaki
to the right $15$ spaces, you'd spend $15^2\cdot30=6750$ movements.

This is better than not moving all the takoyaki into place which results in
a penalty of $10^5$. So you _always_ want to move all the takoyaki.

## $\text{1. Precision Tools}$

A quick visualization shows you can access nearly all squares with different
parity if your arm lengths are powers of 2. Somewhat related to the Fourier
transforms, or how powers of 2 work.

Code for [KA_PJS](https://www.khanacademy.org/computer-programming/new/pjs)

```js
function f(x, y) {
  ellipse(200 + x * 20, 200 + y * 20, 5, 5);
}
f(0, 0);
function dfs(L, x, y, i) {
  if (i < L.length) {
    dfs(L, x + L[i], y, i + 1);
    dfs(L, x - L[i], y, i + 1);
    dfs(L, x, y + L[i], i + 1);
    dfs(L, x, y - L[i], i + 1);
  } else {
    f(x, y);
  }
}
dfs([4, 2, 1], 0, 0, 0);
```

A general strategy could be to use this general arm with maximal mobility
to move takoyaki from one square to another in up to roughly $4$ turns per
takoyaki. To move the arm from an arbitrary location to another arbitrary
location requires $2$ turns ($180\degree$ turn), but you can cover most spaces
with just $1$ turn.

| vertices used $V$ | Manhattan distance reach |
| ----------------- | ------------------------ |
| 1                 | 1                        |
| 2                 | 3                        |
| 3                 | 7                        |
| 4                 | 15                       |
| 5                 | 31                       |

This idea is... rather feasible since the maximum Manhattan distance you need
to cover is at most $2n-2$ spaces, or $58$ which is supported by the arm that
uses $V=5$ vertices.

If you assume $V\ge 6$, then you can construct this full-movement arm and move
all takoyaki into place in $4M$ operations. With $V\ge 11$, it only takes $2M$
operations.

So now its about $4(15\cdot30)=1800$ turns ($V\ge 6$), a decent improvement.

- When $V\ge 11$, $900$ turns.
- When $V=5$, you can go row by row, so still around $1800$ (?)

### $\text{1.1. Precision Sweep}$

A slight tangent, but instead of looking at full-movement, you only need to
worry about a subset of this. Perhaps it is sufficient to sweep over the
takoyaki from one direction, throwing takoyaki far ahead when you don't need
them to be used for later.

## $\text{2. Batched Precision Tools}$

The weakness of the previous method is that you can only grab one takoyaki at
a time. This is the bottleneck. Since only leaves of the tree can grab
takoyaki, we need _more_ leaves. A simple idea is to duplicate the $L=1$ arms
at the end, which can get a group of takoyaki all 2 Manhattan distance apart.

I'm not sure how to go about proving bounds. Intuitively, when the takoyaki
are sparse, then this doesn't give much benefit anyways (?). When the takoyaki
are dense, then it's very likely they are close together. This should give you
a slight improvement (maybe significant) over the full-movement arm.

For choosing how you pair items, probably just greedily pick nearby pairs?

$\textcolor{red}{\texttt{TODO}}$ how?

## $\text{3. High Throughput}$

So what if you had a bush (low depth, a lot of leaves, low $L$) instead of a
tree? You'd essentially be able to pick up many at once and then move them,
similar to the Dump Truck AHC, but with a maximum capacity of $(V-1)$.

Actually, the spirit of this idea is similar to $\texttt{\#1.1}$

$\textcolor{red}{\texttt{TODO}}$ how? any better than $\texttt{\#2}$?
