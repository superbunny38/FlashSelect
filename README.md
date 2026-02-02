# FlashSelect: High-Frequency Content Selection Engine

## Executive Summary

FlashSelect is a high-performance, low-latency selection engine designed to ingest, index, and retrieve content items based on keyword relevance and real-time quota availability.

The system is engineered to simulate a production-grade "hot path" service where P99 latency must remain under 10ms while handling concurrent reads (queries) and writes (quota decrements). A key architectural constraint is the seamless interoperability with a non-modifiable "Legacy System" (simulating high-debt monolithic dependencies), requiring strict RAII wrappers and memory safety guarantees.

In east words, goal: Ingest a stream of millions of user queries and select the most relevant "News Headlines" (~= Ads) within a strict latency budget, while adhering to "View Quotas" (~= Campaign Budgets).

## Domain & Problem Statement
In high-frequency content delivery (e.g., news tickers, stock alerts, real-time inventory), the system must satisfy two competing constraints:

1. **Relevance**: Finding the best matching items for a user query.
2. **Business Logic**: Enforcing strict display quotas (e.g., an item can only be shown 1000 times).
2. **Geotargeting & Metadata Enrichment**: The system must ingest a massive, static dataset of Region Mappings (e.g., 1 -> "United States", 2 -> "Canada").
- **Constraint**: Input queries may specify a target region by Name (e.g., "News in Canada"), but the internal engine must filter using Integer IDs for performance.
- **Challenge**: Efficiently loading and bidirectional mapping (Name ↔ ID) of millions of region codes at startup without slowing down the boot time.

### The Challenge**:

- **Legacy Interop**: Input data comes from a thread-unsafe, pointer-heavy C-style legacy interface.

- **Concurrency**: Multiple request threads compete for the same item quotas simultaneously.

- **Performance**: The system must utilize modern CPU cache hierarchies and minimize heap allocations during the request path.

## Technical Stack
- **Language**: C++20 (utilizing Concepts, Ranges, `std::span`, `std::atomic`).

- **Build System**: CMake 3.20+.

- **Testing**: Google Test (Unit), Google Benchmark (Performance).

- **Concurrency**: std::jthread, `std::shared_mutex` (RWLocks), Atomic arithmetic for quotas.

- **Observability**: Custom internal metrics collector (Counters, Histograms).

## System Architecture

```
graph LR
    L[Legacy Data Source] -->|Raw Pointers| W[RAII Modern Wrappers]
    W -->|Ingest| I[Inverted Index]
    U[User Query] -->|Parsed| E[Selection Engine]
    I -->|Lookup| E
    E -->|Filter/Rank| R[Result Set]
    E -->|Atomic Dec| Q[Quota Manager]
    M[Metrics Collector] -.->|Monitor| E
```

## Roadmap & Execution Plan

Phase 1: Modernization & Ingestion (Week 1)
Goal: Build a memory-safe bridge between the LegacySystem and the Modern C++20 engine.

Deliverables:

ModernItem: A wrapper class ensuring strict ownership semantics over legacy char* and raw pointers.

Indexer: An asynchronous service that builds an std::unordered_map based Inverted Index without blocking the main thread.

Day 4: The "Huge File" Parser.

- Implement RegionMapService.

- Task: Parse a CSV-style file containing ID-to-String mappings.

- Optimization: Do not read the file line-by-line using std::getline (too slow). Use memory mapping (mmap simulation or large buffer reads) and std::from_chars (C++17/20 high-performance parsing).

- Data Structure: Implement a Bidirectional Lookup:

    - ID -> Name: A dense std::vector<std::string> (since IDs are sequential integers).

    - Name -> ID: A std::unordered_map<std::string, int>.

** Milestone:** Zero memory leaks verified by Valgrind/ASan.

Phase 2: The Core Engine (Week 2)
Goal: Implement the "Hot Path" selection logic with thread safety.

Deliverables:

Query Parser: Zero-copy string tokenization using std::string_view.

Selection Pipeline: Usage of C++20 std::views to filter items by relevance and >0 quota.

Concurrency Control: Implementation of atomic operations for quota_remaining to handle race conditions without heavyweight mutex contention.

Phase 3: Scale, Stability & Observability (Week 3)
Goal: Optimization and production-readiness.

Deliverables:

Load Generator: A simulation tool firing 10k QPS (Queries Per Second).

Metrics: Real-time console reporting of Cache Hits, Latency distributions, and Quota Exhaustion rates.

Optimization: Tuning data structures for cache locality (e.g., transitioning from node-based maps to sorted vectors if profiling suggests).

## Success Metrics (KPIs)
To be considered "Production Ready," the system must meet the following Service Level Objectives (SLOs):

| Metric | Target | Description |
| :--- | :--- | :--- |
| **P99 Latency** | `< 10ms` | 99% of requests must be served in under 10 milliseconds. |
| **P50 Latency** | `< 2ms` | Median response time. |
| **Throughput** | `10,000 QPS` | System must handle 10k concurrent queries without crashing. |
| **Correctness** | `100%` | Zero "Over-delivery" (Quotas must never drop below 0 despite race conditions). |
| **Stability** | `0 Crashes` | Must survive malformed queries and empty legacy datasets. |

## Build & Run

```
# Clone
git clone https://github.com/YourUsername/FlashSelect.git
cd FlashSelect

# Configure & Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Run Tests
./bin/flash_test

# Run Benchmarks
./bin/flash_benchmark

# Run Simulation
./bin/flash_engine --threads=8 --duration=60s

```

# The "Principal Slayer" Mindset
- Read Before You Write: You said you need to interact with existing code. This is 90% of the job. In this project, I will give you a "Legacy Interface" you must use but cannot change. This simulates the monolithic codebases you will face.

- Performance is a Feature: You will obsess over CPU cycles, cache locality, and memory allocations.

- Observability is God: If your code crashes, how do I know why? If you don't implement logging, metrics, and tracing, your code is garbage.