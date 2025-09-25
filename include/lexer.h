#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "types.h"

// Maximum lengths for lexer buffers
#define MAX_IDENTIFIER_LENGTH 256
#define MAX_STRING_LENGTH 1024

// Lexer functions
Lexer *lexer_create(const char *input, const char *filename);
void lexer_destroy(Lexer *lexer);
Token lexer_next_token(Lexer *lexer);
Token lexer_peek_token(Lexer *lexer);
const char *token_type_to_string(TokenType type);
bool is_keyword(const char *str, TokenType *type);
void print_token(const Token *token);

// Helper functions for token management
void token_destroy(Token *token);
Token token_copy(const Token *token);

// Objective-C specific helper functions
bool is_objc_at_keyword(const char *str, TokenType *type);
bool is_objc_keyword(const char *str, TokenType *type);
bool is_objc_type(const char *str, TokenType *type);

#endif // LEXER_H