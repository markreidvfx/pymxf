import mxf
import mxf.storage
import mxf.datamodel
import mxf.util
import unittest
import traceback

import os
from datetime import datetime
import time
import uuid

from struct import unpack

cur_dir = os.path.dirname(os.path.abspath(__file__))

sandbox = os.path.join(cur_dir,'sandbox')
files =  os.path.join(cur_dir,'files')

if not os.path.exists(sandbox):
    os.makedirs(sandbox)


class TestFile(unittest.TestCase):
     
    def test_new(self):
        
        test_file = os.path.join(sandbox, 'test_new.mxf')
        f = mxf.open(test_file, 'w')
        
        header = f.create_header()
        
        
        bodySID = 1
        indexSID = 2
        
        now = str(datetime.now())
        def set_and_check(item,key, value):
            item[key].value = value
            print "%s.%s =" % (item.type_name, key),  item[key].value
        
        # Preface
        DVBased_50_625_50_ClipWrapped =  mxf.util.find_essence_container_label("DVBased_50_625_50_ClipWrapped")
        preface = header.create_set("Preface")
        set_and_check(preface, 'LastModifiedDate', now )
        set_and_check(preface, 'Version', 0x0102)
        set_and_check(preface, 'OperationalPattern', mxf.util.find_op_pattern("atom", "NTracks_1SourceClip"))
        preface['EssenceContainers'].append(DVBased_50_625_50_ClipWrapped)
        print preface['EssenceContainers'].value
        
        # Preface - Identification
        gen =  uuid.uuid4()
        ProductUID = uuid.uuid4()
        ident = header.create_set("Identification")
        preface['Identifications'].append(ident)
        set_and_check(ident, 'ThisGenerationUID', gen)
        set_and_check(ident, 'CompanyName',  "This Company")
        set_and_check(ident, 'ProductName', "Some Product")
        set_and_check(ident, 'VersionString', "Alpha version")
        set_and_check(ident, 'ProductUID', ProductUID)
        set_and_check(ident, 'ModificationDate',now)
        print ident['ToolkitVersion']
        set_and_check(ident, 'ToolkitVersion', None)
        set_and_check(ident, 'Platform', mxf.util.get_platform_string())
        
        # Preface - ContentStorage
        storage = header.create_set("ContentStorage")        
        set_and_check(preface, 'ContentStorage', storage)

        # Preface - ContentStorage - MaterialPackage
        m_package = header.create_set("MaterialPackage")
        storage['Packages'].append(m_package)
        materialPackageUMID = mxf.util.generate_umid()
        sourcePackageUMID = mxf.util.generate_umid()
        set_and_check(m_package, 'PackageUID', materialPackageUMID)
        set_and_check(m_package, 'PackageCreationDate', now)
        set_and_check(m_package, 'PackageModifiedDate', now)
        set_and_check(m_package, 'Name', "python writedv50 material")
        
        # Preface - ContentStorage - MaterialPackage - Timeline Track
        sourceTrackID = 1
        sourceTrackNumber = 0x18010201
        m_track = header.create_set("Track")
        m_package['Tracks'].append(m_track)
        set_and_check(m_track, 'TrackID', sourceTrackID)
        set_and_check(m_track, "TrackNumber", sourceTrackNumber)
        set_and_check(m_track, "EditRate" , "25/1")
        set_and_check(m_track, "Origin" , 0)
        
        # Preface - ContentStorage - MaterialPackage - Timeline Track - Sequence
        seq = header.create_set("Sequence")
        set_and_check(m_track,'Sequence', seq)
        
        set_and_check(seq, 'DataDefinition', mxf.util.find_datadef("LegacyPicture"))
        duration = 0
        set_and_check(seq, 'Duration', duration)
        
        durationItem1 = seq['Duration']
        
        # Preface - ContentStorage - MaterialPackage - Timeline Track - Sequence - SourceClip
        source_clip = header.create_set("SourceClip")
        seq['StructuralComponents'].append(source_clip)
        set_and_check(source_clip, 'DataDefinition', mxf.util.find_datadef("LegacyPicture"))
        set_and_check(source_clip, 'Duration', duration)
        set_and_check(source_clip, 'StartPosition', 0)
        set_and_check(source_clip, 'SourcePackageID', sourcePackageUMID)
        set_and_check(source_clip, 'SourceTrackID', sourceTrackID)
        
        durationItem2 = source_clip['Duration']
        
        # Preface - ContentStorage - SourcePackage 
        source_package = header.create_set("SourcePackage")
        storage['Packages'].append(source_package)
        set_and_check(preface, 'PrimaryPackage', source_package)
        set_and_check(source_package, 'PackageUID', sourcePackageUMID)
        set_and_check(source_package, 'PackageModifiedDate', now)
        set_and_check(source_package, 'PackageModifiedDate', now)
        set_and_check(source_package, 'Name', "writedv50 source")
        
        # Preface - ContentStorage - SourcePackage - Timeline Track
        sp_track =  header.create_set("Track")
        source_package['Tracks'].append(sp_track)
        set_and_check(sp_track, 'TrackID', sourceTrackID) 
        set_and_check(sp_track, 'TrackNumber',sourceTrackID)
        set_and_check(sp_track, 'EditRate', '25/1')
        set_and_check(sp_track, 'Origin', 0)
        
        # Preface - ContentStorage - SourcePackage - Timeline Track - Sequence
        sp_seq = header.create_set("Sequence")
        set_and_check(sp_track, 'Sequence', sp_seq)
        set_and_check(sp_seq, 'DataDefinition', mxf.util.find_datadef("LegacyPicture"))
        set_and_check(sp_seq, 'Duration', 0)
        
        durationItem3 = sp_seq['Duration']
        
        # Preface - ContentStorage - SourcePackage - Timeline Track - Sequence - SourceClip
        sp_sourceclip = header.create_set('SourceClip')
        sp_seq['StructuralComponents'].append(sp_sourceclip)
        set_and_check(sp_sourceclip, 'DataDefinition', mxf.util.find_datadef("LegacyPicture"))
        set_and_check(sp_sourceclip, 'Duration', 0)
        set_and_check(sp_sourceclip, 'StartPosition', 0)
        set_and_check(sp_sourceclip, 'SourcePackageID', mxf.util.get_Null_UMID())
        set_and_check(sp_sourceclip, 'SourceTrackID', 0)
        
        durationItem4 = sp_sourceclip['Duration']
        
        # Preface - ContentStorage - SourcePackage - CDCIEssenceDescriptor
        desc = header.create_set("CDCIEssenceDescriptor")
        set_and_check(source_package,'Descriptor', desc)
        set_and_check(desc, 'SampleRate', '25/1')
        set_and_check(desc,'ContainerDuration', 0)
        set_and_check(desc, 'EssenceContainer', mxf.util.find_essence_container_label('DVBased_50_625_50_ClipWrapped'))
        set_and_check(desc, 'PictureEssenceCoding', mxf.util.find_essence_coding_label('DVBased_50_625_50'))
        
        set_and_check(desc,'StoredHeight',288)
        set_and_check(desc,'StoredWidth',720)
        set_and_check(desc,'SampledHeight',288)
        set_and_check(desc,'SampledWidth',288)
        set_and_check(desc,'SampledXOffset',0)
        set_and_check(desc,'SampledYOffset',0)
        set_and_check(desc,'DisplayHeight',288)
        set_and_check(desc,'DisplayWidth',720)
        set_and_check(desc,'DisplayXOffset',0)
        set_and_check(desc,'DisplayYOffset',0)
        set_and_check(desc,'FrameLayout',1)
        
        desc['VideoLineMap'].append(23)
        desc['VideoLineMap'].append(335)
        print 'CDCIEssenceDescriptor.VideoLineMap =', desc['VideoLineMap'].value
        
        aspectRatio = "4/3"
        
        set_and_check(desc, 'AspectRatio',aspectRatio)
        set_and_check(desc, 'ImageAlignmentOffset', 1)
        set_and_check(desc, 'ComponentDepth', 8)
        set_and_check(desc, 'HorizontalSubsampling', 2)
        set_and_check(desc, 'VerticalSubsampling', 1)
        set_and_check(desc, 'ColorSiting', 4)
        set_and_check(desc, 'BlackRefLevel', 16)
        set_and_check(desc, 'WhiteReflevel', 235)
        set_and_check(desc, 'ColorRange', 255)
        
        resolutionID = 0x8e
        frameSize = 288000
        imageSize = 0
        set_and_check(desc,'ResolutionID', resolutionID)
        set_and_check(desc,'FrameSampleSize', frameSize)
        set_and_check(desc,'ImageSize',imageSize)
        
        durationItem5 = desc['ContainerDuration']
        ImageSizeItem = desc['ImageSize']
        
        #Preface - ContentStorage - EssenceContainerData
        ess_container = header.create_set("EssenceContainerData")
        storage['EssenceContainerData'].append(ess_container)
        set_and_check(ess_container, 'LinkedPackageUID', sourcePackageUMID)
        set_and_check(ess_container, 'IndexSID', indexSID)
        set_and_check(ess_container, 'BodySID', bodySID)

        f.write_header()
        
        
        essence = f.open_essence("DVClipWrapped", 'w')
        
        dv_file = os.path.join(files, "input.dv")
        essence.import_from_file(dv_file)
        
        duration = essence.size/ frameSize
        imageSize = essence.size
        
        essence.close()
        
        f.write_footer()
        
        
        index_seg = mxf.storage.IndexTableSegment()
        indexSegmentUUID = uuid.uuid4()
        index_seg.instanceUID = indexSegmentUUID
        print index_seg.instanceUID
        index_seg.edit_rate = "25/1"
        print index_seg.edit_rate
        index_seg.start_position = 0
        index_seg.duration = duration
        index_seg.edit_unit_byte_count = imageSize
        index_seg.indexSID = indexSID
        index_seg.bodySID = bodySID
        index_seg.slice_count = 0
        index_seg.pos_table_count = 0
        
        f.write_index(index_seg)
        
        for item in (durationItem1 ,durationItem2, durationItem3, durationItem4, durationItem5):
            item.value = duration
        imageSizeItem = imageSize
        
        f.update_header()
        
        f.update_partitions()
        
        f.close()
        
        
        
        f= mxf.open(test_file, 'r')
        
        header = f.read_header()
        
        #output = open("new.txt", 'w')
        
        for set_item in header.iter_sets():
            print set_item.type_name
            
            #output.write("%s\n" % str(set_item.type_name))
            for item in set_item.iter_items():
                pass
                print "   ", item.name, item.type_name,  item.key, item.length, item.value
                
                #output.write("   %s %s %s %s %s\n" % (str(item.name), str(item.type_name), str(item.key), str(item.length), str(item.value)))
        #output.close()
        f.close()
        
        
    def test_open(self):
        
       test_file = os.path.join(files,'test_title.mxf')
       #test_file = os.path.join(files,'output.mxf')
        
       f = mxf.open(test_file, 'r')
       header = f.read_header()
       #output = open("open.txt", 'w')
       for set_item in header.iter_sets():
           print set_item.type_name
           #output.write("%s\n" % str(set_item.type_name))
           for item in set_item.iter_items():
               value = None
               try:
                   value = item.value
               except:
                   value = item.raw_value
                   print traceback.format_exc()
                   #print item.name, item.type_name
                   #raise
                   

               #output.write("   %s %s %s %s %s\n" % (str(item.name), str(item.type_name), str(item.key), str(item.length), str(value)))

               print '  ', item.name, item.type_name,  item.key, item.length, value
    
       

if __name__ == '__main__':
    unittest.main()