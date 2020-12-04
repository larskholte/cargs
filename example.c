// example.c

#include "cargs.h"

#include <stdio.h>
#include <stdlib.h>

// Values which can be set by command-line arguments.
const char* foo_val = CARG_YES;
const char* bar_val = NULL;
const char* baz_val = NULL;
const char* key_val = "default key value";
const char* help_val = NULL;
const char* pos1_val = "default pos1 value";
const char* pos2_val = NULL;

// Handlers for command-line arguments.
void special_handler(carg* ca) {
	printf("special handler called\n");
}

// List of command-line arguments this program accepts.
carg cargs[] = {
	{
		CARG_UNARY,		// Type
		&foo_val,		// Value
		" -f, --foo    Sets foo.\n"
		" --no-foo     Unsets foo.", // Description
		"--foo",		// Name
		'f',			// Flag
		"--no-foo",		// Negation
	},
	{
		CARG_UNARY,
		&bar_val,
		" -b, --bar    Sets bar.",
		"--bar",
		'b',
	},
	{
		CARG_UNARY,
		&baz_val,
		" --baz        Sets baz.\n"
		" --no-baz     Unsets baz.",
		"--baz",
		'\0',
		"--no-baz",
	},
	{
		CARG_KEYWORD,
		&key_val,
		" --key <val>  Sets key to val.",
		"--key",
	},
	{
		CARG_UNARY,
		NULL,
		" --special    Calls a special function.",
		"--special",
		.handler = special_handler,
	},
	{
		CARG_UNARY,
		&help_val,
		" --help       Prints this message.",
		"--help",
		'h',
	},
	{
		CARG_POS,
		&pos1_val,
		" pos1         Positional argument 1.",
	},
	{
		CARG_POS,
		&pos2_val,
		" pos2         Positional argument 2.",
	},
	// The list must end with a carg of type CARG_END
	{ CARG_END }
};

int main(int argc, const char* argv[]) {
	// Create argument parser
	carg_parser parser = {
		cargs,
		carg_print_error,
	};
	// Parse arguments
	carg_parse(&parser, argc, argv);
	if (parser.nerrors) {
		// Errors occurred while parsing arguments
		fprintf(stderr, "exiting due to invocation errors\n");
		return 1;
	}
	if (help_val) {
		// Print help
		printf("Usage: example [options] [pos1] [pos2]\n");
		carg_print_descs(stdout, cargs);
		return 0;
	}
	// Normal program execution
	printf("foo: %s\n", foo_val ? "YES" : "NO");
	printf("bar: %s\n", bar_val ? "YES" : "NO");
	printf("baz: %s\n", baz_val ? "YES" : "NO");
	printf("key: %s\n", key_val);
	printf("pos1: %s\n", pos1_val ? pos1_val : "NULL");
	printf("pos2: %s\n", pos2_val ? pos2_val : "NULL");
	return 0;
}
