#pragma once
#include "quantum.h"
#include "esp_console.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdio.h>  // For getchar
#include <cstdlib>  // For strtol
#include "linenoise/linenoise.h"
#include "mbedtls/md5.h"

// Function to register the quantum console command
void register_quantum_cmd();

