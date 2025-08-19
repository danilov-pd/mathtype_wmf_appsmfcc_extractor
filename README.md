# mathtype_wmf_appsmfcc_extractor
A tool to extract Application-specific metafile comments from MathType 6.0b+ WMF documents

There are two versions of this tool, written in C and Python respectively. 

The C version is less picky to overall file structure. It searches only the needed `AppsMFCC` header and then reads the chunks sequentially into file(s). It doesnt't try to determine AppsMFCC content type (MTEF or XML with MathML).

The Python version, on the other hand

*[New Python version](#python-version)
    * [Usage](#usage)
    * [Third-party works used in this script](#third-party-works-used-in-this-script)
    * [How does it operate?](#how-does-it-operate)
*[Old C version](#c-version) 
    *[Compiling](#compiling)
    *[Getting pre-built binaries](#getting-pre-built-binaries)
    *[Usage](#usage-1)
    *[TODO](#todo)

# Python version

This version is located in [python_version/](https://github.com/danilov-pd/mathtype_wmf_appsmfcc_extractor/tree/main/python_version) subdirectory.

## Usage

Python version requires only the Python3 interpreter (tested on Python 3.9 and higher) and no external dependencies.

You can invoke the script like so:
```shell
./get_appsmfcc WMF_FILENAME
```
or so:
```shell
python3 get_appsmfcc WMF_FILENAME
```

The script will parse the given WMF file and if the WMF file is valid and has the needed escape `MFCOMMENT` function with `AppsMFCC` comments in them, it will validate and parse them out in the files with according extensions (`.xml` for XML data and `.bin` for MTEF and other things).

The tool DOESN'T validate the XML. The comment's contents are determined as XML if they start with `<?xml version` bytes.

## Third-party works used in this script

The script relies on:

* [kaitaistruct.py](https://github.com/danilov-pd/mathtype_wmf_appsmfcc_extractor/blob/python_version/kaitaistruct.py), which is a part of [Kaitai Struct: runtime library for Python](https://github.com/kaitai-io/kaitai_struct_python_runtime/). It is MIT Licensed.
* [wmf.py](https://formats.kaitai.io/wmf/src/python/wmf.py), which is generated from the [https://github.com/kaitai-io/kaitai_struct_formats/blob/acdf0733633568c68869af15846abaf1c0eaa59a/image/wmf.ksy](wmf.ksy) description. It is a part of [Kaitai Struct: format library](https://github.com/kaitai-io/kaitai_struct_formats). It is CC0-1.0 licensed.

## How does it operate?

It initially parses and validates the WMF file with Kaitai's WMF format parser. Then it determines does the file contain the `META_ESCAPE` records with the `MFCOMMENT` function (see '2.3.6.1 META_ESCAPE Record' and '2.1.19 MetafileEscapes Enumeration' in [Windows Metafile Format (wmf) Specification](https://www.loc.gov/preservation/digital/formats/digformatspecs/WindowsMetafileFormat(wmf)Specification.pdf)).

Inside these escape functions, the `AppsMFCC` comment structures must be present. The `AppsMFCC` format is defined in [appsmfcc.ksy](https://github.com/danilov-pd/mathtype_wmf_appsmfcc_extractor/blob/python_version/appsmfcc.ksy) and compiled to [appsmfcc.py](https://github.com/danilov-pd/mathtype_wmf_appsmfcc_extractor/blob/python_version/appsmfcc.py) with the [Kaitai Struct compiler](https://github.com/kaitai-io/kaitai_struct_compiler/).

Then the scripts gathers chunks which contain the parts of a single comment, determines the comment's contents and writes them to file with according extensions (`.xml` for XML data and `.bin` for MTEF and other things). During the chunks gathering the script checks the chunks' and overall comment sizes given in `AppsMFCC` headers for consistency. If the sizes are not consistent, the comment is DISCARDED COMPLETELY.

The comments are written to file with the following filename:
```
{input_filename}-appsmfcc-{i}.{ext}
```
where `i` is the number of comment.

# C Version
This version is located in [c_version/](https://github.com/danilov-pd/mathtype_wmf_appsmfcc_extractor/tree/main/c_version) subdirectory.

## Compiling
You will need a C99 compiler and GNU Make to build the executable binary.
To build the binary, type

```shell
make
```
The `get_appsmfcc_bin` executable should be produced.

## Getting pre-built binaries
Pre-built binaries can be obtained in Release section under pre-release tag:
https://github.com/danilov-pd/mathtype_wmf_appsmfcc_extractor/releases/tag/pre-release

I consider these releases as pre-release, because software is quite raw. 

## Usage
You can use this tool to extract AppsMFCC data from MathType WMF documents. Since MathType 6.0b this format is used for embedding MTEF (MathType binary math format) and MathML equation information in MathType WMF documents. Source: https://docs.wiris.com/mathtype/en/mathtype-office-tools/mathtype-7-for-windows-and-mac/mathtype-sdk/application-specific-metafile-comment-convention.html

So you can use this tool to extract MathML from your MathType files, which may be useful.

You can invoke the binary like so:
```shell
./get_appsmfcc_bin INPUT_FILENAME OUTPUT_FILENAME_PREFIX
```
The `INPUT_FILENAME` is the name of the input MathType WMF file.
The `OUTPUT_FILENAME_PREFIX` is the prefix for the output filenames.
The output filenames are constructed like this:
```
OUTPUT_FILENAME_PREFIX-COMMENT_NUMBER.bin
```
Where the `COMMENT_NUMBER` is the number of comment in the file starting from 1.
Please note that the long comments that are separated into many chunks account as one comment and are written sequentially to the same file.

Or you can use a `get_appsmfcc` shell-script. It also accepts `INPUT_FILENAME` and `OUTPUT_FILENAME_PREFIX`, but also runs `file` command upon successful run of `get_appsmfcc_bin` to quickly determine which files are XML.

Shell-script usage:
```shell
./get_appsmfcc example.wmf example_prefix
```

## TODO
- Add error code description.
- Add a verbose mode which lists how many chunks have been read.
- Rewrite parser to finite state machnine. Think about better (and elegant) implementation, maybe using other language. 
