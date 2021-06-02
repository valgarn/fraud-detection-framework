# Copyright 2021 The Fraud Detection Framework Authors.  All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND< either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.

import os

from PIL import Image, ImageDraw
import numpy as np
import random
import json

from pymongo import MongoClient

input_background_test_folder = "T:/FDF/cifar/test/"
input_background_train_folder = "T:/FDF/cifar/train/"
input_foreground_folder = "C:/FDF/numpy/"

output_test_folder = "L:/FDF/test/"
output_train_folder = "L:/FDF/train/"

number = 50426266

def generateForegroundsLengths():
    input_background_files = [f for f in os.listdir(input_background_test_folder) if f.endswith(".png")]
    input_foreground_files = [f for f in os.listdir(input_foreground_folder) if f.endswith(".npy")]
    print("input_background_files: ", len(input_background_files))
    foregrounds = []
    s = 0
    i = 1
    for f in input_foreground_files:    
        l = len(np.load(os.path.join(input_foreground_folder, f)))
        r = {
            "filename": f,
            "length": l
        }
        s += l
        foregrounds.append(r)
        print(i, " : ", r)
        i += 1
    with open('foregrounds.json', 'w') as f:
        json.dump(foregrounds, f)
    print("input_foreground_files: ", s)

def generateForegroundsSet():
    client = MongoClient('localhost', 27017)
    db = client["samples"]
    foregrounds = []
    with open('foregrounds.json', 'r') as f:
        foregrounds = json.load(f)
    s = []
    i = 1
    for f in foregrounds:    
        for j in range(0, f["length"]):
            s.append({
                "filename": f["filename"], 
                "item":j
            })
        db.foregrounds.insert_many(s)
        s = []
        print(i, " : ", f["filename"])
        i += 1        

def generateForegroundsTestTrainSet():
    client = MongoClient('localhost', 27017)
    db = client["samples"]
    foreground_ids = []
    cursor = db.foregrounds.find()
    obj = next(cursor, None)
    while obj:
        foreground_ids.append(obj["_id"])
        obj = next(cursor, None)
    print(len(foreground_ids))
    random.shuffle(foreground_ids)
    db.test.insert_many([{
            "id": id
        } for id in foreground_ids[-10000:]])
    print(len(foreground_ids[-10000:]))
    db.train.insert_many([{
            "id": id
        } for id in foreground_ids[:50000]])
    print(len(foreground_ids[:50000]))

def mergeSamples():
    im = Image.open("/media/val/TEMP/FDF/cifar/test/0_cat.png").convert("RGBA")
    img_array = np.load("/media/val/TEMP/FDF/numpy/aircraft carrier.npy")
    msk = Image.fromarray(img_array[0].reshape(28, 28))
    rgba = msk.convert("RGBA")
    pixdata = rgba.load()
    for y in range(rgba.size[1]):
        for x in range(rgba.size[0]):
            if pixdata[x, y] != (0, 0, 0, 255):
                pixdata[x, y] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255), 255)

    im.paste(rgba, (2, 2), mask=msk)
    im.show()

def generateTestImages():
    backgrounds = [f for f in os.listdir(input_background_test_folder) if f.endswith(".png")]
    client = MongoClient('localhost', 27017)
    db = client["samples"]
    foreground_ids = []
    cursor = db.test.find()
    obj = next(cursor, None)
    b = 0
    while obj:
        f = db.foregrounds.find_one({"_id":obj["id"]})
        img_array = np.load(os.path.join(input_foreground_folder, f["filename"]))[f["item"]]
        msk = Image.fromarray(img_array.reshape(28, 28))
        rgba = msk.convert("RGBA")
        pixdata = rgba.load()
        for y in range(rgba.size[1]):
            for x in range(rgba.size[0]):
                if pixdata[x, y] != (0, 0, 0, 255):
                    pixdata[x, y] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255), 255)
        im = Image.open(os.path.join(input_background_test_folder, backgrounds[b])).convert("RGBA")
        im.paste(rgba, (0, 0), mask=msk)
        rgb = im.crop((0, 0, 28, 28)).convert("RGB")
        rgb.save(os.path.join(output_test_folder, hex(b)[2:]+".png"), "png")
        b += 1
        if b % 100 == 0:
            print(b)
        obj = next(cursor, None)

def cropTestImages():
    backgrounds = [f for f in os.listdir(input_background_test_folder) if f.endswith(".png")]
    for b in range(0, len(backgrounds)):
        im = Image.open(os.path.join(input_background_test_folder, backgrounds[b])).convert("RGBA")
        rgb = im.crop((0, 0, 28, 28)).convert("RGB")
        rgb.save(os.path.join(output_test_folder, hex(b)[2:]+".png"), "png")
        if b % 100 == 0:
            print(b)

cropTestImages()
