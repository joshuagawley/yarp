# Yarp
Aspiring to become just yet another AUR helper?

![Yarp!](https://tenor.com/en-GB/view/yarp-hot-fuzz-gif-12386003)

## Current feature
Currently, only querying the local database `yarp -Q` is supported.

## Building and testing
Building yarp requires the following dependencies:
- A C++23 compiler
- CMake
- libalpm
- jsoncpp

Testing additionally requires the following:
- Catch2
- Python

If you're hacking on Yarp, the following commands will build the project:

```
cmake -B build/Debug 
cmake --build build/Debug
```

And to test:

```
ctest --test-dir build/Debug
```

