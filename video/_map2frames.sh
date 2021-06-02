#!/bin/bash

#test_fake_maps
for filename in /Users/val/Projects/FDF/DEEPFAKES/maps/test_fake_maps/*.maps; do
    filesize=$(stat -f%z "$filename")
    if (( filesize > 1 )); then
        name=${filename##*/}
        name=${name%.maps}
        path="/Users/val/Projects/FDF/DEEPFAKES/maps/test_fake_maps/$name"
        mkdir $path
        deepfake "$filename"
    fi
done
