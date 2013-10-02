import mxf
import mxf.storage
import mxf.datamodel
import mxf.metadata
import mxf.util
import unittest
import traceback

import os

cur_dir = os.path.dirname(os.path.abspath(__file__))

sandbox = os.path.join(cur_dir,'sandbox')
files =  os.path.join(cur_dir,'files')

if not os.path.exists(sandbox):
    os.makedirs(sandbox)

def dump(path):
    f = mxf.open(path, 'r')
    header = f.read_header()
    for set_item in header.iter_sets():
        print set_item.type_name
        for item in set_item.iter_items():
            value = None
            try:
                value = item.value
            except:
                value = item.raw_value
                print traceback.format_exc()
            print '  ', item.name, item.type_name,  item.key, item.length, value

class TestDataModel(unittest.TestCase):
    
    def test_iter_setDefs(self):
        model = mxf.datamodel.DataModel()
        
        for item in model.iter_setdefs():
            print item.name
    
    def _test_iter_typedefs(self):
        model = mxf.datamodel.DataModel()
        
        for item in model.iter_itemdefs():
            print item.name
        



if __name__ == '__main__':
    unittest.main()