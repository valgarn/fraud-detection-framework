#!/bin/bash

# cat test_real.txt | while read line 
for filename in /Users/val/Projects/FDF/DEEPFAKES/videos/test_real/*.mp4; do
    name=${filename##*/}
    name=${name%.mp4}
    path="/Users/val/Projects/FDF/DEEPFAKES/videos/test_real/$name"
    deepfake $path
done
