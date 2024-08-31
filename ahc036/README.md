# AHC036 - Efficient Signal Control

## Unorganized Thoughts

O my goodness, theres a *graph* this time, that's wild.

Hmm... so movement is a standard cost, you can't minimize this too much.
However, you can minimize the signal control cost. In the worst case with
minimal `L_A`, you want to group together commonly used signals. With a
more relaxed `L_A`, you can bring in duplicate signals and cover more commonly
used patterns.

Note that you can reduce signal cost by 75% (minimal `L_B=4`) to 
96% (maximal `L_B=24`). So it's a pretty big factor.

Ideas
- Sampling the paths for most commonly used routes, maybe remove random nodes
  to see which ones are more often used. During sampling, maybe check edges
  since this gives you relations between vertices.
- You can also run APSP if you want, since n=600. Might cost significant time.

How efficiently can you compute the overall cost of a given `A`? It may be
possible to run SA on this.

For an iterative update of A... you'd need to update paths when you break
and existing sequence, as well as extending existing paths when you connect.
So you'd need to check the neighbors.

Ok, there's too many nodes, let's make a road hierarchy of nodes. That should
simplify how expensive updates are. How close should every node be to a hub?
Well... actually it depends based on how frequently the node gets visited.
So to compute the overall hub quality, use BFS to assign a hub to every node.
Maybe you can assign hubs using SA too haha. For now, I can assign hubs
randomly, and then repeatedly make a new hub at the node farthest from a hub
until the all nodes are within 4 i guess.
This is similar to kmeans, but without the mean adjustment.

Now figure out how frequent some edges are by running Dijkstra (maybe you
don't need to do all of them, just a few samples?). Now you should have
hub-to-hub connections (prioritize them first), and hub-to-node connections.
Common hub-to-node connections would be prioritized after hub-to-hub.

To compute these priority links, when a path is processed:
- If every node along the path is unused:
  - Mark them as used.
  - Mark the path endpoints as endpoint nodes.
- If some nodes are endpoints:
  - You can extend the longer part of the path, then make a new path
- If no nodes are endpoints, but some nodes are used,
  - If you have extra nodes: you can duplicate the nodes and make a new path.
  - If you have no extra nodes: just split the path into two and proceed as
    usual.
> The linking behavior is kinda reminiscent of
> a red-black tree or link-cut tree. Unfortunately, this isn't a tree...
> Maybe you can make a tree out of it though,
> but you'd need to add some cross-edges...

For the final step of planning the route, you can use Dijkstra. For now, don't
bother with the register allocation-like signal cost saves. If two nodes are
adjacent, 