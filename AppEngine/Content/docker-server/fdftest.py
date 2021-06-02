
import os
import sys
import json

sys.path.append("/root/srv/fdf")
os.environ["PATH"] += ";/root/srv/fdf" 
import fdf

f = "049.JPG"
info = fdf.Photos.All.INFO()
print(info)
cnn = fdf.Photos.All.CNN(json.dumps({
    "modelPath": "/root/srv/fdf/m88-1.pb",
    "modelInput": "input_input", 
    "modelOutput": "softmax_tensor/Softmax" }))
print(cnn)
pca = fdf.Photos.All.PCA()
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
