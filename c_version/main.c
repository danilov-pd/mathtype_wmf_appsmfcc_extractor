#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <string.h>

#define APPSMFCC_SIZE 18

#define ERROR_STRUCTURE_SIZE    20
#define ERROR_ARG_COUNT         1
#define ERROR_PREFIX            2
#define ERROR_INPUT_FILE_OPEN   3
#define ERROR_OUTPUT_FILE_OPEN  4
#define ERROR_EOF_ON_HEADER     5
#define ERROR_EOF_ON_BODY       6
#define ERROR_BLOCK_SIZE        7

#pragma pack(push,1)
typedef struct {
    char id[8];             // Must be "AppsMFCC"
    uint16_t  version;      // Must be 1
    uint32_t totalLen;      // Total of data bytes in all comments
    uint32_t dataLen;       // # of data bytes in this comment 
    // Then variable length, null terminated signature string and then dataLen
    // bytes of data (aka the chunk data)
} AppsMFCCHeader;
#pragma pack(pop)

void exit_gracefully(FILE* in, FILE* out, int exit_code){
    if (in != NULL) {
        fclose(in);
    }
    if (out != NULL) {
        fclose(out);
    }
    exit(exit_code);
}

int main(int argc, char *argv[])
{
    if (sizeof(AppsMFCCHeader) != APPSMFCC_SIZE){
        fprintf(stderr, "Error: wrong AppsMFCCHeader structure size. Terminating...\n");
        exit(ERROR_STRUCTURE_SIZE);
    }
    assert(sizeof(AppsMFCCHeader) == APPSMFCC_SIZE
        && "Wrong size of AppsMFCCHeader structure.");

    if (argc != 3) {
        fprintf(stderr, "Error: incorrect number of arguments.\n");
        exit(ERROR_ARG_COUNT);
    }

    char *input_filename = argv[1];
    char *output_prefix = argv[2];

    char output_filename[1500];

    if (strlen(output_prefix) + 30 >= sizeof(output_filename)) {
        fprintf(stderr, "Error: output prefix is too long.\n");
        exit(ERROR_PREFIX);
    }
    assert(strlen(output_prefix) + 30 < sizeof(output_filename)
        && "Output prefix is too long.");
    
    for (size_t i = 0; i < sizeof(output_filename); ++i) {
        output_filename[i] = 0;
    }

    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
       perror("Failed to open input file");
       exit(ERROR_INPUT_FILE_OPEN);
    }

    const char appsmfcc_id[8] = "AppsMFCC";
    
    struct RingBuffer {
        char data[APPSMFCC_SIZE];
        size_t start;
        size_t contains;
    } rb = {"\0", 0, 0};

    char buffer[APPSMFCC_SIZE]; 

    struct CommentState {
        FILE* output_file;
        int64_t comment_size;
        int64_t left;
        uint32_t file_n;
    } cs = {NULL, 0, 0, 0};

#define RB_END ((rb.start + rb.contains) % APPSMFCC_SIZE)

    int c;
    while ((c = fgetc(input_file)) != EOF) {
        rb.data[RB_END] = c;
        
        if (rb.contains < APPSMFCC_SIZE) {
            ++rb.contains;
        } else {
            rb.start = (rb.start + 1) % APPSMFCC_SIZE;
        }

        if (rb.contains == APPSMFCC_SIZE) {
            uint8_t compare = 1;
            for (size_t i = 0; i < sizeof(appsmfcc_id); ++i) {
                size_t rb_i = (rb.start + i) % APPSMFCC_SIZE;
                
                if (appsmfcc_id[i] != rb.data[rb_i]){
                    compare = 0;
                    break;
                }
                
            }

            size_t buffer_counter = 0;
            for (size_t i = rb.start; i < APPSMFCC_SIZE; ++i){
                buffer[buffer_counter] = rb.data[i];
                ++buffer_counter;
            }
            for (size_t i = 0; i < RB_END; ++i){
                buffer[buffer_counter] = rb.data[i];
                ++buffer_counter;
            }
            
            if (compare) {
                AppsMFCCHeader *header = (AppsMFCCHeader*)(buffer);

                if (cs.output_file == NULL) {
                    cs.comment_size = header->totalLen;
                    cs.left = header->totalLen;
                    ++cs.file_n;

                    while ((c = fgetc(input_file)) != '\0') {
                        if (c == EOF) {
                            fprintf(stderr, "Error: End of file before the end of the AppsMFCC header.\n");
                            exit_gracefully(input_file, cs.output_file, ERROR_EOF_ON_HEADER);
                        }
                    }
                    
                    snprintf(output_filename, 1024, "%s-%u.bin", output_prefix, cs.file_n);

                    cs.output_file = fopen(output_filename, "w");
                    if (cs.output_file == NULL) {
                       perror("Failed to open output file");
                       exit_gracefully(input_file, cs.output_file, ERROR_OUTPUT_FILE_OPEN);
                    }

                } else {
                    if (cs.comment_size != header->totalLen) {
                        fprintf(stderr, "Error: block total size inconsistence.\n");
                        exit_gracefully(input_file, cs.output_file, ERROR_BLOCK_SIZE);
                    }

                    while ((c = fgetc(input_file)) != '\0') {
                        if (c == EOF) {
                            fprintf(stderr, "Error: End of file before the end of the AppsMFCC header.\n");
                            exit_gracefully(input_file, cs.output_file, ERROR_EOF_ON_HEADER);
                        }
                    }
                    
                }


                for (size_t i = 0; i < header->dataLen; ++i){
                    c = fgetc(input_file);
                    if (c == EOF) {
                        fprintf(stderr, "Error: End of file before the end of the comment body.\n");
                        exit_gracefully(input_file, cs.output_file, ERROR_EOF_ON_BODY);
                    } else {
                        fputc(c, cs.output_file);
                    }
                }

                cs.left -= (int64_t)(header->dataLen);
                if (cs.left <= 0) {
                    fclose(cs.output_file);
                    cs.output_file = NULL;
                    cs.comment_size = 0;
                    cs.left = 0;
                    for (size_t i = 0; i < sizeof(output_filename); ++i) {
                        output_filename[i] = 0;
                    }
                }

            }
        }
    }

#undef RB_END
    exit_gracefully(input_file, cs.output_file, 0);
}
