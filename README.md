# Tiler

Tiler implements a proof of concept of a matrix looping-tiling grammar.

## Introduction

Matrix operations, such as matrix-matrix multiplication and matrix convolution, are typically very compute-intensive. Implementing these operations correctly can accelerate their performance by orders of magnitude. The software engineer should consider optimal use of the cache hierarchy (through memory locality, caching intermediate values, and executing operations in the correct order), vectorization, and parallelization, among other things. 

In this project, I focus on operations that take three input matrices (A,B,C) and writes the result into the third matrix (C). An example is incremental matrix-matrix multiplication (C = C + A*B). The typical way to implement an operation like this is to manually write an optimized kernel for a small fixed-sized instance of the operation. For example, we could manually write a kernel that assumes that A, B, C are 2x2:

```
    // float* A,B,C
    // functions aOffset(), bOffset(), cOffset() that compute the offset of a matrix element
    (*(C + cOffset(0,0))) += (*(A+aOffset(0,0))) * (*(B+bOffset(0,0))) + (*(A+aOffset(0,1))) * (*(B+bOffset(1,0)));
    (*(C + cOffset(0,1))) += (*(A+aOffset(0,0))) * (*(B+bOffset(0,1))) + (*(A+aOffset(0,1))) * (*(B+bOffset(1,1)));
    (*(C + cOffset(1,0))) += (*(A+aOffset(1,0))) * (*(B+bOffset(0,0))) + (*(A+aOffset(1,1))) * (*(B+bOffset(1,0)));
    (*(C + cOffset(1,1))) += (*(A+aOffset(1,0))) * (*(B+bOffset(0,1))) + (*(A+aOffset(1,1))) * (*(B+bOffset(1,1)));
```

A good way to multiply large matrices is to tile them (say, into 2x2x2 tiles) and apply the kernel to different tile combinations. The resulting code will apply the kernel inside multiple nested loops. It is important to implement these loops in the optimal order. Also, it may be beneficial to copy parts of the original matrices within some of the loops to a temporary location, with the goal of improving memory locality. 

This library is a proof-of-concept that attempts to define a syntax for programming such loop nests. One of the goals of this library is to separate the logical loop structure (the loop pseudocode) from the code-optimization choices (such as loop order and caching decisions).


