//  Copyright (c) 2018
//  Marc-Etienne M. Leveille <marc-etienne@nsec.io>
//
//  License: MIT (see LICENSE for details)

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ssd1306.h"

typedef struct {
    int rows;
    int cols;
    float values[];
} nsec_matrix_t;

typedef struct {
    float position[3];
} nsec_vertex_t;

typedef struct {
    unsigned int vertex_index[2];
} nsec_edge_t;

typedef struct {
    int vertex_count;
    int edge_count;
    nsec_vertex_t * vertices;
    nsec_edge_t * edges;
} nsec_mesh_t;

#define NSEC_DECLARE_MESH(name, vertices, edges) \
    const nsec_mesh_t name##_m = { \
        sizeof(vertices) / sizeof(nsec_vertex_t), \
        sizeof(edges) / sizeof(nsec_edge_t), \
        (vertices), \
        (edges) \
    }; \
    const nsec_mesh_t * name = &name##_m;

static const nsec_vertex_t nsec_cube_vertices[] = {
  {-1, -1, -1},
  {-1, -1,  1},
  {-1,  1, -1},
  {-1,  1,  1},
  {1,  -1, -1},
  {1,  -1,  1},
  {1,   1, -1},
  {1,   1,  1}
};

static const nsec_edge_t nsec_cube_edges[] = {
    {0, 1}, {1, 3}, {3, 2}, {2, 0},
    {4, 5}, {5, 7}, {7, 6}, {6, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

NSEC_DECLARE_MESH(nsec_cube, nsec_cube_vertices, nsec_cube_edges);

static const nsec_vertex_t nsec_tetra_vertices[] = {
    { 0,  1,  0},
    {-1, -1, -1},
    { 1, -1, -1},
    { 1, -1,  1},
    {-1, -1,  1},
};

static const nsec_edge_t nsec_tetra_edges[] = {
    {0,1}, {0,2}, {0,3}, {0,4},
    {1,2}, {2,3}, {3,4}, {4,1}
};

NSEC_DECLARE_MESH(nsec_tetra, nsec_tetra_vertices, nsec_tetra_edges);

#define NSEC_MAX_VERTEX_ON_MESH (sizeof(nsec_cube_vertices) / sizeof(nsec_vertex_t))

#define NSEC_DECLARE_MATRIX(name, rows, cols) \
    struct { \
        nsec_matrix_t m; \
        float values[(rows)*(cols)]; \
    } name##_m = { { (rows), (cols) } }; \
    nsec_matrix_t * name = (nsec_matrix_t *) &name##_m; \
    const size_t name##_size __attribute__((unused)) = sizeof(name##_m);

#define NSEC_MATRIX_VALUE(m, row, col) (m)->values[(row) * (m)->cols + (col)]

int nsec_multiply_matrix(nsec_matrix_t * result, nsec_matrix_t * m1, nsec_matrix_t * m2) {
    if(m1 == result || m2 == result) {
        return -1;
    }
    if(m1->cols < 1 || m1->rows < 1 || m2->cols < 1 || m2->rows < 1) {
        return -2;
    }
    if(m1->cols != m2->rows) {
        return -3;
    }
    if(result->rows * result->cols < m1->rows * m2->cols) {
        return -4;
    }
    result->rows = m1->rows;
    result->cols = m2->cols;
    for(int i = 0; i < result->rows; i++) {
        for(int j = 0; j < result->cols; j++) {
          float sum = 0.0f;
          for(int x = 0; x < m1->rows; x++) {
            sum += NSEC_MATRIX_VALUE(m1, i, x) * NSEC_MATRIX_VALUE(m2, x, j);
          }
          NSEC_MATRIX_VALUE(result, i, j) = sum;
        }
    }
    return 0;
}

void nsec_vertex_rotate(nsec_vertex_t * result, nsec_vertex_t * src, float angles[3]) {
    NSEC_DECLARE_MATRIX(point, 3, 1);
    memcpy(point->values, src->position, sizeof(src->position));
    NSEC_DECLARE_MATRIX(result_matrix, 3, 1);
    NSEC_DECLARE_MATRIX(rotation, 3, 3);
    memcpy(rotation->values, (float[9]) {
        1, 0,               0,
        0, cosf(angles[0]), -sinf(angles[0]),
        0, sinf(angles[0]),  cosf(angles[0])
    }, sizeof(float[9]));
    nsec_multiply_matrix(result_matrix, rotation, point);
    memcpy(point, result_matrix, point_size);

    memcpy(rotation->values, (float[9]) {
         cosf(angles[1]), 0, sinf(angles[1]),
         0,               1, 0,
        -sinf(angles[1]), 0, cosf(angles[1])
    }, sizeof(float[9]));
    nsec_multiply_matrix(result_matrix, rotation, point);
    memcpy(point, result_matrix, point_size);

    memcpy(rotation->values, (float[9]) {
        cosf(angles[2]), -sinf(angles[2]), 0,
        sinf(angles[2]),  cosf(angles[2]), 0,
        0              ,  0              , 1
    }, sizeof(float[9]));
    nsec_multiply_matrix(result_matrix, rotation, point);
    memcpy(result, result_matrix->values, sizeof(*result));
}

void nsec_draw_rotated_mesh(nsec_mesh_t * mesh, int center[2], int size, float angles[3]) {
    nsec_vertex_t rotated_vertices[NSEC_MAX_VERTEX_ON_MESH];
    if(mesh->vertex_count > NSEC_MAX_VERTEX_ON_MESH) {
        return;
    }
    for(int i = 0; i < mesh->vertex_count; i++) {
        nsec_vertex_rotate(&rotated_vertices[i], &mesh->vertices[i], angles);
    }
    for(int i = 0; i < mesh->edge_count; i++) {
      nsec_vertex_t * v1 = &rotated_vertices[mesh->edges[i].vertex_index[0]];
      nsec_vertex_t * v2 = &rotated_vertices[mesh->edges[i].vertex_index[1]];
      gfx_drawLine(v1->position[0] * size + center[0], v1->position[1] * size + center[1],
                   v2->position[0] * size + center[0], v2->position[1] * size + center[1],
                   SSD1306_WHITE);
    }
}
