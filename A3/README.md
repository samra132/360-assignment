# ID Header:
# Student Name: Omar Ghzeil, Gurleen Samra
# Student ID:3123197, 3144680
Submission Date: June 7, 2026
File: README.md
Description: README file for CMPT 360 Assignment 3 virtual memory simulator

# CMPT 360 Assignment 3 - vmsim

## Overview

This project implements a small virtual memory simulator called `vmsim`. The program supports two modes: Base and Bounds mode and Segmentation mode. It reads memory accesses from trace files, checks whether each access is valid, prints the result for each access, and then prints summary statistics.

## Solution Logic

### Base and Bounds Mode

In Base and Bounds mode, the program treats the process as one continuous virtual address space. The command includes a base value, a limit value, and a trace file.

For each trace line, the program reads the operation and virtual address. The access is valid only if the virtual address is greater than or equal to 0 and less than the limit.

If the address is valid, the physical address is calculated as:

```text
PA = base + VA
```

If the address is outside the allowed range, the program prints a BOUNDS fault.

The program also counts the total number of valid accesses, successful accesses, and bounds faults.

### Segmentation Mode

In Segmentation mode, the program first reads a configuration file. Each segment has a name, base, limit, and permissions. The program stores these segments in a segment table.

For each trace line, the program checks:

1. Whether the segment exists.
2. Whether the offset is within bounds.
3. Whether the requested operation is allowed by the segment permissions.
4. If all checks pass, the program calculates and prints the physical address.

If the segment does not exist, the program prints a NOSEG fault. If the offset is out of range, it prints a BOUNDS fault. If the operation is not allowed by the segment permissions, it prints a PROTECTION fault.

For normal segments, the physical address is calculated as:

```text
PA = base + offset
```

For the stack segment, the program uses the required downward-growing stack rule:

```text
off_signed = raw_offset - limit
PA = base + off_signed
```

The program also prints summary statistics, including total accesses, successful accesses, bounds faults, protection faults, missing segment faults, and segment hit counts.

## Error Handling

The program handles malformed input lines without crashing. It removes comments that start with `#`, ignores blank lines, and reports malformed trace or config lines with the file name and line number.

The program checks for invalid decimal numbers in addresses, offsets, base values, and limit values.

## How to Compile

To compile the program, run:

```bash
make
```

This creates the executable:

```bash
./vmsim
```

## How to Run

### Base and Bounds Example

```bash
./vmsim --mode=bb --base=4096 --limit=64 --trace=tests/bb/t1.txt
```

### Segmentation Example

```bash
./vmsim --mode=seg --config=tests/seg/three-seg.ini --trace=tests/seg/t2.txt
```

## Testing

To run the included tests, use:

```bash
make test
```

The tests include Base and Bounds translation, malformed Base and Bounds input, segmentation translation, stack downward-growth behavior, malformed segmentation trace input, and malformed segmentation config input.

To check memory safety with Valgrind, run:

```bash
make valgrind
```

To remove the compiled executable and object files, run:

```bash
make clean
```

## Submission Status

The program compiles successfully using `make`.

Base and Bounds mode works for valid accesses, bounds faults, malformed input lines, and summary statistics.

Segmentation mode works for valid accesses, NOSEG faults, BOUNDS faults, PROTECTION faults, downward-growing stack translation, malformed trace lines, malformed config lines, and summary statistics.

The provided tests run successfully using `make test`.

Valgrind reports no memory leaks and 0 errors.


## Academic Integrity Statement

I certify that this submission represents entirely my own work.
