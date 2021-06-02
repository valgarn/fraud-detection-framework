
import os
import sys
import json

import fdf

#f = "/Users/val/Projects/FDF/Tests/50-50/OK/IMG_20190725_121735.jpg"

#f = "./white.jpeg"

f = sys.argv[1]
#f = "Date.jpg"
#f = "049.JPG"
#print(f)
#print()

info = fdf.Photos.All.INFO()
print(info)
#cnn = fdf.Photos.All.CNN('{ "modelPath": "../Service/fdf_graph.pb" }')
#print(os.path.exists("./model-1-50-0.9862-0.0447-0.9891-0.0330.pb"))
cnn = fdf.Photos.All.CNN(json.dumps({
    "modelPath": "../Service/m88-1.pb", 
    "modelInput": "input_input", 
    "modelOutput": "softmax_tensor/Softmax" }))
#print(cnn.check(f))
print(cnn)

pca = fdf.Photos.All.PCA()
#print(pca.check(f))
print(pca)

benford = fdf.Photos.Jpeg.BENFORD()
print(benford)
exif = fdf.Photos.Jpeg.EXIF()
print(exif)
quality = fdf.Photos.Jpeg.QUALITY()
print(quality)

while True:
    print(exif.check(f))
    print(info.check(f))
    print(cnn.check(f))
    print(pca.check(f))
    print(benford.check(f))
    print(quality.check(f))
    exit()

