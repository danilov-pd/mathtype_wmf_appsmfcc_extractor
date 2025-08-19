# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO


if getattr(kaitaistruct, 'API_VERSION', (0, 9)) < (0, 9):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Appsmfcc(KaitaiStruct):
    """This format is used by MathType (starting with MathType 6.0b for Windows) for
    its MTEF and MathML embedded in WMFs.
    
    The Windows Metafile format is Microsoft Windows' native graphics metafile
    (picture) format and is used in WMF files, on the clipboard, and in OLE
    objects. Data is embedded in WMF data using the MFCOMMENT escape function.
    Individual comments are restricted to 32Kb or less.
    
    The convention described here allows for breaking the embedded data into
    multiple comments (or chunks), in order to support embedding data larger than
    32Kb limit.
    
    .. seealso::
       Source - https://docs.wiris.com/en_US/application-specific-metafile-comment-convention
    """
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._read()

    def _read(self):
        self.header = Appsmfcc.Header(self._io, self, self._root)
        self.chunk_data = self._io.read_bytes(self.header.data_length)

    class Header(KaitaiStruct):
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._read()

        def _read(self):
            self.id = self._io.read_bytes(8)
            if not self.id == b"\x41\x70\x70\x73\x4D\x46\x43\x43":
                raise kaitaistruct.ValidationNotEqualError(b"\x41\x70\x70\x73\x4D\x46\x43\x43", self.id, self._io, u"/types/header/seq/0")
            self.version = self._io.read_u2le()
            if not self.version == 1:
                raise kaitaistruct.ValidationNotEqualError(1, self.version, self._io, u"/types/header/seq/1")
            self.total_length = self._io.read_u4le()
            self.data_length = self._io.read_u4le()
            if not self.data_length <= self.total_length:
                raise kaitaistruct.ValidationGreaterThanError(self.total_length, self.data_length, self._io, u"/types/header/seq/3")
            self.signature = (self._io.read_bytes_term(0, False, True, True)).decode(u"UTF-8")



