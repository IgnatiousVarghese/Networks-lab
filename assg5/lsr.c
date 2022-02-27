#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define NUM 20

int n, k;
struct Node
{
    int cost;
    int path[NUM];
    int path_len;
};
struct Rout_Tab
{
    struct Node nodes[NUM];
    int costs[NUM];
};

void display(struct Rout_Tab table)
{
    printf("Routing table is:\ndest  cost  path\n");
    for(int i = 1; i<=n; i++)
    {
        printf("%d    %d\t  ", i, table.nodes[i].cost);
        for(int j = 0; j< table.nodes[i].path_len; j++)
        {
            if(table.nodes[i].path[j] == -1)
                break;
            printf("%d->", table.nodes[i].path[j]);
        }
        printf("\n");
    }
}
void arr_copy(int arr[], int Arr[]){
    for(int i = 0; i<NUM; i++)
    {
        arr[i] = Arr[i];
    }
}
void lsr(int cost_table[n + 1][n + 1])
{
    struct Rout_Tab Rout_Tables[n + 1];
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            Rout_Tables[i].nodes[j].cost = INT_MAX;
            Rout_Tables[i].nodes[j].path[0] = i;
            Rout_Tables[i].nodes[j].path_len = 1;

            if (cost_table[i][j] != -1)
            {
                Rout_Tables[i].nodes[j].cost = cost_table[i][j];
                if (i != j)
                {
                    Rout_Tables[i].nodes[j].path[1] = j;
                    Rout_Tables[i].nodes[j].path_len = 2;
                }
        
            }
            Rout_Tables[i].costs[j] = Rout_Tables[i].nodes[j].cost;
            if(i == j) 
                Rout_Tables[i].costs[j] = -1;
        }
    }
    
    for(int i = 1; i<=n; i++)
    {
        while(1)
        {
            int min_cost = INT_MAX, x = -1;
            for(int j = 1; j<= n; j++)
            {
                int cost = Rout_Tables[i].costs[j];
                if(cost != -1 && cost < min_cost)
                {
                    min_cost = cost;
                    x = j;
                }
            }
            if(x == -1)
                break;

            int j = x;
            int len = Rout_Tables[i].nodes[j].path_len;

            for(int z = 1; z <= n; z++)
            {
                int cost_z = Rout_Tables[i].nodes[z].cost;
                if(cost_table[j][z] != -1 && j != z)
                {
                    int cost_z_via_j = Rout_Tables[i].nodes[j].cost + cost_table[j][z];
                    if(cost_z_via_j < cost_z)
                    {
                        Rout_Tables[i].nodes[z].cost = cost_z_via_j;
                        Rout_Tables[i].costs[z] = cost_z_via_j;
                        Rout_Tables[i].nodes[z].path_len = len+1;
                        arr_copy(Rout_Tables[i].nodes[z].path, Rout_Tables[i].nodes[j].path);
                        Rout_Tables[i].nodes[z].path[len] = z;
                    }
                }
            }

            Rout_Tables[i].costs[j] = -1;
        }
    }
    for(int i = 1; i <= n; i++)
    {
        printf("Routing table for node %d :", i);
        display(Rout_Tables[i]);
    }
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

    lsr(cost_table);
    return 0;
}