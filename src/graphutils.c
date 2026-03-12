#include <graphutils.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// ler a primeira linha (header)
// do arquivo de entrada
GraphHeader read_header(char* filename) {
   FILE* f = fopen(filename, "r");
   int n, m;
   fscanf(f, "%d %d", &n, &m);
   fclose(f);
   return (GraphHeader){n, m};
}

// ler a matriz de adjacência dos vértices do arquivo de entrada
// out é o parâmetro de saída (matriz de adjacência).
void read_vertex_adj(char* filename, int n, char out[n][n]) {
    FILE* f = fopen(filename, "r");

    int tmp_n, tmp_m;
    fscanf(f, "%d %d", &tmp_n, &tmp_m); // pulando o header

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(f, "%hhd", &out[i][j]); // lendo um número de 8 bits
        }
    }

    fclose(f);
}

// como a keyword in do python, verifica
// a existência de certo valor num vetor
static inline char in(int len, int vec[len], int value) {
   for(int i = 0; i < len; i++) {
      if(vec[i] ==  value) return 1;
   }
   return 0;
}

// caso o valor for achado, retorna seu índice no vetor
// caso contrário retorna -1
static inline int search(int len, int vec[len], int value) {
   for(int i = 0; i < len; i++) {
      if(vec[i] ==  value) return i;
   }
   return -1;
}

// dado um vetor em que nil é representado como -1
// essa função insere um valor como se fosse
// uma pilha
static inline char push(int len, int vec[len], int value) {
   for(int i  = 0; i < len; i++) 
      if(vec[i] == -1) {
         vec[i] =  value;
         return 1;
      }
   return 0;
}

static void transpose(int rows, int columns, char input[rows][columns], char out[columns][rows]) {
   for(int i = 0; i < rows; i++)
       for(int j = 0; j < columns; j++)
         out[j][i] = input[i][j]; // auto explicativo
}

// r = row
// c = column
// mat = matrix
// res = saída
void multiply_matrix(int r1, int c1, char mat1[r1][c1], int r2, int c2, char mat2[r2][c2], char res[r1][c2]) {
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            res[i][j] = 0;
            for (int k = 0; k < c1; k++)
                res[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}

// input = matriz de adjacência (vértice)
// out = matriz de incidência
void make_incidence(int n, int m, char input[n][n], char out[n][m]) {
    // para cada vértice v_1
        // testar as outras vértices v_2 com índice maior
        // que v_1 (utiliando a propriedade simétrica) da matriz de adjacência.
        // caso esteja conectado
            // criar uma aresta
    int edge_i = 0;

    // removendo lixo da saída
    memset(out, 0, n * m * sizeof(char));

    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            if(input[i][j]) {
                out[i][edge_i] = 1;
                out[j][edge_i] = 1;
                edge_i++;
            }
        }
    }
}

// input = matriz de incidência
// out =  matriz de adjacência (aresta)
void make_edge_adj_from_inc(int n, int m, char input[n][m], char out[m][m]) {
    // passo 1: multiplicar a transposta pela original
    char input_transposed[m][n];
    transpose(n, m, input, input_transposed);
    multiply_matrix(m, n, input_transposed, n, m, input, out);

    // passo 2:
    // remover auto adjências
    // e colocar '1' uma adjacência nos adjacentes
    for(int i = 0; i < m; i++) {
       for(int j = 0; j < m; j++) {
           if(i == j)
               out[i][j] = 0;
           else
               out[i][j] = out[i][j] > 0;
       }
    }
}

// uma implementação primitiva de uma
// fila
//typedef struct {
//    int len;
//    int capacity;
//} QueueHeader;

//static void enqueue(QueueHeader* h, int queue[], int val) {
//    if (h->len == h->capacity) return;
//
//    for (int i = h->len - 1; i >= 0; i--) {
//        queue[i + 1] = queue[i];
//    }
//    queue[0] = val;
//    h->len++;
//}
//
//static int dequeue(QueueHeader* h, int queue[]) {
//    int val = queue[h->len - 1];
//    h->len--;
//    return val;
//}

// algoritmo guloso para coloração
// out = cor das arestas
//static void color(int p, int m, char adj[m][m], int out[m], int* chromatic_index) {
//    int adj_colors[m];
//    for(int i = 0; i < m; i++)
//        adj_colors[i] = -1;
//
//    // pegando as cores de todos os nós adjacentes
//    for(int i = 0; i < m; i++) {
//        if(adj[p][i] && out[i] != -1) {
//            push(m, adj_colors, out[i]);
//        }
//    }
//
//    // iterando sobre as todas as cores já utilizadas
//    // caso ela não for adjacente: atribui-la para o nó atual
//    // caso todas forem adjacentes, incrementar o índice cromático
//    for(int col = 0; col < m; col++) {
//        if(!in(m, adj_colors, col)) {
//            out[p] = col;
//
//            if(col > *chromatic_index)
//                *chromatic_index = col;
//
//            return;
//        }
//    }
//}

int get_max_degree(int n, int m, char incidence[n][m]) {
    int max  = 0;
    for(int i = 0; i< n; i++) {
        int curr = 0;
        for(int j = 0; j < m; j++) {
            curr += incidence[i][j];
        }
        if(curr > max) max = curr;
    }
    return max;
}

int count_unique(int m, int colors[m]) {
    int buffer[m];
    for(int i=0; i < m; i++) buffer[i] = -1;
    int qtd = 0;
    for(int i = 0; i < m; i++) {
        if(colors[i] != -1 && !in(m, buffer, colors[i])) {
            push(m, buffer, colors[i]);
            qtd++;
        }
    }
    return qtd;
}

int least_common(int m, int colors[m]) {
    int counts[m];

    for (int i = 0; i < m; i++)
        counts[i] = 0;

    for (int i = 0; i < m; i++) {
        if (colors[i] != -1)
            counts[colors[i]]++;
    }

    int min_color = -1;
    int min_count = 0;

    for (int c = 0; c < m; c++) {
        if (counts[c] > 0 && (min_color == -1 || counts[c] < min_count)) {
            min_color = c;
            min_count = counts[c];
        }
    }

    return min_color;
}


typedef struct {
    int n;
    int m;
} PartitionGraph;

typedef struct {
    PartitionGraph g1;
    PartitionGraph g2;
} PartitionHeader;

void compute_degrees(int n, int m, char incidence[n][m], int degrees[n]) {
    memset(degrees, 0, sizeof(int) * n);
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) { 
            degrees[i] += incidence[i][j];
        }
    }
}

void build_subgraphs(int n, int m, char G[n][m], 
                     char edge_subgraph[m],
                     int n1, int m1, char G1[n1][m1],
                     int n2, int m2, char G2[n2][m2]) {
    
    memset(G1, 0, sizeof(char) * n1 * m1);
    memset(G2, 0, sizeof(char) * n2 * m2);

    // Track where to put next edge in each subgraph
    int next_edge_in_G1 = 0;
    int next_edge_in_G2 = 0;
    
    // Distribute edges to subgraphs
    for (int edge = 0; edge < m; edge++) {
        if (edge_subgraph[edge] == 0) {
            // This edge goes to G1
            for (int vertex = 0; vertex < n; vertex++) {
                G1[vertex][next_edge_in_G1] = G[vertex][edge];
            }
            next_edge_in_G1++;
        } else {
            // This edge goes to G2
            for (int vertex = 0; vertex < n; vertex++) {
                G2[vertex][next_edge_in_G2] = G[vertex][edge];
            }
            next_edge_in_G2++;
        }
    }
    
    // Note: We assume m1 = ceil(m/2) and m2 = floor(m/2) from the caller
    // The loops above will fill exactly m1 edges in G1 and m2 edges in G2
}

int find_endpoint_with_odd_degree(int n, int m, char G[n][m], int degree[n], int edge) {
    // Find the two endpoints of this edge
    int endpoints[2];
    int endpoint_count = 0;
    
    for (int vertex = 0; vertex < n; vertex++) {
        if (G[vertex][edge] == 1) {
            endpoints[endpoint_count] = vertex;
            endpoint_count++;
            if (endpoint_count == 2) break;
        }
    }
    
    // Check which endpoint has odd degree
    // The algorithm states: "every odd-degree vertex is the endpoint of exactly one tour"
    // So we want to start tours at odd-degree vertices
    if (degree[endpoints[0]] % 2 == 1) {
        return endpoints[0];
    } else if (degree[endpoints[1]] % 2 == 1) {
        return endpoints[1];
    } else {
        // If both endpoints have even degree, we can start from either
        // But according to the algorithm, no even-degree vertex should be an endpoint
        // So this case shouldn't occur if we're starting a new tour correctly
        return endpoints[0];  // Default to first endpoint
    }
}

int find_next_edge_in_tour(int n, int m, char G[n][m], char edge_used[m], int current_edge) {
    // Find the two endpoints of the current edge
    int endpoints[2];
    int endpoint_count = 0;

    for (int vertex = 0; vertex < n; vertex++) {
        if (G[vertex][current_edge] == 1) {
            endpoints[endpoint_count] = vertex;
            endpoint_count++;
            if (endpoint_count == 2) break;
        }
    }

    // Try to find an unused edge incident to either endpoint
    // We need to track which vertex we came from, but since we don't have that info,
    // we'll need to be careful in the main algorithm to pass the "from vertex"

    // For now, let's assume we're at the first endpoint and look for edges there
    for (int vertex_idx = 0; vertex_idx < 2; vertex_idx++) {
        int current_vertex = endpoints[vertex_idx];

        // Look for any unused edge incident to this vertex that isn't the current edge
        for (int edge = 0; edge < m; edge++) {
            if (!edge_used[edge] && edge != current_edge && G[current_vertex][edge] == 1) {
                return edge;
            }
        }
    }

    return -1;  // No next edge found (tour is complete)
}

void partition(int n, int m, char G[n][m],
               int n1, int m1, char G1[n1][m1],
               int n2, int m2, char G2[n2][m2]) {

    // Track which edges have been used
    char edge_used[m];
    memset(edge_used, 0, sizeof(edge_used));

    // Track vertex degrees (compute from incidence matrix)
    int degree[n];
    compute_degrees(n, m, G, degree);

    // Track which subgraph each edge goes to (0 for G1, 1 for G2)
    char edge_subgraph[m];

    int next_subgraph = 0;  // Start with subgraph 0

    // Find tours greedily
    for (int i = 0; i < m; i++) {
        if (!edge_used[i]) {
            // Start a new tour from this unused edge
            int current_edge = i;
            int start_vertex = find_endpoint_with_odd_degree(n, m, G, degree, current_edge);

            // Traverse the tour
            do {
                edge_used[current_edge] = 1;
                edge_subgraph[current_edge] = next_subgraph;
                next_subgraph = !next_subgraph;  // Alternate assignment

                // Find next edge in tour (through shared vertex)
                current_edge = find_next_edge_in_tour(n, m, G, edge_used, current_edge);
            } while (current_edge != -1 && !edge_used[current_edge]);
        }
    }

    // Build G1 and G2 from edge_subgraph assignments
    build_subgraphs(n, m, G, edge_subgraph, n1, m1, G1, n2, m2, G2);
}

void merge(
    int m,
    int out[m],
    int m1, int out1[m1],
    int m2, int out2[m2]
) {
    int e1 = 0;
    int e2 = 0;

    for (int e = 0; e < m; e++) {
        if (e % 2 == 0)
            out[e] = out1[e1++];
        else
            out[e] = out2[e2++];
    }
}

PartitionHeader partition_header(int n, int m, char G[n][m]) {
    PartitionGraph g1 = {n, (m + 1) / 2};
    PartitionGraph g2 = {n, m / 2};

    return (PartitionHeader){g1, g2};
}

int color_used_at_vertex(int v, int color,
                         int n, int m,
                         char G[n][m],
                         int out[m])
{
    for (int e = 0; e < m; e++)
        if (G[v][e] && out[e] == color)
            return 1;

    return 0;
}

int find_free_color(int v, int k,
                    int n, int m,
                    char G[n][m],
                    int out[m])
{
    for (int c = 0; c < k; c++)
        if (!color_used_at_vertex(v, c, n, m, G, out))
            return c;

    return -1;
}

// tenta colorir grafo com o índice cromático informado
int try_edge_coloring(
    int n, int m,
    int chromatic_index,
    int base_color,
    char G[n][m],
    int out[m]) {
    // caso base:
    // Se d <= 1, colorir
    // cada aresta pela mesma cor e retornar
    int max_degree = get_max_degree(n, m, G);
    if (max_degree <= 1) {
        for(int i = 0; i < m; i++) 
            out[i] = base_color;
        return 0;
    }

    // decompor G em subgrafos G_1 e G_2
    PartitionHeader p_header = partition_header(n, m, G);
    char G_1[p_header.g1.n][p_header.g1.m];
    int out_1[p_header.g1.m];
    for(int i = 0; i < p_header.g1.m; i++) out_1[i] = -1;
    char G_2[p_header.g2.n][p_header.g2.m];
    int out_2[p_header.g2.m];
    for(int i = 0; i < p_header.g2.m; i++) out_2[i] = -1;

    partition(n, m, G,
      p_header.g1.n, p_header.g1.m, G_1,
      p_header.g2.n, p_header.g2.m, G_2
    );

    // recursividade
    int sub_colors = (max_degree + 1) / 2 + 1;
    try_edge_coloring(p_header.g1.n, p_header.g1.m, sub_colors, base_color, G_1, out_1);
    try_edge_coloring(p_header.g2.n, p_header.g2.m, sub_colors, base_color + sub_colors, G_2, out_2);

    merge(
        m, out,
        p_header.g1.m, out_1,
        p_header.g2.m, out_2
    );

    // assegurar
    while(count_unique(m, out) > chromatic_index) {
        // escolher a cor menos comum
        int least_common_color = least_common(m, out);
        // descolorir todas as arestas
        // coloridas com essa cor
        for(int i = 0; i < m; i++)
            if(out[i] == least_common_color) out[i] = -1;
    }
    
    // reparar
    for (int e = 0; e < m; e++) {
    
        if (out[e] != -1)
            continue;
    
        int u = -1, v = -1;
    
        // find endpoints
        for (int i = 0; i < n; i++) {
            if (G[i][e]) {
                if (u == -1) u = i;
                else v = i;
            }
        }
    
        int found = 0;
    
        for (int c = base_color; c < base_color + chromatic_index; c++) {
    
            int used_u = 0;
            int used_v = 0;
    
            for (int f = 0; f < m; f++) {
                if (G[u][f] && out[f] == c) used_u = 1;
                if (G[v][f] && out[f] == c) used_v = 1;
    
                if (used_u && used_v)
                    break;
            }
    
            if (!used_u && !used_v) {
                out[e] = c;
                found = 1;
                break;
            }
        }
    
        if (!found)
            return 1;
    }
    return 0;
}



int edge_coloring(int n, int m, char incidence[n][m], int out[m]) {
    int d = get_max_degree(n, m, incidence);

    for (int k = d; k <= d + 1; k++) {
        for(int i = 0; i < m; i++) out[i] = -1;
        // assegurar que o K
        // é valido de acordo com o teorema de vizing
        assert(k >= d && k <= d + 1);
        if (!try_edge_coloring(n, m, k, 0, incidence, out))
            return k;
    }

    return -1; // should never happen for simple graphs
}

