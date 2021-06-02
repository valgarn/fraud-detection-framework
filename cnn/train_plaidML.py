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

# https://www.pyimagesearch.com/2018/12/31/keras-conv2d-and-convolutional-layers/
# https://keras.io/layers/convolutional/
# https://www.dlology.com/blog/how-to-convert-trained-keras-model-to-tensorflow-and-make-prediction/

# https://keras.io/callbacks/
# https://keras.rstudio.com/reference/fit.html
# https://keras.io/models/model/


import os
os.environ["KERAS_BACKEND"] = "plaidml.keras.backend"

import keras
from keras.models import Sequential
from keras.layers import Dense, Dropout, Conv2D, MaxPooling2D, Activation, Flatten
from keras.optimizers import RMSprop, SGD, Adam
from keras.regularizers import l2
from keras.callbacks import ModelCheckpoint

import scipy.misc
import numpy as np

import matplotlib
matplotlib.use("Agg")
 
from sklearn.preprocessing import LabelBinarizer
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
import matplotlib.pyplot as plt

photos_train_folder = "/Volumes/SSD/FDF/train/"
photos_test_folder = "/Volumes/SSD/FDF/test/"
fakes_train_folder = "/Volumes/SSD/FDF/train-fake/"
fakes_test_folder = "C/Volumes/SSD/FDF/test-fake/"

train_data = None
train_labels = None
eval_data = None
eval_labels = None

'''
def cnn_model_fn():
    model = Sequential()
    model.add(Conv2D(input_shape=(28, 28, 3), filters=128, kernel_size=[5, 5], padding="same", \
                    activation="relu", kernel_initializer="glorot_normal", bias_initializer="zeros", name="input"))
    model.add(MaxPooling2D(pool_size=(2, 2), strides=2))
    model.add(Conv2D(filters=128, kernel_size=[5, 5], padding="same", activation="relu", \
                    kernel_initializer="glorot_normal", bias_initializer="Zeros"))
    model.add(MaxPooling2D(pool_size=(2, 2), strides=2))
    model.add(Flatten())
    model.add(Dense(units=1024, activation="relu"))
    model.add(Dropout(rate=0.7))
    model.add(Dense(units=2))
    model.add(Activation(activation="softmax", name="softmax_tensor"))
    return model
'''
'''
def cnn_model_fn():
    model = Sequential()
    model.add(Conv2D(input_shape=(28, 28, 3), filters=64, kernel_size=[5, 5], padding="same", \
                    activation="relu", kernel_initializer="glorot_normal", bias_initializer="zeros", name="input"))
    model.add(MaxPooling2D(pool_size=(2, 2), strides=2))
    model.add(Conv2D(filters=64, kernel_size=[5, 5], padding="same", activation="relu", \
                    kernel_initializer="glorot_normal", bias_initializer="Zeros"))
    model.add(MaxPooling2D(pool_size=(2, 2), strides=2))
    model.add(Flatten())
    model.add(Dense(units=1024, activation="relu"))
    model.add(Dropout(rate=0.7))
    model.add(Dense(units=2))
    model.add(Activation(activation="softmax", name="softmax_tensor"))
    return model
'''
def cnn_model_fn():
    model = Sequential()
    model.add(Conv2D(input_shape=(28, 28, 3), filters=256, kernel_size=[5, 5], padding="same", \
                    activation="relu", kernel_initializer="glorot_normal", bias_initializer="zeros", name="input"))
    model.add(MaxPooling2D(pool_size=(2, 2), strides=2))
    model.add(Conv2D(filters=256, kernel_size=[5, 5], padding="same", activation="relu", \
                    kernel_initializer="glorot_normal", bias_initializer="Zeros"))
    model.add(MaxPooling2D(pool_size=(2, 2), strides=2))
    model.add(Flatten())
    model.add(Dense(units=1024, activation="relu"))
    model.add(Dropout(rate=0.7))
    model.add(Dense(units=2))
    model.add(Activation(activation="softmax", name="softmax_tensor"))
    return model


if __name__ == "__main__":
    #train_data = np.divide(np.load("/Volumes/SSD/FDF/cnn/train_data_float.npy"), 255.0)
    train_data = np.concatenate([np.divide(np.load("/Volumes/SSD/FDF/cnn/train_data_float.npy"), 255.0), \
                            np.divide(np.load("/Volumes/SSD/FDF/cnn/train_data2_float.npy"), 255.0)])

    #train_labels = np.load("/Volumes/SSD/FDF/cnn/train_labels.npy")
    train_labels = np.concatenate([np.load("/Volumes/SSD/FDF/cnn/train_labels.npy"), \
                                    np.load("/Volumes/SSD/FDF/cnn/train_labels2.npy")])

    idx = np.random.permutation(len(train_labels))
    train_data, train_labels = train_data[idx], train_labels[idx]

    #eval_data = np.divide(np.load("/Volumes/SSD/FDF/cnn/eval_data_float.npy"), 255.0)
    eval_data = np.concatenate([np.divide(np.load("/Volumes/SSD/FDF/cnn/eval_data_float.npy"), 255.0), \
                                    np.divide(np.load("/Volumes/SSD/FDF/cnn/eval_data2_float.npy"), 255.0)])
    
    #eval_labels = np.load("/Volumes/SSD/FDF/cnn/eval_labels.npy")
    eval_labels = np.concatenate([np.load("/Volumes/SSD/FDF/cnn/eval_labels.npy"), \
                                    np.load("/Volumes/SSD/FDF/cnn/eval_labels2.npy")])

    opt = SGD(lr=1e-3)
    model = cnn_model_fn()
    model.compile(loss="categorical_crossentropy", optimizer=opt, metrics=["accuracy"])

    print(train_data.shape)
    print(train_labels.shape)

    epochsN = 50
    filepath="/Volumes/SSD/FDF/cnn/models/model-1-{epoch:02d}-{val_acc:.4f}-{val_loss:.4f}-{acc:.4f}-{loss:.4f}.hdf5"
    checkpoint = ModelCheckpoint(filepath, monitor='val_acc', verbose=1, save_best_only=True, mode='max')
    H = model.fit(x=train_data, y=keras.utils.to_categorical(train_labels, num_classes=2, dtype='float32'), \
                    validation_data=(eval_data, keras.utils.to_categorical(eval_labels, num_classes=2, dtype='float32')), \
                    batch_size=32, epochs=epochsN, shuffle=True, callbacks=[checkpoint])

    os.makedirs('./model', exist_ok=True)
    model.save('./model/keras_model.h5')

    predictions = model.predict(eval_data, batch_size=32)
    print(classification_report(keras.utils.to_categorical(eval_labels, num_classes=2, dtype='float32').argmax(axis=1), predictions.argmax(axis=1)))

    plt.style.use("ggplot")
    plt.figure()
    plt.plot(np.arange(0, epochsN), H.history["loss"], label="train_loss")
    plt.plot(np.arange(0, epochsN), H.history["val_loss"], label="val_loss")
    plt.plot(np.arange(0, epochsN), H.history["acc"], label="train_acc")
    plt.plot(np.arange(0, epochsN), H.history["val_acc"], label="val_acc")
    plt.title("Training Loss and Accuracy on Dataset")
    plt.xlabel("Epoch #")
    plt.ylabel("Loss/Accuracy")
    plt.legend(loc="lower left")
    plt.savefig("plot")
