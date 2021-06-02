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
import scipy.misc

import numpy as np
import tensorflow as tf

from tensorflow.contrib import learn
from tensorflow.contrib.learn.python.learn.estimators import model_fn as model_fn_lib

photos_train_folder = "C:/FDF/data/train/"
photos_test_folder = "C:/FDF/data/test/"
fakes_train_folder = "C:/FDF/data/train-fake/"
fakes_test_folder = "C:/FDF/data/test-fake/"

tf.logging.set_verbosity(tf.logging.INFO)

train_data = None
train_labels = None
eval_data = None
eval_labels = None

def cnn_model_fn(features, labels, mode):
    input_layer = tf.reshape(features, [-1, 28, 28, 3])
    conv1 = tf.layers.conv2d(inputs=input_layer, filters=32, kernel_size=[5, 5], padding="same", activation=tf.nn.relu)
    pool1 = tf.layers.max_pooling2d(inputs=conv1, pool_size=[2, 2], strides=2)
    conv2 = tf.layers.conv2d(inputs=pool1, filters=64, kernel_size=[5, 5], padding="same", activation=tf.nn.relu)
    pool2 = tf.layers.max_pooling2d(inputs=conv2, pool_size=[2, 2], strides=2)
    pool2_flat = tf.reshape(pool2, [-1, 7 * 7 * 64])
    dense = tf.layers.dense(inputs=pool2_flat, units=1024, activation=tf.nn.relu)
    dropout = tf.layers.dropout(inputs=dense, rate=0.4, training=mode == learn.ModeKeys.TRAIN)
    logits = tf.layers.dense(inputs=dropout, units=2)
    loss = None
    train_op = None
    if mode != learn.ModeKeys.INFER:
        onehot_labels = tf.one_hot(indices=tf.cast(labels, tf.int32), depth=2)
        loss = tf.losses.softmax_cross_entropy(onehot_labels=onehot_labels, logits=logits)
    if mode == learn.ModeKeys.TRAIN:
        # TODO: Try Adam optimizer
        train_op = tf.contrib.layers.optimize_loss(loss=loss, global_step=tf.contrib.framework.get_global_step(), learning_rate=0.001, optimizer="SGD")
    predictions = {
        "classes": tf.argmax(input=logits, axis=1),
        "probabilities": tf.nn.softmax(logits, name="softmax_tensor")
    }
    return model_fn_lib.ModelFnOps(mode=mode, predictions=predictions, loss=loss, train_op=train_op)

def loadDataFromImages():
    global train_data
    global train_labels
    global eval_data
    global eval_labels
    n = 50000
    train_data = np.array([scipy.misc.imread(os.path.join(photos_train_folder, f), mode='RGB') \
                                                    for f in os.listdir(photos_train_folder) if f.endswith(".png")]).astype(np.float32)
    #train_labels = np.asarray([0]*n, dtype=np.int32)
    train_data = np.concatenate((train_data, [scipy.misc.imread(os.path.join(fakes_train_folder, f), mode='RGB') \
                                                    for f in os.listdir(fakes_train_folder) if f.endswith(".png")]), axis=0).astype(np.float32)
    #train_labels = np.append(train_labels, np.asarray([1]*n, dtype=np.int32))
    np.save("train_data_float.npy", train_data, False)
    #np.save("train_labels.npy", train_labels, False)
    n = 10000
    eval_data = np.array([scipy.misc.imread(os.path.join(photos_test_folder, f), mode='RGB') \
                                                    for f in os.listdir(photos_test_folder) if f.endswith(".png")]).astype(np.float32)
    #eval_labels = np.asarray([0]*n, dtype=np.int32)
    eval_data = np.concatenate((eval_data, [scipy.misc.imread(os.path.join(fakes_test_folder, f), mode='RGB') \
                                                        for f in os.listdir(fakes_test_folder) if f.endswith(".png")]), axis=0).astype(np.float32)
    #eval_labels = np.append(eval_labels, np.asarray([1]*n, dtype=np.int32))
    np.save("eval_data_float.npy", eval_data, False)
    #np.save("eval_labels.npy", eval_labels, False)

if __name__ == "__main__":
    train_data = np.load("train_data_float.npy")
    train_labels = np.load("train_labels.npy")
    eval_data = np.load("eval_data_float.npy")
    eval_labels = np.load("eval_labels.npy")

    classifier = learn.Estimator(model_fn=cnn_model_fn, model_dir="D:/FDF/cnn/model")
    tensors_to_log = {"probabilities": "softmax_tensor"}
    logging_hook = tf.train.LoggingTensorHook(tensors=tensors_to_log, every_n_iter=50)
    classifier.fit(x=train_data, y=train_labels, batch_size=100, steps=20000, monitors=[logging_hook])

    metrics = {
        "accuracy":
            learn.MetricSpec(
                metric_fn=tf.metrics.accuracy, prediction_key="classes"),
    }

    eval_results = classifier.evaluate(x=eval_data, y=eval_labels, metrics=metrics)
    print(eval_results)
