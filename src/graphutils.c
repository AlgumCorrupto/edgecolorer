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

// não utilizado
static inline int clamp(int min, int max, int val) {
   if(val > max) return max;
   if(val < min) return min;
   return val;
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
typedef struct {
    int len;
    int capacity;
} QueueHeader;

static void enqueue(QueueHeader* h, int queue[], int val) {
    if (h->len == h->capacity) return;

    for (int i = h->len - 1; i >= 0; i--) {
        queue[i + 1] = queue[i];
    }
    queue[0] = val;
    h->len++;
}

static int dequeue(QueueHeader* h, int queue[]) {
    int val = queue[h->len - 1];
    h->len--;
    return val;
}

// algoritmo guloso para coloração
// out = cor das arestas
static void color(int p, int m, char adj[m][m], int out[m], int* chromatic_index) {
    int adj_colors[m];
    for(int i = 0; i < m; i++)
        adj_colors[i] = -1;

    // pegando as cores de todos os nós adjacentes
    for(int i = 0; i < m; i++) {
        if(adj[p][i] && out[i] != -1) {
            push(m, adj_colors, out[i]);
        }
    }

    // iterando sobre as todas as cores já utilizadas
    // caso ela não for adjacente: atribui-la para o nó atual
    // caso todas forem adjacentes, incrementar o índice cromático
    for(int col = 0; col < m; col++) {
        if(!in(m, adj_colors, col)) {
            out[p] = col;
    
            if(col > *chromatic_index)
                *chromatic_index = col;
    
            return;
        }
    }
}

// basicamente um bfs modificado que percorre o grafo 
// e o coloriza.
// Me dá resultados ruins com grafos completos
// out = cor das arestas
int color_edges(int m, char adj[m][m], int out[m]) {
    // inicializando o vetor de saída
    for(int i=0;i<m;i++)
        out[i] = -1;

    int visited[m];
    memset(visited, 0, sizeof(visited));
    int start;
    int chromatic_index = 0;

    // enquanto houver subgrafos disconexos
    // não visitados
    while((start = search(m, visited, 0)) != -1) {
        QueueHeader queue_header = {
            0, // tamanho
            m  // capacidade
        };

        int queue[queue_header.capacity];

        visited[start] = 1;
        enqueue(&queue_header, queue, start);

        // fazer uma travessia BFS nele
        while (queue_header.len != 0) {
            int p = dequeue(&queue_header, queue);
            color(p, m, adj, out, &chromatic_index); // e colorir o nó (aresta) atual

            for (int i = 0; i < m; i++) {
                if (!visited[i] && adj[p][i]) {
                    visited[i] = 1;
                    enqueue(&queue_header, queue, i);
                }
            }
        }
    }
    return ++chromatic_index;
}
