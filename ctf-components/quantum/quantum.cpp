#include "quantum.h"
#include <iostream>
#include <random>

/// Constructor
Quantum::Quantum() {}

// Initialize multi-qubit state
std::vector<Complex> Quantum::multiQubitState(int num_qubits, int initial_state) {
    std::vector<Complex> state(1 << num_qubits, Complex(0, 0));
    state[initial_state] = Complex(1, 0);  // Set the initial state
    return state;
}

// Return the current state vector (statevector) as a 2^N matrix in string format
std::string Quantum::statevector(const std::vector<Complex>& state) {
    int num_states = state.size();  // 2^n
    int num_qubits = std::log2(num_states);  // Number of qubits

    std::string result = "Statevector:\n";

    // Loop through all possible states (2^num_qubits)
    for (int i = 0; i < num_states; ++i) {
        result += "|";
        // Create the binary representation of the state (e.g., |000>, |001>, etc.)
        for (int j = num_qubits - 1; j >= 0; --j) {
            result += std::to_string((i >> j) & 1);  // Extract each qubit as binary
        }
        result += "> : (" + std::to_string(state[i].real()) + " + " + std::to_string(state[i].imag()) + "i)\n";
    }

    return result;
}

// Measure qubits, collapse the state, and return the classical measurement result
std::string Quantum::measure(std::vector<Complex>& state, int num_qubits) {
    if (num_qubits == 0) {
        return "";
    }

    std::string measurement_result;
    int total_states = 1 << num_qubits;  // 2^num_qubits possible states

    for (int qubit_index = 0; qubit_index < num_qubits; ++qubit_index) {
        double prob_zero = 0.0;

        for (int i = 0; i < total_states; ++i) {
            if (((i >> qubit_index) & 1) == 0) {  // Check if the qubit is 0
                prob_zero += std::norm(state[i]);
            }
        }

        // Generate a random value to simulate the measurement outcome
        float random_value = (float)esp_random() / UINT32_MAX;

        // Collapse state and record the result
        if (random_value < prob_zero) {
            measurement_result += "0";
            for (int i = 0; i < total_states; ++i) {
                if (((i >> qubit_index) & 1) == 1) {
                    state[i] = Complex(0, 0);  // Collapse the state
                }
            }
        } else {
            measurement_result += "1";
            for (int i = 0; i < total_states; ++i) {
                if (((i >> qubit_index) & 1) == 0) {
                    state[i] = Complex(0, 0);  // Collapse the state
                }
            }
        }

        // Normalize the remaining state vector after collapse
        double norm = 0.0;
        for (const auto& amp : state) {
            norm += std::norm(amp);
        }
        if (norm > 0.0) {
            for (auto& amp : state) {
                amp /= sqrt(norm);
            }
        }
    }

    // Reverse the measurement result to reflect qubit 0 as the rightmost qubit
    std::reverse(measurement_result.begin(), measurement_result.end());

    // Reinitialize the quantum state after measurement
    state = multiQubitState(num_qubits, 0);  // Reset to |000...0>

    return measurement_result;
}

// Reinitialize the quantum state after measurement
void Quantum::reinitialize(std::vector<Complex>& state, int num_qubits) {
    state = multiQubitState(num_qubits, 0);  // Reset to |000...0> state
}

// Pauli-X Gate (bit flip)
std::vector<std::vector<Complex>> Quantum::pauliX() {
    return {
        {{0.0, 0.0}, {1.0, 0.0}},
        {{1.0, 0.0}, {0.0, 0.0}}
    };
}

// Pauli-Y Gate
std::vector<std::vector<Complex>> Quantum::pauliY() {
    return {
        {{0.0, 0.0}, {0.0, -1.0}},
        {{0.0, 1.0}, {0.0, 0.0}}
    };
}

// Pauli-Z Gate
std::vector<std::vector<Complex>> Quantum::pauliZ() {
    return {
        {{1.0, 0.0}, {0.0, 0.0}},
        {{0.0, 0.0}, {-1.0, 0.0}}
    };
}

// Hadamard Gate
std::vector<std::vector<Complex>> Quantum::hadamardGate() {
    double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    return {
        {{inv_sqrt2, 0.0}, {inv_sqrt2, 0.0}},
        {{inv_sqrt2, 0.0}, {-inv_sqrt2, 0.0}}
    };
}

// CNOT Gate for multi-qubit system
std::vector<std::vector<Complex>> Quantum::cnotGate(int control_qubit, int target_qubit, int total_qubits) {
    int size = 1 << total_qubits;  // 2^total_qubits
    std::vector<std::vector<Complex>> cnot_matrix(size, std::vector<Complex>(size, {0.0, 0.0}));

    for (int i = 0; i < size; ++i) {
        int control_state = (i >> control_qubit) & 1;  // Extract the control qubit state (0 or 1)
        if (control_state == 1) {
            int target_flip = i ^ (1 << target_qubit);  // XOR to flip the target qubit bit
            cnot_matrix[target_flip][i] = {1.0, 0.0};   // Swap |control, target> with |control, !target>
        } else {
            cnot_matrix[i][i] = {1.0, 0.0};  // No change for control = 0
        }
    }

    return cnot_matrix;
}

// Tensor product of two quantum states
std::vector<Complex> Quantum::tensorProduct(const std::vector<Complex>& q1, const std::vector<Complex>& q2) {
    std::vector<Complex> result(q1.size() * q2.size());
    for (size_t i = 0; i < q1.size(); ++i) {
        for (size_t j = 0; j < q2.size(); ++j) {
            result[i * q2.size() + j] = q1[i] * q2[j];
        }
    }
    return result;
}

// Apply gate to qubit state
std::vector<Complex> Quantum::applyGate(const std::vector<std::vector<Complex>>& gate, const std::vector<Complex>& state) {
    std::vector<Complex> result(state.size());
    for (size_t i = 0; i < gate.size(); ++i) {
        result[i] = 0;
        for (size_t j = 0; j < gate[i].size(); ++j) {
            result[i] += gate[i][j] * state[j];
        }
    }
    return result;
}

// Apply a single-qubit gate (X, Y, Z, H) to a specific qubit
void Quantum::apply_single_qubit_gate(char gate_type, int qubit, std::vector<Complex>& state) {
    std::vector<std::vector<Complex>> gate;

    switch (gate_type) {
        case 'X': gate = pauliX(); break;
        case 'Y': gate = pauliY(); break;
        case 'Z': gate = pauliZ(); break;
        case 'H': gate = hadamardGate(); break;
        default:
            std::cerr << "Invalid gate type." << std::endl;
            return;
    }

    // Apply gate only on the selected qubit by tensoring it with the identity matrix for other qubits
    std::vector<Complex> new_state(state.size(), Complex(0, 0));

    for (int i = 0; i < state.size(); ++i) {
        int qubit_value = (i >> qubit) & 1;  // Extract qubit's value (0 or 1)
        for (int j = 0; j < gate.size(); ++j) {
            int new_index = (i & ~(1 << qubit)) | (j << qubit);  // Modify only the selected qubit
            new_state[new_index] += gate[j][qubit_value] * state[i];
        }
    }

    state = new_state;  // Update state after applying the gate
}

// Apply CNOT gate to 2 qubits in a multi-qubit state
void Quantum::apply_cnot_gate(int control_qubit, int target_qubit, int num_qubits, std::vector<Complex>& state) {
    std::vector<Complex> new_state(state.size(), Complex(0, 0));
    int size = 1 << num_qubits;  // 2^num_qubits possible states

    for (int i = 0; i < size; ++i) {
        int control_state = (i >> control_qubit) & 1;  // Extract the control qubit state (0 or 1)
        if (control_state == 1) {
            // Flip the target qubit if the control qubit is 1
            int target_flip = i ^ (1 << target_qubit);  // XOR to flip the target qubit bit
            new_state[target_flip] = state[i];  // Swap |control, target> with |control, !target>
        } else {
            new_state[i] = state[i];  // No change for control = 0
        }
    }

    state = new_state;  // Update the state after applying CNOT
}