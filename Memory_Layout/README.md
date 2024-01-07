## Memory System Assignment

### Introduction
Simulate a simple stack and heap memory system with specific constraints. The memory layout, stack, and heap specifications are defined, and various tasks related to frame creation, deletion, and memory allocation are outlined.

### How to Run
- Execute the program and you will get an interactive shell-like environment.
- Here, you will write commands to create frames, delete functions, and manage variables on both stack and heap.
- The program maintains a character data structure of 500 bytes for stack and heap data and metadata.

### Command Functionalities And Syntax
- CF func_name func_addr: Create a frame.
- DF: Delete the top frame.
- CI int_name int_value: Create an integer.
- CD double_name double_value: Create a double.
- CC char_name char_value: Create a char.
- CH buffer_name size: Create a char buffer on the heap.
- DH buffer_name: Deallocate heap memory.
- SM: Show stack, heap, free, and allocated memory snapshots.

### Functionalities
- Create and delete frames on the stack.
- Manage integer, double, and character variables in the stack frames.
- Allocate and deallocate character buffers on the heap.
- Display stack and heap snapshots.

