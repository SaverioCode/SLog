# Project Roadmap

**Goal**: Transform Simple-Logger into a World-Class C++20 Library.

## Phase 1: Foundation & Refactoring
- [x] **Analysis**: Codebase review and defect identification.
- [/] **Architecture**: Deep Design Phase (Iteration 6 of ~15).
    - [ ] Deep Dive: MPSC vs MMapQueue internals.
    - [ ] Deep Dive: Router Logic & Policy.
    - [ ] Deep Dive: Formatting Engine (Code generation/Templates).
    - [ ] Deep Dive: API & Macros (Pre-processor logic).
- [ ] **Restructuring**: Split existing `slog.hpp` into modular files (`src/api`, `src/impl`).
- [ ] **Distribution Script**: Create Python script to generate the STB-style single header.
- [ ] **Tooling**: Setup CI for C++11 and C++20 Testing matrix.

## Phase 2: Core Implementation
- [ ] **Formatting Engine**: Implement `LogFormatter` using `std::format` (or vendor).
- [ ] **CrashGuard**: Implement Signal Handlers and Manual Queue Drain.
- [ ] **Registry System**: Implement `LoggerRegistry` and `SinkRegistry` with Factory patterns.
- [ ] **Interface**: Create the C++11 compatible public HTTP API.

## Phase 3: Reliability & Performance
- [ ] **MPSC Queue**: Finalize Wait-Free implementation and benchmarks.
- [ ] **Fuzzing**: Integrate Google OneFuzz or LLVM LibFuzzer.
- [ ] **Sanitizers**: clean TSan/ASan runs.

## Phase 4: Documentation
- [ ] **Wiki**: Create "0 to Hero" guides.
- [ ] **Diagrams**: Host Mermaid diagrams in repo.
- [ ] **Benchmarks**: Publish comparison graphs vs spdlog/glog.
