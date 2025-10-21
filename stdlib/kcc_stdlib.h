//
// Created by Pedro Dias Vicente on 19/10/2025.
//

#ifndef KCC_STDLIB_H
#define KCC_STDLIB_H

// ===== STDIO.H =====
// Standard I/O functions
int printf(const char *format, ...);
int fprintf(void *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, unsigned long size, const char *format, ...);
int scanf(const char *format, ...);
int fscanf(void *stream, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

int putchar(int c);
int puts(const char *s);
int getchar(void);
char *gets(char *s);
char *fgets(char *s, int size, void *stream);
int fputs(const char *s, void *stream);

void *fopen(const char *filename, const char *mode);
int fclose(void *stream);
int fflush(void *stream);
unsigned long fread(void *ptr, unsigned long size, unsigned long nmemb, void *stream);
unsigned long fwrite(const void *ptr, unsigned long size, unsigned long nmemb, void *stream);
int fseek(void *stream, long offset, int whence);
long ftell(void *stream);
void rewind(void *stream);

int remove(const char *filename);
int rename(const char *oldname, const char *newname);

void *stdin;
void *stdout;
void *stderr;

// ===== STDLIB.H =====
// Memory allocation
void *malloc(unsigned long size);
void *calloc(unsigned long nmemb, unsigned long size);
void *realloc(void *ptr, unsigned long size);
void free(void *ptr);

// Process control
void exit(int status);
void abort(void);
int atexit(void (*func)(void));

// String conversion
int atoi(const char *str);
long atol(const char *str);
long long atoll(const char *str);
double atof(const char *str);
long strtol(const char *str, char **endptr, int base);
unsigned long strtoul(const char *str, char **endptr, int base);
double strtod(const char *str, char **endptr);

// Random numbers
int rand(void);
void srand(unsigned int seed);

// Searching and sorting
void *bsearch(const void *key, const void *base, unsigned long nmemb,
              unsigned long size, int (*compar)(const void *, const void *));
void qsort(void *base, unsigned long nmemb, unsigned long size,
           int (*compar)(const void *, const void *));

// Absolute value
int abs(int n);
long labs(long n);

// Environment
char *getenv(const char *name);
int system(const char *command);

// ===== STRING.H =====
// String operations
unsigned long strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned long n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, unsigned long n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned long n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);
unsigned long strspn(const char *s, const char *accept);
unsigned long strcspn(const char *s, const char *reject);
char *strpbrk(const char *s, const char *accept);
char *strtok(char *str, const char *delim);
char *strdup(const char *s);

// Memory operations
void *memcpy(void *dest, const void *src, unsigned long n);
void *memmove(void *dest, const void *src, unsigned long n);
int memcmp(const void *s1, const void *s2, unsigned long n);
void *memset(void *s, int c, unsigned long n);
void *memchr(const void *s, int c, unsigned long n);

// ===== MATH.H =====
// Mathematical functions
double sqrt(double x);
double pow(double x, double y);
double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);
double exp(double x);
double log(double x);
double log10(double x);
double ceil(double x);
double floor(double x);
double fabs(double x);
double fmod(double x, double y);

// ===== CTYPE.H =====
// Character testing
int isalpha(int c);
int isdigit(int c);
int isalnum(int c);
int isspace(int c);
int isupper(int c);
int islower(int c);
int isprint(int c);
int iscntrl(int c);
int ispunct(int c);
int isxdigit(int c);

// Character conversion
int toupper(int c);
int tolower(int c);

// ===== TIME.H =====
typedef long time_t;
typedef struct {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} tm;

time_t time(time_t *tloc);
char *ctime(const time_t *timep);
struct tm *localtime(const time_t *timep);
struct tm *gmtime(const time_t *timep);
time_t mktime(struct tm *tm);
unsigned long strftime(char *s, unsigned long max, const char *format, const struct tm *tm);
double difftime(time_t time1, time_t time0);

// ===== ASSERT.H =====
void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function);

#define assert(expr) \
    ((expr) ? (void)0 : __assert_fail(#expr, __FILE__, __LINE__, __func__))

// ===== STDARG.H =====
typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

// ===== LIMITS.H =====
#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN (-2147483647 - 1)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U
#define LONG_MIN (-9223372036854775807L - 1)
#define LONG_MAX 9223372036854775807L
#define ULONG_MAX 18446744073709551615UL

// ===== STDBOOL.H =====
#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#endif

// ===== NULL =====
#ifndef NULL
#define NULL ((void *)0)
#endif

#endif // KCC_STDLIB_H