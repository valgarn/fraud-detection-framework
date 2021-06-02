#!/bin/bash

find /Users/val/Projects/FDF/Tests/50-50/FRAUD -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
#find /Volumes/SSD/FDF/blur -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
    echo ">>> $file"
    #./fdftest "$file"
    python fdftest.py "$file"
    echo
    echo
done
