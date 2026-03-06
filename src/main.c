#include <graphutils.h>
#include <graphpainter.h>
#include <string.h>

int main(int argc, char** argv) {
   // lendo o header primeiro
   char filename[67];
   memset(filename, 0, sizeof(filename));
   strcpy(filename, argv[1]);
   GraphHeader header = read_header(filename);

   // lendo a matriz de adjacencia dos vértices
   // do arquivo
   char vertex_adj[header.n][header.n];
   read_vertex_adj(filename, header.n, vertex_adj);

   // criando uma matrix de incidencia a partir da matrix
   // de adjacencia dos vértices
   char incidence[header.n][header.m];
   make_incidence(header.n, header.m, vertex_adj, incidence);

   // criando uma matrix de adjacência das arestas
   // a partir da matrix de incidencia
   char edge_adj[header.m][header.m];
   make_edge_adj_from_inc(header.n, header.m, incidence, edge_adj);

   // algoritmo principal de coloração
   int edge_colors[header.m];
   int chroma_index = color_edges(header.m, edge_adj, edge_colors);

   // finalmente desenhando o grafo com raylib
   draw_graph(header.n, header.m, incidence, edge_colors, chroma_index);

   return 0;
}
