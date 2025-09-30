# malloc — Custom Memory Allocator

This repository contains a custom implementation of `malloc`, `free`, `realloc`, `calloc`, and related memory-management functions. It supports multiple size classes (tiny / small / large), optimized free-block lookup using red-black trees, thread-safety, debug features, and instrumentation.

## Table of Contents

- [Features](#features)
- [Design & Architecture](#design--architecture)
- [Usage](#usage)
- [Environment Variables](#environment-variables)
- [Limitations & Edge Cases](#limitations--edge-cases)
- [Testing & Debugging](#testing--debugging)
- [Contributing](#contributing)

## Features

- **Size classes**: memory is divided into *tiny*, *small*, and *large* zones based on requested size
- **Red-black tree for free blocks**: efficient O(log n) search for best-fit free blocks, replacing the previous segregated free-list approach
- **Block splitting and merging (defragmentation)**: large free blocks are split for smaller allocations; adjacent free blocks are merged to reduce fragmentation
- **Thread safety**: global and per-zone mutexes to allow safe use in multithreaded programs
- **Debug / Trace modes**: optional verbose logs, forced failures, trace file output
- **Fallback to system `free`** for pointers not allocated by this allocator (to avoid invalid free when interfacing with system libs)

## Design & Architecture

### Data structures

- `t_block`: represents an allocation block; stores size, flags (free / last block / metadata), next & prev pointers for the linked list, and additional pointers for red-black tree navigation
- `t_page`: one contiguous memory region obtained via `mmap`; stores page length, red-black tree root for free blocks, and the first block
- `t_mutex_zone`: encapsulates a zone (`tiny`, `small`, `large`) with its own pages list, mutex, size thresholds
- `g_malloc`: global metadata for all zones plus flags for debug / verbose / forced failure / trace logging

### Workflow for `malloc(size)`

1. **Determine zone:**
   - If `size` ≤ tiny threshold → tiny zone
   - Else if ≤ small threshold → small zone
   - Else → large allocation (dedicated page)

2. Within the chosen zone, search for a suitable free block using the red-black tree
   - The tree is organized by block size for efficient best-fit searches
   - Lookup complexity: O(log n) where n is the number of free blocks

3. **If no appropriate free block:**
   - Create a new page via `mmap()` sized for that zone
   - Initialize it with one large free block
   - Insert the free block into the red-black tree
   - Split that free block to satisfy the allocation if possible

4. Mark block as used, remove it from the red-black tree, and return the pointer to the user space (just after the block header)

### Freeing memory

- Validate pointer: check if it belongs to one of the pages managed by this allocator
- Check for double free
- Mark block as free and insert it into the red-black tree
- Attempt to merge with previous/next adjacent free blocks (if not disabled)
  - When merging, remove individual blocks from the tree and insert the merged block
- If the whole page is free → unmap the page with `munmap()`

### Red-Black Tree Implementation

The red-black tree ensures balanced tree operations with the following properties:
- Each node is either red or black
- The root is always black
- All leaves (NULL pointers) are black
- Red nodes cannot have red children
- Every path from root to leaf contains the same number of black nodes

This guarantees O(log n) time complexity for insertion, deletion, and search operations, significantly improving performance for workloads with many free blocks.

## Usage

### Building

```bash
git clone https://github.com/JudaNanaa/malloc.git
cd malloc
make
```

This builds `libft_malloc_$(HOSTTYPE).so` and a symlink `libft_malloc.so`.

### Using as a shared library

**Option 1: Preload technique**

```bash
LD_PRELOAD=$PWD/libft_malloc.so ./your_program
```

**Option 2: Linking with your code**

Add `-L/path/to/malloc -lft_malloc` and include headers.

## Environment Variables

| Variable | Description |
|----------|-------------|
| `MALLOC_VERBOSE=1` | Enables verbose logging on malloc/free calls |
| `MALLOC_FAIL_SIZE=N` | Forces malloc(N) to fail (returns NULL) for debug |
| `MALLOC_NO_DEFRAG=1` | Disables merging of adjacent free blocks |
| `MALLOC_TRACE_FILE=/path/to/file` | Writes trace of allocation/free activities to that file |
| `MALLOC_TINY_SIZE=N` | Set the size of tiny allocations to N |
| `MALLOC_SMALL_SIZE=N` | Set the size of small allocations to N |

## Limitations & Edge Cases

- Fragmentation can still occur, especially when many small frees and splits happen in non-patterned usage
- If you pass pointers not allocated by this allocator to `free()`, behavior is safe (it will detect invalid pointer), but performance cost for validation is present
- Some debug features slow the execution (verbose, trace file, forced failure)
- Red-black tree operations add minimal overhead but provide significant performance improvements for workloads with many allocations
- On systems with very strict kernel settings (e.g. `perf_event_paranoid` too high), tools like rr or advanced profiling may require configuration

## Performance Improvements

The implementation of red-black trees for free block management provides:
- **Faster allocation**: O(log n) search time instead of O(n) linear search
- **Better scalability**: performance degrades logarithmically rather than linearly as the number of free blocks increases
- **Consistent performance**: balanced tree structure ensures predictable operation times

## Testing & Debugging

- Use Valgrind or ASan to detect invalid reads/writes, double frees, or overflows
- Use rr for reverse debugging to trace corruption backwards
- Provide simple test programs that allocate/free in patterns (tiny/small/large, random sizes) and check memory report invariants (no overlapping blocks, total size matches sum of allocated + free, etc.)
- Benchmark tools can verify the performance improvements from the red-black tree implementation

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.