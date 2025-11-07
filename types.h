/***********************************************************************
 *  File Name   : types.h
 *  Description : Header file containing common type definitions, constants,
 *                and enumerations used across the MP3 tag editing and
 *                viewing modules.
 *
 *                Type Definitions:
 *                - uint
 *                - Status (e_success, e_failure)
 *                - OperationType (e_display, e_edit, e_unsupported)
 *
 *                Macros:
 *                - MAX_FRAME_COUNT
 *                - FRAME_ID_SIZE
 *                - HEADER_SIZE
 *                - FLAG_SIZE
 *
 *                Functions:
 *                - print_help_msg()
 *
 ***********************************************************************/

#ifndef TYPES_H
#define TYPES_H

/* 
 * User-defined types and constants for MP3 tag processing
 */

// Define shorthand for unsigned int
typedef unsigned int uint;

// Maximum number of frames to be processed (can be updated as needed)
#define MAX_FRAME_COUNT 8

// Size of a frame ID in bytes (ID3 uses 4-character frame IDs)
#define FRAME_ID_SIZE 4

// Size of the ID3v2 header (typically 10 bytes)
#define HEADER_SIZE 10

// Size of the flag section in a frame header (typically 2 bytes)
#define FLAG_SIZE 2

/*
 * Enum representing function return statuses
 * e_success   → Operation was successful
 * e_failure   → Operation failed
 */
typedef enum
{
    e_success,
    e_failure
} Status;

/*
 * Enum representing the type of operation requested
 * e_display     → View the ID3 tag data
 * e_edit        → Edit a frame in the ID3 tag (for future extension)
 * e_unsupported → Invalid or unsupported operation
 */
typedef enum
{
    e_display,
    e_edit,
    e_unsupported
} OperationType;

// Function to print help/instructions on how to use the program
void print_help_msg(char ** argv);

#endif  // TYPES_H