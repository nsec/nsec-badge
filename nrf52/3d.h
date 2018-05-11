//  Copyright (c) 2018
//  Marc-Etienne M. Leveille <marc-etienne@nsec.io>
//
//  License: MIT (see LICENSE for details)

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

typedef struct nsec_mesh_s {
    int vertex_count;
    int edge_count;
    nsec_vertex_t * vertices;
    nsec_edge_t * edges;
} nsec_mesh_t;

#define NSEC_UNWRAP(...) __VA_ARGS__

#define NSEC_DECLARE_MESH(name, vertices, edges) \
    _Pragma("GCC diagnostic push"); \
    _Pragma("GCC diagnostic ignored \"-Wmissing-braces\""); \
    static nsec_vertex_t name##_vertices[] = { NSEC_UNWRAP vertices }; \
    static nsec_edge_t name##_edges[] = { NSEC_UNWRAP edges }; \
    _Pragma("GCC diagnostic pop"); \
    const nsec_mesh_t name##_m = { \
        sizeof(name##_vertices) / sizeof(nsec_vertex_t), \
        sizeof(name##_edges) / sizeof(nsec_edge_t), \
        (name##_vertices), \
        (name##_edges) \
    }; \
    const nsec_mesh_t * name = &name##_m;

#define NSEC_DECLARE_MATRIX(name, rows, cols) \
    struct { \
        nsec_matrix_t m; \
        float values[(rows)*(cols)]; \
    } name##_m = { { (rows), (cols) } }; \
    nsec_matrix_t * name = (nsec_matrix_t *) &name##_m; \
    const size_t name##_size __attribute__((unused)) = sizeof(name##_m);

#define NSEC_MAX_VERTEX_ON_MESH (32)

#define NSEC_MATRIX_VALUE(m, row, col) (m)->values[(row) * (m)->cols + (col)]

extern const nsec_mesh_t * nsec_cube;
extern const nsec_mesh_t * nsec_tetra;

void nsec_draw_rotated_mesh(nsec_mesh_t * mesh, int center[2], int size, float angles[3]);
