import unittest

from mxf import util


class TestUtil(unittest.TestCase):
    
    def test_test(self):
        for name, key in util.iter_labels_and_keys():
            print name, '=', key
    
    
if __name__ == '__main__':
    unittest.main()