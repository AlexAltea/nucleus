Nucleus Tools
=============

Collection of Python scripts to make the building and development of Nucleus easier.

* __nucleus-init.py__: Initialize Git submodules and run CMake.
* __nucleus-preprocess.py__: Compile Flex (`.l`) and Bison (`*.y`) files to C++. Compile GLSL (`*.glsl`) files to SPIR-V. If any of these tools is not available in the environment, nothing will happen.
* __nucleus-format.py__: Apply rules on source code files to normalize their style, e.g. trimming trailing spaces or replacing tabs with spaces.
* __nucleus-clean.py__: Remove temporary files and objects.
