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

import json

with open('dataset.json') as f:
    data = json.load(f)
train_real = []
test_real = []
train_fake = []
test_fake = []
for (key, value) in data.items():
    if len(value["augmentations"]) == 0:
        if value["set"] == "train":
            if value["label"] == "real":
                train_real.append(key)
            else:
                train_fake.append(key)
        else:
            if value["label"] == "real":
                test_real.append(key)
            else:
                test_fake.append(key)

with open("train_real.txt", "w") as f:
    print("\n".join("%s"%(s) for s in train_real), file=f)

with open("train_fake.txt", "w") as f:
    print("\n".join("%s"%(s) for s in train_fake), file=f)

with open("test_real.txt", "w") as f:
    print("\n".join("%s"%(s) for s in test_real), file=f)

with open("test_fake.txt", "w") as f:
    print("\n".join("%s"%(s) for s in test_fake), file=f)
