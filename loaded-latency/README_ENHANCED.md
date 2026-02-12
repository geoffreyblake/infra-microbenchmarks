# loaded-latency: Memory Latency and Bandwidth Testing Tool

## Overview
loaded-latency is a tool for measuring memory latency under various bandwidth load conditions. It can run latency measurement threads alongside bandwidth-generating threads to characterize memory subsystem behavior under contention.

## Recent Enhancements (Feb 2026)

### New Bandwidth Operations
The tool now supports 14 different bandwidth operation patterns:

**Basic Operations:**
- `read` - Pure memory read operations
- `write` - Pure memory write operations  
- `memcpy` - Memory copy (read + write)
- `memset1` - Write 1 byte per cache line
- `memset64` - Write full 64-byte cache lines

**Mixed Read/Write Operations (aarch64 only):**
- `mix5w` - 5% writes, 95% reads
- `mix10w` - 10% writes, 90% reads
- `mix15w` - 15% writes, 85% reads
- `mix20w` - 20% writes, 80% reads
- `mix25w` - 25% writes, 75% reads
- `mix30w` - 30% writes, 70% reads
- `mix35w` - 35% writes, 65% reads
- `mix40w` - 40% writes, 60% reads
- `mix50w` - 50% writes, 50% reads

### New Features
- **Configurable Stride**: Control memory access stride with `--bw-stride`
- **Random Jumps**: Add pseudo-random access patterns with `--bw-random-jump`
- **Byte Tracking**: Accurate tracking of bytes read vs written

## Building

```bash
make
```

Requirements:
- GCC compiler
- pthread library
- Linux on aarch64 or x86_64

## Usage

### Basic Syntax
```bash
./loaded-latency [options]
```

### Common Options

**General:**
- `-D, --duration <seconds>` - How long to run (default: 10)
- `-d, --delay-seconds <seconds>` - Thread synchronization delay
- `-f, --cpu-freq-mhz <freq>` - CPU frequency in MHz
- `-t, --cpu-cycle-time-ns <ns>` - CPU cycle time in nanoseconds

**Latency Thread Options:**
- `-l, --lat-cpu <cpu>` - CPU for latency thread (repeat for multiple)
- `-n, --lat-cacheline-count <count>` - Memory size in cache lines
- `-i, --lat-iterations <iters>` - Iterations between reports
- `-r, --lat-randomize` - Randomize dependent load ordering
- `-z, --lat-cacheline-bytes <bytes>` - Cache line size (default: 64)
- `-h, --lat-use-hugepages <size>` - Use hugepages

**Bandwidth Thread Options:**
- `-B, --bw-cpu <cpu>` - CPU for bandwidth thread (repeat for multiple)
- `-L, --bw-buflen <bytes>` - Memory buffer size (default: 8MB)
- `-I, --bw-iterations <iters>` - Iterations between reports
- `-O, --bw-operation <op>` - Operation type (see list above)
- `-F, --bw-fine-delay <count>` - Inner loop nops (reduces bandwidth)
- `-C, --bw-coarse-delay <count>` - Outer loop nops (reduces bandwidth)
- `--bw-stride <bytes>` - Stride in bytes (default: 64)
- `--bw-random-jump <freq>` - Jump to random location every N iterations
- `-H, --bw-use-hugepages <size>` - Use hugepages
- `-Z, --bw-cacheline-bytes <bytes>` - Cache line size

## Examples

### 1. Basic Latency Measurement
Measure latency on CPU 0 with no bandwidth load:
```bash
./loaded-latency --lat-cpu 0 --lat-iterations 100000 -D 10
```

### 2. Latency Under Read Bandwidth Load
Measure latency on CPU 0 while CPU 1 generates read bandwidth:
```bash
./loaded-latency --lat-cpu 0 --lat-iterations 100000 \
                 --bw-cpu 1 --bw-operation read \
                 -D 10
```

### 3. Mixed Read/Write Pattern
Test with 25% writes, 75% reads:
```bash
./loaded-latency --lat-cpu 0 --lat-iterations 100000 \
                 --bw-cpu 1 --bw-operation mix25w \
                 -D 10
```

### 4. Custom Stride Pattern
Test with 128-byte stride:
```bash
./loaded-latency --lat-cpu 0 --lat-iterations 100000 \
                 --bw-cpu 1 --bw-operation mix50w \
                 --bw-stride 128 \
                 -D 10
```

### 5. Random Access Pattern
Add random jumps every 100 iterations:
```bash
./loaded-latency --lat-cpu 0 --lat-iterations 100000 \
                 --bw-cpu 1 --bw-operation mix20w \
                 --bw-random-jump 100 \
                 -D 10
```

### 6. Multiple Bandwidth Threads
Run bandwidth on multiple CPUs:
```bash
./loaded-latency --lat-cpu 0 --lat-iterations 100000 \
                 --bw-cpu 1 --bw-cpu 2 --bw-cpu 3 \
                 --bw-operation write \
                 -D 10
```

### 7. Large Memory Test with Hugepages
Use 64MB latency loop with 2MB hugepages:
```bash
./loaded-latency --lat-cpu 0 \
                 --lat-cacheline-count $((64*1024*1024/64)) \
                 --lat-use-hugepages 2MB \
                 --bw-cpu 1 --bw-buflen $((96*1024*1024)) \
                 --bw-use-hugepages 2MB \
                 -D 10
```

### 8. Comprehensive Test
Full-featured test with all options:
```bash
./loaded-latency \
  --lat-cpu 0 --lat-iterations 10000 --lat-randomize \
  --lat-cacheline-count $((32*1024*1024/64)) \
  --bw-cpu 1 --bw-operation mix35w \
  --bw-stride 192 --bw-random-jump 75 \
  --bw-buflen $((64*1024*1024)) \
  --bw-iterations 50 \
  -D 30
```

## Output

The tool outputs:
1. **Configuration summary** - All parameters used
2. **Per-thread startup messages** - Thread initialization info
3. **Periodic reports**:
   - Latency threads: Average latency in nanoseconds and cycles
   - Bandwidth threads: Bandwidth in MB/sec
4. **Final summary** - Average metrics across entire run

Example output:
```
CPU0 LATTHREAD0: 10.5 ns, 27.3 cycles
CPU1 BWTHREAD0: 25432.5 MB/sec
```

## Performance Tips

### Maximizing Bandwidth
- Use `--bw-stride` values that match cache line size or larger
- Disable random jumps (`--bw-random-jump 0`)
- Use hugepages for large buffers
- Set inner/outer nops to 0

### Reducing Bandwidth
- Increase `--bw-fine-delay` (inner loop nops)
- Increase `--bw-coarse-delay` (outer loop nops)
- Enable frequent random jumps

### Accurate Latency Measurement
- Use `--lat-randomize` to prevent prefetching
- Ensure sufficient iterations for stable averages
- Pin threads to specific CPUs
- Consider NUMA topology when selecting CPUs

## Architecture Notes

### aarch64 (ARM64)
- Full support for all operations
- Uses CNTVCT for timing
- Mixed operations use ldr/str instructions
- Optimized with ldp/stp for memcpy

### x86_64
- Supports: read, write, memcpy, memset1, memset64
- Mixed operations (mix5w-mix50w) not implemented
- Uses RDTSC for timing
- Some operations may need testing/optimization

## Troubleshooting

### Build Issues
- Ensure GCC and pthread are installed
- Check architecture is supported (aarch64 or x86_64)

### Runtime Issues
- **Permission denied**: May need root for CPU pinning
- **Invalid CPU**: Check CPU numbers with `lscpu`
- **Hugepage errors**: Ensure hugepages are configured in kernel
- **Low bandwidth**: Check for thermal throttling or power management

### Unexpected Results
- Verify CPU frequency is correct (`-f` option)
- Check for background processes interfering
- Consider NUMA effects on multi-socket systems
- Ensure CPUs are not in power-saving mode

## Files

- `main.c` - Main program, thread management
- `bandwidth.c/h` - Bandwidth operation implementations
- `memlatency.c/h` - Latency measurement implementations
- `args.c/h` - Command-line argument parsing
- `alloc.c/h` - Memory allocation with hugepage support
- `cntvct.h` - ARM counter access
- `rdtsc.h` - x86 TSC access

## License

SPDX-License-Identifier: BSD-3-Clause
Copyright 2019-2023 Arm Limited and/or its affiliates

## References

- Original loaded-latency tool
- cpu_loaded_latency (source of enhanced bandwidth operations)
- ENHANCEMENTS_SUMMARY.md (detailed implementation notes)
