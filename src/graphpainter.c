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

static void init_vertices_pos(int n, Vector2 vertices_pos[n]) {
    for(int i = 0; i < n; i++)
        vertices_pos[i] = (Vector2){random_float(), random_float()};
}

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

void init_vertex_pos(int n, int m, char incidence[n][m], Vector2 vertices_pos[n]) {
    #define MAX_ITERATION 100
    #define DAMPING 0.9f
    #define STIFFNESS 0.01f
    #define REPULSION 0.5f
    #define EDGE_LENGTH 1.0f

    Vector2 velocities[n];
    memset(vertices_pos, 0, sizeof(Vector2) * n);
    memset(velocities, 0, sizeof(velocities));

    // Initialize positions randomly in [-0.5, 0.5]
    for (int i = 0; i < n; i++) {
        vertices_pos[i].x = ((float)rand() / RAND_MAX) - 0.5f;
        vertices_pos[i].y = ((float)rand() / RAND_MAX) - 0.5f;
    }

    // --- Force-directed iterations ---
    for (int iter = 0; iter < MAX_ITERATION; iter++) {
        Vector2 forces[n];
        memset(forces, 0, sizeof(forces));

        // Repulsive forces
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

        // Attractive forces along edges
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

        // Update velocities and positions
        for (int v = 0; v < n; v++) {
            velocities[v].x = (velocities[v].x + forces[v].x) * DAMPING;
            velocities[v].y = (velocities[v].y + forces[v].y) * DAMPING;

            vertices_pos[v].x += velocities[v].x;
            vertices_pos[v].y += velocities[v].y;
        }
    }

    // --- Compute bounding box ---
    float min_x = vertices_pos[0].x, max_x = vertices_pos[0].x;
    float min_y = vertices_pos[0].y, max_y = vertices_pos[0].y;
    for (int i = 1; i < n; i++) {
        if (vertices_pos[i].x < min_x) min_x = vertices_pos[i].x;
        if (vertices_pos[i].x > max_x) max_x = vertices_pos[i].x;
        if (vertices_pos[i].y < min_y) min_y = vertices_pos[i].y;
        if (vertices_pos[i].y > max_y) max_y = vertices_pos[i].y;
    }

    float scale_x = max_x - min_x;
    float scale_y = max_y - min_y;

    float target_min = -1.0f + PADDING;
    float target_max =  1.0f - PADDING;

    float target_size = target_max - target_min;

    // --- Normalize to [-1 + padding, 1 - padding] ---
    for (int i = 0; i < n; i++) {
        vertices_pos[i].x = target_min + ((vertices_pos[i].x - min_x) / scale_x) * target_size;
        vertices_pos[i].y = target_min + ((vertices_pos[i].y - min_y) / scale_y) * target_size;
    }
}


void draw_graph(int n, int m, char incidence[n][m], int edge_colors[m], int chroma_index) {
    // inicializando o raylib
    SetTraceLogLevel(LOG_NONE); // calando a boca do raylib
    SetConfigFlags(FLAG_MSAA_4X_HINT); // anti aliasing
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // auto explicativo
    InitWindow(SIZE * (16/9), SIZE, "Edger");
    SetTargetFPS(60);

    // importante para init_node_pos
    // e init_color
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
