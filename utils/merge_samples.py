#!/bin/python3
import os
import sys
import json
from shutil import copyfile

class SampleMerger:

    def __init__(self, src, dst):
        self.src = src
        self.dst = dst
        with open(src+'/config.json') as f:
            self.src_config = json.load(f)
        with open(dst+'/config.json') as f:
            self.dst_config = json.load(f)
    
    def merge(self):
        idGen = self.dst_config['idGenerator']
        for leaf in self.src_config['leafs']:
            oldId = leaf['id']
            idGen += 1
            leaf['id'] = idGen
            if os.path.exists(self.src+'/img_{}.jpg'.format(oldId)):
                copyfile(self.src+'/img_{}.jpg'.format(oldId), self.dst+'/img_{}.jpg'.format(idGen))
                self.dst_config['leafs'].append(leaf)
            
        self.dst_config['idGenerator'] = idGen
        with open(self.dst+'/config.json', "w") as f:
            json.dump(self.dst_config, f)


if __name__ == '__main__':
    print(sys.argv)
    source = sys.argv[1]
    destination = sys.argv[2]
    if os.path.exists(source+'/config.json') and os.path.exists(destination+'/config.json'):
        merger = SampleMerger(source, destination)
        merger.merge()
        print("Done")
    else:
        print('Usage: {} <source_folder> <destination_folder>'.format(sys.argv[0]))
        