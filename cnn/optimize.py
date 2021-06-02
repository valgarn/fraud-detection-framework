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

import tensorflow as tf
from tensorflow.python.platform import gfile
from tensorflow.tools.graph_transforms import TransformGraph


INPUT_FILE = "/Volumes/SSD/FDF/cnn/models/model-1-13-0.9964-0.0099-0.9974-0.0088.pb"
OUTPUT_FILE = "/Volumes/SSD/FDF/cnn/models/m88-1.pb"
MODEL_OUTPUT = "softmax_tensor/Softmax"

'''
from tensorflow.python.tools import freeze_graph

output_graph_filename = os.path.join(saved_model_dir, output_filename)
initializer_nodes = ''

freeze_graph.freeze_graph(input_saved_model_dir=saved_model_dir,
      output_graph=output_graph_filename,
      saved_model_tags = tag_constants.SERVING,
      output_node_names=output_node_names,initializer_nodes=initializer_nodes,
      input_graph=None, input_saver=False, input_binary=False, 
      input_checkpoint=None, restore_op_name=None, filename_tensor_name=None,
      clear_devices=False, input_meta_graph=False)
'''

def get_graph_def_from_file(sess, graph_filepath):
    sess.graph.as_default()
    with tf.gfile.GFile(graph_filepath, 'rb') as f:
      graph_def = tf.GraphDef()
      graph_def.ParseFromString(f.read())
      return graph_def

def optimize_graph(sess, graph_filename, transforms, output_node):
  input_names = []
  output_names = [output_node]
  #graph_def = get_graph_def_from_saved_model(model_dir)
  graph_def = get_graph_def_from_file(sess, graph_filename)
  optimized_graph_def = TransformGraph(graph_def, input_names, output_names, transforms)
  tf.train.write_graph(optimized_graph_def, logdir="./", as_text=False, name=OUTPUT_FILE)
  print('Graph optimized!')

with tf.device('/cpu:0'):
    sess = tf.Session()
    transforms = ['remove_nodes(op=Identity)', 'merge_duplicate_nodes',
                    'strip_unused_nodes','fold_constants(ignore_errors=true)',
                    'fold_batch_norms']
    #transforms = ['quantize_nodes', 'quantize_weights']

    optimize_graph(sess, INPUT_FILE , transforms, MODEL_OUTPUT)
