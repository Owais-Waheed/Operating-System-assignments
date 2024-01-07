## Multi-Threading File Processor Assignment

### Introduction
Develop a multi-threaded C program using POSIX Threads API to process large datasets concurrently. The program reads data from a file, calculates statistics using a single thread, and then parallelizes the computation using multiple threads.

### How to Run
- Provide the program name and the data file to be processed as command line arguments.
- Optionally, specify the number of threads for concurrent processing.

### Required Tasks
1. Read dataset from a file and calculate statistics using a single thread.
2. Create worker threads (default 4 if not provided) to process the dataset concurrently.
3. Implement thread-safe mechanisms for computing sum, finding minimum and maximum values.
4. Print results for both single and multi-threaded versions.

### Input
- Single-Threaded Program: `./program_name data_file.txt`
- Multi-Threaded Program: `./program_name data_file.txt num_threads` (Default 4 threads if not provided)

Ensure proper resource management, thread synchronization, and error handling for file operations and thread creation.

---
