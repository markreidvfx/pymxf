
cdef extern from "mxf/mxf.h":

    # Operational patterns labels
    # OP-Atom labels
    
    cdef mxfUL op_atom_1Track_1SourceClip "MXF_OP_L(atom, 1Track_1SourceClip)"
    cdef mxfUL op_atom_1Track_NSourceClips "MXF_OP_L(atom, 1Track_NSourceClips)"
    cdef mxfUL op_atom_NTracks_1SourceClip "MXF_OP_L(atom, NTracks_1SourceClip)"
    cdef mxfUL op_atom_NTracks_NSourceClips "MXF_OP_L(atom, NTracks_NSourceClips)"
    
    # OP-1A labels
    
    cdef mxfUL op_1a_UniTrack_Stream_Internal "MXF_OP_L(1a, UniTrack_Stream_Internal)"
    cdef mxfUL op_1a_MultiTrack_Stream_Internal "MXF_OP_L(1a, MultiTrack_Stream_Internal)"
    cdef mxfUL op_1a_MultiTrack_Stream_External "MXF_OP_L(1a, MultiTrack_Stream_External)"
    
    # OP-1B labels
    
    cdef mxfUL op_1b_MultiTrack_Stream_External "MXF_OP_L(1b, MultiTrack_Stream_External)"
   
    
    cdef int is_op_atom(mxfUL *label)
    cdef int is_op_1a(mxfUL *label)
    cdef int is_op_1b(mxfUL *label)
    
    
    # Data definition labels
    
    cdef mxfUL DDEF_Picture "MXF_DDEF_L(Picture)"
    cdef mxfUL DDEF_Sound "MXF_DDEF_L(Sound)"
    cdef mxfUL DDEF_Timecode "MXF_DDEF_L(Timecode)"
    cdef mxfUL DDEF_LegacyPicture "MXF_DDEF_L(LegacyPicture)" 
    cdef mxfUL DDEF_LegacySound "MXF_DDEF_L(LegacySound)"
    cdef mxfUL DDEF_LegacyTimecode "MXF_DDEF_L(LegacyTimecode)"
    cdef mxfUL DDEF_Data "MXF_DDEF_L(Data)"
    cdef mxfUL DDEF_DescriptiveMetadata "MXF_DDEF_L(DescriptiveMetadata)"
    
    # Picture/Sound coding labels
    
    # IEC-DV and DV-based mappings
    
    cdef mxfUL CMDEF_DVBased_50_625_50 "MXF_CMDEF_L(DVBased_50_625_50)"
    
    
    # Essence container labels
    
    cdef mxfUL EC_DVBased_50_625_50_ClipWrapped "MXF_EC_L(DVBased_50_625_50_ClipWrapped)"
    
    cdef mxfUMID g_Null_UMID