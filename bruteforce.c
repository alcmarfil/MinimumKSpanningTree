#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// structure to represent a graph edge
typedef struct {
  int src, dest, weight;
} Edge;

// structure to represent a subset for union-find data structure
typedef struct {
  int parent;
  int rank;
} Subset;

// find function with path compression to find the root of a vertex
int find(Subset subsets[], int i) {
  if (subsets[i].parent != i)
    subsets[i].parent = find(subsets, subsets[i].parent);
  return subsets[i].parent;
}

// union function by rank to merge two sets
void Union(Subset subsets[], int x, int y) {
  int xroot = find(subsets, x);
  int yroot = find(subsets, y);

  if (subsets[xroot].rank < subsets[yroot].rank)
    subsets[xroot].parent = yroot;
  else if (subsets[xroot].rank > subsets[yroot].rank)
    subsets[yroot].parent = xroot;
  else {
    subsets[yroot].parent = xroot;
    subsets[xroot].rank++;
  }
}

// comparator function for qsort to sort edges by weight in ascending order
int compareEdges(const void *a, const void *b) {
  Edge *a1 = (Edge *)a;
  Edge *b1 = (Edge *)b;
  return a1->weight - b1->weight;
}

long long min_weight = LLONG_MAX;
int *best_subset = NULL;
int best_subset_size = 0; // track the number of nodes in the best tree found

// core function to check if a subset of 'current_k' vertices forms a valid MST
void check_subset(int V, int E, Edge edges[], int current_k, int current_subset[],
                  Edge subset_edges[], int is_in_subset[], Subset subsets[]) {
  int edge_count = 0;

  memset(is_in_subset, 0, V * sizeof(int));
  for (int i = 0; i < current_k; i++) {
    is_in_subset[current_subset[i]] = 1;
  }

  for (int i = 0; i < E; i++) {
    if (is_in_subset[edges[i].src] && is_in_subset[edges[i].dest]) {
      subset_edges[edge_count++] = edges[i];
    }
  }

  // to be a tree of current_k nodes, we need current_k - 1 edges
  if (edge_count < current_k - 1) {
    return;
  }

  qsort(subset_edges, edge_count, sizeof(Edge), compareEdges);

  for (int i = 0; i < V; i++) {
    subsets[i].parent = i;
    subsets[i].rank = 0;
  }

  long long current_weight = 0;
  int edges_in_mst = 0;

  for (int i = 0; i < edge_count && edges_in_mst < current_k - 1; i++) {
    int x = find(subsets, subset_edges[i].src);
    int y = find(subsets, subset_edges[i].dest);

    if (x != y) {
      Union(subsets, x, y);
      current_weight += subset_edges[i].weight;
      edges_in_mst++;
    }
  }

  // if the subgraph is connected (edges_in_mst == current_k - 1)
  if (edges_in_mst == current_k - 1) {
    if (current_weight < min_weight) {
      min_weight = current_weight;
      best_subset_size = current_k;
      memcpy(best_subset, current_subset, current_k * sizeof(int));
    }
  }
}

// recursive function to generate all combinations of 'current_k' vertices
void generate_combinations(int V, int E, Edge edges[], int current_k,
                           int current_subset[], int start, int index,
                           Edge subset_edges[], int is_in_subset[],
                           Subset subsets[]) {
  if (index == current_k) {
    check_subset(V, E, edges, current_k, current_subset, subset_edges,
                 is_in_subset, subsets);
    return;
  }

  for (int i = start; i <= V - (current_k - index); i++) {
    current_subset[index] = i;
    generate_combinations(V, E, edges, current_k, current_subset, i + 1,
                          index + 1, subset_edges, is_in_subset, subsets);
  }
}

int main() {
  int V, E, k_min;
  // read input: total vertices, total edges, and the MINIMUM k required
  if (scanf("%d %d %d", &V, &E, &k_min) != 3) {
    fprintf(stderr, "Invalid input header. Expected V E k\n");
    return 1;
  }

  Edge *edges = malloc(E * sizeof(Edge));
  for (int i = 0; i < E; i++) {
    if (scanf("%d %d %d", &edges[i].src, &edges[i].dest, &edges[i].weight) !=
        3) {
      fprintf(stderr, "Invalid edge input at line %d\n", i + 2);
      return 1;
    }
  }

  best_subset = malloc(V * sizeof(int));
  int *current_subset = malloc(V * sizeof(int));
  Edge *subset_edges = malloc(E * sizeof(Edge));
  int *is_in_subset = malloc(V * sizeof(int));
  Subset *subsets = malloc(V * sizeof(Subset));

  clock_t start_time = clock();

  // FORMAL k-MST: Check all possible tree sizes from k_min up to V
  for (int current_k = k_min; current_k <= V; current_k++) {
    generate_combinations(V, E, edges, current_k, current_subset, 0, 0,
                          subset_edges, is_in_subset, subsets);
  }

  clock_t end_time = clock();
  double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

  if (min_weight == LLONG_MAX) {
    printf("No valid spanning tree of size >= %d found.\n", k_min);
  } else {
    printf("Minimum Weight (for at least k=%d): %lld\n", k_min, min_weight);
    printf("Tree Size Found: %d nodes\n", best_subset_size);
    printf("Vertices in the tree: ");
    for (int i = 0; i < best_subset_size; i++) {
      printf("%d ", best_subset[i]);
    }
    printf("\n");
    printf("Total execution time: %f seconds\n", cpu_time_used);
  }

  free(edges);
  free(best_subset);
  free(current_subset);
  free(subset_edges);
  free(is_in_subset);
  free(subsets);

  return 0;
}
