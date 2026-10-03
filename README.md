# SI2002 Formal Languages - Assignment 2

## Student Information
* **Student's Full Name:** Juan Camilo Anzola Gómez
* **Student's Class Number:** (Completa aquí con tu número de grupo, ej. 01 o 02)

## Environment and Tools
* **Operating System:** CachyOS x86_64
* **Kernel:** Linux 7.2.8-2-cachyos
* **Programming Language:** C (C11 standard)
* **Build System:** CMake (Version 3.10 or higher) and GCC/Clang/MSVC compiler

## Compilation and Execution Instructions

This project uses CMake to ensure platform-independent compilation. Follow the steps below to build and run the implementation from your terminal:

1. **Create a build directory and navigate into it:**
   ```bash
   mkdir build && cd build
   ```

2. **Generate the build files using CMake:**
   ```bash
   cmake ..
   ```

3. **Compile the project:**
   ```bash
   cmake --build .
   ```

4. **Execution:**
   * **Interactive mode (via Standard Input):**
     ```bash
     ./subset_construction
     ```
   * **Using an input file (Recommended):**
     If you have a file named `input.txt` inside your build directory, run:
     ```bash
     ./subset_construction < input.txt
     ```

## Algorithm Explanation
The program implements the **Subset Construction** algorithm to convert a Non-Deterministic Finite Automaton (NFA) into an equivalent Deterministic Finite Automaton (DFA).

1. **State Set Representation (Bitmasking):** Since C lacks high-level object sets natively, subsets of NFA states are stored as a 64-bit unsigned integer (`uint64_t`). Each bit position represents whether a state \(q_i\) is active in the sub-set, ensuring fast union (`|`) and intersection (`&`) operations.
2. **Parsing Strategy:** The engine reads inputs dynamically using standard line parsing. It skips structured syntax tokens such as `{` and `}` while recognizing `0` as the empty set \(\emptyset\).
3. **Exploration Loop:** Starting from the NFA's initial states subset, the algorithm computes transitions for each alphabet symbol. If a newly discovered subset does not match any existing tracking ID, it is registered as a new DFA state.
4. **Final State Verification:** A DFA state is flagged as an accepting state if its associated NFA state bitmask shares at least one common bit with the NFA's target final states.
