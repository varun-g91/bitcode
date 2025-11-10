# The minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# 1. Define the project name
project(BitLangCompiler C)

# 2. Define the directory where header files are located.
# We set this property for the whole project.
include_directories(src/include)

# 3. Collect all source files
# Note: Since your main() is in lexer.c, we treat lexer.c and logger.c
# as the main sources for the executable.
set(COMPILER_SOURCES
    src/lexer/lexer.c
    src/logger.c
)

# 4. Define the final executable
# This tells CMake to compile the sources into an executable named 'lexer'.
# The output will automatically go into a binary directory (e.g., 'build/lexer').
add_executable(lexer ${COMPILER_SOURCES})

# 5. Optional: Specify where to put the executable (build directory)
# This is a standard practice to keep source separate from binaries.
set_target_properties(lexer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
)

add_definitions(-DCOMPILER_DEBUG_BUILD)
