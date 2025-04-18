# mathtype_wmf_appsmfcc_extractor
A tool to extract Application-specific metafile comments from MathType 6.0b+ WMF documents

# Compiling
You will need a C99 compiler and GNU Make to build the executable binary.
To build the binary, type
```
make
```
The `get_appsmfcc_bin` executable should be produced.

# Usage
You can use this tool to extract AppsMFCC data from MathType WMF documents. Since MathType 6.0b this format is used for embedding MTEF (MathType binary math format) and MathML equation information in MathType WMF documents. Source: https://docs.wiris.com/mathtype/en/mathtype-office-tools/mathtype-7-for-windows-and-mac/mathtype-sdk/application-specific-metafile-comment-convention.html

So you can use this tool to extract MathML from your MathType files, which may be useful.

You can invoke binary like so:
```
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
```
./get_appsmfcc example.wmf example_prefix
```

# TODO
- Add error code description.
- Add a verbose mode which lists how many chunks have been read.
