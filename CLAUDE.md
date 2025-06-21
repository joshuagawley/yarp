# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Pacman++ is a C++23 reimplementation of functionality from the Pacman package manager for Arch Linux. It interfaces with the ALPM (Arch Linux Package Management) library to provide package query and management capabilities.

## Common Development Commands

### Building
```bash
# Debug build (recommended for development)
cmake -B build/Debug -G Ninja
cmake --build build/Debug

# Release build with debug info
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build

# Build with warnings as errors
cmake -B build/Debug -G Ninja -DWARNINGS_AS_ERRORS=ON
cmake --build build/Debug
```

### Testing
```bash
# Run all tests
cd build/Debug && ctest

# Run tests with output
cd build/Debug && ctest -V

# Run specific test
cd build/Debug && ctest -R test_name

# Run a single Python test directly
cd build/Debug && python ../../tests/test_name.py
```

### Code Formatting
```bash
# Format all source files
find src -name "*.cc" -o -name "*.h" | xargs clang-format -i

# Check formatting without modifying
find src -name "*.cc" -o -name "*.h" | xargs clang-format -n --Werror
```

## Architecture Overview

### Core Components

1. **App Class** (`src/app.h`, `src/app.cc`)
   - Central application controller
   - Manages command-line parsing and operation dispatch
   - Implements lazy ALPM initialization
   - Key responsibilities:
     - Parse arguments via ArgumentParser
     - Initialize ALPM when needed
     - Dispatch to appropriate operation handlers

2. **ALPM Wrapper** (`src/alpm.h`)
   - RAII wrapper around ALPM C library
   - Provides type-safe C++ interface
   - Static methods wrap ALPM functions
   - Converts between ALPM lists and C++ containers

3. **Argument Parser** (`src/argument_parser.h`)
   - Command-line argument processing
   - Maps arguments to Operation and QueryOptions enums
   - Handles both short and long options

4. **Operation System** (`src/operation.h`)
   - Enum-based operation types (Help, Query, Version)
   - Bitwise-enabled QueryOptions for combinable flags
   - Type-safe operation handling

### Key Design Patterns

- **RAII**: Alpm class manages ALPM handle lifetime automatically
- **Lazy Initialization**: ALPM only initialized when needed (not for help/version)
- **Modern C++**: Uses C++23 features including std::print, std::span, concepts
- **Exception-based Error Handling**: Errors propagate via exceptions

### Testing Architecture

Tests are Python-based using a custom framework (`tests/pptest.py`) that:
- Creates mock ALPM databases
- Simulates package installations
- Verifies command output and behavior
- Tests are registered in `tests/CMakeLists.txt`

## Important Development Notes

- The project uses CMake with Ninja as the preferred generator
- Compiler flags include `-Wall -Wextra -Wpedantic` for maximum warnings
- Code style follows Google C++ style with modifications (see `.clang-format`)
- The ALPM library is a C library; the wrapper provides C++ safety
- Tests require Python and create temporary ALPM databases for isolation