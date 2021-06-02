#!/bin/bash

# https://www.bugcodemaster.com/article/extract-images-frame-frame-video-file-using-ffmpeg
# -vf fps=1 
#ffmpeg -i $1 frames/frame%07d.png -hide_banner

for filename in /Users/val/Projects/FDF/DEEPFAKES/videos/test_real/*.mp4; do
    name=${filename##*/}
    name=${name%.mp4}
    path="/Users/val/Projects/FDF/DEEPFAKES/videos/test_real/$name"
    mkdir $path
    ffmpeg -i $filename -vf "select=eq(pict_type\,I)" -vsync vfr "$path/frame%03d.png" -hide_banner
done
