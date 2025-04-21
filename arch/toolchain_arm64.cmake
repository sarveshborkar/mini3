# arch/toolchain_arm64.cmake

# Set the system name to Darwin for macOS
set(CMAKE_SYSTEM_NAME Darwin)

# Target architecture is arm64
set(CMAKE_SYSTEM_PROCESSOR arm64)

# Instruct Xcode/clang to build for arm64
set(CMAKE_OSX_ARCHITECTURES "arm64")

# Optionally specify compilers explicitly. Often not required if Xcode/clang is the default.
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# If cross-compiling from Intel macOS to Apple Silicon,
# you may need additional flags or a specific sysroot.
# For example (adjust this path as necessary):
# set(CMAKE_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")

# If needed, specify a minimum deployment target:
# set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
