#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

// Ensure we have GNU extensions available
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// Standard C library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

// POSIX includes
#include <unistd.h>

// Ensure stderr, stdin, stdout are available
#ifndef stderr
extern FILE *stderr;
#endif
#ifndef stdout  
extern FILE *stdout;
#endif
#ifndef stdin
extern FILE *stdin;
#endif

#endif // COMMON_INCLUDES_H