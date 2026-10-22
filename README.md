# Project 01: UniqueFd — A RAII File Descriptor Wrapper

A small, production-style C++20 systems programming exercise for Linux.

**Status:** Learning project / implementation in progress.

## Goal

Implement a move-free, non-copyable `sys::UniqueFd` wrapper that exclusively owns a Linux file descriptor and closes it when the wrapper is destroyed. The purpose is to practice object lifetime, resource ownership, encapsulation, separate compilation, automated testing, and disciplined CMake builds.

**Scope for this iteration:** constructors, destructor, deleted copying, observers, and explicit ownership release. **Do not implement move construction or move assignment yet**; those are reserved for the next lesson. The project is intentionally small, not a general-purpose file I/O or networking library.

## API Contract

Public header: `include/sys/unique_fd.hpp`

```cpp
#pragma once

namespace sys {

class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) noexcept;
    ~UniqueFd();

    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    [[nodiscard]] int get() const noexcept;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] int release() noexcept;

private:
    int fd_;
};

} // namespace sys
```

### Required behavior

| Operation | Contract |
| --- | --- |
| `UniqueFd()` | Owns no descriptor; `get() == -1`, `valid() == false`. |
| `UniqueFd(fd)` | Adopts ownership of a valid, already-open descriptor. The caller must not independently close it while owned. |
| `get()` | Returns the stored descriptor without transferring ownership. |
| `valid()` | Returns whether the wrapper holds a descriptor (`fd_ != -1`); it does not check whether an external actor closed it. |
| `release()` | Returns the stored descriptor and leaves the wrapper empty. It **must not** call `close()`. The caller becomes responsible for closing the returned descriptor. |
| `~UniqueFd()` | Calls `close()` exactly once if it still holds a descriptor; does nothing if empty. Must not throw. |
| Copy construction/assignment | Forbidden at compile time to prevent double ownership. |

**Preconditions and limitations:** On construction, `fd` must be either `-1` or an open Linux file descriptor whose ownership can be transferred. Passing another negative integer, an already-closed fd, or adopting the same fd into two wrappers violates the intended contract. This iteration does not support ownership transfer between `UniqueFd` objects; use `release()` only when explicitly passing responsibility to a caller. `get()` is a borrowed view, not an owning handle. External code must not close a descriptor while `UniqueFd` owns it. Do not retry `close()` in the destructor after an `EINTR` result on Linux, because the descriptor number may already have been released and reused.

## Repository Layout

```text
01-unique-fd/
├── CMakeLists.txt
├── include/sys/unique_fd.hpp
├── src/unique_fd.cpp
├── examples/main.cpp
├── tests/unique_fd_test.cpp
├── .gitignore
└── README.md
```

Separate the public header, implementation, example application, and tests. Keep `main()` limited to demonstrating use; it must not contain the library implementation. Avoid unnecessary frameworks, inheritance, templates, singletons, or global state.

## Build and Test Requirements

Use CMake 3.20+ and C++20. Define three targets:

- `unique_fd`: a static library containing the implementation;
- `unique_fd_demo`: a standalone example linked against the library;
- `unique_fd_test`: an automated test executable linked against the library and registered with CTest.

Use target-scoped include directories, compile features, and link dependencies. Enable `-Wall -Wextra -Wpedantic` for project code and aim for a warning-free build on GCC or Clang.

Expected developer commands (after implementing the project):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Add a documented way to compile and run **AddressSanitizer (ASan)** and **UndefinedBehaviorSanitizer (UBSan)** builds. Sanitizers complement tests; a clean sanitizer run is not proof of correctness. Generated build files should be ignored by Git. There is no requirement for an external testing framework; a small test executable that returns nonzero on failure is sufficient. Avoid relying solely on `assert()` for tests if `NDEBUG` may disable it.

## Required Tests

1. **Empty wrapper:** default construction reports `-1` and invalid status; destruction has no effect on unrelated descriptors.
2. **Adoption:** constructing from a descriptor returned by `open("/dev/null", O_RDONLY)` reports the same descriptor and valid status.
3. **Scope exit:** after a wrapper goes out of scope, the owned descriptor is closed. In a controlled single-threaded test, `fcntl(fd, F_GETFD)` should return `-1` with `errno == EBADF`.
4. **Release:** `release()` returns the original fd, leaves the wrapper empty, and does not close the fd. The test must close the released descriptor manually.
5. **Idempotent empty release:** releasing an empty wrapper returns `-1` and leaves it empty.
6. **Invalid sentinel:** constructing from `-1` remains empty and does not close unrelated descriptors.
7. **Non-copyability:** compile-time checks (e.g. `static_assert` with type traits) show that copying is disabled.

Handle failures of `open()` in tests. Keep descriptor-lifetime tests single-threaded and minimize unrelated fd allocations because Linux may reuse a closed descriptor number. Tests should verify observable OS behavior, not only the wrapper's internal integer state.

## Engineering Standards

- **Ownership:** One active wrapper owns at most one fd. Document borrowed vs. owned handles at API boundaries.
- **RAII:** Destruction is safe for empty and owning wrappers, including during stack unwinding.
- **Error handling:** Callers check for `open()` errors. The destructor does not throw or attempt to propagate `close()` errors. Document this limitation rather than silently inventing a recovery policy.
- **Modularity:** No implementation in the example `main()`; expose only the minimal public API.
- **Readability:** Prefer clear names, small functions, minimal dependencies, consistent formatting, and meaningful comments for non-obvious invariants.
- **Verification:** Build cleanly, run CTest, exercise failure and ownership paths, and test with ASan/UBSan.
- **Documentation:** Explain the build, the ownership contract, assumptions, and relevant limitations.

## Definition of Done

- [ ] Public API matches the declared interface and behavior.
- [ ] The library, demo, and test are distinct CMake targets.
- [ ] Debug build succeeds without project-code warnings.
- [ ] All seven test categories pass under CTest.
- [ ] ASan/UBSan builds and tests run without reported issues.
- [ ] `README.md` accurately describes implemented behavior and limitations.
- [ ] No copy or move support has been added beyond the stated scope.

## Instructions for Code Reviewers (Including Codex)

Review this repository **against the API contract, tests, and engineering requirements above**, rather than adding features not requested. Treat this as a focused learning exercise with production-style expectations, not a production-ready general-purpose fd library.

Prioritize findings in this order:

1. **Correctness and resource safety:** leaks, double close, incorrect `release()` behavior, fd ownership violations, UB, missing failure handling.
2. **Test coverage:** untested contracts, flaky tests caused by fd reuse, missing negative paths.
3. **Build and API quality:** CMake target relationships, header hygiene, declarations/definitions, warnings, const-correctness, `noexcept` consistency.
4. **Maintainability:** unnecessary complexity, unclear contracts, accidental coupling, documentation gaps.

For each finding, provide **severity**, **file and line**, **why it matters**, and **a minimal suggested fix**. Differentiate bugs from optional stylistic improvements. Do not rewrite the whole solution or implement move semantics unless specifically asked. State explicitly if no issues are found, along with any remaining testing limitations.

## Later Extension (Out of Scope)

After studying copy/move semantics, add `noexcept` move construction and move assignment, test moved-from states and replacement of an already-owned descriptor, and revisit the class invariants. Do not implement these in Project 01.
