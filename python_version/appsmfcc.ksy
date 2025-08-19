meta:
  id: appsmfcc
  title: AppsMFCC comment found in MathType WMF documents
  application: MathType
  tags: 
    - windows
    - mathtype
  endian: le
  license: MIT
doc: |
  This format is used by MathType (starting with MathType 6.0b for Windows) for
  its MTEF and MathML embedded in WMFs.
  
  The Windows Metafile format is Microsoft Windows' native graphics metafile
  (picture) format and is used in WMF files, on the clipboard, and in OLE
  objects. Data is embedded in WMF data using the MFCOMMENT escape function.
  Individual comments are restricted to 32Kb or less.
  
  The convention described here allows for breaking the embedded data into
  multiple comments (or chunks), in order to support embedding data larger than
  32Kb limit.
doc-ref: https://docs.wiris.com/en_US/application-specific-metafile-comment-convention
seq:
  - id: header
    type: header
  - id: chunk_data
    size: header.data_length
types:
  header:
    seq:
      - id: id
        size: 8
        contents: 'AppsMFCC'
      - id: version
        type: u2
        valid: 1
      - id: total_length
        type: u4
      - id: data_length
        type: u4
        valid:
          max: total_length
      - id: signature
        type: str
        terminator: 0
        encoding: UTF-8