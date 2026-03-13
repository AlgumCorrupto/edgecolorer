// aqui é definido como
// o grafo deve ser desenhado,
// se está procurando para
// a implementação do algorítmo
// de coloração olhe o arquivo
// ./graphutils.c

#include <graphpainter.h>
#include <raylib/raylib.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// visuais
static const Color bg_color = (Color){0,0,0,255};
static const Color fg_color = (Color){255, 255, 255, 255};

// largura e altura da tela
static int w;
static int h;

#define SIZE 400
#define PADDING 0.25

inline static float random_float() {
    return (float)rand() / (float) RAND_MAX;
}

inline static unsigned char random_char() {
    return (unsigned char)rand();
}

// https://youtu.be/Ed3GLO90FVU?si=mDrWF7VAzKJ3PIco
static void init_color(int chroma_index, Color colors[chroma_index]) {
    for(int i = 0; i < chroma_index; i++) {
        float hue = i * (360 / chroma_index);
        colors[i] = ColorFromHSV(hue, 1.0, 1.0);
    }
}

static void draw_vertices(int n, Vector2 vertices_pos[n]) {
    #define VERTEX_RADIUS 5//px

    for(int i = 0; i < n; i++) {
        int x = (int)((vertices_pos[i].x + 1.0) * .5  * w);
        int y = (int)((vertices_pos[i].y + 1.0) * .5 * h);
        DrawCircle(x, y, VERTEX_RADIUS, fg_color);
    }
}

static void draw_edges(
    int n,
    int m,
    int chroma_index,
    char incidence[n][m],
    int edge_colors[m],
    Color colors[chroma_index],
    Vector2 vtx_pos[n]
) {

    #define LINE_THICKNESS 3//px

    for(int i = 0; i < m; i++) {
        int vtx[2] = {-1, -1};
        int incidentes = 0;

        for(int j = 0; j < n; j++) {
            if(incidence[j][i]) {
                vtx[incidentes++] = j;
                if(incidentes == 2) break;
            }
        }

        if(incidentes != 2)
            continue;

        DrawLineEx(
            (Vector2)
            {(int)((vtx_pos[vtx[0]].x + 1.0) * .5 * w),
             (int)((vtx_pos[vtx[0]].y + 1.0) * .5 * h)},
            (Vector2)
            {(int)((vtx_pos[vtx[1]].x + 1.0) * .5 * w),
             (int)((vtx_pos[vtx[1]].y + 1.0) * .5 * h)},
            LINE_THICKNESS,
            colors[edge_colors[i]]
        );
    }
}

// computar bounding box das vértices
// NOTE: eu sei que BB tem 3 valores XYZ
// Mas não estou utilizando o Z, então praq
// inicializar?
static BoundingBox compute_bounding_box(int n, Vector2 vertices[n]) {
    BoundingBox box;
    box.min.x = box.max.x = vertices[0].x;
    box.min.y = box.max.y = vertices[0].y;

    for (int i = 1; i < n; i++) {
        if (vertices[i].x < box.min.x) box.min.x = vertices[i].x;
        if (vertices[i].x > box.max.x) box.max.x = vertices[i].x;
        if (vertices[i].y < box.min.y) box.min.y = vertices[i].y;
        if (vertices[i].y > box.max.y) box.max.y = vertices[i].y;
    }
    return box;
}

// Normalizar um conjunto de coordenadas
// para dentro de uma BoundingBox
static void normalize_vertices(int n, Vector2 vertices[n], BoundingBox box, float target_min, float target_max) {
    float scale_x = box.max.x - box.min.x;
    float scale_y = box.max.y - box.min.y;
    float target_size = target_max - target_min;

    for (int i = 0; i < n; i++) {
        vertices[i].x = target_min + ((vertices[i].x - box.min.x) / scale_x) * target_size;
        vertices[i].y = target_min + ((vertices[i].y - box.min.y) / scale_y) * target_size;
    }
}

// Layout baseado nas leis de hooke
// https://en.wikipedia.org/wiki/Force-directed_graph_drawing
static void init_vertex_pos(int n, int m, char incidence[n][m], Vector2 vertices_pos[n]) {
    #define MAX_ITERATION 10000
    #define DAMPING 0.9f
    #define STIFFNESS 0.01f
    #define REPULSION 0.5f
    #define EDGE_LENGTH 1.0f

    Vector2 velocities[n];
    memset(vertices_pos, 0, sizeof(Vector2) * n);
    memset(velocities, 0, sizeof(velocities));

    // inicializar as posições dos vetores aleatóriamente
    // entre [-.5, .5]
    for (int i = 0; i < n; i++) {
        vertices_pos[i].x = ((float)rand() / RAND_MAX) - 0.5f;
        vertices_pos[i].y = ((float)rand() / RAND_MAX) - 0.5f;
    }

    // fazendo o sistema de simulação
    for (int iter = 0; iter < MAX_ITERATION; iter++) {
        Vector2 forces[n];
        memset(forces, 0, sizeof(forces));

        // para cada vértice
        // aplicar uma força que
        // repele os outros vértices
        for (int a = 0; a < n; a++) {
            for (int b = a + 1; b < n; b++) {
                float dx = vertices_pos[b].x - vertices_pos[a].x;
                float dy = vertices_pos[b].y - vertices_pos[a].y;
                float dist2 = dx*dx + dy*dy + 0.0001f;
                float f = REPULSION / dist2;

                forces[a].x -= f * dx;
                forces[a].y -= f * dy;
                forces[b].x += f * dx;
                forces[b].y += f * dy;
            }
        }

        // para cada aresta
        // "puxar" seus vértices
        // incidentes para mais perto.
        // As arestas basicamente
        // funcionam como uma suspensão
        for (int e = 0; e < m; e++) {
            int v1 = -1, v2 = -1;
            for (int v = 0; v < n; v++) {
                if (incidence[v][e]) {
                    if (v1 == -1) v1 = v;
                    else v2 = v;
                }
            }
            if (v1 != -1 && v2 != -1) {
                float dx = vertices_pos[v2].x - vertices_pos[v1].x;
                float dy = vertices_pos[v2].y - vertices_pos[v1].y;
                float dist = sqrtf(dx*dx + dy*dy) + 0.0001f;
                float f = STIFFNESS * (dist - EDGE_LENGTH);

                float fx = f * dx / dist;
                float fy = f * dy / dist;

                forces[v1].x += fx;
                forces[v1].y += fy;
                forces[v2].x -= fx;
                forces[v2].y -= fy;
            }
        }

        // finalmente integrar a posição
        for (int v = 0; v < n; v++) {
            velocities[v].x = (velocities[v].x + forces[v].x) * DAMPING;
            velocities[v].y = (velocities[v].y + forces[v].y) * DAMPING;

            vertices_pos[v].x += velocities[v].x;
            vertices_pos[v].y += velocities[v].y;
        }
    }

    // colocar todas as vértices na mesma viewport
    BoundingBox box = compute_bounding_box(n, vertices_pos);
    float target_min = -1.0f + PADDING;
    float target_max = 1.0f - PADDING;
    normalize_vertices(n, vertices_pos, box, target_min, target_max);
}

void draw_graph(int n, int m, char incidence[n][m], int edge_colors[m], int chroma_index) {
    // inicializando o raylib
    SetTraceLogLevel(LOG_NONE); // calando a boca do raylib
    SetConfigFlags(FLAG_MSAA_4X_HINT); // anti aliasing
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // janela pode mudar de tamanho
    InitWindow(SIZE * (16/9), SIZE, "Edger"); // aspect ratio 16:9 por p'':adrão
    SetTargetFPS(60);

    // importante para init_vertex_pos
    srand(time(NULL));

    Vector2 vertices_pos[n];
    init_vertex_pos(n, m, incidence, vertices_pos);

    Color colors[chroma_index];
    init_color(chroma_index, colors);

    char chroma_index_txt[32];
    sprintf(chroma_index_txt, "Índice Cromático: %d", chroma_index);
    Font font = GetFontDefault();

    while(!WindowShouldClose()) {
        PollInputEvents();
        h = GetScreenHeight();
        w = GetScreenWidth();

        // bem auto explicativo
        BeginDrawing();
            ClearBackground(bg_color);
            draw_edges(n, m, chroma_index, incidence, edge_colors, colors, vertices_pos);
            draw_vertices(n, vertices_pos);
            DrawTextEx(font, chroma_index_txt, (Vector2){10, 10}, 32, 2, fg_color);
        EndDrawing();

        SwapScreenBuffer();
    }

    CloseWindow();
}
