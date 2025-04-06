#pragma once
#include <vector>
#include <complex>
#include "esp_random.h"
#include <algorithm>  // For std::reverse

using Complex = std::complex<double>;

class Quantum {
public:
    Quantum();
    std::vector<Complex> multiQubitState(int num_qubits, int initial_state);

    // Add statevector, measure, and reinitialize functions
    std::string statevector(const std::vector<Complex>& state);
    std::string measure(std::vector<Complex>& state, int num_qubits);
    void reinitialize(std::vector<Complex>& state, int num_qubits);
    // Apply single-qubit gates (X, Y, Z, H) to the given qubit
    void apply_single_qubit_gate(char gate_type, int qubit, std::vector<Complex>& state);
    // Apply CNOT gate between two qubits (control and target)
    void apply_cnot_gate(int control_qubit, int target_qubit, int num_qubits, std::vector<Complex>& state);


    // Pauli and other gates
    std::vector<std::vector<Complex>> pauliX();
    std::vector<std::vector<Complex>> pauliY();
    std::vector<std::vector<Complex>> pauliZ();
    std::vector<std::vector<Complex>> hadamardGate();
    std::vector<std::vector<Complex>> cnotGate(int control_qubit, int target_qubit, int num_qubits);

    // Tensor product function declaration
    std::vector<Complex> tensorProduct(const std::vector<Complex>& q1, const std::vector<Complex>& q2);

    // Helper function to apply a gate
    std::vector<Complex> applyGate(const std::vector<std::vector<Complex>>& gate, const std::vector<Complex>& state);
};