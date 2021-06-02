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

import numpy as np
import imageio
import multiprocessing


MODEL = "model-1-01-0.9682-0.1272-0.8627-0.3608"

from keras import backend as K
# This line must be executed before loading Keras model.
K.set_learning_phase(0)

from keras.models import load_model
model = load_model('/Volumes/SSD/FDF/cnn/models/{}.hdf5'.format(MODEL))
print(model.outputs)
# [<tf.Tensor 'dense_2/Softmax:0' shape=(?, 10) dtype=float32>]
print(model.inputs)
# [<tf.Tensor 'conv2d_1_input:0' shape=(?, 28, 28, 1) dtype=float32>]

import tensorflow as tf
from tensorflow.python.platform import gfile

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
    img = np.divide(np.array(imageio.imread(path, pilmode='RGB')).astype(np.float32), 255.)
    # no difference in rotation, width, height
    return multiprocessing.Pool().map(getLine, [(img, x) if x + 28 <= img.shape[0] else (img, img.shape[0] - 28) for x in range(0, img.shape[0], 28)])

def freeze_session(session, keep_var_names=None, output_names=None, clear_devices=True):
    """
    Freezes the state of a session into a pruned computation graph.

    Creates a new computation graph where variable nodes are replaced by
    constants taking their current value in the session. The new graph will be
    pruned so subgraphs that are not necessary to compute the requested
    outputs are removed.
    @param session The TensorFlow session to be frozen.
    @param keep_var_names A list of variable names that should not be frozen,
                          or None to freeze all the variables in the graph.
    @param output_names Names of the relevant graph outputs.
    @param clear_devices Remove the device directives from the graph for better portability.
    @return The frozen graph definition.
    """
    from tensorflow.python.framework.graph_util import convert_variables_to_constants
    graph = session.graph
    with graph.as_default():
        freeze_var_names = list(set(v.op.name for v in tf.global_variables()).difference(keep_var_names or []))
        output_names = output_names or []
        output_names += [v.op.name for v in tf.global_variables()]
        # Graph -> GraphDef ProtoBuf
        input_graph_def = graph.as_graph_def()
        if clear_devices:
            for node in input_graph_def.node:
                node.device = ""
        frozen_graph = convert_variables_to_constants(session, input_graph_def, output_names, freeze_var_names)
        return frozen_graph

with tf.device('/cpu:0'):
  
  sess = tf.Session()

  #tf.identity(sess.graph.get_tensor_by_name('activation_1/Softmax:0'), name="softmax_tensor")
  #tf.identity(sess.graph.get_tensor_by_name('conv2d_1_input:0'), name="input")  
  #frozen_graph = freeze_session(K.get_session(), output_names=[out.op.name for out in model.outputs])

  frozen_graph = freeze_session(K.get_session(), output_names=["softmax_tensor/Softmax"])
  tf.train.write_graph(frozen_graph, "/Volumes/SSD/FDF/cnn/models", "{}.pb".format(MODEL), as_text=False)

  f = gfile.FastGFile("/Volumes/SSD/FDF/cnn/models/{}.pb".format(MODEL), 'rb')
  graph_def = tf.GraphDef()
  # Parses a serialized binary message into the current message.
  graph_def.ParseFromString(f.read())
  f.close()

  sess.graph.as_default()
  # Import a serialized TensorFlow `GraphDef` protocol buffer
  # and place into the current default `Graph`.
  tf.import_graph_def(graph_def)
  
  sess.run(tf.global_variables_initializer())

        #[<tf.Tensor 'activation_1/Softmax:0' shape=(?, 2) dtype=float32>]
        #[<tf.Tensor 'conv2d_1_input:0' shape=(?, 28, 28, 3) dtype=float32>]
        #for filename in os.listdir(TEST_FOLDER):
        #data = np.array(loadImage(os.path.join(TEST_FOLDER, filename))).reshape(-1, 28, 28, 3)
        #data = np.divide(np.array([scipy.misc.imread('car-damaged-bumper.png', mode='RGB')]).astype(np.float32), 255.)

  data = np.array(loadImage('1.png')).reshape(-1, 28, 28, 3)

  #softmax_tensor = sess.graph.get_tensor_by_name('activation_1/Softmax:0')
  #predictions = sess.run(softmax_tensor, {'conv2d_1_input:0': data})

  softmax_tensor = sess.graph.get_tensor_by_name('softmax_tensor/Softmax:0')
  predictions = sess.run(softmax_tensor, {'input_input:0': data})
  
  print(predictions)

  '''
  n = len(probabilities[0]["classes"])
  print(n)
  #print(([p["probabilities"] for p in probabilities]))
  s = sum(probabilities[0]["classes"])
  #print(filename)
  print(s)
  print(s/n)
  print()
  '''
