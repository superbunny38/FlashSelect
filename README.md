# FlashSelect: High-Frequency Content Selection Engine

## Executive Summary

FlashSelect is a high-performance, low-latency selection engine designed to ingest, index, and retrieve content items based on keyword relevance and real-time quota availability.


The system is engineered to simulate a production-grade "hot path" service where P99 latency must remain under 10ms while handling concurrent reads (queries) and writes (quota decrements). A key architectural constraint is the seamless interoperability with a non-modifiable "Legacy System" (simulating high-debt monolithic dependencies), requiring strict RAII wrappers and memory safety guarantees.

## Domain & Problem Statement
In high-frequency content delivery (e.g., news tickers, stock alerts, real-time inventory), the system must satisfy two competing constraints:

1. **Relevance**: Finding the best matching items for a user query.
2. **Business Logic**: Enforcing strict display quotas (e.g., an item can only be shown 1000 times).

**The Challenge**:

- Legacy Interop: Input data comes from a thread-unsafe, pointer-heavy C-style legacy interface.

- Concurrency: Multiple request threads compete for the same item quotas simultaneously.

- Performance: The system must utilize modern CPU cache hierarchies and minimize heap allocations during the request path.

