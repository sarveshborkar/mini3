# arch/toolchain_x86_64.cmake

# Set the system name to Darwin for macOS
set(CMAKE_SYSTEM_NAME Darwin)

# Target architecture is x86_64
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Instruct Xcode/clang to build for x86_64
set(CMAKE_OSX_ARCHITECTURES "x86_64")

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Similarly, if cross-compiling on Apple Silicon to x86_64, you may want:
# set(CMAKE_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")
# set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")