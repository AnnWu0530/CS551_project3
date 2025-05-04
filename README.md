# mysh - A Minimal Unix Shell

## Overview

**mysh** is a custom shell designed to offer basic Unix shell functionalities in an interactive environment. This project aims to provide a lightweight and user-friendly command-line interpreter that supports:

- **Command Execution:** Running commands with arguments.
- **Background Execution:** Using `&` to execute commands in the background.
- **Signal Handling:** Managing signals like Ctrl+C, Ctrl+Z, and Ctrl+\ to ensure smooth user interactions.
- **Job Management:** Providing a `jobs` command to display and manage active background processes.
- **History Command:** Able to store previous command, search by index, and print them out.

The shell continuously accepts user input and provides feedback similar to traditional Unix shells.


## Achieved Command

### Built-in Commands
```
  cd         Change the current directory (except cd - and cd ~)
  pwd        Print the current working directory
  echo       Print arguments to stdout
  export     Set environment variables
  unset      Unset environment variables
  exit       Exit the shell (or quit)
  help       Display help about built-in commands
  alias      Change the current directory
  jobs       List active jobs
  fg         Resume job in foreground
  bg         Resume job in background
  kill       Send signal to jobs/processes
  history    Display command history, search by !(index)
```

&nbsp; cd&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Change the current directory (except cd - and cd ~)  
&nbsp; pwd&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Print the current working directory  
&nbsp; echo&nbsp;&nbsp;&nbsp;&nbsp;Print arguments to stdout  
&nbsp; export&nbsp;&nbsp;Set environment variables  
&nbsp; unset&nbsp;&nbsp;&nbsp;Unset environment variables  
&nbsp; exit&nbsp;&nbsp;&nbsp;&nbsp;Exit the shell (or quit)  
&nbsp; help&nbsp;&nbsp;&nbsp;&nbsp;Display help about built-in commands  
&nbsp; alias&nbsp;&nbsp;&nbsp;Change the current directory  
&nbsp; jobs&nbsp;&nbsp;&nbsp;&nbsp;List active jobs  
&nbsp; fg&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Resume job in foreground  
&nbsp; bg&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Resume job in background  
&nbsp; kill&nbsp;&nbsp;&nbsp;&nbsp;Send signal to jobs/processes  
&nbsp; history&nbsp;Display command history, search by !(index)  

### External Commands
All of External Command contained.


## Getting Started

### Prerequisites

Ensure your development environment has the necessary tools installed (e.g., a C/C++ compiler if applicable).

### Installation

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/AnnWu0530/CS551_project3.git
   ```
2. **Navigate to the Project Directory:**
   ```bash
   cd CS551_project3-main
   ```
3. **Build the Project:**
   Follow the build instructions provided in the documentation (e.g., using `make` or your preferred build system).


## Usage

Start the shell by running:
```bash
./mysh
```
Once launched, you can:
- Use `help` to look usable Built-in Command
- Execute commands by typing them in.
- Use `&` at the end of a command to run it in the background.
- Press Ctrl+C, Ctrl+Z, or Ctrl+\ to see how the shell handles signals.
- Use `./test_basic.sh` to test some of built-in commands after `make`. (may need `chmod +x test_basic.sh` before that to give the access right)


## Contributions (Alphabetical order)

- Annie Wu: Basic and ideal of project; executor; jobs; parser; signals; main; Functional test
- Austine Oloo: alias; builtins; completion; config; env; error; executor; glob; redirection ; main; Functional test
- Furui Xiao: builtins; main; Report; Functional test
- Xinwei Guo: history; main；Functional test


## License

MIT License


## Acknowledgements (Alphabetical order)

- Annie Wu
- Austine Oloo
- Furui Xiao
- Xinwei Guo
