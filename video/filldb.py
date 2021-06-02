
import json

import pg

with open('dataset.json') as f:
    data = json.load(f)
samples = []
for (key, value) in data.items():
    #print(key)
    is_original = False
    source_video = value.get("source_video", None)
    if source_video is None:
        is_original = True
        source_video = key[key.rindex("/")+1:key.rindex("_")]
        print(key, source_video)
    samples.append((
        pg.Text(key),
        pg.Text(json.dumps(value["augmentations"])),
        pg.Text(value["label"]),
        pg.Text(value["set"]),
        pg.Text(source_video),
        pg.Text(value["swapped_id"]),
        pg.Text(value["target_id"]),
        pg.Bit(is_original)
    ))
pg.insertSamples(samples)
