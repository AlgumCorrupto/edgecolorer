#include <graphutils.h>
#include <stdio.h>
#include <string.h>


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


// retorna 1 se aresta "e" pode ser colorida
// com cor "c"
static int can_color_edge(int e, int m, char edge_adj[m][m], int edge_colors[m], int c) {
    for (int i = 0; i < m; i++) {
        if (edge_adj[e][i] && edge_colors[i] == c)
            return 0;
    }
    return 1;
}

// e = aresta que vai ser utilizada como ponto de entrada
static int backtrack_edge_coloring(int e, int m, int max_colors,
                                   char edge_adj[m][m], int edge_colors[m],
                                   int current_max, int *best_max_found,
                                   int best_coloring[m]) {
    if (e == m) {
        for (int i = 0; i < m; i++)
            best_coloring[i] = edge_colors[i];
        return current_max;
    }

    int best_max = -1;

    for (int c = 0; c < max_colors; c++) {
        if (can_color_edge(e, m, edge_adj, edge_colors, c)) {
            edge_colors[e] = c;
            int new_max = (c > current_max) ? c : current_max;

            if (*best_max_found != -1 && new_max >= *best_max_found) {
                edge_colors[e] = -1;
                continue;
            }

            int result = backtrack_edge_coloring(e + 1, m, max_colors,
                                                edge_adj, edge_colors,
                                                new_max, best_max_found,
                                                best_coloring);
            if (result != -1 && (best_max == -1 || result < best_max)) {
                best_max = result;
                *best_max_found = result;
            }

            edge_colors[e] = -1; // backtrack only for unsuccessful paths
        }
    }

    return best_max;
}

// API pública para coloração
int color_edges_backtracking(int m, char edge_adj[m][m], int edge_colors[m]) {
    int best_coloring[m];
    for (int i = 0; i < m; i++) edge_colors[i] = -1;

    // calculando pior caso
    // pegando o grau máximo
    // e incrementando mais 1
    int max_colors = 0;
    for (int i = 0; i < m; i++) {
        int deg = 0;
        for (int j = 0; j < m; j++) deg += edge_adj[i][j];
        if (deg > max_colors) max_colors = deg;
    }
    max_colors += 1;

    int best_max_found = -1;
    int chroma = backtrack_edge_coloring(0, m, max_colors, edge_adj, edge_colors,
                                         -1, &best_max_found, best_coloring);

    // copiar a melhor coloração para ser desenhado
    for (int i = 0; i < m; i++)
        edge_colors[i] = best_coloring[i];

    return chroma + 1;
}
