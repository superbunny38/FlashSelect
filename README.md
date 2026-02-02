# FlashSelect: High-Frequency Content Selection Engine

## Executive Summary

FlashSelect is a high-performance, low-latency selection engine designed to ingest, index, and retrieve content items based on keyword relevance and real-time quota availability.


The system is engineered to simulate a production-grade "hot path" service where P99 latency must remain under 10ms while handling concurrent reads (queries) and writes (quota decrements). A key architectural constraint is the seamless interoperability with a non-modifiable "Legacy System" (simulating high-debt monolithic dependencies), requiring strict RAII wrappers and memory safety guarantees.

## Domain & Problem Statement
In high-frequency content delivery (e.g., news tickers, stock alerts, real-time inventory), the system must satisfy two competing constraints:

1. **Relevance**: Finding the best matching items for a user query.
2. **Business Logic**: Enforcing strict display quotas (e.g., an item can only be shown 1000 times).

### The Challenge**:

- **Legacy Interop**: Input data comes from a thread-unsafe, pointer-heavy C-style legacy interface.

- **Concurrency**: Multiple request threads compete for the same item quotas simultaneously.

- **Performance**: The system must utilize modern CPU cache hierarchies and minimize heap allocations during the request path.

## Technical Stack
- Language: C++20 (utilizing Concepts, Ranges, `std::span`, `std::atomic`).

- Build System: CMake 3.20+.

- Testing: Google Test (Unit), Google Benchmark (Performance).

- Concurrency: std::jthread, `std::shared_mutex` (RWLocks), Atomic arithmetic for quotas.

- Observability: Custom internal metrics collector (Counters, Histograms).

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