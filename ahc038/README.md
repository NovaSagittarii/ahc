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

Code for [KA_PJS](https://www.khanacademy.org/computer-programming/new/pjs) -
[render/reach.js](./render/range.js)

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
| 5                 | 31 (always full reach)   |

This idea is... rather feasible since the maximum Manhattan distance you need
to cover is at most $2n-2$ spaces, or $58$ which is supported by the arm that
uses $V=5$ vertices.

If you assume $V\ge 5$, then you can construct this full-movement arm and move
all takoyaki into place in $4M$ operations. With $V\ge 10$, it only takes $2M$
operations.

So now its about $4(15\cdot30)=1800$ turns ($V\ge 5$), a decent improvement.

- When $V\ge 10$, then it takes $900$ turns.
- When $V\ge 15$, then it takes $600$ turns.
- With careful routing, you may be able to do transfers in $1$ turn. (?)

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
are sparse, then this doesn't give much benefit anyways (generally true).
When the takoyaki are dense, then it's very likely they are close together.
This should give you a slight improvement (maybe significant) over the
full-movement arm.

For choosing how you pair items, probably just greedily pick nearby pairs?

### $\text{2.1. Takoyaki Clustering}$

When you do batch grab/release, there is a common point, the root of the
subtree (core). For the takoyaki assigned to the $i$th leaf with length$L_i$
must be $L_i$ straight distance away from the core position.
As such, you need to cluster takoyaki.

Clustered takoyaki can be applied to a clustered ending position of equal or
lesser size.

Takoyaki can be clustered depending on how the leaves are set up (possible $L$).
For example, you only need to consider all $[1,\max L_i]$ offsets.
Clusters should be of size no more than the number of leaves $|L|$ and the
number of takoyaki straight-distance $d$ away cannot exceed the number of
leaves with length $d$, $|\{\forall i:L_i=d\}|$.

You can greedily cluster, but maybe a quick simulated anneal on the clustering
will improve the quality.

Let's try this out. [render/cluster.js](./render/cluster.js)

Greedy-random does not work very well.

For $L=1$, seems you can always take horizontally (if it is good), and
vertically otherwise.

```
? o . o ? o
o . o ? o .

. o # o ! o
o . o ! o .

. o . o # o
o # o ! o .

? o . o ? o
o . o ? o .
```

This greedy solution works decently, much better than the random-greedy.
However, it doesn't work very well for more complex $L$.

To simulated anneal this, you can assign each spot to a candidate center, and
then compute how many centers you would need. So on dense sections, both
greedy and SA perform similarly. On sparse sections, SA performs much better,
especially with complex $L$. A more complex greedy matching strategy could be
used, but this is essentially a form of the
[dominating set problem](https://en.wikipedia.org/wiki/Dominating_set) with
additional constraints (fixed $L$), so it probably won't do that well.

Visualization of SA Clustering: [render/cluster_sa.js](./render/cluster_sa.js)

### $\text{2.2 Simulated Annealing for Takoyaki Clustering}$

The SA state is defined as the cluster position assigned to each node.

The SA transition consists of assigning the cluster of a node.
An optimized transition can be to take two nodes with a common cluster candidate, and then assigning both nodes' to that cluster. Since this is the
final goal, to cluster at least two together.

The SA evaluation function should be able to just count clusters, maybe you
can add a penalty based on the average cluster saturation.

## $\text{3. Hamiltonian Path}$

So what if you had a bush (low depth, a lot of leaves, low $L$) instead of a
tree? You'd essentially be able to pick up many at once and then move them,
similar to the Dump Truck AHC, but with a maximum capacity of $(V-1)$.

A short arm means you cannot reach everything. However, you only need to make
sure you can access the cluster core of the takoyaki cluster and then access
the cluster core of the destination cluster. This means you need to find a
decently short Hamiltonian Path on the bipartitate graph corresponding to
the cluster cores for takoyaki clusters and destination clusters.

However, this isn't quite accurate since you can only send a takoyaki cluster
to a destination cluster when you have sufficient takoyaki. Additionally,
certain arms may be occupied so you may fail to pick up afterwards.

How can you plan a route? Actaully, you may never need to plan a route. It is
rarely worth sacrificing reach for carry capacity, rotations are much faster
than moving the root.

From the arm table earlier, an arm consisting of $V=5$ vertices has a reach of
31 and a takoyaki from anywhere to anywhere in two turns. An arm consisting of
$V=4$ vertices has a reach of 15 is able to cover the entire column or row when
$N=30$ but can move twice as many things, assuming optimal clustering. This
does very badly when the takoyaki and destinations are in opposite corners, but
otherwise should perform a bit better, since you can do a sweep, with decent
throughput (depends on clustering quality). Implementation of this would be
difficult though.

So, you wouldn't really want to move the root (excluding parity reasons) unless
you perform some sweep, because rotations are faster.

### $\text{3.1. Delivery Order}$

How do you pick up takoyaki? Let's find out bounds first. There are up to
$M = 450$ takoyaki, but if there is no clustering, then any order works.
Suppose there is up to 2-clustering, then there will be around $250$ clusters.

Actually, $500$ clusters, half are takoyaki clusters and the other half are
destination clusters. You can view each cluster as an $|\{L_i\}|$-tuple
(one number for each length type). You can have at most $10$ different arm
lengths then you have at most $2^{10}=1024$ grab states.

> The full-reach arm uses $5$ vertices, then you can attach $10$ leaves to this.

### $\text{3.2. Complex Arms}$

Suppose instead of having a $V=1$ leaf attachment, but a $V=2$ leaf attachment.
This would give more degrees of freedom to cluster, but seems quite painful to
implement. Additionally, a $(2,1)$ arm can reach everything a $(3)$ arm can,
and more.
However, you suffer the loss of a leaf. This only makes sense if this results
in a $\ge 50\%$ reduction in cluster count without worsening the routing much.

## $\text{4. Pairing is Hard}$

Actually, pairing is too difficult if you build clusters first. Rather, you
can't really find an good order for fixed clusters due to partial clusters.
You can resolve this by performing clustering and pairing at the same time.

## $\text{5. Layers all the way down}$

What if you SA'd an arm hierarchy with a clustering for every level, recursively. It's kinda what's the entire task is about...