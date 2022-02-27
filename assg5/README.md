## National Institute of Technology Calicut
## Department of Computer Science and Engineering
## CS3093D: Networks Laboratory
## Assignment - 5

### Implement the following algorithms in C language.
1. Distance Vector Routing
2. Link State Routing

#### Input format:
Two integers n and m, where n denotes the number of nodes and m denotes the
total number of links, followed by m lines denoting source, destination, and cost
respectively of each link.
Output format:
1. For DVR, display the final routing table at all nodes in the format {destination,
next_hop, cost}
2. For LSR, your program should print out the shortest path to all network nodes
from all node with the complete path and the total cost.
Sample Input:
```bash
4 5
1 2 2
2 3 3
3 4 11
4 1 1
2 4 7
```
Sample Output:
```
For DVR,
Routing table at node 1 (similarly, display for all nodes)
1 1 0
2 2 2
3 2 5
4 4 1
For LSR, at node 1 (Path to all nodes from Node 1, similarly, display for all nodes)
1 -> 1 0
1 -> 2 2
1 -> 2 -> 3 5
1 -> 4 1
```