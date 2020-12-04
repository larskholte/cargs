//
// cargs.c
//

#include "cargs.h"

#include <stdlib.h>
#include <string.h>

// Helper function to parse a positional command-line argument.
static void carg_parse_pos(carg_parser* parser, const char* arg) {
	// Iterate through cargs, looking for an unused positional carg.
	carg* ca;
	if (parser->last_pos) ca = parser->last_pos; // Pick up where we left off
	else ca = parser->cargs; // Begin at the beginning
	for (; ca->type != CARG_END; ca++) {
		if (ca->type != CARG_POS) continue; // Only consider positional cargs
		if (ca == parser->last_pos && ca->value) continue; // Only set a positional carg value once
		// We found an unused positional carg
		parser->last_pos = ca;
		if (ca->value) *(ca->value) = arg;
		if (ca->handler) ca->handler(ca);
		break;
	}
	if (ca->type == CARG_END) { // We reached the end of all positional cargs without finding a match.
		parser->nerrors++;
		if (parser->error_handler) {
			parser->error_handler(CARG_ERROR_UNEXPECTED_ARGUMENT, arg, NULL);
		}
		parser->last_pos = ca;
	}
}

// Helper function to parse a collection of flags.
static void carg_parse_flags(carg_parser* parser, const char* arg) {
	// Process each flag character.
	const char* flag_ptr;
	for (flag_ptr = arg + 1; *flag_ptr; flag_ptr++) {
		char flag = *flag_ptr;
		// Iterate through cargs, looking for matching flags
		carg* ca;
		for (ca = parser->cargs; ca->type != CARG_END; ca++) {
			if (ca->type != CARG_UNARY) continue; // Only unary arguments have flags
			if (ca->flag == flag) {
				if (ca->value) *(ca->value) = arg;
				if (ca->handler) ca->handler(ca);
				break;
			}
		}
		if (ca->type == CARG_END) {
			parser->nerrors++;
			if (parser->error_handler) {
				parser->error_handler(CARG_ERROR_INVALID_FLAG, arg, flag_ptr);
			}
			// Don't bother parsing the rest of the flags
			break;
		}
	}
}

void carg_parse(carg_parser* parser, int argc, const char* argv[]) {
	parser->nerrors = 0;
	parser->last_pos = NULL;
	int state = 0;
	carg* key = NULL; // Current keyword carg
	const char* arg; // Current command-line argument
	for (int i = 1; i < argc; i++) {
		arg = argv[i];
		switch (state) {
		case 0: { // Default state
			// Iterate through cargs, looking for a match
			carg* ca;
			for (ca = parser->cargs; ca->type != CARG_END; ca++) {
				if (ca->type == CARG_POS) continue; // Skip positional arguments for now
				if (strcmp(arg, ca->name) == 0) {
					// carg name matches
					if (ca->type == CARG_UNARY) {
						if (ca->value) *(ca->value) = arg;
						if (ca->handler) ca->handler(ca);
					} else { // Keyword argument
						// Expect a value to follow this keyword argument
						key = ca;
						state = 1;
					}
					break;
				}
				// Only unary arguments can have negations
				if (ca->type != CARG_UNARY) continue;
				if (ca->neg_name && strcmp(arg, ca->neg_name) == 0) {
					// carg negation matches
					if (ca->value) *(ca->value) = NULL;
					if (ca->neg_handler) ca->neg_handler(ca);
					break;
				}
			}
			if (ca->type != CARG_END) break; // A match was found
			// Check if the argument is a flag collection
			if (arg[0] == '-' && arg[1] != '\0') {
				if (arg[1] == '-') {
					if (arg[2] == '\0') { // -- begins positional arguments
						state = 2;
						break;
					}
					// Argument begins with -- but is unrecognized. It may be misspelled.
					parser->nerrors++;
					if (parser->error_handler) {
						parser->error_handler(CARG_ERROR_INVALID_ARGUMENT, arg, NULL);
					}
				} else {
					// The argument appears to be a collection of flags. E.g., "-abcdef".
					carg_parse_flags(parser, arg);
				}
			} else {
				// Argument must be a positional argument
				carg_parse_pos(parser, arg);
			}
		}	break;
		case 1: // Value of keyword argument
			if (key->value) *(key->value) = arg;
			if (key->handler) key->handler(key);
			state = 0;
			break;
		case 2: // Positional-only arguments
			carg_parse_pos(parser, arg);
			break;
		}
	}
	// Check that parser is not expecting another argument
	if (state == 1) {
		parser->nerrors++;
		if (parser->error_handler) {
			parser->error_handler(CARG_ERROR_EXPECTED_ARGUMENT_AFTER, arg, NULL);
		}
	}
}

void carg_print_descs(FILE* out, const carg* cargs) {
	const carg* ca;
	for (ca = cargs; ca->type != CARG_END; ca++) {
		if (ca->desc) fprintf(out, "%s\n", ca->desc);
	}
}

void carg_print_error(carg_error err, const char* p1, const char* p2) {
	if (err == CARG_ERROR_INVALID_ARGUMENT) {
		fprintf(stderr, "error: invalid argument: \"%s\"\n", p1);
	}
	else if (err == CARG_ERROR_INVALID_FLAG) {
		fprintf(stderr, "error: invalid flag '%c' in argument \"%s\"\n", *p2, p1);
	}
	else if (err == CARG_ERROR_EXPECTED_ARGUMENT_AFTER) {
		fprintf(stderr, "error: expected an argument after \"%s\"\n", p1);
	}
	else if (err == CARG_ERROR_UNEXPECTED_ARGUMENT) {
		fprintf(stderr, "error: unexpected positional argument \"%s\"\n", p1);
	}
}
