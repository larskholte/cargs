# cargs

Fast, intuitive command-line argument handling in just under 250 lines of straight C!

Correctly parsing the command-line arguments to your program can be difficult. Overly simple approaches can result in a limiting interface to your program. On the other hand, complex approaches are hard to get right and can require significant development time. cargs allows you to develop a rich and consistent command-line interface for your program with minimal effort.

### Usage

The basic structure of a program that uses cargs is shown below:

``` C
#include "cargs.h"
#include <stdio.h>

// Variables which can be set by the command-line arguments.
const char* foo_val = CARG_YES;             // The --foo unary argument
const char* key_val = "default key value";  // The --key keyword argument
const char* pos_val = NULL;                 // The first and only positional argument

// List of command-line arguments this program accepts.
carg cargs[] = {
    {
        CARG_UNARY,     // Type
        &foo_val,       // Value
        " -f, --foo    Sets foo.\n"
        " --no-foo     Unsets foo.", // Description
        "--foo",        // Name
        'f',            // Flag
        "--no-foo",     // Negation
    },
    {
        CARG_KEYWORD,   // Type
        &key_val,       // Value
        " --key <val>  Sets key to val.", // Description
        "--key",        // Name
    },
    {
        CARG_POS,       // Type
        &pos_val,      // Value
        " pos          Positional argument 1.", // Description
    },
    // The list must end with a carg of type CARG_END
    { CARG_END }
};

int main(int argc, const char* argv[]) {
    // Create argument parser
    carg_parser parser = {
        cargs,              // List of accepted arguments
        carg_print_error,   // Error handler that prints error information
    };
    // Parse arguments
    carg_parse(&parser, argc, argv);
    if (parser.nerrors) {
        // Errors occurred while parsing arguments
        fprintf(stderr, "exiting due to invocation errors\n");
        return 1;
    }
    // Normal program execution
    printf("foo: %s\n", foo_val ? "YES" : "NO");
    printf("key: %s\n", key_val);
    printf("pos: %s\n", pos_val ? pos_val : "NULL");
    return 0;
}
```

When compiled as *test*, the program above works as you would expect it to:

``` Bash
# Show defaults
./test
foo: YES
key: default key value
pos: NULL

# Supply our own argument values
./test --key mykey --no-foo myarg
foo: NO
key: mykey
pos: myarg

# Use -- to begin positional-only arguments
./test --key mykey -- --hello--
foo: YES
key: mykey
pos: --hello--
```

cargs can also detect many common invocation errors. When it detects an error, it invokes a user-defined handler. In the above program, the handler simply prints error information to stderr.

``` Bash
# Pass an invalid argument
./test --bar
error: invalid argument "--bar"
exiting due to invocation errors

# Missing value for key
./test --key
error: expected an argument after "--key"
exiting due to invocation errors

# Invalid flag
./test -h
error: invalid flag 'h' in argument "-h"
exiting due to invocation errors

# Extra arguments
./test myarg1 myarg2
error: unexpected positional argument "myarg2"
exiting due to invocation errors
```

For a more fully-featured example program that prints help text and uses a custom argument handler, see example.c.
