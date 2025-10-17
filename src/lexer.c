#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "kcc.h"
#include "lexer.h"

// Keywords table
static const struct {
    const char *keyword;
    TokenType type;
} keywords[] = {
    {"int", TOKEN_INT},
    {"char", TOKEN_CHAR_KW},
    {"void", TOKEN_VOID},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"return", TOKEN_RETURN},
    {"break", TOKEN_BREAK},
    {"continue", TOKEN_CONTINUE},
    {"id", TOKEN_ID},
    {"YES", TOKEN_YES},
    {"NO", TOKEN_NO},
    {"nil", TOKEN_NIL},
    {"self", TOKEN_SELF},
    {"super", TOKEN_SUPER},
    {"typedef", TOKEN_TYPEDEF},
    {"struct", TOKEN_STRUCT},
    {"union", TOKEN_UNION},
    {"enum", TOKEN_ENUM},
    {NULL, TOKEN_UNKNOWN}
};

Lexer *lexer_create(const char *input, const char *filename) {
    if (!input) return NULL;
    
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    memset(lexer, 0, sizeof(Lexer));
    lexer->input_length = strlen(input);
    lexer->input = strdup(input);
    lexer->source = lexer->input;
    lexer->pos = 0;
    lexer->position = 0;
    lexer->current = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->has_error = false;
    lexer->objc_mode = false;
    
    return lexer;
}

void lexer_destroy(Lexer *lexer) {
    if (lexer) {
        free(lexer->input);
        free(lexer->error_message);
        free(lexer);
    }
}

static char current_char(Lexer *lexer) {
    if (lexer->pos >= lexer->input_length) return '\0';
    return lexer->input[lexer->pos];
}

static void advance(Lexer *lexer) {
    if (lexer->pos < lexer->input_length) {
        if (lexer->input[lexer->pos] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->pos++;
        lexer->position = lexer->pos;
        lexer->current = lexer->pos;
    }
}

static void skip_whitespace(Lexer *lexer) {
    while (isspace(current_char(lexer))) {
        advance(lexer);
    }
}

bool is_keyword(const char *str, TokenType *type) {
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(str, keywords[i].keyword) == 0) {
            *type = keywords[i].type;
            return true;
        }
    }
    return false;
}

static Token read_identifier(Lexer *lexer) {
    Token token;
    memset(&token, 0, sizeof(Token));
    token.line = lexer->line;
    token.column = lexer->column;
    
    char buffer[256];
    int i = 0;
    
    while ((isalnum(current_char(lexer)) || current_char(lexer) == '_') && i < 255) {
        buffer[i++] = current_char(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    token.value = strdup(buffer);
    
    if (!is_keyword(buffer, &token.type)) {
        token.type = TOKEN_IDENTIFIER;
    }
    
    return token;
}

static Token read_number(Lexer *lexer) {
    Token token;
    memset(&token, 0, sizeof(Token));
    token.type = TOKEN_NUMBER;
    token.line = lexer->line;
    token.column = lexer->column;
    
    char buffer[64];
    int i = 0;
    
    while (isdigit(current_char(lexer)) && i < 63) {
        buffer[i++] = current_char(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    token.value = strdup(buffer);
    token.literal.int_value = atoi(buffer);
    
    return token;
}

static Token read_string(Lexer *lexer) {
    Token token;
    memset(&token, 0, sizeof(Token));
    token.type = TOKEN_STRING;
    token.line = lexer->line;
    token.column = lexer->column;
    
    advance(lexer); // skip opening quote
    
    char buffer[1024];
    int i = 0;
    
    while (current_char(lexer) != '"' && current_char(lexer) != '\0' && i < 1023) {
        buffer[i++] = current_char(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    if (current_char(lexer) == '"') {
        advance(lexer); // skip closing quote
    }
    
    token.value = strdup(buffer);
    return token;
}

Token lexer_next_token(Lexer *lexer) {
    Token token;
    memset(&token, 0, sizeof(Token));




    while (current_char(lexer) != '\0') {
        skip_whitespace(lexer);
        
        token.line = lexer->line;
        token.column = lexer->column;
        
        char c = current_char(lexer);

        // Handle multi-line comments
        if (c == '/' && current_char(lexer) == '*') {
            advance(lexer); // skip '/'
            advance(lexer); // skip '*'
            while (current_char(lexer) != '\0') {
                if (current_char(lexer) == '*') {
                    advance(lexer);
                    if (current_char(lexer) == '/') {
                        advance(lexer);
                        break;
                    }
                } else {
                    advance(lexer);
                }
            }
            continue;
        }

        
        if (c == '@') {
            advance(lexer);
            if (isalpha(current_char(lexer))) {
                Token id_token = read_identifier(lexer);
                // Handle @interface, @implementation, etc.
                if (strcmp(id_token.value, "interface") == 0) {
                    token.type = TOKEN_AT_INTERFACE;
                } else if (strcmp(id_token.value, "implementation") == 0) {
                    token.type = TOKEN_AT_IMPLEMENTATION;
                } else if (strcmp(id_token.value, "protocol") == 0) {
                    token.type = TOKEN_AT_PROTOCOL;
                } else if (strcmp(id_token.value, "property") == 0) {
                    token.type = TOKEN_AT_PROPERTY;
                } else if (strcmp(id_token.value, "end") == 0) {
                    token.type = TOKEN_AT_END;
                } else {
                    token.type = TOKEN_AT_IDENTIFIER;
                }
                token.value = malloc(strlen(id_token.value) + 2);
                sprintf(token.value, "@%s", id_token.value);
                free(id_token.value);
                return token;
            } else if (c == '"') {
                token = read_string(lexer);
                token.type = TOKEN_NSSTRING_LITERAL;
                return token;
            } else {
                token.type = TOKEN_AT;
                token.value = strdup("@");
                return token;
            }
        }
        
        if (isalpha(c) || c == '_') {
            return read_identifier(lexer);
        }
        
        if (isdigit(c)) {
            return read_number(lexer);
        }
        
        if (c == '"') {
            return read_string(lexer);
        }
        
        advance(lexer);
        token.value = malloc(2);
        token.value[0] = c;
        token.value[1] = '\0';
        
        switch (c) {
            case '+': token.type = TOKEN_PLUS; break;
            case '-': token.type = TOKEN_MINUS; break;
            case '*': token.type = TOKEN_MULTIPLY; break;
            case '/': token.type = TOKEN_DIVIDE; break;
            case '=': token.type = TOKEN_ASSIGN; break;
            case ';': token.type = TOKEN_SEMICOLON; break;
            case ',': token.type = TOKEN_COMMA; break;
            case '(': token.type = TOKEN_LPAREN; break;
            case ')': token.type = TOKEN_RPAREN; break;
            case '{': token.type = TOKEN_LBRACE; break;
            case '}': token.type = TOKEN_RBRACE; break;
            case '[': token.type = TOKEN_LBRACKET; break;
            case ']': token.type = TOKEN_RBRACKET; break;
            case ':': token.type = TOKEN_COLON; break;
            case '.': token.type = TOKEN_DOT; break;
            // Add this case to your switch statement in lexer_next_token function
            // Add it right before the default case:

            case '#': token.type = TOKEN_HASH; break;
            case '?': token.type = TOKEN_QUESTION; break;
            case '~': token.type = TOKEN_BITWISE_NOT; break;
            case '^': token.type = TOKEN_BITWISE_XOR; break;
            case '<': token.type = TOKEN_LESS; break;
            case '>': token.type = TOKEN_GREATER; break;
            case '!': token.type = TOKEN_NOT; break;
            case '&': token.type = TOKEN_AMPERSAND; break;
            case '|': token.type = TOKEN_PIPE; break;
            case '%': token.type = TOKEN_MODULO; break;
            default: token.type = TOKEN_UNKNOWN; break;
        }

        return token;
    }

    token.type = TOKEN_EOF;
    token.value = strdup("EOF");
    token.line = lexer->line;
    token.column = lexer->column;

    return token;
}

const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_INT: return "INT";
        case TOKEN_CHAR_KW: return "CHAR_KW";
        case TOKEN_VOID: return "VOID";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_CONTINUE: return "CONTINUE";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_COLON: return "COLON";
        case TOKEN_DOT: return "DOT";
        case TOKEN_AT: return "AT";
        case TOKEN_AT_INTERFACE: return "AT_INTERFACE";
        case TOKEN_AT_IMPLEMENTATION: return "AT_IMPLEMENTATION";
        case TOKEN_AT_PROTOCOL: return "AT_PROTOCOL";
        case TOKEN_AT_PROPERTY: return "AT_PROPERTY";
        case TOKEN_AT_END: return "AT_END";
        case TOKEN_AT_IDENTIFIER: return "AT_IDENTIFIER";
        case TOKEN_NSSTRING_LITERAL: return "NSSTRING_LITERAL";
        case TOKEN_ID: return "ID";
        case TOKEN_YES: return "YES";
        case TOKEN_NO: return "NO";
        case TOKEN_NIL: return "NIL";
        case TOKEN_SELF: return "SELF";
        case TOKEN_SUPER: return "SUPER";
        case TOKEN_HASH: return "HASH";
        case TOKEN_QUESTION: return "QUESTION";
        case TOKEN_BITWISE_NOT: return "BITWISE_NOT";
        case TOKEN_BITWISE_XOR: return "BITWISE_XOR";
        case TOKEN_TYPEDEF: return "TYPEDEF";
        case TOKEN_STRUCT: return "STRUCT";
        case TOKEN_UNION: return "UNION";
        case TOKEN_ENUM: return "ENUM";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_BRIDGE: return "BRIDGE";
        case TOKEN_BRIDGE_RETAINED: return "BRIDGE_RETAINED";
        case TOKEN_BRIDGE_TRANSFER: return "BRIDGE_TRANSFER";
        case TOKEN_LONG: return "LONG";
        case TOKEN_SHORT: return "SHORT";
        case TOKEN_UNSIGNED: return "UNSIGNED";
        case TOKEN_SIGNED: return "SIGNED";
        case TOKEN_SIZEOF: return "SIZEOF";
        case TOKEN_SWITCH: return "switch";
        case TOKEN_CASE: return "case";
        case TOKEN_DEFAULT: return "default";
        case TOKEN_CONST: return "const";
        case TOKEN_VOLATILE: return "volatile";
        case TOKEN_RESTRICT: return "restrict";
        default: return "UNKNOWN";
    }
}

