# malloc — Custom Memory Allocator

This repository contains a custom implementation of `malloc`, `free`, `realloc`, `calloc`, and related memory-management functions. It supports multiple size classes (tiny / small / large), free-lists by size, thread-safety, debug features, and instrumentation.

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
- **Segregated free lists**: each page maintains free-lists indexed by classes of size; fast lookup for free blocks
- **Block splitting and merging (defragmentation)**: large free blocks are split for smaller allocations; adjacent free blocks are merged to reduce fragmentation
- **Thread safety**: global and per-zone mutexes to allow safe use in multithreaded programs
- **Debug / Trace modes**: optional verbose logs, forced failures, trace file output
- **Fallback to system `free`** for pointers not allocated by this allocator (to avoid invalid free when interfacing with system libs)

## Design & Architecture

### Data structures

- `t_block`: represents an allocation block; stores size, flags (free / last block / metadata), next & prev pointers for free lists
- `t_free_list`: per-page or per-zone free-list by size class; holds `head[]` and `last[]` pointers
- `t_page`: one contiguous memory region obtained via `mmap`; stores page length, free lists, and the first block
- `t_mutex_zone`: encapsulates a zone (`tiny`, `small`, `large`) with its own pages list, mutex, size thresholds
- `g_malloc`: global metadata for all zones plus flags for debug / verbose / forced failure / trace logging

### Workflow for `malloc(size)`

1. **Determine zone:**
   - If `size` ≤ tiny threshold → tiny zone
   - Else if ≤ small threshold → small zone
   - Else → large allocation (dedicated page)

2. Within the chosen zone, attempt to find a free block of adequate size using segregated free lists

3. **If no appropriate free block:**
   - Create a new page via `mmap()` sized for that zone
   - Initialize it with one large free block
   - Split that free block to satisfy the allocation if possible

4. Mark block as used and return the pointer to the user space (just after the block header)

### Freeing memory

- Validate pointer: check if it belongs to one of the pages managed by this allocator
- Check for double free
- Mark block as free and add it to the appropriate free list
- Attempt to merge with previous/next adjacent free blocks (if not disabled)
- If the whole page is free → unmap the page with `munmap()`

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
- On systems with very strict kernel settings (e.g. `perf_event_paranoid` too high), tools like rr or advanced profiling may require configuration

## Testing & Debugging

- Use Valgrind or ASan to detect invalid reads/writes, double frees, or overflows
- Use rr for reverse debugging to trace corruption backwards
- Provide simple test programs that allocate/free in patterns (tiny/small/large, random sizes) and check memory report invariants (no overlapping blocks, total size matches sum of allocated + free, etc.)

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
