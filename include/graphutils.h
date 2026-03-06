#pragma once

typedef struct  {
   int n; // quantidade de vértices
   int m; // quantidade de arestas
}GraphHeader;

// ler o header do arquivo contendo matrix de adjacência
// da vértice
GraphHeader read_header(char* filename);

// ler a matrix de adjacência dos vértices do arquivo
void read_vertex_adj(char* filename, int n, char out[n][n]);

// in = matriz de adjacencia dos vértices
// out = matrix de incidência gerada
void make_incidence(int n, int m, char in[n][n], char out[n][m]);

// in = matrix de incidência
// out = matrix de adjacência das arestas gerada
void make_edge_adj_from_inc(int n, int m, char in[n][m], char out[m][m]);

int color_edges(int m, char edge_adj[m][m], int out_colors[m]);
