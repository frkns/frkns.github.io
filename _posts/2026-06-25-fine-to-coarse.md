---
title: Fine-to-coarse Search
---

Here's an idea for improving inference speed of approximate search algorithms I came up with while competing in the Cambridge Battlecode competition. I implemented it for pathfinding, although this idea generalises to most tree search algorithms.

When applying a tree search algorithm like Minimax or MCTS to a game, the accuracy of your model of the environment often matters most near the root of the tree, and less the deeper down you go. So it may be worth it to tradeoff corectness for faster inference by using a lower resolution for search deep down the tree. This makes intuitive sense as near-term actions tend to have a more immediate, "tactical" effect. 

One way try to justify this intuition mathematically is with discounted return and a bit of RL theory. In RL, the state-action value function $Q(a)$ tells us the total reward we expect to recieve if we choose action $a$ now.

$$Q(a) = \mathbb{E}\left[\sum_{t=0}^\infty \gamma^tr_t \mid a_0 = a\right],\quad \gamma < 1$$

If the model of the environment makes an error of at most $\epsilon_d$ at depth $d$, then the effect on the root is weighted by $\gamma^d \epsilon_d$. So deeper nodes can afford to have larger errors. For example, let's suppose $\gamma = 0.9$. At depth 10 the error of the model only matters $0.9^{10} \times 100\% \approx 35\%$ as much as it does at the root.

The effect of this is that we can get away with having a less good model deep in the search tree. For example, say you wanted to implement MCTS for Atari Breakout. You could modify your environment model to have higher frameskip the deeper it searches in the tree. This frameskip modification should work with 2-player games also. With Minimax, this modification would probably be more effective and more pronounced because Minimax normally sets a fixed depth and frameskipping this way fundamentally expands the search horizon.

In the Battlecode competition, I wrote a kind of fine-to-coarse pathfinding. Essentially there are two algorithms with their own tradeoffs: Dijkstra's and BitBFS. Dijkstra's has complexity $O((E+V) \lg V)$ with binary heap. It is slow but handles real/fractional weights flexibly. BitBFS achieves complexity $O(\frac{V}{\text{word size}} \times \text{path weight})$ by exploiting the structure of the grid. BitBFS is super fast with unit weights but struggles with larger weights (notice how the time complexity is dependent on weights). My pathfinding solution combined both algorithms naively by first running Dijkstra's until a fixed depth and then switching to BitBFS. This was what got me thinking about *why* my solution even worked as well as it did. For more details see the pathfinding section of our [postmortem](https://game.battlecode.cam/postmortems/something_else_postmortem.pdf). 

I couldn't find literature on this idea, so I labeled it self-descriptively as fine-to-coarse search - a reversal of the "coarse-to-fine" concept in image processing. 

My clanker's thoughts (GPT-5.5):

```clanker
At the root, tree search looks a lot like best-arm identification. Each legal action is an arm, and the search is trying to figure out which one has the highest expected return. But unlike a normal bandit problem, evaluating an arm is itself a search problem: taking an action opens up a whole subtree of future states, decisions, and opponent responses. 
```
