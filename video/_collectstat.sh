#!/bin/bash

#test_fake_maps
for filename in /Users/val/Projects/FDF/DEEPFAKES/maps/test_fake_maps/*.maps; do
    #filesize=$(stat -f%z "$filename")
    filesize=$(stat -c%s "$filename")
    if (( filesize > 0 )); then
        deepfake "$filename" >> stats.txt
    fi
done

