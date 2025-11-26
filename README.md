## ELF-Binary-C Systems & Reverse Engineering Assignment

This project contains solutions for five practical real practical system level programming tasks that go from reverse engineering, server creation, client regulating and thread diversing, assembly and even creation of Python extensions. Each task is organized into its own directory with source files, sample outputs, and documentation.

# Question 1 – Reverse Engineering a Compiled ELF Binary

This section includes a short report summarizing the analysis performed using objdump, strace, and gdb. It can be found in its respective food
Deliverables include:

Function Listing: Identified from objdump -d, with explanations of control flows instructions, particularly conditional and unconditional jumps.

Runtime Behavior: System calls captured using strace, showing file I/O, memory operations, and the processes control.

Debugging Analysis: GDB breakpoints, function transition tracing, and notes on stack/heap/global memory usage.

All findings are in ELF_Analysis.md.

# Question 2 – Assembly Program for Sensor Log Processing

The assembly file (sensor_counter.asm) demonstrates:

Opening and reading sensor_log.txt.

Loading file contents into memory.

Traversing the buffer line by line and counting total readings, including all empty lines.

Printing the total as:
Total sensor readings: X.

Comments explain file handling, string traversal, and loop/branch control flow.

Run by going in the directory through the terminal and inputting:
`./sensor_log`

# Question 3 – Python C Extension for Temp Stats

A custom Python C extension processes floater point temperature readings that can be imported into a python script.

Deliverables:

temp_stats.c — Implements min_temp, max_temp, avg_temp, variance_temp, and count_readings.

setup.py — Builds configuration using setuptools. These can be found in setup.py

test.py — Demonstration of function usage, as we need a way to test our functionality.

Documentation describes algorithm behavior, memory handling of Python lists and C arrays, and time complexities of DSA's used.

To use the test file and test if everything is operational you go into terminal directory of program and type:
`python test.py`

# Question 4 – Multithreaded Producer–Consumer Simulation

This directory contains an implementation of a barista (producer) and waiter (consumer) system using mutexes and condition variables.

Features:

Shared fixed-size queue (max 8 drinks).

Barista produces every 4 seconds; waiter consumes every 3 seconds. This is the standard operations interactions.

Creating thread safe synchronization prevents race conditions and enforcing full/empty waits.

Console logs show real-time queue changes and thread activity. 
Comments showcase logic and queue state transitions.

To run, go to directory terminal and input:
`./produ_consumer.exe`

# Question 5 – Concurrent TCP Exam Platform

A real time live client server application using socket programming functionality.

It includes:

server.c — Handles up to 4 students concurrently, authenticates usernames, sends questions, receives answers, and broadcasts active users live. In on terminal output.

client.c — Connects, authenticates, receives questions, submits answers, and displays feedback. In one terminal output and lists them sequentially.

The design programs ensure independent sessions, and reliable concurrency using threads or select.

To run programs, open two terminals in the folder 5 directory. Then:
1. Run `./exam_server` then go to the second open terminal.
2. Run `./exam_client` then enter input from there.

NOTE: You can open multiple clients in different terminals but NOT multiple servers.