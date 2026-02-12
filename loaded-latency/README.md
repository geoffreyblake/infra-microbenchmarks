# loaded-latency Benchmark

**SPDX-FileCopyrightText**: Copyright 2019-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>  
**SPDX-License-Identifier**: BSD-3-Clause

## Overview

The `loaded-latency` benchmark measures memory latency against a controlled amount of concurrent memory bandwidth in a single measurement. It supports multiple bandwidth operation patterns, configurable memory access patterns, and advanced features for realistic workload simulation.

## Quick Start

### Building

```bash
make
```

Requirements:
- GCC compiler
- pthread library
- Linux system (aarch64 or x86_64)

### Basic Usage Examples

**Latency-only test:**
```bash
./loaded-latency --lat-cpu 0 --duration 5
```

**Bandwidth-only test:**
```bash
./loaded-latency --bw-cpu 1 --bw-operation read --duration 5
```

**Combined latency and bandwidth:**
```bash
./loaded-latency --lat-cpu 0 --lat-randomize \
                 --bw-cpu 1 --bw-operation write \
                 --duration 10
```

**Mixed read/write with custom stride:**
```bash
./loaded-latency --bw-cpu 1 --bw-operation mix25w \
                 --bw-stride 128 --duration 5
```

**Random access pattern:**
```bash
./loaded-latency --bw-cpu 1 --bw-operation read \
                 --bw-random-jump 100 --duration 5
```

## Features

### Bandwidth Operations

The tool supports multiple bandwidth operation types via `--bw-operation`:

| Operation | Description | Architecture |
|-----------|-------------|--------------|
| `read` | Pure read operations | aarch64, x86_64 |
| `write` | Pure write operations | aarch64, x86_64 |
| `memcpy` | Copy operations (read + write) | aarch64, x86_64 |
| `memset1` | 1 byte write per cache line | aarch64, x86_64 |
| `memset64` | Full cache line writes | aarch64, x86_64 |
| `mix5w` | 5% writes, 95% reads | aarch64 only |
| `mix10w` | 10% writes, 90% reads | aarch64 only |
| `mix15w` | 15% writes, 85% reads | aarch64 only |
| `mix20w` | 20% writes, 80% reads | aarch64 only |
| `mix25w` | 25% writes, 75% reads | aarch64 only |
| `mix30w` | 30% writes, 70% reads | aarch64 only |
| `mix35w` | 35% writes, 65% reads | aarch64 only |
| `mix40w` | 40% writes, 60% reads | aarch64 only |
| `mix50w` | 50% writes, 50% reads | aarch64 only |

### Memory Access Patterns

**Configurable Stride:**
- `--bw-stride <bytes>`: Set stride through memory (default: 64)
- Allows testing different cache line access patterns
- Each operation accesses one 64-bit word per stride

**Pseudo-Random Jumps:**
- `--bw-random-jump <freq>`: Jump to random location every N iterations
- Uses lightweight Linear Congruential Generator (LCG)
- Enables random access pattern testing
- Set to 0 to disable (default)

### CPU Frequency Auto-Detection

The tool now automatically estimates CPU frequency if not specified:
- Runs a brief calibration on CPU 0
- Rounds to nearest 100 MHz
- Can be overridden with `-f` or `-t` flags

## Command-Line Options

### General Flags

```
-D | --duration              seconds     How many seconds to run
-S | --random-seed           seedval     Set random seed to seedval
-d | --delay-seconds         seconds     Seconds to wait for threads to start together
     --delay-ticks           ticks       HWCOUNTER ticks to wait for threads to start
     --show-per-thread-concurrency       Show per-thread concurrency metrics
-Q | --mitigate-spectre-v4               Enable Spectre v4 mitigation (SSBD=1 or SSBS=0)
-q | --hwclock-freq          freq_hz     Frequency in Hz of hwclock counter
     --estimate-hwclock-freq cpu_num     Estimate hardware clock frequency on CPU
-f | --cpu-freq-mhz          frequency   CPU frequency in MHz for calculating cycles
-t | --cpu-cycle-time-ns     nanoseconds CPU cycle time in nanoseconds
```

### Latency Flags

```
-l | --lat-cpu               cpu_num     CPU for latency thread (repeat for multiple)
-n | --lat-cacheline-count   count       Number of cachelines for latency measurement
-e | --lat-secondary-delay   ticks       Additional ticks for secondary latency threads
-i | --lat-iterations        iters       Iterations between interim reports
-z | --lat-cacheline-bytes   bytes       Cacheline length for latency measurement
-j | --lat-cacheline-stride  count       Cachelines to skip between loads
-o | --lat-offset            count       Deploads to advance secondary threads
-c | --lat-clear-cache                   Clear caches before latency run
-r | --lat-randomize                     Randomize ordering of dependent loads
-h | --lat-use-hugepages     size        Hugepage size (use "-h help" for sizes)
-w | --lat-warmup-cpu        cpu_num     CPU to warm up latency loop
-s | --lat-shared-memory                 Use same memory for all latency threads
-u | --lat-shared-memory-init-cpu cpu    CPU to initialize shared memory
```

### Bandwidth Flags

```
-B | --bw-cpu                cpu_num     CPU for bandwidth thread (repeat for multiple)
-L | --bw-buflen             bytes       Memory buffer size for bandwidth loop
-I | --bw-iterations         iters       Iterations between interim reports
-F | --bw-fine-delay         count       Inner loop nops (increase to slow bandwidth)
-C | --bw-coarse-delay       count       Outer loop nops (increase to slow bandwidth)
-H | --bw-use-hugepages      size        Hugepage size (use "-H help" for sizes)
-Z | --bw-cacheline-bytes    bytes       Cacheline length for bandwidth region
-O | --bw-operation          op          Operation type (see table above)
     --bw-stride             bytes       Stride in bytes (default: 64)
     --bw-random-jump        freq        Random jump every N iterations (0=disabled)
```

## Advanced Usage Examples

### Latency vs Bandwidth Characterization

**64MB latency loop with 96MB bandwidth:**
```bash
./loaded-latency --lat-cpu 0 \
                 --lat-cacheline-count $((64*1024*1024/64)) \
                 --lat-iterations 100000 \
                 --lat-randomize \
                 --bw-cpu 1 \
                 --bw-buflen $((96*1024*1024)) \
                 --bw-fine-delay 100 \
                 --bw-iterations 30
```

### Multiple Bandwidth Threads

**4 bandwidth threads with mixed operations:**
```bash
./loaded-latency --lat-cpu 0 --lat-randomize \
                 --bw-cpu 1 --bw-cpu 2 --bw-cpu 3 --bw-cpu 4 \
                 --bw-operation mix30w \
                 --bw-stride 128 \
                 --duration 10
```

### Hugepage Support

**Using 2MB hugepages:**
```bash
# First, allocate hugepages
sudo hugeadm --pool-pages-max 2M:100

# Run with hugepages
./loaded-latency --lat-cpu 0 \
                 --lat-use-hugepages 2M \
                 --lat-cacheline-count $((100*1024*1024/64))
```

### Shared Memory Latency

**Multiple latency threads sharing memory:**
```bash
./loaded-latency --lat-cpu 0 --lat-cpu 1 --lat-cpu 2 \
                 --lat-shared-memory \
                 --lat-shared-memory-init-cpu 0 \
                 --lat-offset 1000 \
                 --duration 10
```

### Random Access Patterns

**Bandwidth with random jumps:**
```bash
./loaded-latency --bw-cpu 1 \
                 --bw-operation mix20w \
                 --bw-stride 256 \
                 --bw-random-jump 50 \
                 --bw-buflen $((200*1024*1024)) \
                 --duration 10
```

### Bandwidth Throttling

**Controlled bandwidth with delays:**
```bash
./loaded-latency --bw-cpu 1 \
                 --bw-operation read \
                 --bw-fine-delay 50 \
                 --bw-coarse-delay 10 \
                 --duration 5
```

## Running the Provided Scripts

### Latency-Only Measurement
```bash
./run-200mb.latency-only.sh
```
Measures random memory latency over 200MB from CPU1.

### Bandwidth-Only Measurement
```bash
./run-200mb.bandwidth-only.sh
```
Measures/generates bandwidth loading on 200MB from CPU0.

### Combined Latency and Bandwidth
```bash
./run-200mb.bandwidth-latency.sh
```
Measures bandwidth on CPU0 and latency on CPU1 simultaneously.

### Bandwidth Scaling
```bash
./bandwidth-scaling.sh
```
Demonstrates bandwidth scaling across multiple CPUs.

### Latency vs Bandwidth Sweep
```bash
./sweep.finedelay.sh -B{0..14} | tee results.log
./summarize.sh results.log
```
Sweeps through different bandwidth levels and measures latency impact.

## Understanding Output

### Interim Measurements

During execution, threads report interim performance:
```
CPU0 LATTHREAD0: 113.882800 ns, 455.531200 cycles
CPU1 BWTHREAD0: 15124.239562 MB/sec
```

### Final Summary

```
Total Bandwidth = 60437.699393 MB/sec
Average Latency = 113.839950 ns
```

### Concurrency Coverage Metrics

Shows how well threads synchronized:
```
bw_hwcounter_start_max  = 0xeb270c62182bb
bw_hwcounter_start_min  = 0xeb270c62182ad     max-min diff = 14 (0.000000 seconds)
lat_hwcounter_start_max = 0xeb270c62182b4
lat_hwcounter_start_min = 0xeb270c62182b4     max-min diff = 0 (0.000000 seconds)

bw_lat_start_spread_ticks = -7 (-0.000000 seconds)
bw_lat_stop_spread_ticks  = -379172653 (-0.110947 seconds)
```

Smaller spreads indicate better thread synchronization.

## Performance Considerations

### Operation Performance (Typical)

On modern ARM systems:
- **read**: 20-30 GB/sec
- **write**: 20 GB/sec
- **memcpy**: 15-20 GB/sec (combined read+write)
- **memset1**: 40-50 GB/sec
- **memset64**: 50-60 GB/sec
- **mix operations**: 20-30 GB/sec (varies by write %)

### Factors Affecting Performance

1. **Stride**: Larger strides can increase bandwidth by reducing memory pressure
2. **Random jumps**: Reduce bandwidth due to cache misses and reduced prefetching
3. **Delays**: Fine and coarse delays throttle bandwidth for controlled testing
4. **Hugepages**: Can improve performance by reducing TLB misses
5. **CPU affinity**: Proper CPU selection affects NUMA performance

## Architecture Support

- **aarch64**: Full support for all operations
- **x86_64**: Supports read, write, memcpy, memset1, memset64
  - Mixed operations (mix5w-mix50w) are aarch64 only

## Known Limitations

1. CPU frequency is auto-detected but may need manual override with `-f` flag
2. Hardware clock frequency may need override with `-q` flag on some x86_64 systems
3. Same CPU cannot run multiple threads of same type (but can run both latency and bandwidth)
4. Hugepage sizes are hard-coded and not filtered against system support
5. Cacheline sizes other than 64 bytes have not been extensively tested
6. No built-in NUMA memory policy control (use `numactl` wrapper)

## Troubleshooting

### Threads Not Starting Together

If `bw_lat_start_spread_ticks` is large, increase delay:
```bash
./loaded-latency --delay-seconds 10 ...
```

### CPU Frequency Issues

Override auto-detection:
```bash
./loaded-latency --cpu-freq-mhz 2600 ...
```

### Hardware Clock Frequency

Estimate on your system:
```bash
./loaded-latency --estimate-hwclock-freq 0
```

Then use the result:
```bash
./loaded-latency --hwclock-freq 3417600000 ...
```

### Hugepage Allocation Failed

Check available hugepages:
```bash
hugeadm --pool-list
```

Allocate more:
```bash
sudo hugeadm --pool-pages-max 2M:100
```

## Technical Details

### Memory Latency Measurement

- Traverses circular loop of dependent pointers (one per cache line)
- Computes average time per pointer dereference
- Supports randomized ordering with `--lat-randomize`
- Can use shared memory across threads with `--lat-shared-memory`

### Bandwidth Generation

- Sequential or strided access through contiguous memory
- Optional pseudo-random jumps using LCG PRNG
- Configurable delays for bandwidth throttling
- Accurate byte tracking for read/write operations

### Thread Synchronization

- Uses hardware counter (CNTVCT_EL0 on ARM, TSC on x86) for timing
- Threads poll counter to start simultaneously
- No barriers used (to avoid synchronization overhead)
- Concurrency metrics show actual overlap

## Additional Documentation

- `ENHANCEMENTS_SUMMARY.md` - Detailed enhancement history
- `MIXED_RW_IMPLEMENTATION.md` - Mixed read/write pattern details
- `RANDOM_JUMP_FEATURE.md` - Random jump feature documentation
- `README.txt` - Original comprehensive documentation

## License

BSD-3-Clause - See file headers for details.

## Contributing

This is an Arm Limited project. For issues or contributions, contact open-source-office@arm.com.
