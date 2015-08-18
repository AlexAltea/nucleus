#!/usr/bin/python

import os

# Directories
NUCLEUS_DIR = os.path.abspath("..")

def init_windows():
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
