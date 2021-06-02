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
import multiprocessing
import scipy.misc
import numpy as np
import tensorflow as tf

tf.logging.set_verbosity(tf.logging.INFO)

#TEST_FOLDER = "C:/FDF/images/OK_modified"
TEST_FOLDER = "G:/OnSource/Projects/Fraud detection/Images/FROUD"

def getLine(params):
    batch = np.array([])
    for y in range(0, params[0].shape[1], 28):
        if y + 28 > params[0].shape[1]:
            y = params[0].shape[1] - 28
        data = np.array([params[0].data[params[1] + dx, y + dy, c] for dx in range(28) for dy in range(28) for c in range(3)], dtype=np.float32).reshape(1, 28, 28, 3)
        batch = np.vstack([batch, data]) if batch.size else data
    return batch

# 1. - the fake
def loadImage(path):
    img = np.divide(np.array(scipy.misc.imread(path, mode='RGB')).astype(np.float32), 255.)
    # no difference in rotation, width, height
    return multiprocessing.Pool().map(getLine, [(img, x) if x + 28 <= img.shape[0] else (img, img.shape[0] - 28) for x in range(0, img.shape[0], 28)])

if __name__ == "__main__":
    ckpt = tf.train.get_checkpoint_state("D:/FDF/tensorflow/cnn/model-final")
    if ckpt and ckpt.model_checkpoint_path:
        with tf.device('/cpu:0'):
            g = tf.Graph()
            with g.as_default():
                input_layer = tf.placeholder(tf.float32, [None, 28, 28, 3], name="input")
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
                dropout = tf.layers.dropout(inputs=dense, rate=0.7, training=False)
                logits = tf.layers.dense(inputs=dropout, units=2)
                predictions = {
                    "classes": tf.argmax(input=logits, axis=1),
                    "probabilities": tf.nn.softmax(logits, name="softmax_tensor")
                }

                sess = tf.Session()
                saver = tf.train.Saver()
                sess.run(tf.global_variables_initializer())
                saver.restore(sess, ckpt.model_checkpoint_path)

                # Freeze the graph
                #output_node_names = ["softmax_tensor"]
                #frozen_graph_def = tf.graph_util.convert_variables_to_constants(
                #sess,
                #sess.graph_def,
                #output_node_names)
                # Save the frozen graph
                #with open('model-frozen/fdf_graph.pb', 'wb') as f:
                #    f.write(frozen_graph_def.SerializeToString())
                #exit()              

                #for filename in os.listdir(TEST_FOLDER):
                #data = np.array(loadImage(os.path.join(TEST_FOLDER, filename))).reshape(-1, 28, 28, 3)
                #data = np.divide(np.array([scipy.misc.imread('car-damaged-bumper.png', mode='RGB')]).astype(np.float32), 255.)
                data = np.array(loadImage('IMG_20150827_074803.jpg')).reshape(-1, 28, 28, 3)
                print(len(data))
                probabilities = sess.run([predictions], feed_dict={input_layer: data})
                print(probabilities)
                n = len(probabilities[0]["classes"])
                print(n)
                #print(([p["probabilities"] for p in probabilities]))
                s = sum(probabilities[0]["classes"])
                #print(filename)
                print(s)
                print(s/n)
                print()
    else:
        print('No checkpoint file found. Nothing to do.')
