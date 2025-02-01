# Imperative Language Compiler

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This compiler is designed to translate source code written in a simple imperative language into assembly instructions, which are then executed by a virtual machine.
It was developed as part of the **"Formal Languages and Translation Techniques"** course, taught by **Professor Maciej Gębala** at **Wrocław University of Science and Technology** during the **2024/2025 academic year**.

The compiler is implemented in **C++**, using **Flex** for lexical analysis and **Bison** for parsing, while **Python** is used for testing.

## 🔗Quick Links
- [Language Description](#language-description)
- [Virtual Machine](#virtual-machine)
- [Dependencies](#dependencies)
- [Usage](#usage)
- [Testing](#testing)
- [License](#license)

## 📝Language Description

The language to be compiled is a simple imperative programming language with the following features:
- **Arithmetic Operations**: Addition, subtraction, multiplication, division and modulo operations
- **Control Flow Structures**: If-else statements, while loops, repeat-until loops and for loops
- **Procedures**: User-defined procedures with parameter passing by reference (IN-OUT)
- **Variables**: Integers and arrays of integers. Arrays are declared with a range of available indexes such as `tab[-10:10]` (the range does not need to include 0)
- **Input/Output**: Reading integer values from standard input and writing them to standard output
- **Comments**: Lines starting with `#` are considered comments and are ignored by the compiler

A more complete description of the language (in Polish) can be found in the [specification](Specification.pdf) file.

Example program:
```
# This program calculates the greatest common divisor of two integers using the Euclidean algorithm

PROCEDURE gcd(a, b, result) IS
x, y, remainder
BEGIN
    x := a;
    y := b;

    WHILE y != 0 DO
        remainder := x % y;
        x := y;
        y := remainder;
    ENDWHILE

    result := x;
END

PROGRAM IS
a, b, result
BEGIN
    READ a;
    READ b;
    gcd(a, b, result);
    WRITE result;
END
```

Usage: 
```bash
./compiler gcd_code.imp output_assembler.mr
<path_to_virtual_machine> output_assembler.mr
```

Input:
```bash
? 24
? 16
```

Output:
```
> 8
```

## 🌐Virtual Machine

The compiler generates code for the virtual machine, which executes a sequence of instructions. The virtual machine has the following characteristics:
- **Registers**: The virtual machine has memory cells $p_i$ where $i$ ranges from 0 to $2^{62}$. Cell $p_0$ acts as the accumulator
- **Program Counter**: The program counter $k$ holds the index of the current instruction. The machine begins execution at instruction $k = 0$ and halts when it encounters the `HALT` instruction
- **Instructions**: The virtual machine supports a set of instructions:
    - `GET i`: Read a number into memory at $p_i$, $k = k + 1$
    - `PUT i`: Output the contents of memory cell $p_i$, $k = k + 1$
    - `LOAD i`: Load the value of $p_i$ into the accumulator, $k = k + 1$
    - `LOADI i`: Load the value of $p_{p_i}$ into the accumulator, $k = k + 1$
    - `STORE i`: Store the accumulator value in $p_i$, $k = k + 1$
    - `STOREI i`: Store the accumulator value in $p_{p_i}$, $k = k + 1$
    - `ADD i`: Add the value of $p_i$ to the accumulator, $k = k + 1$
    - `ADDI i`: Add the value of $p_{p_i}$ to the accumulator, $k = k + 1$
    - `SUB i`: Subtract the value of $p_i$ from the accumulator, $k = k + 1$
    - `SUBI i`: Subtract the value of $p_{p_i}$ from the accumulator, $k = k + 1$
    - `SET x`: Set the accumulator to the value of $x$, $k = k + 1$
    - `HALF`: Divide the accumulator value by 2, $k = k + 1$
    - `JUMP j`: Jump to instruction $k + j$
    - `JPOS j`: If $p_0 > 0$, jump to instruction $k + j$; otherwise, $k = k + 1$
    - `JZERO j`: If $p_0 = 0$, jump to instruction $k + j$; otherwise, $k = k + 1$
    - `JNEG j`: If $p_0 < 0$, jump to instruction $k + j$; otherwise, $k = k + 1$
    - `RTRN i`: Set the program counter to the value stored in $p_i$
    - `HALT`: Terminate the program

The virtual machine for this project was provided by professor Gębala and it can be downloaded from [his webiste](https://ki.pwr.edu.pl/gebala/dyd/jftt2024.html).

## 📦Dependencies

This project uses `g++`, `make`, `flex` and `bison`. To install these tools on a Debian-based distribution, use the following command:
```bash
sudo apt install g++ make flex bison
```

## 🚀Usage

To compile the project, use the provided `Makefile`:
```bash
make
```

To compile source code into assembly code, run:
```bash
./compiler <source_code_file_name> <output_assembler_file_name>
```

To execute the generated assembly code, use the virtual machine:
```bash
<path_to_virtual_machine> <output_assembler_file_name>
```

## 🧪Testing

This project uses `pytest` for testing. To run the tests, ensure you have `python` and `pytest` installed. You can install `pytest` using:
```bash
pip install pytest
```

To run all the tests, use the following command:
```bash
pytest tests/
```

## 📜License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
