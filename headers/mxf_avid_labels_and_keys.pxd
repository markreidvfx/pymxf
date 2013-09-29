
cdef extern from "mxf/mxf.h":

    # Essence element keys
    cdef mxfKey EE_AvidMJPEGClipWrapped "MXF_EE_K(AvidMJPEGClipWrapped)"
    cdef mxfKey EE_AvidMPEGClipWrapped "MXF_EE_K(AvidMPEGClipWrapped)"
    cdef mxfKey EE_DNxHD "MXF_EE_K(DNxHD)"
    cdef mxfKey EE_DVClipWrapped "MXF_EE_K(DVClipWrapped)"
    cdef mxfKey EE_BWFClipWrapped "MXF_EE_K(BWFClipWrapped)"
    cdef mxfKey EE_AES3ClipWrapped "MXF_EE_K(AES3ClipWrapped)"
    cdef mxfKey EE_UncClipWrapped "MXF_EE_K(UncClipWrapped)"
    cdef mxfKey EE_IMX "MXF_EE_K(IMX)"
    cdef mxfKey EE_AvidUnc10BitClipWrapped "MXF_EE_K(AvidUnc10BitClipWrapped)"