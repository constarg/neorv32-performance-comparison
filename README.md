# Introduction 
This repository consists of the implementation of various hash functions in the NEORV32 soft processor as Custom Fuction Unit (CFU), ALU, instructions and as Custom Function Subsystem (CFS) co-processors. This exercise was made for an MSc, and the purpose was to compare the performance of software vs hardware acceleration.

# Installation Requirements
Before compiling, please install the following packages, depending on your setup.

## Debian-based
```bash
sudo apt-get install build-essential make git gcc
```

## RHEL-based
```bash
sudo dnf group install c-development
sudo dnf group install "development-tools"
```

# Setup 
Before proceeding with the compilation of the project, there are two things that must be configured. The first is the toolchain for the **RISC-V** architecture. The second is the NEORV32 project itself.

## Tool Chain 
The project was compiled and tested using **riscv-none-elf-gcc-15.2.0-1**; please consider using this version.<br><br>

**IMPORTANT: If you do not have any RISC-V-compatible compiler on your system, you must install one; otherwise, this project will not compile.**<br><br>

To download the RISC-V toolchain used for this project, please visit: https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/<br><br>

This is a precompiled version of the required toolchain. To download and set up the compiler, you can use the following steps.

```bash
wget https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v15.2.0-1/xpack-riscv-none-elf-gcc-15.2.0-1-linux-x64.tar.gz
```
```bash
tar -xf xpack-riscv-none-elf-gcc-15.2.0-1-linux-x64.tar.gz
```
```bash
export PATH="PATH_TO_DOWNLOAD_FOLDER/xpack-riscv-none-elf-gcc-15.2.0-1-linux-x64/bin:PATH"
```

## Neorv32 
Another requirement for the project to compile is to have the NEORV32 project installed. You can install the project with the following steps. The project uses **NEORV32 v1.12.5**; therefore, I suggest using the same version. I have provided the instructions to switch to the corresponding version using the `git checkout v1.12.5` command.

```bash
git clone https://github.com/stnolting/neorv32.git
cd neorv32
git checkout v1.12.5
```

**Finally, make sure to export the following environment variable.**

```bash
NEORV32_HOME="PATH_TO_DOWNLOAD_FOLDER/neorv32/
```

## Compile 
To download and compile the project, follow the instructions below.

```bash
git clone https://github.com/constarg/neorv32-performance-comparison.git
cd neorv32-performance-comparison
make all 
```

If you want to modify the contents of the project or program using the **NEORV32** environment, I suggest executing the following command using the tool **bear**.

```bash
bear -- make all 
```

This command will produce a `compile_commands.json` file, which can be used to efficiently traverse the project through most IDEs, like Code, CLion, or Vim (if configured to do so).<br><br>

**IMPORTANT: Make sure the project is cleaned before executing the bear command; otherwise, the produced compile_commands.json file will be incorrect!**.



