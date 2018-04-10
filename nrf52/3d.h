//  Copyright (c) 2018
//  Marc-Etienne M. Leveille <marc-etienne@nsec.io>
//
//  License: MIT (see LICENSE for details)

struct nsec_mesh_s;
typedef struct nsec_mesh_s nsec_mesh_t;
extern const nsec_mesh_t * nsec_cube;
extern const nsec_mesh_t * nsec_tetra;

void nsec_draw_rotated_mesh(nsec_mesh_t * mesh, int center[2], int size, float angles[3]);
