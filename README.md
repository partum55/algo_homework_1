# Algorithms & Data Structures: Homework 1

## Author: [Nazar Mykhailyshchuk](https://github.com/partum55)

## Overview

This project implements and compares different data structures for managing student records, focusing on performance analysis and optimization techniques. The project measures three key operations across different data structure implementations:

1. **Operation 1**: Find the group with the most students
2. **Operation 2**: Change a student's group by email
3. **Operation 3**: Find the group with the highest average rating

## Implementations

### Data Structures

#### Hash Table Implementation (`standart/hash/`)
Uses an unordered hash map (`unordered_map`) for O(1) average-case lookup, insertion, and deletion. Best for scenarios requiring fast individual student lookups by email.

**Advantages:**
- Fast email-based lookups
- Efficient for change operations
- Good for sparse data

**Disadvantages:**
- Group-based operations require full iteration
- No natural ordering
- Potential hash collisions

#### Vector Implementation (`standart/vector/`)
Uses a dynamic array (`vector`) to store all students sequentially. Simple and cache-friendly but requires linear search.

**Advantages:**
- Cache-friendly memory layout
- Simple implementation
- Good memory locality

**Disadvantages:**
- O(n) search complexity
- Inefficient for large datasets
- Linear time for most operations

#### AVL Tree Implementation (`standart/avl/`)
Uses a self-balancing binary search tree. Maintains sorted order and guarantees O(log n) operations.

**Advantages:**
- Guaranteed O(log n) operations
- Maintains sorted order
- Balanced structure

**Disadvantages:**
- More complex implementation
- Extra memory for tree pointers
- Rebalancing overhead

#### Optimized Implementation (`optimized/`)
Enhanced version with optimizations:
- Pre-computed caching for group statistics
- Efficient memory layout
- Optimized algorithms for frequent operations
- Reduced redundant computations

**Target:** Minimize computational overhead and maximize throughput for mixed workloads.

## How to use

### Non-optimsed

#### Using hash table
```bash
cd standart/hash
g++ measure.cpp -o main
./main
```

#### Using only vector
```bash
cd standart/vector
g++ measure.cpp -o main
./main
```

#### Using AVL tree
```bash
cd standart/avl
g++ measure.cpp -o main
./main
```

### Optimized
```bash
cd optimized
g++ measure_optimized.cpp -o main
./main
```

### Demo
```bash
cd standart/hash
g++ demo.cpp -o demo
./demo
```

### Compare sort funcstions
```bash
cd standart/hash
g++ compare.cpp -o compare
./compare
```

### Clear
```bash
rm standart/vector/main standart/hash/main standart/avl/main standart/hash/demo optimized/main standart/hash/students_sorted.csv standart/hash/compare
```

## How to run plots

### Quick Start (Automated):

**This script will automatically compile all implementations, run measurements, collect data, and generate plots:**

```bash
python3 run_all_measurements.py
```

This will:
1. Compile and run all 4 implementations (Hash Table, Vector, AVL Tree, Optimized)
2. Collect performance metrics (operations/second, total operations, execution time)
3. Generate comprehensive comparison plots
4. Create a detailed performance report
5. Save all results in the `results/` directory

### Manual Setup:

If you want to run the plotting separately:

#### Install dependencies:
```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

#### Run automated measurement and plotting:
```bash
source .venv/bin/activate
python3 run_all_measurements.py
```

### Output

The script generates:
- **`results/performance_comparison.png`** - Main comparison charts
- **`results/operations_comparison.png`** - Individual operation breakdown
- **`results/performance_report.txt`** - Detailed text report
- **`results/measurement_results.json`** - Raw data in JSON format
- PDF versions of all plots

![performance_comparison.png](results/performance_comparison.png)
![operations_comparison.png](results/operations_comparison.png)

(c) By Nazar Mykhailyshchuk
