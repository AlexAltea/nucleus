#!/usr/bin/python

import os
import shutil
import subprocess

# Directories
NUCLEUS_DIR = os.path.abspath("..")

# Flex
def preprocess_flex():
    dirs = [
        os.path.join(NUCLEUS_DIR, "nucleus", "graphics", "frontend")
    ]
    for path in dirs:
        for root, dirs, files in os.walk(path):
            for filename in files:
                if not filename.endswith(".l"):
                    continue
                
                print filename
    return

# Bison
def preprocess_bison():
    dirs = [
        os.path.join(NUCLEUS_DIR, "nucleus", "graphics", "frontend")
    ]
    for path in dirs:
        for root, dirs, files in os.walk(path):
            for filename in files:
                if not filename.endswith(".y"):
                    continue
                
                print filename
    return

# GLSL
def preprocess_glsl():
    dirs = [
        os.path.join(NUCLEUS_DIR, "resources", "shaders")
    ]
    for path in dirs:
        for root, dirs, files in os.walk(path):
            for filename in files:
                if not filename.endswith(".glsl"):
                    continue

                origFilepath = os.path.join(root, filename)
                tempFilepath = origFilepath
                destFilepath = origFilepath[:-5] + ".spv"
                
                shaderType = filename[-7:-5]
                if shaderType == "vs":
                    tempFilepath += ".vert"
                elif shaderType == "ds":
                    tempFilepath += ".tesc"
                elif shaderType == "hd":
                    tempFilepath += ".tese"
                elif shaderType == "gs":
                    tempFilepath += ".geom"
                elif shaderType == "ps":
                    tempFilepath += ".frag"
                
                shutil.copyfile(origFilepath, tempFilepath)
                args = ['glslangValidator.exe', '-V', tempFilepath]
                subprocess.check_output(args)
                os.remove(tempFilepath)
                
                if shaderType == "vs":
                    shutil.move("vert.spv", destFilepath)
                elif shaderType == "ds":
                    shutil.move("tesc.spv", destFilepath)
                elif shaderType == "hd":
                    shutil.move("tese.spv", destFilepath)
                elif shaderType == "gs":
                    shutil.move("geom.spv", destFilepath)
                elif shaderType == "ps":
                    shutil.move("frag.spv", destFilepath)
    return

def main():
    preprocess_flex()
    preprocess_bison()
    preprocess_glsl()
    return


if __name__ == "__main__":
    main()
