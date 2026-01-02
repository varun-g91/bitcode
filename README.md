# BitLang: Assembler & VM

BitLang is a low-level learning project focused on building a custom **Register-based Virtual Machine** and a corresponding **Assembler**. The project is designed to explore systems programming, manual memory management, and compiler front-end patterns in idiomatic C.

## 🛠 Project Core

BitLang transforms human-readable assembly-like source (`.bl`) into bytecode that runs on a custom virtual machine.

### Key Architectural Patterns

* **Register-Based VM:** Uses a fixed set of registers for instruction execution rather than a stack-oriented approach.
* **Arena Allocation:** The Lexer and Parser use an **Arena** (Bump) allocator. This ensures fast, contiguous memory allocation and simplifies cleanup by freeing entire pools at once.
* **Fixed 5MB Memory Model:** As of now, The VM operates on a single `uint8_t` array of 5MB. All segments—**Code, Data, Read-Only Data, Heap, and Stack**—are managed within this contiguous block. I do plan to replace this fixed array with virtual memory hopefully in the future
* **Idiomatic C Structures:** Extensive use of `enums`, `structs`, and `tagged unions` to represent AST nodes and instructions safely.
* **Efficient Lookups:** Integrates `uthash` for high-performance symbol table lookups and internal mapping.

## 🧪 Testing & Safety

* **Unity Test Framework:** All core components (Lexer, Parser, VM Logic) are validated using the **Unity** C testing framework to ensure instruction accuracy and regression testing.
* **Memory Sanitizers:** Compiled with **AddressSanitizer (ASan)** to guarantee a leak-free and memory-safe implementation.

---

## 🚀 Getting Started (Linux)

This project is built specifically for Linux. Support for cross-platform builds is not currently planned.

### Prerequisites

Ensure you have `cmake`, `make`, and a C compiler (GCC/Clang) installed.

### Installation 

```bash
git clone https://github.com/varun-g91/bitlang.git
cd bitlang
```

### Building the Project

```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests

To run the Unity-based test suite:

```bash
cd build && ctest
```

---

## 📂 Project Structure

* `src/`: Core logic for the Assembler and VM.
* `src/include/`: Header files, data structure definitions and external libraries.
* `test/`: Unity test suites for various components.
---

## 📝 Example Source (.bl)

The finished assembler would parse such instructions into bytecode for the VM:

```asm
; Example BitLang Assembly
MOV R1, 10
MOV R2, 20
ADD R1, R2
HALT

```
