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
import imageio

#from pymongo import MongoClient

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

def generateTrainImages():
    backgrounds = [f for f in os.listdir(input_background_train_folder) if f.endswith(".png")]
    client = MongoClient('localhost', 27017)
    db = client["samples"]
    foreground_ids = []
    cursor = db.train.find()
    obj = next(cursor, None)
    b = 0
    while obj:
        im = Image.open(os.path.join(input_background_train_folder, backgrounds[b])).convert("RGBA")
        f = db.foregrounds.find_one({"_id":obj["id"]})
        img_array = np.load(os.path.join(input_foreground_folder, f["filename"]))[f["item"]]
        msk = Image.fromarray(img_array.reshape(28, 28))
        rgba = msk.convert("RGBA")
        pixdata = rgba.load()
        for y in range(rgba.size[1]):
            for x in range(rgba.size[0]):
                if pixdata[x, y] != (0, 0, 0, 255):
                    pixdata[x, y] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255), 255)
        im.paste(rgba, (0, 0), mask=msk)
        rgb = im.crop((0, 0, 28, 28)).convert("RGB")
        rgb.save(os.path.join(output_train_folder, hex(b)[2:]+".png"), "png")
        b += 1
        if b % 100 == 0:
            print(b)
        obj = next(cursor, None)

def cropTrainImages():
    backgrounds = [f for f in os.listdir(input_background_train_folder) if f.endswith(".png")]
    for b in range(0, len(backgrounds)):
        im = Image.open(os.path.join(input_background_train_folder, backgrounds[b])).convert("RGBA")
        rgb = im.crop((0, 0, 28, 28)).convert("RGB")
        rgb.save(os.path.join(output_train_folder, hex(b)[2:]+".png"), "png")
        if b % 100 == 0:
            print(b)

def cropImages(folder):
    images = [f for f in os.listdir(folder) if f.endswith(".jpg")]
    p = os.path.join(folder,"images")
    for b in range(0, len(images)):
        if os.path.isfile(os.path.join(folder, images[b])):
            print(images[b])
            im = Image.open(os.path.join(folder, images[b])).convert("RGBA")
            for y in range(0, im.size[1], 28):
                if y+28 < im.size[1]:
                    for x in range(0, im.size[0], 28):
                        if x+28 < im.size[0]:
                            rgb = im.crop((x, y, x+28, y+28)).convert("RGB")
                            n = "{}.png".format(x+y*im.size[1])
                            rgb.save(os.path.join(p, n), "png")

#cropImages("/Volumes/SSD/FDF/Data2")

def generateImages(folder):
    backgrounds = [f for f in os.listdir(os.path.join(folder, "images")) if f.endswith(".png")]
    g = True
    for b in range(0, len(backgrounds)):
        im = Image.open(os.path.join(folder, "images", backgrounds[b])).convert("RGBA")
        if g:
            g = False
            im.save(os.path.join(os.path.join(folder, "good", backgrounds[b])), "png")
        else:
            g = True
            draw = ImageDraw.Draw(im)
            n = random.randint(3, 7)
            x = random.randint(0, 27)
            y = random.randint(0, 27)
            for i in range(n):
                x1 = random.randint(0, 27)
                y1 = random.randint(0, 27)
                draw.line((x, y, x1, y1), fill=(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)), width=1)
                x = x1
                y = y1
            im.save(os.path.join(os.path.join(folder, "fake", backgrounds[b])), "png")

def npyFromImages(folder):
    train_data = np.asarray([], dtype=np.float32).reshape(0,28,28,3)
    #train_labels = np.asarray([], dtype=np.int32)

    eval_data = np.asarray([], dtype=np.float32).reshape(0,28,28,3)
    #eval_labels = np.asarray([], dtype=np.int32)

    files = [f for f in os.listdir(os.path.join(folder, "good")) if f.endswith(".png")]
    r = np.concatenate(([0]*10000, [1]*3856))
    r = np.random.permutation(r)
    for i in range(len(files)):
        if r[i]==0:
            train_data = np.vstack([train_data, [imageio.imread( os.path.join(folder, "good", files[i]), pilmode='RGB')]])
        else:
            eval_data = np.vstack([eval_data, [imageio.imread( os.path.join(folder, "good", files[i]), pilmode='RGB')]])
    train_labels = np.asarray([0]*10000, dtype=np.int32)
    eval_labels = np.asarray([0]*3856, dtype=np.int32)

    files = [f for f in os.listdir(os.path.join(folder, "fake")) if f.endswith(".png")]
    r = np.concatenate(([0]*10000, [1]*3856))
    r = np.random.permutation(r)
    for i in range(len(files)):
        if r[i]==0:
            train_data = np.vstack([train_data, [imageio.imread( os.path.join(folder, "fake", files[i]), pilmode='RGB')]])
        else:
            eval_data = np.vstack([eval_data, [imageio.imread( os.path.join(folder, "fake", files[i]), pilmode='RGB')]])
    train_labels = np.append(train_labels, np.asarray([1]*10000, dtype=np.int32))
    eval_labels = np.append(eval_labels, np.asarray([1]*3856, dtype=np.int32))

    print("train_data: ", len(train_labels))
    print("train_labels: ", len(train_labels))
    print("eval_data: ", len(eval_labels))
    print("eval_labels: ", len(eval_labels))

    np.save("/Volumes/SSD/FDF/cnn/train_data2_float.npy", train_data, False)
    np.save("/Volumes/SSD/FDF/cnn/eval_data2_float.npy", eval_data, False)
    np.save("/Volumes/SSD/FDF/cnn/train_labels2.npy", train_labels, False)
    np.save("/Volumes/SSD/FDF/cnn/eval_labels2.npy", eval_labels, False)    

#generateImages("/Volumes/SSD/FDF/Data2")

npyFromImages("/Volumes/SSD/FDF/Data2")
