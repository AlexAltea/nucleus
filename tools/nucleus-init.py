#!/usr/bin/python

import os

# Directories
NUCLEUS_DIR = os.path.abspath("..")
NUCLEUS_LLVM = os.path.join(NUCLEUS_DIR, "externals", "llvm")

def init_windows():
    # Configure LLVM
    command = "cmake"
    command += " -G \"Visual Studio 12 2013 Win64\""
    command += " -DCMAKE_CONFIGURATION_TYPES=\"Debug;Release\""
    command += " -DLLVM_TARGETS_TO_BUILD=X86"
    command += " -DLLVM_BUILD_RUNTIME=OFF"
    command += " -DLLVM_BUILD_TOOLS=OFF"
    command += " -DLLVM_INCLUDE_DOCS=OFF"
    command += " -DLLVM_INCLUDE_EXAMPLES=OFF"
    command += " -DLLVM_INCLUDE_TESTS=OFF"
    command += " -DLLVM_INCLUDE_TOOLS=OFF"
    command += " -DLLVM_INCLUDE_UTILS=OFF"
    command += " -DWITH_POLLY=OFF"
    command += " ."
    os.chdir(NUCLEUS_LLVM)
    os.system(command)
    return

def init_linux():
    return

def init_macos():
    return

def main():
    # Get required submodules
    command = "git"
    command += " submodule update --init"
    os.system(command)

    if os.name == "nt":
        init_windows()
    if os.name == "posix":
        init_linux()
    if os.name == "mac":
        init_macos()


if __name__ == "__main__":
    main()
