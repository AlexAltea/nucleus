#!/usr/bin/python

import os
import shutil

# Directories
NUCLEUS_DIR = os.path.abspath("..")

# Delete target folders
def main():
    target = [
        os.path.join(NUCLEUS_DIR, "Debug"),
        os.path.join(NUCLEUS_DIR, "ipch"),
        os.path.join(NUCLEUS_DIR, "x64"),
    ]

    for folder in target:
        shutil.rmtree(folder)


if __name__ == "__main__":
    main()
