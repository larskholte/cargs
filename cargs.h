//
// cargs.h
//

#pragma once

#include <stdio.h>

// A string literal which will have a non-NULL address and which can used as a placeholder for positive unary argument values.
#define CARG_YES "YES"

// Command-line argument types
typedef enum {
	CARG_END,		// An argument of this type denotes the end of a carg array.
	CARG_UNARY,		// An argument that denotes a simple yes/no option.
	CARG_KEYWORD,	// An argument that is followed by a string value on the command line.
	CARG_POS,		// A positional argument.
} carg_type;

typedef struct carg carg; // Predeclaration

// A function that is called when an argument is encountered on the command line.
typedef void (*carg_handler)(carg* ca);

// Structure that defines an argument accepted by the program.
struct carg {
	// The type of this argument.
	carg_type type;
	// Pointer to the string value associated with the argument.
	// For positional arguments, this value will be set to the argument itself.
	// For keyword arguments, this value will be set to the next argument on the command line after the keyword argument.
	// For unary arguments, this value will be set to the argument on the command line that set the argument, or will be set to NULL if it was negated.
	// This pointer may be NULL, in which case no value is associated with this argument.
	// If the pointer is NULL and the type is CARG_POS, all positional arguments will use the handler in this carg.
	const char** value;
	// Description of how the argument affects program behavior. May be NULL.
	const char* desc;
	// Name of the argument. E.g. "--foo". May be NULL for positional arguments.
	const char* name;
	// The flag character of the argument. E.g., 'f' as in "-abcdef".
	// Only relevant for unary arguments.
	// If there is no associated flag, set to '\0'.
	char flag;
	// Negation of the argument. E.g., "--no-foo".
	// If NULL, the argument has no negation.
	// Only relevant for unary arguments.
	const char* neg_name;
	// If non-NULL, this handler is called when the argument is encountered on the command line.
	carg_handler handler;
	// If non-NULL, this handler is called when the argument negation is encountered on the command line.
	carg_handler neg_handler;
};

// Errors that can occur during command-line argument parsing.
typedef enum {
	CARG_ERROR_INVALID_ARGUMENT,		// An invalid argument starting with "--" was encountered.
	CARG_ERROR_INVALID_FLAG,			// An invalid flag character was encountered in an argument starting with "-".
	CARG_ERROR_EXPECTED_ARGUMENT_AFTER,	// An argument was expected after the last argument on the command line.
	CARG_ERROR_UNEXPECTED_ARGUMENT,		// An unexpected (additional) positional argument was encountered.
} carg_error;

// Prints a description of the given error information to stderr.
void carg_print_error(carg_error, const char* p1, const char* p2);

// Structure that represents a command-line argument parser.
typedef struct {
	// A list of all the arguments accepted by the program, terminated by a carg of type CARG_END.
	carg* cargs;
	// Handler for errors that occur during command-line argument parsing.
	// If NULL, no handler is called. nerrors is simply incremented for each error.
	// The values of the two parameters p1 and p2 depend on the error.
	void (*error_handler)(carg_error, const char* p1, const char* p2);
	// Number of errors that occurred while parsing command-line arguments.
	int nerrors;
	carg* last_pos; // Used internally.
} carg_parser;

// Parses the given command-line arguments using the given parser.
// Sets the appropriate values and invokes the appropriate handlers for each argument encountered.
// Invokes parser->error_handler for each error.
// Sets parser->nerrors to the total number of errors that occurred.
void carg_parse(carg_parser* parser, int argc, const char* argv[]);

// Prints descriptions of the given cargs to the given file handle, until a carg of type CARG_END is encountered.
// Useful for printing help text.
void carg_print_descs(FILE* out, const carg* cargs);
