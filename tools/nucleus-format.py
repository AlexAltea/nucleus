#!/usr/bin/python

import os
import re

# Directories
NUCLEUS_DIR = os.path.abspath("..")
NUCLEUS_SOLUTION = os.path.join(NUCLEUS_DIR, "nucleus")


# Formatting rules for *.c, *.cpp and *.h files
def formatGeneric(codeInput):

    # Replace LF with CRLF
    codeInput = re.sub(r'^[\r]\n', r'\r\n', codeInput)

    # Replace tabs at the beginning of lines with spaces (!)
    codeInput = re.sub(r'([\n,\r]+)([\t]*)[\t]', r'\1\2    ', codeInput)

    # Remove tabs or spaces at the end of lines
    codeInput = re.sub(r'([ \t]+)\r', r'\1\r', codeInput)
    
    return codeInput


# Search and edit files of the Nucleus project
def main():
    for root, dirs, files in os.walk(NUCLEUS_SOLUTION):
        for filename in files:
            if not filename.endswith((".c",".cc",".cpp",".h",".hpp")):
                continue

            # Read and format the code
            f = open(os.path.join(root, filename), "rb")
            codeInput = f.read()
            codeOutput = formatGeneric(codeInput)
            f.close()

            # Update file if necessary
            if codeInput != codeOutput:
                w = open(os.path.join(root, filename), "wb")
                w.write(codeOutput)
                w.close()
                break


if __name__ == "__main__":
    main()
