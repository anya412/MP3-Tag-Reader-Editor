/***********************************************************************
 *  File Name   : edit.h
 *  Description : Header file for the MP3 Tag Editing Module.
 *                Declares structures and function prototypes used for
 *                editing ID3v2 tag frames in an MP3 file.
 *
 *                Structures:
 *                - Edit
 *
 *                Functions:
 *                - read_and_validate_edit_args()
 *                - check_edit_operation()
 *                - replace_old_file()
 *                - open_edit_files()
 *                - edit_tag()
 *                - read_edit_frame_id()
 *                - read_edit_frame_size()
 *                - replace_edit_frame_size()
 *                - read_old_frame_data()
 *                - write_data_to_file()
 *                - write_size_to_file()
 *                - copy_header_to_file()
 *                - copy_flag_to_file()
 *                - write_new_frame_data()
 *                - copy_frame_data_to_file()
 *                - copy_remainig_data()
 *
 ***********************************************************************/

#ifndef EDIT_H
#define EDIT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"  // Includes Status and other common definitions

// Structure to hold all necessary information for editing MP3 tag frames
typedef struct Edit
{
    FILE *fptr_old;                      // File pointer to the original MP3 file
    FILE *fptr_new;                      // File pointer to the temporary new MP3 file
    char *old_fname;                     // Name of the original MP3 file
    char frame_id[FRAME_ID_SIZE + 1];    // Frame ID to be edited (null-terminated)
    char old_frame_id[FRAME_ID_SIZE + 1];// Currently read frame ID (for comparison)
    int frame_size;                      // Original size of the frame to be edited
    int new_frame_size;                  // New size of the frame data
    char *old_frame_data;                // Pointer to original frame data
    char *new_frame_data;                // Pointer to new frame data (to replace with)
    char *new_fname;                     // Name of the temporary edited file
} Edit;

// Function to validate and initialize arguments for edit operation
Status read_and_validate_edit_args(char **argv, Edit *edit);

// Function to check if the operation passed is valid (e.g., "-e" for edit)
int check_edit_operation(char *op);

// Function to replace the original MP3 file with the newly edited one
Status replace_old_file(char *old_fname, char *new_fname);

// Function to open both original and new files required for editing
Status open_edit_files(Edit *edit);

// Function that performs the overall tag editing process
Status edit_tag(Edit *edit);

// Function to read a frame ID during edit processing
Status read_edit_frame_id(Edit *edit);

// Function to read the size of a frame during edit processing
Status read_edit_frame_size(Edit *edit);

// Function to write the new frame size to the output file
Status replace_edit_frame_size(Edit *edit);

// Function to read the original frame data
Status read_old_frame_data(Edit *edit);

// Writes generic data of given size to a file
Status write_data_to_file(char *data, int size, FILE *fptr);

// Writes frame size to a file (typically 4 bytes)
Status write_size_to_file(int *data, int size, FILE *fptr);

// Copies the initial 10-byte MP3 ID3v2 header to the new file
Status copy_header_to_file(Edit *edit);

// Copies the 2-byte flag section of the frame to the new file
Status copy_flag_to_file(Edit *edit);

// Writes the new frame data into the output file
Status write_new_frame_data(Edit *edit);

// Copies other frame data (non-edited) to the output file
Status copy_frame_data_to_file(Edit *edit);

// Copies remaining data (after frame data) from original to new file
Status copy_remainig_data(Edit *edit);

#endif  // EDIT_H