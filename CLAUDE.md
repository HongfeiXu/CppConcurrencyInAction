# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a learning repository for "C++ Concurrency in Action (2nd Edition)" with hands-on code examples. The project uses C++20 features and focuses on learning concurrent programming patterns through practical implementation.

## Build and Development Commands

The project uses **CMake Presets** (`CMakePresets.json`) for cross-platform builds. VS Code CMake Tools extension auto-detects presets.

### macOS
```bash
# Configure + Build (Debug)
cmake --preset macos-debug
cmake --build --preset macos-debug

# Release
cmake --preset macos-release
cmake --build --preset macos-release

# Build specific target
cmake --build --preset macos-debug --target chapter02_managing_threads

# Run
./bin/chapter01_hello_world
./bin/chapter02_managing_threads
./bin/chapter03_sharing_data
```

### Windows
```powershell
# Configure + Build (Debug)
cmake --preset windows-debug
cmake --build --preset windows-debug

# Release
cmake --preset windows-release
cmake --build --preset windows-release

# Build specific target
cmake --build --preset windows-debug --target chapter02_managing_threads

# Run
.\bin\Debug\chapter01_hello_world.exe
.\bin\Debug\chapter02_managing_threads.exe
.\bin\Debug\chapter03_sharing_data.exe
```

### Visual Studio Integration (Windows only)
```powershell
start build\CppConcurrencyInAction.sln
.\open_vs.bat
```

## Project Architecture

### Chapter-Based Organization

The codebase is organized by book chapters, with each chapter containing multiple header-based examples:

```
src/
├── chapter01/          # Introduction
│   └── hello_world.cpp
├── chapter02/          # Managing threads
│   ├── main.cpp        # Entry point - includes one example at a time
│   ├── 2.1_thread_guard.h
│   ├── 2.2_pass_params.h
│   ├── 2.3_scoped_thread.h
│   ├── 2.3_joining_thread.h
│   ├── 2.4_batch_threads.h
│   └── 2.4_multi_thread_accumulate.h
├── chapter03/          # Sharing data between threads
│   ├── main.cpp
│   ├── 3.1_data_race.h
│   └── 3.2_mutex.h
└── chapterNN/          # Future chapters...
```

### Example Selection Pattern

Each chapter's `main.cpp` includes ONE example header at a time for testing:

```cpp
// Uncomment one example to test
// #include "wait_a_thread_to_complete.h"
// #include "pass_param_to_thread.h"
#include "joining_thread.h"  // Currently active

int main() {
    test();  // Each header provides a test() function
}
```

**When adding new examples:**
1. Create a new `.h` file in the appropriate chapter directory
2. Implement your example with a `test()` or similar function
3. Comment out other includes in `main.cpp` and include your new header
4. Add the header to `target_sources()` in `CMakeLists.txt`

### Adding New Chapters

To add a new chapter (e.g., Chapter 4):

1. Create directory: `src/chapter04/`
2. Add to `CMakeLists.txt`:
```cmake
add_executable(chapter04_example
    src/chapter04/main.cpp
)

target_sources(chapter04_example PRIVATE
    src/chapter04/example1.h
    src/chapter04/example2.h
)

target_link_libraries(chapter04_example PRIVATE Threads::Threads)
```

### RAII Thread Wrappers

The project demonstrates several RAII patterns for safe thread management:

- **`thread_guard`**: Ensures `join()` in destructor (basic RAII)
- **`scoped_thread`**: Takes ownership via move, validates `joinable()` in constructor
- **`joining_thread`**: Full `std::thread` wrapper with automatic joining (mimics C++20's `std::jthread`)

These wrappers solve the common problem where forgetting to call `join()` or `detach()` before a `std::thread` destructs causes `std::terminate()`.

## Coding Patterns

### Thread Function Signatures

Examples demonstrate various ways to create threads:

```cpp
// Lambda
std::thread t([]{ /* work */ });

// Function with arguments
void f(int i, std::string const& s);
std::thread t(f, 3, "hello");

// Reference parameters require std::ref
void update(widget_data& data);
std::thread t(update, std::ref(data));

// Member functions
class X {
    void do_work(int i);
};
X obj;
std::thread t(&X::do_work, &obj, 42);

// Move-only types
std::unique_ptr<big_object> p(new big_object);
std::thread t(process, std::move(p));
```

### Thread Ownership Transfer

`std::thread` is move-only. Transfer examples:

```cpp
std::thread t1(some_function);
std::thread t2 = std::move(t1);        // Explicit move
t1 = std::thread(other_function);      // Temporary - implicit move
std::thread t3;
t3 = std::move(t2);
```

**Critical**: Assigning to an already-running thread calls `std::terminate()`.

## Build System Details

### CMake Presets

Cross-platform build is managed via `CMakePresets.json`:
- **Windows**: Visual Studio 18 2026 generator, x64
- **macOS**: Ninja generator
- **Linux**: Ninja generator

Each platform has Debug and Release presets. Presets use `condition` to only show on the matching OS.

### Compiler Settings

- **Standard**: C++20 (`CMAKE_CXX_STANDARD 20`)
- **MSVC**: `/W4 /EHsc /permissive-` + `/Zi` (debug info)
- **GCC/Clang**: `-Wall -Wextra -pedantic`
- **Debug**: `/Od` for MSVC, `-g -O0` for GCC/Clang
- **Output**: `bin/` (macOS/Linux), `bin/Debug/` or `bin/Release/` (Windows with VS generator)

### Multi-threading Support

All targets link against `Threads::Threads`:
```cmake
find_package(Threads REQUIRED)
target_link_libraries(chapter01_hello_world PRIVATE Threads::Threads)
```

## Visual Studio Debugging (Windows only)

Key debugging tools for concurrent code:

- **Threads Window**: View all threads, IDs, and states
- **Parallel Stacks**: Visualize thread call stacks
- **Parallel Tasks**: Monitor concurrent tasks
- **Diagnostics Tools**: Thread activity timeline

See `docs/build_and_debug_vs.md` for detailed debugging workflows.

## Documentation

- **`docs/plan.md`**: 10-12 week study plan for the book
- **`docs/study_notes.md`**: Chapter-by-chapter notes with examples
- **`docs/build_and_debug_vs.md`**: Complete Visual Studio workflow guide
- **`study_materials/`**: Chinese translation of "C++ Concurrency in Action 2nd Edition"

## Important Constraints

- **Platform**: Windows (MSVC) + macOS (Apple Clang) + Linux (GCC/Clang)
- **Line Endings**: Auto-managed via `.gitattributes` (`* text=auto`)
- **Encoding**: UTF-8
- **Indentation**: Tabs (width: 4)
- **No Tests**: This is a learning project; examples are run manually
- **Single-Example Testing**: Only one example runs at a time per chapter
