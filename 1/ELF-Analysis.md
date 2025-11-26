# ELF Binary Analysis Report

## Control Flow Analysis
So this program follows a sequence of workflow that goes as such : `_start` → `main` → data collection → file operations → sorting → output.  This is pretty basic and standard, however the main point of contention is the sole main function of the program.

The main function is orchestrating the the entire process, line by line.  Calling each previously stated  specialized function for each task/function purpose. Essentially, control flows linearly with loops for user input and sorting operations. A basic program and we have deconstructed it in a backward sequence.

## Functions Identified & Purposes
- **main**: Program orchestrator, manages workflow
- **getStudentNames**: Collects user input via stdin (10 student names)
- **writeToFile**: Writing data to filesystem with error handling already factored in
- **readFromFile**: Reads stored data from filesystem to get the student details
- **sortNames**: Implements alphabetical sorting using string comparisons 
- **printNames**: Outputs results to stdout (aka: users terminal for visibility)

## Memory Access Patterns
- **Stack**: Primary storage for student names array (1000+ bytes allocated). 
This is standard amount, more is not neccessary as that will be a useless amount of primary storage for this data type.
- **Heap**: Dynamic memory via `brk()` system calls for runtime data
Standard for Heap sequencing in order to get that runtime data.
- **Libraries**: Multiple `mmap()` calls load shared libraries (libc)
- **File Buffers**: Memory-mapped file operations for I/O efficiency
Memory mapping is important as it lets you manually allocate the memory based on needs across different functions. Memory optimisation in essence.

## Strace Runtime Analysis
Key system calls reveal program behavior:
- **File I/O**: `openat`, `read`, `write` for file operations. Very standard for this type of program engaging with file reading and writing.
- **Memory Management**: `mmap`, `mprotect`, `brk` for dynamic memory. This is Necessary in order for your program to run dynamically.
- **Process Setup**: `execve` initialization, `arch_prctl` configuration
- **User Interaction**: `write` prompts to stdout, `read` input from stdin. This is the key interaction between the system and the user.

## Program Summary
This student management system demonstrates robust file handling and data processing. It tries to collect user input, using filesystem based funcs, performs inmemory sorting, and outputs results. 

The control flow shows careful error handling with proper cleanup through `fclose` and stack protection mechanisms. Memory usage patterns indicate efficient stack based storage for the fixed size array whilst leveraging heap DSA for dynamic runtime needs, ergo for dynamic output regardless of file size given.