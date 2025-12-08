
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
# 3. Source files
# --------------------------------------------------------
set(BITLANG_SOURCES
    src/main.c
    src/utils/logger/logger.c
    src/lexer/lexer.c
    src/parser/parser.c
    src/utils/arena_allocator/arena_allocator.c
    src/utils/token_stream/token_stream.c
    src/utils/symbol_table/symbol_table.c
    src/vm/vm.c
    src/vm/opcodes.c
    src/vm/registers.c
    src/vm/instruction_format_table.c
    src/assembler/assembler_context.c
    src/vm/instruction_format_table.c
)

# --------------------------------------------------------
# --------------------------------------------------------
# Set the base source and include directories
set(BITLANG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(BITLANG_INCLUDE_DIR ${BITLANG_SOURCE_DIR}/include)

# Add include directories to the compiler's include path
include_directories(${BITLANG_INCLUDE_DIR})

# Add a custom target for building with address sanitizer
option(ENABLE_ASAN "Enable AddressSanitizer" ON)
if(ENABLE_ASAN AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"))
    message(STATUS "AddressSanitizer enabled")
    add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
    add_link_options(-fno-omit-frame-pointer -fsanitize=address)
endif()

# --------------------------------------------------------
# 5. Create executable and add include paths
# --------------------------------------------------------
add_executable(bitlang ${BITLANG_SOURCES})

# Add include directories to the target
target_include_directories(bitlang PRIVATE ${BITLANG_INCLUDE_DIR})

# Add compile definitions for debug builds
target_compile_definitions(bitlang PRIVATE $<$<CONFIG:Debug>:COMPILER_DEBUG_BUILD=1>)

# --------------------------------------------------------
# 6. Installation
# --------------------------------------------------------
install(TARGETS bitlang
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY ${BITLANG_INCLUDE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# --------------------------------------------------------
# 8. Output directory
# --------------------------------------------------------
set_target_properties(bitlang PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

