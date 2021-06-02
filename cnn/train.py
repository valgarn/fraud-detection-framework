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
    f = features["x"]
    #f = tf.Print(f, [labels], "LABELS: ", summarize=1000)
    input_layer = tf.reshape(f, [-1, 28, 28, 3])
    conv1 = tf.layers.conv2d(inputs=input_layer, filters=128, kernel_size=[5, 5], \
                                kernel_initializer=tf.contrib.layers.xavier_initializer(), \
                                bias_initializer=tf.zeros_initializer(), padding="same", activation=tf.nn.relu)
    pool1 = tf.layers.max_pooling2d(inputs=conv1, pool_size=[2, 2], strides=2)
    conv2 = tf.layers.conv2d(inputs=pool1, filters=128, kernel_size=[5, 5], \
                                kernel_initializer=tf.contrib.layers.xavier_initializer(), \
                                bias_initializer=tf.zeros_initializer(), padding="same", activation=tf.nn.relu)
    pool2 = tf.layers.max_pooling2d(inputs=conv2, pool_size=[2, 2], strides=2)
    pool2_flat = tf.reshape(pool2, [-1, 7 * 7 * 128])
    dense = tf.layers.dense(inputs=pool2_flat, units=1024, activation=tf.nn.relu)
    dropout = tf.layers.dropout(inputs=dense, rate=0.7, training=mode == tf.estimator.ModeKeys.TRAIN)
    logits = tf.layers.dense(inputs=dropout, units=2)
    predictions = {
        "classes": tf.argmax(input=logits, axis=1),
        "probabilities": tf.nn.softmax(logits, name="softmax_tensor")
    }
    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)
    onehot_labels = tf.one_hot(indices=tf.cast(labels, tf.int32), depth=2)
    loss = tf.losses.softmax_cross_entropy(onehot_labels=onehot_labels, logits=logits)
    if mode == tf.estimator.ModeKeys.TRAIN:
        optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.001)
        train_op = optimizer.minimize(loss=loss, global_step=tf.train.get_global_step())
        return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)
    eval_metric_ops = {
        "accuracy": tf.metrics.accuracy(labels=labels, predictions=predictions["classes"])
    }
    return tf.estimator.EstimatorSpec(mode=mode, loss=loss, eval_metric_ops=eval_metric_ops)

def loadDataFromImages():
    global train_data
    global train_labels
    global eval_data
    global eval_labels
    n = 50000
    train_data = np.array([scipy.misc.imread(os.path.join(photos_train_folder, f), mode='RGB')
                           for f in os.listdir(photos_train_folder) if f.endswith(".png")]).astype(np.float32)
    #train_labels = np.asarray([0]*n, dtype=np.int32)
    train_data = np.concatenate((train_data, [scipy.misc.imread(os.path.join(fakes_train_folder, f), mode='RGB')
                                              for f in os.listdir(fakes_train_folder) if f.endswith(".png")]), axis=0).astype(np.float32)
    #train_labels = np.append(train_labels, np.asarray([1]*n, dtype=np.int32))
    np.save("train_data_float.npy", train_data, False)
    #np.save("train_labels.npy", train_labels, False)
    n = 10000
    eval_data = np.array([scipy.misc.imread(os.path.join(photos_test_folder, f), mode='RGB')
                          for f in os.listdir(photos_test_folder) if f.endswith(".png")]).astype(np.float32)
    #eval_labels = np.asarray([0]*n, dtype=np.int32)
    eval_data = np.concatenate((eval_data, [scipy.misc.imread(os.path.join(fakes_test_folder, f), mode='RGB')
                                            for f in os.listdir(fakes_test_folder) if f.endswith(".png")]), axis=0).astype(np.float32)
    #eval_labels = np.append(eval_labels, np.asarray([1]*n, dtype=np.int32))
    np.save("eval_data_float.npy", eval_data, False)
    #np.save("eval_labels.npy", eval_labels, False)


if __name__ == "__main__":
    train_data = np.divide(np.load("train_data_float.npy"), 255.0)
    train_labels = np.load("train_labels.npy")
    idx = np.random.permutation(len(train_labels))
    train_data, train_labels = train_data[idx], train_labels[idx]

    eval_data = np.divide(np.load("eval_data_float.npy"), 255.0)
    eval_labels = np.load("eval_labels.npy")

    with tf.device('/gpu:0'):
        classifier = tf.estimator.Estimator(model_fn=cnn_model_fn, model_dir="model")
        tensors_to_log = {"probabilities": "softmax_tensor"}
        logging_hook = tf.train.LoggingTensorHook(tensors=tensors_to_log, every_n_iter=50)
        train_input_fn = tf.estimator.inputs.numpy_input_fn(x={"x": train_data}, y=train_labels, batch_size=1000, num_epochs=None, shuffle=True)
        classifier.train(input_fn=train_input_fn, steps=15000, hooks=[logging_hook])
        eval_input_fn = tf.estimator.inputs.numpy_input_fn(x={"x": eval_data}, y=eval_labels, num_epochs=1, shuffle=False)
        eval_results = classifier.evaluate(input_fn=eval_input_fn)
        print(eval_results)
