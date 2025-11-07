/***********************************************************************
 *  File Name   : view.h
 *  Description : Header file for the MP3 Tag Viewing Module.
 *                Declares structures and function prototypes used for
 *                reading and displaying ID3v2 tag frames from an MP3 file.
 *
 *                Structures:
 *                - TagInfo
 *
 *                Functions:
 *                - read_and_validate_args()
 *                - open_files()
 *                - check_operation_type()
 *                - display_tag()
 *                - read_frame_id()
 *                - read_frame_size()
 *                - read_frame_data()
 *                - read_data_from_file()
 *                - read_size_from_file()
 *
 ***********************************************************************/

#ifndef VIEW_H
#define VIEW_H

// Include necessary standard and custom header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "types.h"  // Includes custom Status and OperationType definitions

// Structure to hold tag information extracted from the MP3 file
typedef struct TagInfo
{
    FILE *fptr_src_mp3;                         // File pointer to source MP3 file
    char *src_mp3_fname;                        // Name of the source MP3 file
    char frame_id[MAX_FRAME_COUNT][FRAME_ID_SIZE + 1];   // Array of frame IDs (each is a 4-character string + null terminator)
    int frame_Size[MAX_FRAME_COUNT];           // Array holding sizes of corresponding frames
    char *frame_data[MAX_FRAME_COUNT];         // Array of pointers to frame data (dynamically allocated)
} TagInfo;

// Function to validate command-line arguments and initialize TagInfo
Status read_and_validate_args(char **argv, TagInfo *tagInfo);

// Function to open required files and assign file pointers
Status open_files(TagInfo *tagInfo);

// Function to determine the type of operation to perform (e.g., view, edit)
OperationType check_operation_type(char *argv[]);

// Function to display tag/frame information from the MP3 file
Status display_tag(TagInfo *tagInfo);

// Function to read a frame ID at a given index
Status read_frame_id(int index, TagInfo *tagInfo);

// Function to read the size of a frame at a given index
Status read_frame_size(int index, TagInfo *tagInfo);

// Function to read the actual frame data at a given index
Status read_frame_data(int index, TagInfo *tagInfo);

// Generic function to read binary data from a file into a buffer
Status read_data_from_file(char *data, uint size, FILE *fptr_src_mp3);

// Function to read frame size from file 
Status read_size_from_file(int *data, uint size, FILE *fptr_src_mp3);

#endif  // VIEW_H