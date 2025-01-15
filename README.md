# Compiler

## 🔗Quick Links
- [Dependencies](#dependencies)
- [Usage](#usage)

## 📦Dependencies

This project uses `flex` and `bison`. To install these tools on a Debian-based distribution, use the following commands:
```bash
sudo apt update
sudo apt install flex bison
```

## 🚀Usage

First, compile the project using the Makefile:
```bash
make
```

Then, to use the compiler, run the following command:
```bash
./compiler <input_file_name> <output_file_name>
```
