#!/bin/bash

cat test_fake.txt | while read line 
do
    cp "/Volumes/SSD/DEEPFAKE/fb_dfd_release_0.1_final/${line}" ./images
done
