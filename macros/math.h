//
// Created by Pedro Dias Vicente on 03/09/2025.
//
/*
 * simple_math.h - A simplified, educational example of what the
 * C standard math header (math.h) might look like.
 *
 * This file demonstrates function prototypes and macros for common
 * mathematical operations.
 */

// =============================================================================
//      1. Header Guard
// =============================================================================
#ifndef MATH_H
#define MATH_H

// =============================================================================
//      2. Macro Definitions for Mathematical Constants
// =============================================================================
#define M_PI 3.14159265358979323846         // Pi
#define M_E 2.71828182845904523536          // Euler's number
#define M_SQRT2 1.41421356237309504880      // Square root of 2

// =============================================================================
//      3. Function Prototypes (Declarations)
// =============================================================================
// These functions typically operate on 'double' precision floating-point numbers.
// The actual implementations are in the C standard library.

// --- Trigonometric Functions ---
// Note: These functions usually take angles in radians.
double sin(double x);  // Sine of x
double cos(double x);  // Cosine of x
double tan(double x);  // Tangent of x

// --- Inverse Trigonometric Functions ---
double asin(double x); // Arc sine of x
double acos(double x); // Arc cosine of x
double atan(double x); // Arc tangent of x

// --- Hyperbolic Functions ---
double sinh(double x); // Hyperbolic sine of x
double cosh(double x); // Hyperbolic cosine of x
double tanh(double x); // Hyperbolic tangent of x

// --- Exponential and Logarithmic Functions ---
double exp(double x);    // Returns e raised to the power of x
double log(double x);    // Natural logarithm (base e)
double log10(double x);  // Common logarithm (base 10)

// --- Power Functions ---
double pow(double base, double exponent); // base raised to the power of exponent
double sqrt(double x);                    // Square root of x

// --- Rounding and Remainder Functions ---
double ceil(double x);  // Smallest integer value not less than x
double floor(double x); // Largest integer value not greater than x
double fabs(double x);  // Absolute value of x
double fmod(double x, double y); // Floating-point remainder of x/y

int sum(int a, int b);
int div(int a, int b);
int mod(int a, int b);
int abs(int a);
int min(int a, int b);
int max(int a, int b);

#endif // End of header guard MATH_H
