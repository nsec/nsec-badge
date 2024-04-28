// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "driver/rmt_types.h"
#include "driver/rmt_tx.h"

#ifdef __cplusplus
extern "C" {
#endif

// void setup_ir(void);
void register_ir_cmd(void);
int ir_cmd(int argc, char **argv);

#ifdef __cplusplus
}
#endif
