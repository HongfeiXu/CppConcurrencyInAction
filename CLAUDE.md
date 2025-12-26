# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a learning repository for "C++ Concurrency in Action (2nd Edition)" with hands-on code examples. The project uses C++20 features and focuses on learning concurrent programming patterns through practical implementation.

## Build and Development Commands

### CMake Configuration
```powershell
# Configure with Visual Studio generator
cmake -S . -B build -G "Visual Studio 17 2022" -A x64

# Configure for Debug (default)
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug

# Configure for Release
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

### Build Commands
```powershell
# Build all targets (Debug)
cmake --build build --config Debug

# Build all targets (Release)
cmake --build build --config Release

# Build specific target
cmake --build build --config Debug --target chapter02_managing_threads

# Clean build
cmake --build build --config Debug --target clean
```

### Run Executables
```powershell
# Chapter 1 example
.\bin\Debug\chapter01_hello_world.exe

# Chapter 2 examples
.\bin\Debug\chapter02_managing_threads.exe
```

### Visual Studio Integration
```powershell
# Open solution in Visual Studio
start build\CppConcurrencyInAction.sln

# Or use the convenience script
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
│   ├── wait_a_thread_to_complete.h
│   ├── pass_param_to_thread.h
│   ├── transfer_ownership_of_a_thread.h
│   ├── joining_thread.h
│   └── batch_create_threads.h
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

To add a new chapter (e.g., Chapter 3):

1. Create directory: `src/chapter03/`
2. Add to `CMakeLists.txt`:
```cmake
add_executable(chapter03_sharing_data
    src/chapter03/main.cpp
)

target_sources(chapter03_sharing_data PRIVATE
    src/chapter03/example1.h
    src/chapter03/example2.h
)

target_link_libraries(chapter03_sharing_data PRIVATE Threads::Threads)
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

### Compiler Settings

- **Standard**: C++20 (`CMAKE_CXX_STANDARD 20`)
- **MSVC**: `/W4 /EHsc /permissive-` + `/Zi` (debug info)
- **Debug**: `/Od` (no optimization) for MSVC, `-g -O0` for GCC/Clang
- **Output**: Executables go to `bin/Debug/` or `bin/Release/`

### Multi-threading Support

All targets link against `Threads::Threads`:
```cmake
find_package(Threads REQUIRED)
target_link_libraries(chapter01_hello_world PRIVATE Threads::Threads)
```

## Visual Studio Debugging

This project is designed for debugging with Visual Studio. Key tools:

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

- **Platform**: Primarily Windows with MSVC (Visual Studio 2022)
- **Line Endings**: CRLF (Windows)
- **Encoding**: UTF-8
- **Indentation**: Tabs (width: 4)
- **No Tests**: This is a learning project; examples are run manually
- **Single-Example Testing**: Only one example runs at a time per chapter
