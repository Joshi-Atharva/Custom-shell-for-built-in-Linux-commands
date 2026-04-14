# Custom Shell for Built-in Linux Commands

## Overview

This project implements a custom Linux shell that supports execution of built-in commands along with standard system commands. The primary objective of this project is to understand how command-line interpreters work internally, including process creation, command parsing, and interaction with the operating system.

The shell is developed as a systems programming exercise, focusing on core UNIX concepts such as process management, system calls, and command execution.

---

## Features

- Execution of built-in Linux commands
- Support for external system commands via PATH resolution
- Command parsing and tokenization
- Interactive command-line interface
- Process creation using `fork()` and execution using `exec()` family calls
- Basic error handling for invalid commands

Custom shells typically replicate behavior of standard shells like `bash` or `sh`, acting as an interface between the user and the operating system.

---

## Getting Started

### Prerequisites

- GCC or any standard C compiler
- Linux-based operating system (or WSL on Windows)

---

### Compilation

```bash
./shellMake.sh
````

or

```bash
gcc myshell.c -o shell
```

---

### Running the Shell

```bash
./shell
```

---

## Usage

Once the shell is running, you can:

* Execute standard Linux commands:

  ```bash
  ls
  pwd
  echo Hello
  ```
  or redirect input from stored files
  ```
  ./shell < <filename>
  ```
  
* Use built-in commands implemented within the shell

The shell reads user input, parses it into tokens, and executes the corresponding command using system calls.

---

## Key Concepts Covered

This project demonstrates:

* Process creation (`fork`)
* Program execution (`exec`)
* Waiting for child processes (`wait`)
* Command parsing
* File descriptors and basic I/O handling

Such concepts are fundamental to how UNIX shells operate and manage command execution.




