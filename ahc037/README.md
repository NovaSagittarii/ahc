# AHC037 - Soda

Hm... so this time, its a 2D visiting game...

When you can maintain some sort of skyline, this is not interesting.
```
     x
   x-+
   |
x--+
```

When you can't maintain a skyline, this is when you get to do fun stuff.
```
   x
   +--x
   |
x--+
```

This is viewable as merging two points. However, just merging two isn't
sufficient. You'll get 2N points in the end, but I'd like to use all 5N
alloted points.

Can you merge more than two points? Well, not in the case of just two points
here...
- You can't do much when it forms a decreasing skyline.
    ```
      x
      |x
      ++x
    x-+
    ```
If you view it as merges, you won't have more than 2N points in the end,
since it'd form a tree with N leaves and N internal nodes. I guess that's
fine. Not sure what the extra 3N points used for, but its a start.

So how to pick merges?
- Simulated Annealing: might work but what am I even annealing??
- Minimum spanning tree: might be a good baseline. since N=1000, it's possible
    to conside all edges.

Let's build an MST! So you can do edges normally, but in the strange case
(shown below), add a node `o`.
```
x
|
o-x
```

Ok, I kinda built it wrong, the MST doesn't work out since edges are strange.
Or I coded it wrong.

However, the next good idea is making a grid, and the picking the closest point.
So how fine is the grid? Well you have N=1000 points you need to visit, and
1000 points of internal nodes if you were to merge (not now though). So you get
4000 points for the grid (later on it is 3000), so evenly space out a 63x63
grid.