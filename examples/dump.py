import mxf
import sys



def find_preface(f):
    header = f.read_header()
    for item in header.iter_sets():
        
        item.type_name == "Preface"
        return item
    
def dump(f):
    header = f.read_header()
    for set_item in header.iter_sets():
        print set_item.type_name, set_item.setdef().key
        for item in set_item.iter_items():
            value = None
            try:
                value = item.value
            except:
                value = item.raw_value
                print traceback.format_exc()
            print '  ', item.name, item.type_name,  item.key, item.length, value


def walk_metadata(set_item, space = ""):
    
    print space, set_item.type_name, set_item.instanceID
    
    for item in set_item.iter_items():
        value = None
        try:
            value = item.value
        except:
            value = item.raw_value
            print traceback.format_exc()
       
        if isinstance(value, mxf.metadata.MetaDataSet):
            print space, '  ', item.name, item.type_name,  item.key, item.length, value
            walk_metadata(value, space + '    ')
        elif isinstance(value, list):
            print space, '  ', item.name, item.type_name,  item.key, item.length, value
            for v in value:
                if isinstance(v, mxf.metadata.MetaDataSet):
                    walk_metadata(v, space + '    ')
        else:
            print space, '  ', item.name, item.type_name,  item.key, item.length, value

path = sys.argv[1]

f = mxf.open(path, 'r')

preface = find_preface(f)

#print preface.type_name

walk_metadata(preface)

dump(f)

