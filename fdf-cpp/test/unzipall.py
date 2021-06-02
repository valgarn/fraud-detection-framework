
import os
import sys
import uuid
import zipfile

def extract(source, destination):
    z = zipfile.ZipFile(source)
    for f in z.namelist():
        if(f.upper().endswith(".JPG") or f.upper().endswith(".JPEG")):
            with open(os.path.join(destination, "{}.jpg".format(str(uuid.uuid4()))), "wb") as outfile:
                outfile.write(z.read(f))

extract("/media/val/Sources/OnSource/servicerequests/servicerequests-20170223T162149Z-004.zip", "/media/val/SSD/temp")


