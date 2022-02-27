#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define NUM 10

int n, k;
struct Rout_Tab
{
    int cost[NUM];
    int dest[NUM];
    int next_hop[NUM];
};

void display(struct Rout_Tab Rout_Table, int i)
{
    printf("Routing table of %d th router:\nDest  next hop   cost\n", i);
    for (int j = 1; j <= n; j++)
    {
        if (Rout_Table.cost[j] == INT_MAX)
            printf("%d \t%d \tInf \n", j,
                   Rout_Table.next_hop[j]);
        else
            printf("%d \t%d \t  %d \n", j,
                   Rout_Table.next_hop[j], Rout_Table.cost[j]);
    }
}

void dvr(int cost_table[][n + 1])
{
    struct Rout_Tab Rout_Tables[n + 1];
    // Initialize Routing tables
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            Rout_Tables[i].cost[j] = INT_MAX;
            Rout_Tables[i].dest[j] = j;
            Rout_Tables[i].next_hop[j] = -1;
            if (cost_table[i][j] != -1)
            {
                Rout_Tables[i].cost[j] = cost_table[i][j];
                Rout_Tables[i].next_hop[j] = j;
            }
        }
    }

    int N = n;
    while (N--)
    {
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                if (cost_table[i][j] != -1)
                {
                    // optimize cost of ith router table with cost of jth
                    int cost_to_j = Rout_Tables[i].cost[j];
                    for (int z = 1; z <= n; z++)
                    {
                        // cost to z from j : cost_to_z
                        int cost_to_z_from_j = Rout_Tables[j].cost[z];
                        if (cost_to_z_from_j != INT_MAX)
                        {
                            int cost_to_z_from_i = cost_to_z_from_j + cost_to_j;
                            if (cost_to_z_from_i < Rout_Tables[i].cost[z])
                            {
                                Rout_Tables[i].cost[z] = cost_to_z_from_i;
                                Rout_Tables[i].next_hop[z] = j;
                            }
                        }
                    }
                }
            }
        }
    }
    for (int i = 1; i <= n; i++)
    {
        display(Rout_Tables[i], i);
    }
    return;
}

int main()
{
    printf("Enter number of nodes and edges:\n");
    scanf("%d%d", &n, &k);
    printf("Enter nodes and cost\n");
    int cost_table[n + 1][n + 1];
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            cost_table[i][j] = -1;
            if (i == j)
                cost_table[i][j] = 0;
        }
    }

    for (int i = 0; i < k; i++)
    {
        int a, b, cost;
        scanf("%d%d%d", &a, &b, &cost);
        cost_table[a][b] = cost;
        cost_table[b][a] = cost;
    }

    dvr(cost_table);
    return 0;
}