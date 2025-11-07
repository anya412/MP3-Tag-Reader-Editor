/***********************************************************************
 *  File Name   : view.c
 *  Description : Source file for the MP3 Tag Viewing Module.
 *                Implements functionality to parse and display
 *                ID3v2 tag frames from a given MP3 file.
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



#include "view.h"
#include "types.h"

// Array of known frame IDs in ID3v2 tags
const char *tags_name[MAX_FRAME_COUNT] = {"TPE1", "TIT2", "TALB", "TYER", "TCON", "TEXT", "TCOM", "COMM"};
// Corresponding human-readable labels for the above frame IDs
const char *tag_labels[MAX_FRAME_COUNT] = {"Artist", "Title", "Album", "Year", "Genre", "Lyricist", "Composer", "Comments"};  

// Function to validate input arguments and extract the MP3 filename
Status read_and_validate_args(char **argv, TagInfo *tagInfo)
{
    // Check if filename argument is present
    if(argv[2] == NULL)
        return e_failure;

    // Ensure file ends with ".mp3"
    if(strncmp(argv[2] + strlen(argv[2]) - 4, ".mp3", 4) != 0)
    {
        fprintf(stderr, "File should be .mp3 file\n");
        return e_failure;
    }

    // Store the filename in the tagInfo structure
    tagInfo->src_mp3_fname = strdup(argv[2]);
    return e_success;
}

// Function to display the ID3 tag frames from the MP3 file
Status display_tag(TagInfo *tagInfo)
{
    int index = 0;

    // Determine the file size and set file pointer to start after header
    fseek(tagInfo->fptr_src_mp3, 0, SEEK_END);
    int file_end = ftell(tagInfo->fptr_src_mp3);
    fseek(tagInfo->fptr_src_mp3, 10, SEEK_SET);  // Skip 10-byte ID3 header

    // Read each frame sequentially
    while(index < MAX_FRAME_COUNT && ftell(tagInfo->fptr_src_mp3) < file_end)
    {
        // Read frame ID
        if(read_frame_id(index, tagInfo) == e_failure)
        {
            // If unknown frame, still move pointer forward by size and continue
            if(read_frame_size(index, tagInfo) == e_failure)
                return e_failure;  
            fseek(tagInfo->fptr_src_mp3, tagInfo->frame_Size[index] + 2, SEEK_CUR); // skip data and flags
            continue;
        }

        // Read frame size and data
        if(read_frame_size(index, tagInfo) == e_failure)
            return e_failure;

        fseek(tagInfo->fptr_src_mp3, 3, SEEK_CUR);  // Skip encoding byte and 2-byte flags

        if(read_frame_data(index, tagInfo) == e_failure)
            return e_failure;

        index++;
    }

    // Print the tag information in a formatted table
    printf("===========================================================================\n");
    printf("| %-15s:%6s%-50s|\n", "Tag Name", " ", "Tag Data");
    printf("===========================================================================\n");

    for (int i = 0; i < index; i++)
    {
        for (int j = 0; j < MAX_FRAME_COUNT; j++)
        {
            if (strcmp(tagInfo->frame_id[i], tags_name[j]) == 0)
            {
                printf("| %-15s:%6s%-50s|\n", tag_labels[j], " ", tagInfo->frame_data[i]);
                break;
            }
        }
    }
    printf("===========================================================================\n");
    return e_success;
}

// Function to determine the operation type from command-line arguments
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
        return e_edit;
    if(strcmp(argv[1], "-v") == 0) 
        return e_display;

    // Invalid operation
    fprintf(stderr, "Error: Invalid Operation => %s\n", argv[1]);
    return e_unsupported;
}

// Function to open the source MP3 file
Status open_files(TagInfo *tagInfo)
{
    // Open MP3 file in binary read mode
    tagInfo->fptr_src_mp3 = fopen(tagInfo->src_mp3_fname, "rb");
    if (tagInfo->fptr_src_mp3 == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", tagInfo->src_mp3_fname);
        return e_failure;
    }
    return e_success;
}

// Helper function to check if the current frame ID is supported
Status check_frame_index(char *frame_id)
{
    for(int i = 0; i < MAX_FRAME_COUNT; i++)
    {
        if(strcmp(tags_name[i], frame_id) == 0)
            return e_success;
    }
    return e_failure;
}

// Function to read the 4-byte frame ID and store it
Status read_frame_id(int index, TagInfo *tagInfo)
{
    char frame_id[5] = {0}; // Temporary buffer to store 4-byte frame ID + null terminator

    // Read 4 bytes of frame ID
    if(read_data_from_file(frame_id, 4, tagInfo->fptr_src_mp3) == e_failure)
        return e_failure;

    // If not a valid/known frame, skip
    if(check_frame_index(frame_id) == e_failure)
        return e_failure;

    // Store the valid frame ID
    strcpy(tagInfo->frame_id[index], frame_id);
    return e_success;
}

// Function to read the 4-byte frame size, reorder bytes to convert to int
Status read_frame_size(int index, TagInfo *tagInfo)
{
    int frame_size;

    // Read 4 bytes as raw int
    if(read_size_from_file(&frame_size, 4, tagInfo->fptr_src_mp3) == e_failure)
    {
        fprintf(stderr, "Cannot read the size\n");
        return e_failure;
    }

    // Convert from big-endian to little-endian (ID3v2 uses sync-safe integers)
    unsigned int b0 = (frame_size & 0x000000FF) << 24;
    unsigned int b1 = (frame_size & 0x0000FF00) << 8;
    unsigned int b2 = (frame_size & 0x00FF0000) >> 8;
    unsigned int b3 = (frame_size & 0xFF000000) >> 24;

    frame_size = b0 | b1 | b2 | b3;

    tagInfo->frame_Size[index] = frame_size;
    return e_success;
}

// Function to read the frame data content based on previously read size
Status read_frame_data(int index, TagInfo *tagInfo)
{  
    int size = tagInfo->frame_Size[index];

    // Allocate memory to hold frame data
    tagInfo->frame_data[index] = malloc(size);
    if(tagInfo->frame_data[index] == NULL)
        return e_failure;

    // Read frame data (excluding last byte for null termination)
    if(read_data_from_file(tagInfo->frame_data[index], size - 1, tagInfo->fptr_src_mp3) == e_failure)
        return e_failure;

    // Null-terminate the string
    tagInfo->frame_data[index][size - 1] = '\0';

    return e_success;
}

// Utility to read binary data of specified size from a file
Status read_data_from_file(char *data, uint size, FILE * fptr_src_mp3)
{
    if(fread(data, size, 1, fptr_src_mp3) != 1)
        return e_failure;
    return e_success;
}

// Utility to read an integer (size) from the file
Status read_size_from_file(int *data, uint size, FILE * fptr_src_mp3)
{
    if(fread(data, size, 1, fptr_src_mp3) != 1)
        return e_failure;
    return e_success;
}