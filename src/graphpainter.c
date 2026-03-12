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
#include <stdio.h>

// visuais
static const Color bg_color = (Color){0,0,0,255};
static const Color fg_color = (Color){255, 255, 255, 255};

// largura e altura da tela
static int w;
static int h;

#define SIZE 400

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
        colors[i].r = random_char();
        colors[i].g = random_char();
        colors[i].b = random_char();
        colors[i].a = 255;
    }
}

static void draw_vertices(int n, Vector2 vertices_pos[n]) {
    #define VERTEX_RADIUS 5//px

    for(int i = 0; i < n; i++) {
        int x = (int)(vertices_pos[i].x  * w);
        int y = (int)(vertices_pos[i].y  * h);
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
            {(int)(vtx_pos[vtx[0]].x * w),
            (int)(vtx_pos[vtx[0]].y * h)},
            (Vector2)
            {(int)(vtx_pos[vtx[1]].x * w),
            (int)(vtx_pos[vtx[1]].y * h)},
            LINE_THICKNESS,
            colors[edge_colors[i]]
        );
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
    init_vertices_pos(n, vertices_pos);

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
