/***********************************************************************
 *  File Name   : main.c
 *  Description : Entry point for the MP3 Tag Editor and Viewer.
 *                Handles user input, determines operation mode (view/edit),
 *                validates command-line arguments, and dispatches the
 *                requested functionality.
 *
 *                Supports the following operations:
 *                - Viewing MP3 tag information
 *                - Editing a specific MP3 tag using tag code
 *                - Displaying help with tag code descriptions
 *
 *                Functions:
 *                - main()
 *                - print_help_msg()
 *
 ***********************************************************************/

#include "view.h"
#include "types.h"
#include "edit.h"

int main(int argc, char *argv[])
{
    TagInfo tagInfo;  // Structure to hold information for viewing tags

    // Check if minimum required arguments are passed
    if (argc < 2)
    {
        fprintf(stderr, "For help: %s --help\n", argv[0]);
        return 0;
    }

    // Display help message if --help is selected
    if (strcmp(argv[1], "--help") == 0)
    {
        print_help_msg(argv);
        return 0;
    }

    // Ensure that arguments for operation are sufficient
    if (argc < 3)
    {
        printf("To View MP3 Tags : %s -v <file_name.mp3>\n", argv[0]);
        printf("To Edit MP3 Tags : %s -e <tag_code> <file_name.mp3> <new_tag_data>\n", argv[0]); 
        printf("For help, type: \n%s --help\n", argv[0]);
        return -1;
    }

    // Determine the type of operation: view/edit
    OperationType op = check_operation_type(argv);
    if (op == e_unsupported)
        return -1;

    // If operation is 'view' (-v)
    else if (op == e_display)
    {
        // Check for correct number of arguments for view operation
        if (argc == 3)
        {
            // Validate command-line arguments
            if (read_and_validate_args(argv, &tagInfo) == e_failure)
                return e_failure;

            // Open the mp3 file
            if (open_files(&tagInfo) == e_failure)
                return e_failure;

            // Display the tags from the mp3 file
            if (display_tag(&tagInfo) == e_failure)
                return e_failure;
        }
        else
        {
            fprintf(stderr, "ERROR: Please Enter Correct Syntax. For Help, Type: \n%s --help\n", argv[0]);
            return -1;
        }
    }

    // If operation is 'edit' (-e)
    else if (op == e_edit)
    {
        Edit edit;  // Structure to hold information for editing tags

        // Check if sufficient arguments are passed for editing
        if (argc >= 4)
        {
            // Validate command-line arguments for editing
            if (read_and_validate_edit_args(argv, &edit) == e_failure)
                return e_failure;

            // Open the old and new files needed for editing
            if (open_edit_files(&edit) == e_failure)
                return e_failure;

            // Perform the editing operation on the tag
            if (edit_tag(&edit) == e_failure)
                return e_failure;
        }
        else
        {
            printf("To View MP3 Tags : %s -v <file_name.mp3>\n", argv[0]);
            printf("To Edit MP3 Tags : %s -e <tag_code> <file_name.mp3> <new_tag_data>\n", argv[0]);
            printf("For help, type: \n%s --help\n", argv[0]);
            return -1; 
        }
    }

    return 0; 
}

// External arrays containing tag label and operation names
extern const char *tag_labels[MAX_FRAME_COUNT];
extern const char *edit_operation[MAX_FRAME_COUNT];

// Function to print the help message for usage
void print_help_msg(char ** argv)
{
    printf("To View MP3 Tags : %s -v <file_name.mp3>\n", argv[0]);
    printf("To Edit MP3 Tags : %s -e <tag_code> <file_name.mp3> <new_tag_data>\n", argv[0]);
    printf("===================================\n");
    printf("| %-15s:%15s |\n", "Tag Code", "Tag Name");
    printf("===================================\n");

    // Display supported tag codes and their human-readable names
    for (int i = 0; i < MAX_FRAME_COUNT; i++)
    {
        printf("| %-15s:%15s |\n", edit_operation[i], tag_labels[i]);
    }

    printf("===================================\n");
}