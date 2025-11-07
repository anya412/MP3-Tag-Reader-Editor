/***********************************************************************
 *  File Name   : edit.c
 *  Description : Source file for the MP3 Tag Editing Module.
 *                Provides functions to locate and modify specific
 *                ID3v2 tag frames in an MP3 file.
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

#include "edit.h"
#include "view.h"

// External frame ID array defined in view.c
extern const char *tags_name[MAX_FRAME_COUNT];

// Mapping edit options to frame IDs (e.g., -a → TPE1)
const char *edit_operation[MAX_FRAME_COUNT] = {"-a", "-t", "-A", "-y", "-m", "-l", "-c", "-C"};

/*
 * Validates and parses the command-line arguments for edit operation.
 * Stores target frame ID, source file name, and new tag data into the Edit struct.
 */
Status read_and_validate_edit_args(char **argv, Edit *edit)
{
    int index = check_edit_operation(argv[2]);
    if(index == -1)
    {
        fprintf(stderr, "ERROR: Invalid operation => %s\n", argv[2]);
        return e_failure;
    }

    // Get corresponding frame ID based on edit option
    strcpy(edit->frame_id, tags_name[index]);

    // Validate MP3 file
    if(argv[3] == NULL)
        return e_failure;

    if(strncmp(argv[3] + strlen(argv[3]) - 4, ".mp3", 4) != 0)
    {
        fprintf(stderr, "File should be .mp3 file\n");
        return e_failure;
    }

    edit->old_fname = strdup(argv[3]);

    // Collect all new data passed after filename
    if(argv[4] == NULL)
    {
        fprintf(stderr, "The new Data should not be Empty\n");
        return e_failure;
    }

    int i = 4;
    char buffer[100] = {0};
    while(argv[i])
    {
        strcat(buffer, argv[i++]);
        strcat(buffer, " ");
    }

    // Store new frame data and its length
    edit->new_frame_data = strdup(buffer);
    edit->new_frame_size = strlen(edit->new_frame_data);
    return e_success;
}

/*
 * Maps the edit option (like "-a") to index of tags_name[]
 */
int check_edit_operation(char *op)
{
    for(int i = 0; i < MAX_FRAME_COUNT; i++)
    {
        if(strcmp(edit_operation[i], op) == 0)
            return i;
    }
    return -1;
}

/*
 * Opens old MP3 file for reading and a temp file for writing updated data
 */
Status open_edit_files(Edit *edit)
{
    edit->fptr_old = fopen(edit->old_fname, "rb");
    if (edit->fptr_old == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", edit->old_fname);
        return e_failure;
    }

    edit->new_fname = strdup("temp.mp3");
    edit->fptr_new = fopen(edit->new_fname, "wb");
    if (edit->fptr_new == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", edit->new_fname);
        return e_failure;
    }
    return e_success;
}

/*
 * The main logic to edit the tag. It:
 * - Copies header and all frames from old to new file.
 * - Replaces the target frame's data and size.
 * - Copies rest of the MP3 data.
 * - Replaces the old file with new one.
 */
Status edit_tag(Edit *edit)
{
    fseek(edit->fptr_old, 0, SEEK_END);
    int file_end = ftell(edit->fptr_old);
    fseek(edit->fptr_old, 0, SEEK_SET);

    if(copy_header_to_file(edit) == e_failure)
        return e_failure;

    while(ftell(edit->fptr_old) < file_end)
    {
        if(read_edit_frame_id(edit) == e_failure)
            return e_failure;

        if(read_edit_frame_size(edit) == e_failure)
            return e_failure;

        if(copy_flag_to_file(edit) == e_failure)
            return e_failure;

        if(read_old_frame_data(edit) == e_failure)
            return e_failure;

        // If the frame matches the one to be edited
        if(strncmp(edit->old_frame_id, edit->frame_id, FRAME_ID_SIZE) == 0)
        {
            printf("INFO: Frame Id found!\n");
            
            // Move file pointer back to size field (4 bytes) before flag (2 bytes) and null (1 byte)
            fseek(edit->fptr_new, -7, SEEK_CUR);

            if(replace_edit_frame_size(edit) == e_failure)
                return e_failure;

            fseek(edit->fptr_new, FLAG_SIZE + 1, SEEK_CUR);  // Skip flag and null byte

            if(write_new_frame_data(edit) == e_failure)
                return e_failure;

            break;  // Stop after replacing the required frame
        }
        else if(copy_frame_data_to_file(edit) == e_failure)
        {
            return e_failure;
        }

        free(edit->old_frame_data);
    }

    if(copy_remainig_data(edit) == e_failure)
        return e_failure;

    if(replace_old_file(edit->old_fname, edit->new_fname) == e_failure)
        return e_failure;

    printf("INFO: Tag Edited Successfully\n");
    return e_success;
}

/*
 * Reads frame ID and writes it to the new file
 */
Status read_edit_frame_id(Edit *edit)
{
    char frame_id[5] = {0};
    if(read_data_from_file(frame_id, FRAME_ID_SIZE, edit->fptr_old) == e_failure)
        return e_failure;

    if(write_data_to_file(frame_id, FRAME_ID_SIZE, edit->fptr_new) == e_failure)
        return e_failure;

    strcpy(edit->old_frame_id, frame_id);
    return e_success;
}

/*
 * Reads frame size and writes it to new file.
 * Also converts it from big-endian (sync-safe format).
 */
Status read_edit_frame_size(Edit *edit)
{
    int frame_size;
    if(read_size_from_file(&frame_size, 4, edit->fptr_old) == e_failure)
        return e_failure;

    if(write_size_to_file(&frame_size, 4, edit->fptr_new) == e_failure)
        return e_failure;

    unsigned int b0 = (frame_size & 0x000000FF) << 24;
    unsigned int b1 = (frame_size & 0x0000FF00) << 8;
    unsigned int b2 = (frame_size & 0x00FF0000) >> 8;
    unsigned int b3 = (frame_size & 0xFF000000) >> 24;

    frame_size = b0 | b1 | b2 | b3;
    edit->frame_size = frame_size;
    return e_success;
}

/*
 * Writes the new frame size in big-endian (sync-safe) format
 */
Status replace_edit_frame_size(Edit *edit)
{
    int frame_size = edit->new_frame_size;

    unsigned int b0 = (frame_size & 0x000000FF) << 24;
    unsigned int b1 = (frame_size & 0x0000FF00) << 8;
    unsigned int b2 = (frame_size & 0x00FF0000) >> 8;
    unsigned int b3 = (frame_size & 0xFF000000) >> 24;

    frame_size = b0 | b1 | b2 | b3;

    if(write_size_to_file(&frame_size, 4, edit->fptr_new) == e_failure)
        return e_failure;

    printf("INFO: Old Frame Size Replaced\n");
    return e_success;
}

/*
 * Reads old frame data and stores it
 */
Status read_old_frame_data(Edit *edit)
{
    int size = edit->frame_size;
    edit->old_frame_data = malloc(size);
    if(read_data_from_file(edit->old_frame_data, size - 1, edit->fptr_old) == e_failure)
        return e_failure;

    edit->old_frame_data[size - 1] = '\0';
    return e_success;
}

/*
 * Writes the new frame data to the file
 */
Status write_new_frame_data(Edit *edit)
{
    if(write_data_to_file(edit->new_frame_data, edit->new_frame_size - 1, edit->fptr_new) == e_failure)
        return e_failure;

    printf("INFO: Old Frame Data Replaced\n");
    return e_success;
}

/*
 * Writes the existing frame data (not edited) to the new file
 */
Status copy_frame_data_to_file(Edit *edit)
{
    if(write_data_to_file(edit->old_frame_data, edit->frame_size - 1, edit->fptr_new) == e_failure)
        return e_failure;
    return e_success;
}

/*
 * Writes binary data to the file
 */
Status write_data_to_file(char *data, int size, FILE *fptr)
{
    if(fwrite(data, size, 1, fptr) != 1)
        return e_failure;
    return e_success;
}

/*
 * Writes frame size (4 bytes) to file
 */
Status write_size_to_file(int *data, int size, FILE *fptr)
{
    if(fwrite(data, size, 1, fptr) != 1)
        return e_failure;
    return e_success;
}

/*
 * Copies the 10-byte ID3 header from old file to new file
 */
Status copy_header_to_file(Edit *edit)
{
    char buffer[10];
    if(read_data_from_file(buffer, HEADER_SIZE, edit->fptr_old) == e_failure)
        return e_failure;

    if(write_data_to_file(buffer, HEADER_SIZE, edit->fptr_new) == e_failure)
        return e_failure;

    return e_success;
}

/*
 * Copies the 3-byte (2 flag + 1 null) data to new file
 */
Status copy_flag_to_file(Edit *edit)
{
    char buffer[3];
    if(read_data_from_file(buffer, FLAG_SIZE + 1, edit->fptr_old) == e_failure)
        return e_failure;

    if(write_data_to_file(buffer, FLAG_SIZE + 1, edit->fptr_new) == e_failure)
        return e_failure;

    return e_success;
}

/*
 * Copies all remaining data after frames to the new file
 */
Status copy_remainig_data(Edit *edit)
{
    char buffer;
    while(fread(&buffer, 1, 1, edit->fptr_old) == 1)
    {
        if(fwrite(&buffer, 1, 1, edit->fptr_new) != 1)
            return e_failure;
    }

    printf("INFO: Remaining Data Copied Successfully\n");
    return e_success;
}

/*
 * Replaces the original file with the edited one
 */
Status replace_old_file(char *old_fname, char *new_fname)
{
    rename(new_fname, old_fname);  // Rename temp to original
    return e_success;
}