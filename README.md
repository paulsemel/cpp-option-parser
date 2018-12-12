# C++ option parser

This is highly inspired from LLVM tools option parser.

The goal of this option parser is to provide a simple interface to be able to
parse very simple command lines.

It obviously misses features, but PRs are always welcomed.

## Example

To see an example of how to use it, see `src/test.cc`.

## Dependencies

*Fortunately*, it depends on `clang++`, `libc++` and `c++2a` features (std::span).
