# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.39
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.
# This file is compatible with both classic and new-style classes.

from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_XdmfUtils', [dirname(__file__)])
        except ImportError:
            import _XdmfUtils
            return _XdmfUtils
        if fp is not None:
            try:
                _mod = imp.load_module('_XdmfUtils', fp, pathname, description)
            finally:
                fp.close()
                return _mod
    _XdmfUtils = swig_import_helper()
    del swig_import_helper
else:
    import _XdmfUtils
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


import Xdmf
class XdmfDiff(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfDiff, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfDiff, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _XdmfUtils.new_XdmfDiff(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _XdmfUtils.delete_XdmfDiff
    __del__ = lambda self : None;
    def GetDiffs(self, *args): return _XdmfUtils.XdmfDiff_GetDiffs(self, *args)
    def SetIgnoreTime(self, value = True): return _XdmfUtils.XdmfDiff_SetIgnoreTime(self, value)
    def GetIgnoreTime(self): return _XdmfUtils.XdmfDiff_GetIgnoreTime(self)
    def SetIgnoreGeometry(self, value = True): return _XdmfUtils.XdmfDiff_SetIgnoreGeometry(self, value)
    def GetIgnoreGeometry(self): return _XdmfUtils.XdmfDiff_GetIgnoreGeometry(self)
    def SetIgnoreTopology(self, value = True): return _XdmfUtils.XdmfDiff_SetIgnoreTopology(self, value)
    def GetIgnoreTopology(self): return _XdmfUtils.XdmfDiff_GetIgnoreTopology(self)
    def SetIgnoreAllAttributes(self, value = True): return _XdmfUtils.XdmfDiff_SetIgnoreAllAttributes(self, value)
    def GetIgnoreAllAttributes(self): return _XdmfUtils.XdmfDiff_GetIgnoreAllAttributes(self)
    def SetDisplayFailuresOnly(self, value = True): return _XdmfUtils.XdmfDiff_SetDisplayFailuresOnly(self, value)
    def GetDisplayFailuresOnly(self): return _XdmfUtils.XdmfDiff_GetDisplayFailuresOnly(self)
    def SetVerboseOutput(self, value = True): return _XdmfUtils.XdmfDiff_SetVerboseOutput(self, value)
    def GetVerboseOutput(self): return _XdmfUtils.XdmfDiff_GetVerboseOutput(self)
    def SetCreateDiffFile(self, value = True): return _XdmfUtils.XdmfDiff_SetCreateDiffFile(self, value)
    def GetCreateDiffFile(self): return _XdmfUtils.XdmfDiff_GetCreateDiffFile(self)
    def SetDiffFileName(self, *args): return _XdmfUtils.XdmfDiff_SetDiffFileName(self, *args)
    def GetDiffFileName(self): return _XdmfUtils.XdmfDiff_GetDiffFileName(self)
    def SetRelativeError(self, *args): return _XdmfUtils.XdmfDiff_SetRelativeError(self, *args)
    def GetRelativeError(self): return _XdmfUtils.XdmfDiff_GetRelativeError(self)
    def SetAbsoluteError(self, *args): return _XdmfUtils.XdmfDiff_SetAbsoluteError(self, *args)
    def GetAbsoluteError(self): return _XdmfUtils.XdmfDiff_GetAbsoluteError(self)
    def IncludeGrid(self, *args): return _XdmfUtils.XdmfDiff_IncludeGrid(self, *args)
    def IgnoreGrid(self, *args): return _XdmfUtils.XdmfDiff_IgnoreGrid(self, *args)
    def IncludeAttribute(self, *args): return _XdmfUtils.XdmfDiff_IncludeAttribute(self, *args)
    def IgnoreAttribute(self, *args): return _XdmfUtils.XdmfDiff_IgnoreAttribute(self, *args)
    def ParseSettingsFile(self, *args): return _XdmfUtils.XdmfDiff_ParseSettingsFile(self, *args)
    def AreEquivalent(self): return _XdmfUtils.XdmfDiff_AreEquivalent(self)
XdmfDiff_swigregister = _XdmfUtils.XdmfDiff_swigregister
XdmfDiff_swigregister(XdmfDiff)

class XdmfDataTransform(Xdmf.XdmfDataItem):
    __swig_setmethods__ = {}
    for _s in [Xdmf.XdmfDataItem]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfDataTransform, name, value)
    __swig_getmethods__ = {}
    for _s in [Xdmf.XdmfDataItem]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfDataTransform, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _XdmfUtils.new_XdmfDataTransform()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _XdmfUtils.delete_XdmfDataTransform
    __del__ = lambda self : None;
    def GetClassName(self): return _XdmfUtils.XdmfDataTransform_GetClassName(self)
    def UpdateInformation(self): return _XdmfUtils.XdmfDataTransform_UpdateInformation(self)
XdmfDataTransform_swigregister = _XdmfUtils.XdmfDataTransform_swigregister
XdmfDataTransform_swigregister(XdmfDataTransform)

class XdmfValuesBinary(Xdmf.XdmfValues):
    __swig_setmethods__ = {}
    for _s in [Xdmf.XdmfValues]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfValuesBinary, name, value)
    __swig_getmethods__ = {}
    for _s in [Xdmf.XdmfValues]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfValuesBinary, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _XdmfUtils.new_XdmfValuesBinary()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _XdmfUtils.delete_XdmfValuesBinary
    __del__ = lambda self : None;
    def GetClassName(self): return _XdmfUtils.XdmfValuesBinary_GetClassName(self)
    def Read(self, Array = None): return _XdmfUtils.XdmfValuesBinary_Read(self, Array)
    def Write(self, *args): return _XdmfUtils.XdmfValuesBinary_Write(self, *args)
    def GetEndian(self): return _XdmfUtils.XdmfValuesBinary_GetEndian(self)
    def SetEndian(self, *args): return _XdmfUtils.XdmfValuesBinary_SetEndian(self, *args)
    def GetSeek(self): return _XdmfUtils.XdmfValuesBinary_GetSeek(self)
    def SetSeek(self, *args): return _XdmfUtils.XdmfValuesBinary_SetSeek(self, *args)
    def GetCompression(self): return _XdmfUtils.XdmfValuesBinary_GetCompression(self)
    def SetCompression(self, *args): return _XdmfUtils.XdmfValuesBinary_SetCompression(self, *args)
XdmfValuesBinary_swigregister = _XdmfUtils.XdmfValuesBinary_swigregister
XdmfValuesBinary_swigregister(XdmfValuesBinary)

class XdmfExodusReader(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfExodusReader, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfExodusReader, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _XdmfUtils.new_XdmfExodusReader()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _XdmfUtils.delete_XdmfExodusReader
    __del__ = lambda self : None;
    def read(self, *args): return _XdmfUtils.XdmfExodusReader_read(self, *args)
XdmfExodusReader_swigregister = _XdmfUtils.XdmfExodusReader_swigregister
XdmfExodusReader_swigregister(XdmfExodusReader)



