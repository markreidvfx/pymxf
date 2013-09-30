import unittest

from mxf import util


class TestUtil(unittest.TestCase):
    
    def test_test(self):
        for name, key in util.iter_labels_and_keys('essence_containers'):
            print name, '=', key
        print ''
        for name, key in util.iter_labels_and_keys('essence_coding_labels'):
            print name, '=', key
    
if __name__ == '__main__':
    unittest.main()