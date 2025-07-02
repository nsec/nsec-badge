/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Patrick Downing <padraignix@gmail.com>
 */

#include "quantum_gates.h"

// Quantum system variables
static Quantum quantum;
static std::vector<Complex> state;
static int num_qubits = 0;
static std::vector<std::string> history;

void initialize_n_qubits(int qubits) {
    if (qubits < 1 || qubits > 8) {
        printf("Error: You can only initialize between 1 and 8 qubits.\n");
        return;
    }
    num_qubits = qubits;
    state = quantum.multiQubitState(num_qubits, 0);  // Initialize |000...0>
    printf("Initialized %d-qubit state: |%0*d>\n", num_qubits, num_qubits, 0);
    history.clear();
}

// Function to apply a single-qubit gate (X, Y, Z, H) to the given qubit
// --------------------------------------------------------------------
void apply_single_qubit_gate(char gate_type, int qubit) {
    if (qubit >= num_qubits || qubit < 0) {
        printf("Error: Invalid qubit. You have initialized %d qubits.\n", num_qubits);
        return;
    }

    quantum.apply_single_qubit_gate(gate_type, qubit, state);
    printf("Applying gate %c to qubit %d...\n", gate_type, qubit);
    history.push_back("gate " + std::string(1, gate_type) + " " + std::to_string(qubit));
}

// Function to apply CNOT gate to 2 qubits
void apply_cnot_gate(int control_qubit, int target_qubit) {
    if (num_qubits < 2) {
        printf("Error: CNOT requires at least 2 qubits.\n");
        return;
    }
    if (control_qubit >= num_qubits || target_qubit >= num_qubits || control_qubit < 0 || target_qubit < 0) {
        printf("Error: Invalid qubit selection. You have initialized %d qubits.\n", num_qubits);
        return;
    }
    if (control_qubit == target_qubit) {
        printf("Error: Control and target qubits must be different.\n");
        return;
    }

    quantum.apply_cnot_gate(control_qubit, target_qubit, num_qubits, state);
    printf("Applying CNOT gate (control: %d, target: %d)...\n", control_qubit, target_qubit);
    history.push_back("gate CNOT " + std::to_string(control_qubit) + " " + std::to_string(target_qubit));
}

void print_gate_history() {
    if (history.empty()) {
        printf("No gates have been applied yet.\n");
    } else {
        printf("Gate operation history:\n");
        for (const auto& entry : history) {
            printf("  %s\n", entry.c_str());
        }
    }
}

void print_statevector() {
    std::string state_str = quantum.statevector(state);
    printf("%s", state_str.c_str());
}

void compute_md5(const char *input, char *output) {
    unsigned char digest[16];
    mbedtls_md5_context ctx;

    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    mbedtls_md5_update(&ctx, (const unsigned char *)input, strlen(input));
    mbedtls_md5_finish(&ctx, digest);
    mbedtls_md5_free(&ctx);

    for (int i = 0; i < 16; i++) {
        sprintf(output + (i * 2), "%02x", digest[i]);
    }
    output[32] = '\0';
}

void print_hash() {
    std::string state_str = quantum.statevector(state);
    char md5_output[33];
    compute_md5(state_str.c_str(), md5_output);
    printf("Hash: %s\n", md5_output);
}

void measure_qubits() {
    std::string result = quantum.measure(state, num_qubits);
    printf("Measured qubit state: |%s>\n", result.c_str());
    printf("Qubits have been reinitialized...\n");
}

void quantum_interactive_mode() {
    char *input;

    printf("Quantum interactive mode. Type 'exit' to quit, 'help' for commands, or 'list' to show applied gates.\n");

    while (true) {
        input = linenoise("q>: ");

        if (!input) {
            printf("\nExiting quantum mode...\n");
            break;
        }

        while (*input == ' ') ++input;

        if (*input) {
            linenoiseHistoryAdd(input);
        }

        if (strcmp(input, "exit") == 0) {
            printf("Exiting quantum mode...\n");
            free(input);
            break;
        }

        if (strcmp(input, "help") == 0) {
            printf("Commands available:\n");
            printf("  g <X|Y|Z|H> <qubit>\n");
            printf("  g CNOT <control_qubit> <target_qubit>\n");
            printf("  list (to see applied gates)\n");
            printf("  sv (to view the qubit statevector)\n");
            printf("  hash (return the hash of the current statevector)\n");
            printf("  measure (collapses the state and automatically reinitializes)\n");
            printf("  exit (to quit)\n");
            free(input);
            continue;
        }

        if (strcmp(input, "list") == 0) {
            print_gate_history();
            free(input);
            continue;
        }

        if (strcmp(input, "sv") == 0) {
            print_statevector();
            free(input);
            continue;
        }

        if (strcmp(input, "measure") == 0) {
            measure_qubits();
            free(input);
            continue;
        }

        if (strcmp(input, "hash") == 0) {
            print_hash();
            free(input);
            continue;
        }

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        if (command == "g") {
            std::string gate_name;
            iss >> gate_name;

            if (gate_name == "CNOT") {
                int control_qubit, target_qubit;
                if (!(iss >> control_qubit >> target_qubit)) {
                    printf("Error: Invalid input format for CNOT. Expected: gate CNOT <control_qubit> <target_qubit>\n");
                    free(input);
                    continue;
                }
                apply_cnot_gate(control_qubit, target_qubit);
            } else if (gate_name.length() == 1) {
                char gate_type = gate_name[0];
                int qubit;
                if (!(iss >> qubit)) {
                    printf("Error: Invalid input format for gate. Expected: gate <X|Y|Z|H> <qubit>\n");
                    free(input);
                    continue;
                }
                apply_single_qubit_gate(gate_type, qubit);
            } else {
                printf("Unknown gate command. Type 'help' for available commands.\n");
            }
        } else {
            printf("Unknown command. Type 'help' for available commands.\n");
        }

        free(input);
    }

    printf("Quantum state cleared.\n");
}

bool is_valid_number(const char* str) {
    char* endptr;
    long val = strtol(str, &endptr, 10);
    return *endptr == '\0';
}

int cmd_quantum(int argc, char **argv) {
    if (argc == 2) {
        if (!is_valid_number(argv[1])) {
            printf("Error: Invalid input. Please provide a number between 1 and 8 for qubits.\n");
            return 0;
        }

        int qubits = std::strtol(argv[1], nullptr, 10);

        if (qubits < 1 || qubits > 8) {
            printf("Error: You must initialize between 1 and 8 qubits.\n");
            return 0;
        }

        initialize_n_qubits(qubits);
        quantum_interactive_mode();
        return 0;
    } else {
        printf("Usage: quantum init <1-8>\n");
        return 0;
    }
}

void register_quantum_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "quantum",
        .help = "Quantum computing simulator - initialize with 1-8 qubits",
        .hint = "<1-8>",
        .func = &cmd_quantum,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
