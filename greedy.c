#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int (*edge)[3];
    int k;
    int e;
    int v;
} graph;

typedef struct {
    int (*edge)[3];
    int cost;
    int e;
} res;

res modifiedPrimsAlgo(int V, int E, int k, int edge[E][3]){
    int visited[V];
    for (int i = 0; i < V; i++) visited[i] = 0;

    // start from first edge
    int start = edge[0][0];
    visited[start] = 1;

    int nodesUsed = 1;
    int edgesUsed = 0;
    int minCost = 0;

    int (*edges)[3] = malloc((k - 1) * sizeof *edges);

    while (nodesUsed < k) {
        int bestEdge = -1;
        int bestWeight = 1e9;

        for (int i = 0; i < E; i++) {
            int u = edge[i][0];
            int v = edge[i][1];
            int w = edge[i][2];

            // pick edge that connects tree to outside
            if ((visited[u] && !visited[v]) || (!visited[u] && visited[v])) {
                if (w < bestWeight) {
                    bestWeight = w;
                    bestEdge = i;
                }
            }
        }

        if (bestEdge == -1) break; // no connection possible

        int u = edge[bestEdge][0];
        int v = edge[bestEdge][1];
        int w = edge[bestEdge][2];

        edges[edgesUsed][0] = u;
        edges[edgesUsed][1] = v;
        edges[edgesUsed][2] = w;

        minCost += w;
        edgesUsed++;

        if (!visited[u]) {
            visited[u] = 1;
            nodesUsed++;
        }
        if (!visited[v]) {
            visited[v] = 1;
            nodesUsed++;
        }
    }

    res r;
    r.edge = edges;
    r.cost = minCost;
    r.e = edgesUsed;

    return r;
}

graph readFile(){
    FILE *fp;
    int v, e, k;
    
    fp = fopen("input.txt", "r");
    if(fp == NULL){
        printf("Error opening file.\n");
        exit(1);
    }
    
    fscanf(fp, "%d %d %d", &v, &e, &k);

    int (*edge)[3] = malloc(e * sizeof *edge);

    for (int i = 0; i < e; i++) {
        if (fscanf(fp, "%d %d %d", &edge[i][0], &edge[i][1], &edge[i][2]) != 3) {
            printf("Error: not enough edge data at line %d\n", i);
            exit(1);
        }
    }

    graph g;
    g.edge = edge;
    g.k = k;
    g.e = e;
    g.v = v;
    return g;
}

int *getVertices(int n, int edge[n][3]){
    int done[100] = {0}; // assuming vertices are less than 100
    
    int *vertices = malloc((n) * sizeof(int));
    int idx = 0;
    for(int i = 0; i < n; i++){
        for (int j = 0; j < 2; j++){
            if (!done[edge[i][j]]){ // if vertex has not been added to the list
                done[edge[i][j]] = 1; // mark as added
                vertices[idx++] = edge[i][j];
            }
        }
    }
    return vertices;
}

int main(){
    graph g = readFile();

    for (int i = 0; i < g.e; i++) {
        printf("%d %d %d\n", g.edge[i][0], g.edge[i][1], g.edge[i][2]);
    }

    clock_t start_time = clock();

    res r = modifiedPrimsAlgo(g.v, g.e, g.k, g.edge);

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Selected Edges:\n");
    for (int i = 0; i < r.e; i++) {
        printf("%d %d %d\n", r.edge[i][0], r.edge[i][1], r.edge[i][2]);
    }

    printf("Minimum Weight (for at least k=%d): %d\n", g.k, r.cost);
    printf("Tree Size Found: %d nodes\n", r.e + 1); // + 1 since edges = v - 1
    printf("Vertices in the tree: ");
    int *vertices = getVertices(r.e + 1, r.edge);
    for (int i = 0; i <= r.e; i++) {
        printf("%d ", vertices[i]);
    }
    printf("\nTotal execution time: %f seconds\n", cpu_time_used);

    free(vertices);
    free(g.edge);
    free(r.edge);

    return 0;
}