cmake_minimum_required(VERSION 3.10)
project(BitLangCompiler C)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# --------------------------------------------------------
# 1. Set C standard and compiler flags
# --------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Common compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

# --------------------------------------------------------
# 2. Source files (Dynamic Listing - RECOMMENDED CHANGE)
# --------------------------------------------------------
# Use file(GLOB) or GLOB_RECURSE to automatically find all source files
# Adjust glob patterns based on your exact file structure.
file(GLOB BITLANG_CORE_SOURCES
    "${CMAKE_SOURCE_DIR}/src/utils/*.c"
    "${CMAKE_SOURCE_DIR}/src/utils/*/*.c"
    "${CMAKE_SOURCE_DIR}/src/lexer/*.c"
    "${CMAKE_SOURCE_DIR}/src/parser/*.c"
    "${CMAKE_SOURCE_DIR}/src/vm/*.c"
    "${CMAKE_SOURCE_DIR}/src/assembler/*.c"
)
# Note: The main file src/main.c should NOT be included here.

# --------------------------------------------------------
# 3. Directories and ASAN Configuration
# --------------------------------------------------------
set(BITLANG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(BITLANG_INCLUDE_DIR ${BITLANG_SOURCE_DIR}/include)
# Removed global add_definitions(-DCOMPILER_DEBUG_BUILD)
option(ENABLE_ASAN "Enable AddressSanitizer" ON)
if(ENABLE_ASAN AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"))
    message(STATUS "AddressSanitizer enabled")
    add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
    add_link_options(-fno-omit-frame-pointer -fsanitize=address)
endif()

# --------------------------------------------------------
# 4. Create Library Target (vm_library)
# --------------------------------------------------------

# Define the library that holds all the core VM and compiler logic
add_library(vm_library ${BITLANG_CORE_SOURCES})

# Ensure the library has access to its own includes and makes them PUBLIC
target_include_directories(vm_library PUBLIC ${BITLANG_INCLUDE_DIR})

# Apply compile definitions (like COMPILER_DEBUG_BUILD) to the library
target_compile_definitions(vm_library PUBLIC 
    $<$<CONFIG:Debug>:COMPILER_DEBUG_BUILD=1>
)


# --------------------------------------------------------
# 5. Create Executable (The Main Compiler)
# --------------------------------------------------------
add_executable(bitlang src/main.c)

# Link the executable to the core library (inherits includes and compile definitions)
target_link_libraries(bitlang PRIVATE vm_library)


# --------------------------------------------------------
# 6. Installation
# --------------------------------------------------------
install(TARGETS bitlang
    RUNTIME DESTINATION bin
)

install(TARGETS vm_library
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install headers
install(DIRECTORY ${BITLANG_INCLUDE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# --------------------------------------------------------
# 7. Output directory
# --------------------------------------------------------
set_target_properties(bitlang PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# --------------------------------------------------------
# 8. Tests
# --------------------------------------------------------
# 8.1. Enable CTest
enable_testing()

# Define the source files for the test executable
set(TEST_SOURCES
    tests/unity.c
    tests/test_main.c
    tests/test_lexer.c
    # tests/test_decoder.c 
)

# 8.2. Create the test executable
add_executable(vm_tests ${TEST_SOURCES})

# 8.3. Link the test executable to the core VM library
target_link_libraries(vm_tests PRIVATE vm_library)

# 8.4. Register the test executable with CTest
add_test(NAME RunAllTests COMMAND vm_tests)

# 8.5. Set output directory for tests
set_target_properties(vm_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
