#include <stdio.h>
#include "kcc.h"
#include "parser.h"

Parser *parser_create(Lexer *lexer) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) {
        error_fatal("Memory allocation failed for parser");
        return NULL;
    }

    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    parser->peek_token = lexer_next_token(lexer);
    parser->objc_mode = false; // Default to C mode

    return parser;
}

void parser_destroy(Parser *parser) {
    if (parser) {
        free(parser);
    }
}

void parser_advance(Parser *parser) {
    parser->current_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
}

bool parser_match(Parser *parser, TokenType type) {
    return parser->current_token.type == type;
}

bool parser_expect(Parser *parser, TokenType type) {
    if (parser_match(parser, type)) {
        parser_advance(parser);
        return true;
    } else {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected %s, got %s",
                    token_type_to_string(type),
                    token_type_to_string(parser->current_token.type));
        return false;
    }
}

bool parser_is_type_specifier(TokenType type) {
    return type == TOKEN_INT || type == TOKEN_CHAR_KW || type == TOKEN_VOID ||
           type == TOKEN_FLOAT || type == TOKEN_DOUBLE ||
           // Objective-C types
           type == TOKEN_ID || type == TOKEN_CLASS_KW || type == TOKEN_SEL ||
           type == TOKEN_IMP || type == TOKEN_BOOL_KW || type == TOKEN_INSTANCETYPE ||
           type == TOKEN_NSSTRING || type == TOKEN_NSARRAY ||
           type == TOKEN_NSDICTIONARY || type == TOKEN_NSOBJECT;
}

bool parser_is_objc_directive(TokenType type) {
    return type == TOKEN_AT_INTERFACE || type == TOKEN_AT_IMPLEMENTATION ||
           type == TOKEN_AT_PROTOCOL || type == TOKEN_AT_END ||
           type == TOKEN_AT_PROPERTY || type == TOKEN_AT_SYNTHESIZE ||
           type == TOKEN_AT_DYNAMIC || type == TOKEN_AT_CLASS;
}

DataType parser_parse_type_specifier(Parser *parser) {
    DataType type = token_to_data_type(parser->current_token.type);
    if (type != TYPE_UNKNOWN) {
        parser_advance(parser);
        return type;
    }

    error_syntax(parser->current_token.line, parser->current_token.column,
                "Expected type specifier");
    return TYPE_UNKNOWN;
}

// Helper function to convert TokenType to DataType
DataType token_type_to_data_type(TokenType type) {
    switch (type) {
    case TOKEN_INT:         return TYPE_INT;
    case TOKEN_CHAR_KW:     return TYPE_CHAR;
    case TOKEN_VOID:        return TYPE_VOID;
    case TOKEN_FLOAT:       return TYPE_FLOAT;
    case TOKEN_DOUBLE:      return TYPE_DOUBLE;
    case TOKEN_BOOL_KW:     return TYPE_BOOL;
    // Objective-C types
    case TOKEN_ID:          return TYPE_ID;
    case TOKEN_CLASS_KW:    return TYPE_CLASS;
    case TOKEN_SEL:         return TYPE_SEL;
    case TOKEN_INSTANCETYPE: return TYPE_ID; // instancetype is essentially id
    default:                return TYPE_UNKNOWN;
    }
}

ASTNode *parser_parse_program(Parser *parser) {
    ASTNode *program = ast_create_program();
    int last_line = -1;
    int last_column = -1;
    int stuck_count = 0;

    while (!parser_match(parser, TOKEN_EOF)) {
        // Prevent infinite loops on same position
        if (parser->current_token.line == last_line &&
            parser->current_token.column == last_column) {
            stuck_count++;
            if (stuck_count > 10) {
                fprintf(stderr, "Error: Parser stuck at line %d, column %d with token type %d. Skipping to next line.\n",
                       parser->current_token.line, parser->current_token.column, parser->current_token.type);

                // Force advance until we get to a different line or EOF
                int current_line = parser->current_token.line;
                while (parser->current_token.line == current_line &&
                       !parser_match(parser, TOKEN_EOF)) {
                    parser_advance(parser);
                }
                stuck_count = 0;
                last_line = -1;
                last_column = -1;
                continue;
            }
        } else {
            last_line = parser->current_token.line;
            last_column = parser->current_token.column;
            stuck_count = 0;
        }

        // Skip any UNKNOWN tokens with better error reporting
        if (parser->current_token.type == TOKEN_UNKNOWN) {
            fprintf(stderr, "Warning: Skipping unknown token at line %d, column %d: '%.20s'\n",
                   parser->current_token.line, parser->current_token.column,
                   parser->current_token.value ? parser->current_token.value : "");
            parser_advance(parser);
            continue;
        }

        ASTNode *declaration = NULL;

        // Check for Objective-C directives first
        if (parser_is_objc_directive(parser->current_token.type)) {
            parser->objc_mode = true;
            declaration = parser_parse_objc_declaration(parser);
        } else {
            declaration = parser_parse_declaration_extended(parser);        }

        if (declaration) {
            ast_add_declaration(program, declaration);
        } else {
            // Skip to next potential declaration with safety check
            int skip_line = parser->current_token.line;
            int skip_column = parser->current_token.column;
            int skip_attempts = 0;

            while (!parser_match(parser, TOKEN_EOF) &&
                   !parser_is_type_specifier(parser->current_token.type) &&
                   !parser_is_objc_directive(parser->current_token.type)) {
                parser_advance(parser);
                skip_attempts++;

                // Don't get stuck skipping on the same position
                if (parser->current_token.line != skip_line ||
                    parser->current_token.column != skip_column) {
                    break;
                }

                // Safety valve - don't skip forever
                if (skip_attempts > 50) {
                    fprintf(stderr, "Warning: Excessive skipping at line %d, column %d. Moving to next line.\n",
                           skip_line, skip_column);
                    // Force move to next line
                    while (parser->current_token.line == skip_line &&
                           !parser_match(parser, TOKEN_EOF)) {
                        parser_advance(parser);
                    }
                    break;
                }
            }
        }
    }

    return program;
}

ASTNode *parser_parse_declaration(Parser *parser) {
    // Parse type specifier (int, char, etc.)
    if (!parser_is_type_specifier(parser->current_token.type)) {
        return NULL;
    }

    TokenType type_token = parser->current_token.type;
    DataType data_type = token_type_to_data_type(type_token);
    parser_advance(parser);

    // Parse identifier
    if (parser->current_token.type != TOKEN_IDENTIFIER) {
        return NULL;
    }

    char *name = strdup(parser->current_token.value);
    parser_advance(parser);

    // Check what comes after the identifier
    if (parser->current_token.type == TOKEN_LPAREN) {
        // This is a function (declaration or definition)
        ASTNode *result = parser_parse_function(parser, data_type, name);
        free(name);
        return result;
    } else if (parser->current_token.type == TOKEN_SEMICOLON) {
        // This is a variable declaration
        parser_advance(parser); // consume semicolon
        ASTNode *result = parser_create_variable_declaration(data_type, name);
        free(name);
        return result;
    } else {
        // Handle error gracefully
        free(name);
        return NULL;
    }
}

// Objective-C parsing functions
ASTNode *parser_parse_objc_declaration(Parser *parser) {
    switch (parser->current_token.type) {
        case TOKEN_AT_INTERFACE:
            return parser_parse_objc_interface(parser);
        case TOKEN_AT_IMPLEMENTATION:
            return parser_parse_objc_implementation(parser);
        case TOKEN_AT_PROTOCOL:
            return parser_parse_objc_protocol(parser);
        case TOKEN_AT_PROPERTY:
            return parser_parse_objc_property(parser);
        case TOKEN_AT_SYNTHESIZE:
            return parser_parse_objc_synthesize(parser);
        case TOKEN_AT_DYNAMIC:
            return parser_parse_objc_dynamic(parser);
        case TOKEN_AT_CLASS:
            return parser_parse_objc_class_forward(parser);
        default:
            error_syntax(parser->current_token.line, parser->current_token.column,
                        "Unexpected Objective-C directive");
            return NULL;
    }
}

ASTNode *parser_parse_objc_interface(Parser *parser) {
    parser_advance(parser); // consume '@interface'

    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected class name after @interface");
        return NULL;
    }

    char *class_name = strdup(parser->current_token.value);
    parser_advance(parser);

    // Parse optional superclass
    char *superclass_name = NULL;
    if (parser_match(parser, TOKEN_COLON)) {
        parser_advance(parser);
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            superclass_name = strdup(parser->current_token.value);
            parser_advance(parser);
        }
    }

    // Parse optional protocol list <Protocol1, Protocol2>
    ASTNode **protocols = NULL;
    int protocol_count = 0;
    parser_expect(parser, TOKEN_GREATER);
    (void)protocols; // Suppress unused warning
    if (parser_match(parser, TOKEN_LESS)) {
        parser_advance(parser);
        // Parse protocol list (simplified)
        while (!parser_match(parser, TOKEN_GREATER) && !parser_match(parser, TOKEN_EOF)) {
            if (parser_match(parser, TOKEN_IDENTIFIER)) {
                protocol_count++;
                parser_advance(parser);
            }
            if (parser_match(parser, TOKEN_COMMA)) {
                parser_advance(parser);
            }
        }
        parser_expect(parser, TOKEN_GREATER);
    }

    // Create interface node
    ASTNode *interface = ast_create_objc_interface(class_name, superclass_name);

    // Parse interface body until @end
    while (!parser_match(parser, TOKEN_AT_END) && !parser_match(parser, TOKEN_EOF)) {
        ASTNode *member = NULL;

        if (parser_match(parser, TOKEN_AT_PROPERTY)) {
            member = parser_parse_objc_property(parser);
        } else if (parser_match(parser, TOKEN_MINUS) || parser_match(parser, TOKEN_PLUS)) {
            member = parser_parse_objc_method_declaration(parser);
        } else {
            // Skip unknown tokens in interface
            parser_advance(parser);
            continue;
        }

        if (member) {
            ast_add_objc_interface_member(interface, member);
        }
    }

    parser_expect(parser, TOKEN_AT_END);

    free(class_name);
    if (superclass_name) free(superclass_name);
    return interface;
}

ASTNode *parser_parse_objc_implementation(Parser *parser) {
    parser_advance(parser); // consume '@implementation'

    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected class name after @implementation");
        return NULL;
    }

    char *class_name = strdup(parser->current_token.value);
    parser_advance(parser);

    // Parse optional category name (ClassName)
    char *category_name = NULL;
    if (parser_match(parser, TOKEN_LPAREN)) {
        parser_advance(parser);
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            category_name = strdup(parser->current_token.value);
            parser_advance(parser);
        }
        parser_expect(parser, TOKEN_RPAREN);
    }

    // Create implementation node
    ASTNode *implementation = ast_create_objc_implementation(class_name, category_name);

    // Parse implementation body until @end
    while (!parser_match(parser, TOKEN_AT_END) && !parser_match(parser, TOKEN_EOF)) {
        ASTNode *member = NULL;

        if (parser_match(parser, TOKEN_AT_SYNTHESIZE)) {
            member = parser_parse_objc_synthesize(parser);
        } else if (parser_match(parser, TOKEN_AT_DYNAMIC)) {
            member = parser_parse_objc_dynamic(parser);
        } else if (parser_match(parser, TOKEN_MINUS) || parser_match(parser, TOKEN_PLUS)) {
            member = parser_parse_objc_method_implementation(parser);
        } else {
            // Skip unknown tokens in implementation
            parser_advance(parser);
            continue;
        }

        if (member) {
            ast_add_objc_implementation_member(implementation, member);
        }
    }

    parser_expect(parser, TOKEN_AT_END);

    free(class_name);
    if (category_name) free(category_name);
    return implementation;
}

ASTNode *parser_parse_objc_method_declaration(Parser *parser) {
    ObjCMethodType method_type = OBJC_INSTANCE_METHOD;

    if (parser_match(parser, TOKEN_PLUS)) {
        method_type = OBJC_CLASS_METHOD;
        parser_advance(parser);
    } else if (parser_match(parser, TOKEN_MINUS)) {
        method_type = OBJC_INSTANCE_METHOD;
        parser_advance(parser);
    }

    // Parse return type (returnType)
    DataType return_type = TYPE_VOID;
    if (parser_match(parser, TOKEN_LPAREN)) {
        parser_advance(parser);
        if (parser_is_type_specifier(parser->current_token.type)) {
            return_type = parser_parse_type_specifier(parser);
        }
        parser_expect(parser, TOKEN_RPAREN);
    }

    // Parse method selector and parameters
    char *selector = parser_parse_objc_method_selector(parser);

    parser_expect(parser, TOKEN_SEMICOLON);

    ASTNode *method = ast_create_objc_method(method_type, return_type, selector, NULL);
    free(selector);
    return method;
}

ASTNode *parser_parse_objc_method_implementation(Parser *parser) {
    ObjCMethodType method_type = OBJC_INSTANCE_METHOD;

    if (parser_match(parser, TOKEN_PLUS)) {
        method_type = OBJC_CLASS_METHOD;
        parser_advance(parser);
    } else if (parser_match(parser, TOKEN_MINUS)) {
        method_type = OBJC_INSTANCE_METHOD;
        parser_advance(parser);
    }

    // Parse return type (returnType)
    DataType return_type = TYPE_VOID;
    if (parser_match(parser, TOKEN_LPAREN)) {
        parser_advance(parser);
        if (parser_is_type_specifier(parser->current_token.type)) {
            return_type = parser_parse_type_specifier(parser);
        }
        parser_expect(parser, TOKEN_RPAREN);
    }

    // Parse method selector and parameters
    char *selector = parser_parse_objc_method_selector(parser);

    // Parse method body
    ASTNode *body = parser_parse_compound_statement(parser);

    ASTNode *method = ast_create_objc_method(method_type, return_type, selector, body);
    free(selector);
    return method;
}

char *parser_parse_objc_method_selector(Parser *parser) {
    char *selector = malloc(256);
    selector[0] = '\0';

    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        free(selector);
        return NULL;
    }

    // Simple method: methodName
    strcat(selector, parser->current_token.value);
    parser_advance(parser);

    // Check for parameters: methodName:(type)param withParam:(type)param2
    while (parser_match(parser, TOKEN_COLON)) {
        strcat(selector, ":");
        parser_advance(parser);

        // Skip parameter type and name
        if (parser_match(parser, TOKEN_LPAREN)) {
            parser_advance(parser);
            if (parser_is_type_specifier(parser->current_token.type)) {
                parser_advance(parser);
            }
            parser_expect(parser, TOKEN_RPAREN);
        }

        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            parser_advance(parser); // parameter name
        }

        // Additional selector parts
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            strcat(selector, " ");
            strcat(selector, parser->current_token.value);
            parser_advance(parser);
        }
    }

    return selector;
}

ASTNode *parser_parse_objc_property(Parser *parser) {
    parser_advance(parser); // consume '@property'

    // Parse property attributes (atomic, nonatomic, retain, etc.)
    ObjCPropertyAttributes attributes = 0;
    if (parser_match(parser, TOKEN_LPAREN)) {
        parser_advance(parser);

        do {
            if (parser_match(parser, TOKEN_ATOMIC)) {
                attributes |= OBJC_PROPERTY_ATOMIC;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_NONATOMIC)) {
                attributes |= OBJC_PROPERTY_NONATOMIC;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_RETAIN)) {
                attributes |= OBJC_PROPERTY_RETAIN;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_ASSIGN_ATTR)) {
                attributes |= OBJC_PROPERTY_ASSIGN;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_COPY)) {
                attributes |= OBJC_PROPERTY_COPY;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_WEAK)) {
                attributes |= OBJC_PROPERTY_WEAK;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_STRONG)) {
                attributes |= OBJC_PROPERTY_STRONG;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_READONLY)) {
                attributes |= OBJC_PROPERTY_READONLY;
                parser_advance(parser);
            } else if (parser_match(parser, TOKEN_READWRITE)) {
                attributes |= OBJC_PROPERTY_READWRITE;
                parser_advance(parser);
            } else {
                break;
            }

            if (parser_match(parser, TOKEN_COMMA)) {
                parser_advance(parser);
            }
        } while (true);

        parser_expect(parser, TOKEN_RPAREN);
    }

    // Parse property type
    DataType property_type = TYPE_UNKNOWN;
    if (parser_is_type_specifier(parser->current_token.type)) {
        property_type = parser_parse_type_specifier(parser);
    }

    // Handle pointer types (id*, NSString*, etc.)
    while (parser_match(parser, TOKEN_MULTIPLY)) {
        property_type = TYPE_POINTER;
        parser_advance(parser);
    }

    // Parse property name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected property name");
        return NULL;
    }

    char *property_name = strdup(parser->current_token.value);
    parser_advance(parser);

    parser_expect(parser, TOKEN_SEMICOLON);

    ASTNode *property = ast_create_objc_property(property_type, property_name, attributes);
    free(property_name);
    return property;
}

ASTNode *parser_parse_objc_synthesize(Parser *parser) {
    parser_advance(parser); // consume '@synthesize'

    // Parse property list: @synthesize prop1, prop2 = ivar2;
    while (parser_match(parser, TOKEN_IDENTIFIER)) {
        char *property_name = strdup(parser->current_token.value);
        parser_advance(parser);

        // Optional backing ivar: property = ivar
        char *ivar_name = NULL;
        if (parser_match(parser, TOKEN_ASSIGN)) {
            parser_advance(parser);
            if (parser_match(parser, TOKEN_IDENTIFIER)) {
                ivar_name = strdup(parser->current_token.value);
                parser_advance(parser);
            }
        }

        // Keep this line:
        ast_destroy(ast_create_objc_synthesize(property_name, ivar_name));
        
        free(property_name);
        if (ivar_name) free(ivar_name);

        if (parser_match(parser, TOKEN_COMMA)) {
            parser_advance(parser);
        } else {
            break;
        }
    }

    parser_expect(parser, TOKEN_SEMICOLON);
    return NULL; // Return appropriate synthesize node
}

ASTNode *parser_parse_objc_dynamic(Parser *parser) {
    parser_advance(parser); // consume '@dynamic'

    // Parse property list: @dynamic prop1, prop2;
    while (parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_advance(parser);
        if (parser_match(parser, TOKEN_COMMA)) {
            parser_advance(parser);
        } else {
            break;
        }
    }

    parser_expect(parser, TOKEN_SEMICOLON);
    return NULL; // Return appropriate dynamic node
}

ASTNode *parser_parse_objc_protocol(Parser *parser) {
    parser_advance(parser); // consume '@protocol'

    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected protocol name");
        return NULL;
    }

    char *protocol_name = strdup(parser->current_token.value);
    parser_advance(parser);

    ASTNode *protocol = ast_create_objc_protocol(protocol_name);

    // Parse protocol body until @end
    while (!parser_match(parser, TOKEN_AT_END) && !parser_match(parser, TOKEN_EOF)) {
        if (parser_match(parser, TOKEN_OPTIONAL)) {
            parser_advance(parser);
            // Methods following are optional
        } else if (parser_match(parser, TOKEN_REQUIRED)) {
            parser_advance(parser);
            // Methods following are required
        } else if (parser_match(parser, TOKEN_MINUS) || parser_match(parser, TOKEN_PLUS)) {
            ASTNode *method = parser_parse_objc_method_declaration(parser);
            if (method) {
                ast_add_objc_protocol_method(protocol, method);
            }
        } else if (parser_match(parser, TOKEN_AT_PROPERTY)) {
            ASTNode *property = parser_parse_objc_property(parser);
            if (property) {
                ast_add_objc_protocol_property(protocol, property);
            }
        } else {
            parser_advance(parser);
        }
    }

    parser_expect(parser, TOKEN_AT_END);

    free(protocol_name);
    return protocol;
}

ASTNode *parser_parse_objc_class_forward(Parser *parser) {
    parser_advance(parser); // consume '@class'

    // Parse class list: @class Class1, Class2;
    while (parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_advance(parser);
        if (parser_match(parser, TOKEN_COMMA)) {
            parser_advance(parser);
        } else {
            break;
        }
    }

    parser_expect(parser, TOKEN_SEMICOLON);
    return NULL; // Return appropriate forward declaration node
}

// Enhanced primary expression parsing for Objective-C
ASTNode *parser_parse_primary_expression(Parser *parser) {
    ASTNode *primary = NULL;

    switch (parser->current_token.type) {
        case TOKEN_NUMBER: {
            int value = atoi(parser->current_token.value);
            primary = ast_create_number(value);
            parser_advance(parser);
            break;
        }
        case TOKEN_STRING: {
            primary = ast_create_string(parser->current_token.value);
            parser_advance(parser);
            break;
        }
        case TOKEN_NSSTRING_LITERAL: {
            primary = ast_create_objc_string(parser->current_token.value);
            parser_advance(parser);
            break;
        }
        case TOKEN_YES: {
            primary = ast_create_objc_boolean(true);
            parser_advance(parser);
            break;
        }
        case TOKEN_NO: {
            primary = ast_create_objc_boolean(false);
            parser_advance(parser);
            break;
        }
        case TOKEN_NIL: {
            primary = ast_create_objc_nil();
            parser_advance(parser);
            break;
        }
        case TOKEN_SELF: {
            primary = ast_create_objc_self();
            parser_advance(parser);
            break;
        }
        case TOKEN_SUPER: {
            primary = ast_create_objc_super();
            parser_advance(parser);
            break;
        }
        case TOKEN_IDENTIFIER: {
            primary = ast_create_identifier(parser->current_token.value);
            parser_advance(parser);
            break;
        }
        case TOKEN_LPAREN: {
            parser_advance(parser);
            primary = parser_parse_expression(parser);
            parser_expect(parser, TOKEN_RPAREN);
            break;
        }
        case TOKEN_LBRACKET: {
            // Objective-C message send [receiver message]
            primary = parser_parse_objc_message_send(parser);
            break;
        }
        case TOKEN_AT_SELECTOR: {
            primary = parser_parse_objc_selector_expression(parser);
            break;
        }
        case TOKEN_AT_ENCODE: {
            primary = parser_parse_objc_encode_expression(parser);
            break;
        }
        default:
            error_syntax(parser->current_token.line, parser->current_token.column,
                        "Expected primary expression, got %s",
                        token_type_to_string(parser->current_token.type));
            // CRITICAL FIX: Advance the parser to prevent infinite loop
            parser_advance(parser);
            return NULL;
    }

    // Handle function calls and property access
    if (primary && primary->type == AST_IDENTIFIER) {
        if (parser_match(parser, TOKEN_LPAREN)) {
            primary = parser_parse_call_expression(parser, primary);
        } else if (parser_match(parser, TOKEN_DOT)) {
            primary = parser_parse_property_access(parser, primary);
        }
    }

    return primary;
}

ASTNode *parser_parse_objc_message_send(Parser *parser) {
    parser_advance(parser); // consume '['

    // Parse receiver
    ASTNode *receiver = parser_parse_expression(parser);

    // Parse selector and arguments
    char *selector = malloc(256);
    selector[0] = '\0';

    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        strcat(selector, parser->current_token.value);
        parser_advance(parser);

        // Handle method with arguments: [obj method:arg1 withArg:arg2]
        while (parser_match(parser, TOKEN_COLON)) {
            strcat(selector, ":");
            parser_advance(parser);

            // Parse argument
            ASTNode *arg = parser_parse_expression(parser);
            (void)arg; // TODO: Store argument properly

            // Additional selector parts
            if (parser_match(parser, TOKEN_IDENTIFIER)) {
                strcat(selector, " ");
                strcat(selector, parser->current_token.value);
                parser_advance(parser);
            }
        }
    }

    parser_expect(parser, TOKEN_RBRACKET);

    ASTNode *message = ast_create_objc_message_send(receiver, selector);
    free(selector);
    return message;
}

ASTNode *parser_parse_objc_selector_expression(Parser *parser) {
    parser_advance(parser); // consume '@selector'
    parser_expect(parser, TOKEN_LPAREN);

    char *selector_name = strdup(parser->current_token.value);
    parser_advance(parser);

    parser_expect(parser, TOKEN_RPAREN);

    ASTNode *selector = ast_create_objc_selector(selector_name);
    free(selector_name);
    return selector;
}

ASTNode *parser_parse_objc_encode_expression(Parser *parser) {
    parser_advance(parser); // consume '@encode'
    parser_expect(parser, TOKEN_LPAREN);

    // Parse type to encode
    DataType type = parser_parse_type_specifier(parser);

    parser_expect(parser, TOKEN_RPAREN);

    return ast_create_objc_encode(type);
}

ASTNode *parser_parse_property_access(Parser *parser, ASTNode *object) {
    parser_advance(parser); // consume '.'

    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected property name after '.'");
        return object;
    }

    char *property_name = strdup(parser->current_token.value);
    parser_advance(parser);

    ASTNode *access = ast_create_property_access(object, property_name);
    free(property_name);
    return access;
}

// Rest of the original parser functions remain the same...

ASTNode *parser_parse_function(Parser *parser, DataType return_type, char *name) {
    // Expect '('
    if (parser->current_token.type != TOKEN_LPAREN) {
        return NULL;
    }
    parser_advance(parser);

    // Parse parameters (for now, just handle empty parameter list)
    if (parser->current_token.type != TOKEN_RPAREN) {
        // Skip parameters for now - advance until we find ')'
        while (parser->current_token.type != TOKEN_RPAREN &&
               parser->current_token.type != TOKEN_EOF) {
            parser_advance(parser);
        }
    }

    if (parser->current_token.type == TOKEN_RPAREN) {
        parser_advance(parser); // consume ')'
    }

    // Check if this is a declaration or definition
    if (parser->current_token.type == TOKEN_SEMICOLON) {
        // Function declaration: int func();
        parser_advance(parser);
        return parser_create_function_declaration(return_type, name);
    } else if (parser->current_token.type == TOKEN_LBRACE) {
        // Function definition: int func() { ... }
        return parser_parse_function_definition(parser, return_type, name);
    } else {
        return NULL;
    }
}

ASTNode *parser_parse_function_definition(Parser *parser, DataType return_type, char *name) {
    // Parse the function body
    ASTNode *body = parser_parse_compound_statement(parser);
    if (!body) {
        return NULL;
    }

    // Create a function declaration node with body
    ASTNode *func = parser_create_function_declaration(return_type, name);
    if (func) {
        // If your AST supports function definitions, set the body here
        // This depends on your AST structure
        // func->data.function.body = body; // Adjust according to your AST
    }

    return func;
}

// Helper function to create function declaration
ASTNode *parser_create_function_declaration(DataType return_type, const char *name) {
    // This should match your AST creation functions
    // Adjust according to your actual AST API
    return ast_create_function_decl(return_type, name, NULL, NULL);
}

// Helper function to create variable declaration
ASTNode *parser_create_variable_declaration(DataType type, const char *name) {
    // This should match your AST creation functions
    // Adjust according to your actual AST API
    return ast_create_var_decl(type, name, NULL);
}

ASTNode *parser_parse_variable_declaration(Parser *parser, DataType var_type) {
    ASTNode *initializer = NULL;

    if (parser_match(parser, TOKEN_ASSIGN)) {
        parser_advance(parser);
        initializer = parser_parse_expression(parser);
    }

    parser_expect(parser, TOKEN_SEMICOLON);

    return ast_create_var_decl(var_type, "", initializer);
}

ASTNode *parser_parse_statement(Parser *parser) {
    switch (parser->current_token.type) {
        case TOKEN_LBRACE:
            return parser_parse_compound_statement(parser);
        case TOKEN_RETURN:
            return parser_parse_return_statement(parser);
        case TOKEN_IF:
            return parser_parse_if_statement(parser);
        case TOKEN_WHILE:
            return parser_parse_while_statement(parser);
        case TOKEN_FOR:
            return parser_parse_for_statement(parser);
        case TOKEN_BREAK:
            parser_advance(parser);
            parser_expect(parser, TOKEN_SEMICOLON);
            return ast_create_break_stmt();
        case TOKEN_CONTINUE:
            parser_advance(parser);
            parser_expect(parser, TOKEN_SEMICOLON);
            return ast_create_continue_stmt();
        // Objective-C statements
        case TOKEN_AT_TRY:
            return parser_parse_objc_try_statement(parser);
        case TOKEN_AT_THROW:
            return parser_parse_objc_throw_statement(parser);
        case TOKEN_AT_SYNCHRONIZED:
            return parser_parse_objc_synchronized_statement(parser);
        case TOKEN_AUTORELEASEPOOL:
            return parser_parse_objc_autoreleasepool_statement(parser);
        default:
            return parser_parse_expression_statement(parser);
    }
}

// Objective-C statement parsing functions
ASTNode *parser_parse_objc_try_statement(Parser *parser) {
    parser_advance(parser); // consume '@try'

    ASTNode *try_body = parser_parse_compound_statement(parser);

    // Parse @catch blocks
    ASTNode **catch_blocks = NULL;
    int catch_count = 0;

    while (parser_match(parser, TOKEN_AT_CATCH)) {
        parser_advance(parser); // consume '@catch'

        parser_expect(parser, TOKEN_LPAREN);

        // Parse exception type and variable
        DataType exception_type = TYPE_ID;
        char *exception_var = NULL;

        if (parser_is_type_specifier(parser->current_token.type)) {
            exception_type = parser_parse_type_specifier(parser);
        }

        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            exception_var = strdup(parser->current_token.value);
            parser_advance(parser);
        }

        parser_expect(parser, TOKEN_RPAREN);

        ASTNode *catch_body = parser_parse_compound_statement(parser);

        // Create catch block node
        ASTNode *catch_block = ast_create_objc_catch(exception_type, exception_var, catch_body);
        (void)catch_block; // TODO: Add to catch blocks array

        // Add to catch blocks array (simplified - you'd need proper array management)
        catch_count++;

        if (exception_var) free(exception_var);
    }

    // Parse optional @finally block
    ASTNode *finally_block = NULL;
    if (parser_match(parser, TOKEN_AT_FINALLY)) {
        parser_advance(parser);
        finally_block = parser_parse_compound_statement(parser);
    }

    return ast_create_objc_try(try_body, catch_blocks, catch_count, finally_block);
}

ASTNode *parser_parse_objc_throw_statement(Parser *parser) {
    parser_advance(parser); // consume '@throw'

    ASTNode *exception_expr = NULL;
    if (!parser_match(parser, TOKEN_SEMICOLON)) {
        exception_expr = parser_parse_expression(parser);
    }

    parser_expect(parser, TOKEN_SEMICOLON);

    return ast_create_objc_throw(exception_expr);
}

ASTNode *parser_parse_objc_synchronized_statement(Parser *parser) {
    parser_advance(parser); // consume '@synchronized'

    parser_expect(parser, TOKEN_LPAREN);
    ASTNode *sync_object = parser_parse_expression(parser);
    parser_expect(parser, TOKEN_RPAREN);

    ASTNode *sync_body = parser_parse_compound_statement(parser);

    return ast_create_objc_synchronized(sync_object, sync_body);
}

ASTNode *parser_parse_objc_autoreleasepool_statement(Parser *parser) {
    parser_advance(parser); // consume '@autoreleasepool'

    ASTNode *pool_body = parser_parse_compound_statement(parser);

    return ast_create_objc_autoreleasepool(pool_body);
}

ASTNode *parser_parse_compound_statement(Parser *parser) {
    ASTNode *compound = ast_create_compound_stmt();

    parser_expect(parser, TOKEN_LBRACE);

    while (!parser_match(parser, TOKEN_RBRACE) && !parser_match(parser, TOKEN_EOF)) {
        ASTNode *stmt = NULL;

        if (parser_is_type_specifier(parser->current_token.type)) {
            // Variable declaration
            DataType var_type = parser_parse_type_specifier(parser);

            if (parser_match(parser, TOKEN_IDENTIFIER)) {
                char *var_name = strdup(parser->current_token.value);
                parser_advance(parser);

                ASTNode *var_decl = parser_parse_variable_declaration(parser, var_type);
                if (var_decl) {
                    free(var_decl->data.var_decl.name);
                    var_decl->data.var_decl.name = var_name;
                    stmt = var_decl;
                } else {
                    free(var_name);
                }
            }
        } else {
            stmt = parser_parse_statement(parser);
        }

        if (stmt) {
            ast_add_statement(compound, stmt);
        }
    }

    parser_expect(parser, TOKEN_RBRACE);

    return compound;
}

ASTNode *parser_parse_expression_statement(Parser *parser) {
    ASTNode *expr = parser_parse_expression(parser);
    parser_expect(parser, TOKEN_SEMICOLON);
    return ast_create_expression_stmt(expr);
}

ASTNode *parser_parse_return_statement(Parser *parser) {
    parser_advance(parser); // consume 'return'

    ASTNode *expr = NULL;
    if (!parser_match(parser, TOKEN_SEMICOLON)) {
        expr = parser_parse_expression(parser);
    }

    parser_expect(parser, TOKEN_SEMICOLON);

    return ast_create_return_stmt(expr);
}

ASTNode *parser_parse_if_statement(Parser *parser) {
    parser_advance(parser); // consume 'if'

    parser_expect(parser, TOKEN_LPAREN);
    ASTNode *condition = parser_parse_expression(parser);
    parser_expect(parser, TOKEN_RPAREN);

    ASTNode *then_stmt = parser_parse_statement(parser);

    ASTNode *else_stmt = NULL;
    if (parser_match(parser, TOKEN_ELSE)) {
        parser_advance(parser);
        else_stmt = parser_parse_statement(parser);
    }

    return ast_create_if_stmt(condition, then_stmt, else_stmt);
}


ASTNode *parser_parse_expression(Parser *parser) {
    return parser_parse_assignment_expression(parser);
}

ASTNode *parser_parse_logical_or_expression(Parser *parser) {
    ASTNode *left = parser_parse_logical_and_expression(parser);

    while (parser_match(parser, TOKEN_OR)) {
        TokenType op = parser->current_token.type;
        parser_advance(parser);
        ASTNode *right = parser_parse_logical_and_expression(parser);
        left = ast_create_binary_expr(op, left, right);
    }

    return left;
}

ASTNode *parser_parse_logical_and_expression(Parser *parser) {
    ASTNode *left = parser_parse_equality_expression(parser);

    while (parser_match(parser, TOKEN_AND)) {
        TokenType op = parser->current_token.type;
        parser_advance(parser);
        ASTNode *right = parser_parse_equality_expression(parser);
        left = ast_create_binary_expr(op, left, right);
    }

    return left;
}

ASTNode *parser_parse_equality_expression(Parser *parser) {
    ASTNode *left = parser_parse_relational_expression(parser);

    while (parser_match(parser, TOKEN_EQUAL) || parser_match(parser, TOKEN_NOT_EQUAL)) {
        TokenType op = parser->current_token.type;
        parser_advance(parser);
        ASTNode *right = parser_parse_relational_expression(parser);
        left = ast_create_binary_expr(op, left, right);
    }

    return left;
}

ASTNode *parser_parse_relational_expression(Parser *parser) {
    ASTNode *left = parser_parse_additive_expression(parser);

    while (parser_match(parser, TOKEN_LESS) || parser_match(parser, TOKEN_LESS_EQUAL) ||
           parser_match(parser, TOKEN_GREATER) || parser_match(parser, TOKEN_GREATER_EQUAL)) {
        TokenType op = parser->current_token.type;
        parser_advance(parser);
        ASTNode *right = parser_parse_additive_expression(parser);
        left = ast_create_binary_expr(op, left, right);
    }

    return left;
}

ASTNode *parser_parse_additive_expression(Parser *parser) {
    ASTNode *left = parser_parse_multiplicative_expression(parser);

    while (parser_match(parser, TOKEN_PLUS) || parser_match(parser, TOKEN_MINUS)) {
        TokenType op = parser->current_token.type;
        parser_advance(parser);
        ASTNode *right = parser_parse_multiplicative_expression(parser);
        left = ast_create_binary_expr(op, left, right);
    }

    return left;
}


// Extended type specifier checking
bool parser_is_type_specifier_extended(TokenType type) {
    return parser_is_type_specifier(type) ||
           type == TOKEN_STRUCT ||
           type == TOKEN_UNION ||
           type == TOKEN_ENUM ||
           type == TOKEN_TYPEDEF;
}

// Parse typedef declarations
ASTNode *parser_parse_typedef(Parser *parser) {
    parser_advance(parser); // consume 'typedef'

    // Parse the base type
    ASTNode *base_type = parser_parse_type_declaration(parser);
    if (!base_type) {
        return NULL;
    }

    // Parse the alias name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected typedef name");
        ast_destroy(base_type);
        return NULL;
    }

    char *alias_name = strdup(parser->current_token.value);
    parser_advance(parser);

    parser_expect(parser, TOKEN_SEMICOLON);

    ASTNode *typedef_node = ast_create_typedef(base_type, alias_name);
    free(alias_name);
    return typedef_node;
}

// Parse struct declarations
ASTNode *parser_parse_struct(Parser *parser) {
    parser_advance(parser); // consume 'struct'

    char *struct_name = NULL;
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        struct_name = strdup(parser->current_token.value);
        parser_advance(parser);
    }

    ASTNode *struct_node = ast_create_struct(struct_name);

    // If there's a body, parse it
    if (parser_match(parser, TOKEN_LBRACE)) {
        parser_advance(parser);

        while (!parser_match(parser, TOKEN_RBRACE) && !parser_match(parser, TOKEN_EOF)) {
            ASTNode *member = parser_parse_struct_member(parser);
            if (member) {
                ast_add_struct_member(struct_node, member);
            }
        }

        parser_expect(parser, TOKEN_RBRACE);
    }

    if (struct_name) free(struct_name);
    return struct_node;
}

// Parse union declarations
ASTNode *parser_parse_union(Parser *parser) {
    parser_advance(parser); // consume 'union'

    char *union_name = NULL;
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        union_name = strdup(parser->current_token.value);
        parser_advance(parser);
    }

    ASTNode *union_node = ast_create_union(union_name);

    // If there's a body, parse it
    if (parser_match(parser, TOKEN_LBRACE)) {
        parser_advance(parser);

        while (!parser_match(parser, TOKEN_RBRACE) && !parser_match(parser, TOKEN_EOF)) {
            ASTNode *member = parser_parse_struct_member(parser); // Same as struct member
            if (member) {
                ast_add_union_member(union_node, member);
            }
        }

        parser_expect(parser, TOKEN_RBRACE);
    }

    if (union_name) free(union_name);
    return union_node;
}

// Parse enum declarations
ASTNode *parser_parse_enum(Parser *parser) {
    parser_advance(parser); // consume 'enum'

    char *enum_name = NULL;
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        enum_name = strdup(parser->current_token.value);
        parser_advance(parser);
    }

    ASTNode *enum_node = ast_create_enum(enum_name);

    // If there's a body, parse it
    if (parser_match(parser, TOKEN_LBRACE)) {
        parser_advance(parser);

        int enum_value = 0;
        while (!parser_match(parser, TOKEN_RBRACE) && !parser_match(parser, TOKEN_EOF)) {
            if (!parser_match(parser, TOKEN_IDENTIFIER)) {
                error_syntax(parser->current_token.line, parser->current_token.column,
                            "Expected enum constant name");
                break;
            }

            char *const_name = strdup(parser->current_token.value);
            parser_advance(parser);

            // Check for explicit value assignment
            if (parser_match(parser, TOKEN_ASSIGN)) {
                parser_advance(parser);
                if (parser_match(parser, TOKEN_NUMBER)) {
                    enum_value = atoi(parser->current_token.value);
                    parser_advance(parser);
                } else {
                    error_syntax(parser->current_token.line, parser->current_token.column,
                                "Expected enum value");
                    free(const_name);
                    break;
                }
            }

            ASTNode *enum_const = ast_create_enum_constant(const_name, enum_value);
            ast_add_enum_constant(enum_node, enum_const);

            enum_value++; // Auto-increment for next constant
            free(const_name);

            if (parser_match(parser, TOKEN_COMMA)) {
                parser_advance(parser);
                // Allow trailing comma
                if (parser_match(parser, TOKEN_RBRACE)) {
                    break;
                }
            } else {
                break;
            }
        }

        parser_expect(parser, TOKEN_RBRACE);
    }

    if (enum_name) free(enum_name);
    return enum_node;
}

// Parse struct/union members (supports bitfields)
ASTNode *parser_parse_struct_member(Parser *parser) {
    // Parse type
    DataType member_type = TYPE_UNKNOWN;
    ASTNode *type_node = NULL;

    if (parser_is_type_specifier(parser->current_token.type)) {
        member_type = parser_parse_type_specifier(parser);
    } else if (parser_match(parser, TOKEN_STRUCT)) {
        type_node = parser_parse_struct(parser);
    } else if (parser_match(parser, TOKEN_UNION)) {
        type_node = parser_parse_union(parser);
    } else if (parser_match(parser, TOKEN_ENUM)) {
        type_node = parser_parse_enum(parser);
    } else {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected type specifier in struct member");
        return NULL;
    }

    // Parse member name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        error_syntax(parser->current_token.line, parser->current_token.column,
                    "Expected member name");
        if (type_node) ast_destroy(type_node);
        return NULL;
    }

    char *member_name = strdup(parser->current_token.value);
    parser_advance(parser);

    // Check for bitfield specification
    int bitfield_width = -1;
    if (parser_match(parser, TOKEN_COLON)) {
        parser_advance(parser);
        if (parser_match(parser, TOKEN_NUMBER)) {
            bitfield_width = atoi(parser->current_token.value);
            parser_advance(parser);
        } else {
            error_syntax(parser->current_token.line, parser->current_token.column,
                        "Expected bitfield width");
            free(member_name);
            if (type_node) ast_destroy(type_node);
            return NULL;
        }
    }

    parser_expect(parser, TOKEN_SEMICOLON);

    ASTNode *member = ast_create_struct_member(member_type, member_name, bitfield_width);
    if (type_node) {
        ast_set_member_type_node(member, type_node);
    }

    free(member_name);
    return member;
}

// Parse general type declarations (for typedef, function parameters, etc.)
ASTNode *parser_parse_type_declaration(Parser *parser) {
    if (parser_match(parser, TOKEN_STRUCT)) {
        return parser_parse_struct(parser);
    } else if (parser_match(parser, TOKEN_UNION)) {
        return parser_parse_union(parser);
    } else if (parser_match(parser, TOKEN_ENUM)) {
        return parser_parse_enum(parser);
    } else if (parser_is_type_specifier(parser->current_token.type)) {
        DataType type = parser_parse_type_specifier(parser);
        return ast_create_basic_type(type);
    }

    return NULL;
}

// Updated declaration parser to handle new types
ASTNode *parser_parse_declaration_extended(Parser *parser) {
    // Check for typedef first
    if (parser_match(parser, TOKEN_TYPEDEF)) {
        return parser_parse_typedef(parser);
    }

    // Check for struct/union/enum declarations
    if (parser_match(parser, TOKEN_STRUCT)) {
        ASTNode *struct_node = parser_parse_struct(parser);
        // Handle variable declarations with this struct type
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            char *var_name = strdup(parser->current_token.value);
            parser_advance(parser);
            parser_expect(parser, TOKEN_SEMICOLON);

            ASTNode *var_decl = ast_create_var_decl_with_type_node(struct_node, var_name);
            free(var_name);
            return var_decl;
        } else {
            parser_expect(parser, TOKEN_SEMICOLON);
            return struct_node;
        }
    }

    if (parser_match(parser, TOKEN_UNION)) {
        ASTNode *union_node = parser_parse_union(parser);
        // Handle variable declarations with this union type
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            char *var_name = strdup(parser->current_token.value);
            parser_advance(parser);
            parser_expect(parser, TOKEN_SEMICOLON);

            ASTNode *var_decl = ast_create_var_decl_with_type_node(union_node, var_name);
            free(var_name);
            return var_decl;
        } else {
            parser_expect(parser, TOKEN_SEMICOLON);
            return union_node;
        }
    }

    if (parser_match(parser, TOKEN_ENUM)) {
        ASTNode *enum_node = parser_parse_enum(parser);
        // Handle variable declarations with this enum type
        if (parser_match(parser, TOKEN_IDENTIFIER)) {
            char *var_name = strdup(parser->current_token.value);
            parser_advance(parser);
            parser_expect(parser, TOKEN_SEMICOLON);

            ASTNode *var_decl = ast_create_var_decl_with_type_node(enum_node, var_name);
            free(var_name);
            return var_decl;
        } else {
            parser_expect(parser, TOKEN_SEMICOLON);
            return enum_node;
        }
    }

    // Fall back to original declaration parsing for basic types
    return parser_parse_declaration(parser);
}

// Add these missing functions to your parser.c file

ASTNode *parser_parse_assignment_expression(Parser *parser) {
    ASTNode *left = parser_parse_logical_or_expression(parser);

    if (parser_match(parser, TOKEN_ASSIGN)) {
        parser_advance(parser);
        ASTNode *right = parser_parse_assignment_expression(parser);

        if (left && left->type == AST_IDENTIFIER) {
            return ast_create_assignment(left->data.identifier.name, right);
        } else {
            error_syntax(parser->current_token.line, parser->current_token.column,
                        "Invalid left-hand side in assignment");
        }
    }

    return left;
}

ASTNode *parser_parse_multiplicative_expression(Parser *parser) {
    ASTNode *left = parser_parse_unary_expression(parser);

    while (parser_match(parser, TOKEN_MULTIPLY) || parser_match(parser, TOKEN_DIVIDE) ||
           parser_match(parser, TOKEN_MODULO)) {
        TokenType op = parser->current_token.type;
        parser_advance(parser);
        ASTNode *right = parser_parse_unary_expression(parser);
        left = ast_create_binary_expr(op, left, right);
    }

    return left;
}

ASTNode *parser_parse_unary_expression(Parser *parser) {
    if (parser_match(parser, TOKEN_MINUS) || parser_match(parser, TOKEN_NOT)) {
        TokenType op = parser->current_token.type;
        parser_advance(parser);
        ASTNode *operand = parser_parse_unary_expression(parser);
        return ast_create_unary_expr(op, operand);
    }

    return parser_parse_primary_expression(parser);
}

ASTNode *parser_parse_while_statement(Parser *parser) {
    parser_advance(parser); // consume 'while'

    parser_expect(parser, TOKEN_LPAREN);
    ASTNode *condition = parser_parse_expression(parser);
    parser_expect(parser, TOKEN_RPAREN);

    ASTNode *body = parser_parse_statement(parser);

    return ast_create_while_stmt(condition, body);
}

ASTNode *parser_parse_for_statement(Parser *parser) {
    parser_advance(parser); // consume 'for'

    parser_expect(parser, TOKEN_LPAREN);

    ASTNode *init = NULL;
    if (!parser_match(parser, TOKEN_SEMICOLON)) {
        init = parser_parse_expression(parser);
    }
    parser_expect(parser, TOKEN_SEMICOLON);

    ASTNode *condition = NULL;
    if (!parser_match(parser, TOKEN_SEMICOLON)) {
        condition = parser_parse_expression(parser);
    }
    parser_expect(parser, TOKEN_SEMICOLON);

    ASTNode *update = NULL;
    if (!parser_match(parser, TOKEN_RPAREN)) {
        update = parser_parse_expression(parser);
    }
    parser_expect(parser, TOKEN_RPAREN);

    ASTNode *body = parser_parse_statement(parser);

    return ast_create_for_stmt(init, condition, update, body);
}


ASTNode *parser_parse_call_expression(Parser *parser, ASTNode *primary) {
    if (!primary || primary->type != AST_IDENTIFIER) {
        return primary;
    }

    ASTNode *call = ast_create_call_expr(primary->data.identifier.name);

    parser_advance(parser); // consume '('

    if (!parser_match(parser, TOKEN_RPAREN)) {
        do {
            ASTNode *arg = parser_parse_expression(parser);
            if (arg) {
                ast_add_argument(call, arg);
            }

            if (parser_match(parser, TOKEN_COMMA)) {
                parser_advance(parser);
            } else {
                break;
            }
        } while (true);
    }

    parser_expect(parser, TOKEN_RPAREN);

    // Clean up the original identifier node
    ast_destroy(primary);

    return call;
}


/* ============================================
 * PARSER UPDATES (parser.h and parser.c)
 * ============================================ */

/* ============================================
 * FIXED ARRAY PARSING FUNCTIONS - Replace the broken ones at the end of parser.c
 * ============================================ */

// Parse array declaration
ASTNode* parser_parse_array_declaration(Parser* parser, ASTNode* element_type) {
    if (!parser_match(parser, TOKEN_LBRACKET)) {
        return NULL;
    }

    int line = parser->current_token.line;   // Fixed: use . instead of ->
    int column = parser->current_token.column;

    parser_advance(parser); // consume '['

    ASTNode* size_expr = NULL;
    int is_dynamic = 0;

    // Check for empty brackets (dynamic array or unsized)
    if (!parser_match(parser, TOKEN_RBRACKET)) {
        if (parser_match(parser, TOKEN_IDENTIFIER) &&
            strcmp(parser->current_token.value, "dynamic") == 0) {
            is_dynamic = 1;
            parser_advance(parser);
            parser_expect(parser, TOKEN_RBRACKET);
        } else {
            // Parse size expression
            size_expr = parser_parse_expression(parser);  // Fixed: use correct function name
            parser_expect(parser, TOKEN_RBRACKET);
        }
    } else {
        parser_advance(parser); // consume ']'
    }

    return ast_create_array_declaration(element_type, size_expr, is_dynamic, line, column);
}

// Parse multidimensional array declaration
ASTNode* parser_parse_multidim_array_declaration(Parser* parser, ASTNode* element_type) {
    ASTNode** dimensions = malloc(sizeof(ASTNode*) * 10); // Max 10 dimensions
    int dim_count = 0;
    int line = parser->current_token.line;   // Fixed: use . instead of ->
    int column = parser->current_token.column;

    while (parser_match(parser, TOKEN_LBRACKET)) {
        parser_advance(parser); // consume '['

        if (parser_match(parser, TOKEN_RBRACKET)) {
            dimensions[dim_count++] = NULL; // Unsized dimension
        } else {
            dimensions[dim_count++] = parser_parse_expression(parser);  // Fixed: use correct function name
        }
        parser_expect(parser, TOKEN_RBRACKET);
    }

    return ast_create_multidim_array_declaration(element_type, dimensions, dim_count, line, column);
}

// Parse array access
ASTNode* parser_parse_array_access(Parser* parser, ASTNode* primary) {
    while (parser_match(parser, TOKEN_LBRACKET)) {
        int line = parser->current_token.line;   // Fixed: use . instead of ->
        int column = parser->current_token.column;

        parser_advance(parser); // consume '['

        ASTNode* index = parser_parse_expression(parser);  // Fixed: use correct function name
        parser_expect(parser, TOKEN_RBRACKET);

        primary = ast_create_array_access(primary, index, line, column);
    }

    return primary;
}

// Parse array literal
ASTNode* parser_parse_array_literal(Parser* parser) {
    if (!parser_match(parser, TOKEN_LBRACE)) {
        return NULL;
    }

    int line = parser->current_token.line;   // Fixed: use . instead of ->
    int column = parser->current_token.column;

    parser_advance(parser); // consume '{'

    ASTNode** elements = malloc(sizeof(ASTNode*) * 1000); // Max 1000 elements initially
    int element_count = 0;

    if (!parser_match(parser, TOKEN_RBRACE)) {
        do {
            elements[element_count++] = parser_parse_expression(parser);  // Fixed: use correct function name

            if (parser_match(parser, TOKEN_COMMA)) {
                parser_advance(parser);
            } else {
                break;
            }
        } while (!parser_match(parser, TOKEN_RBRACE) && !parser_match(parser, TOKEN_EOF));
    }

    parser_expect(parser, TOKEN_RBRACE);

    return ast_create_array_literal(elements, element_count, line, column);
}

// Parse address-of operator
ASTNode* parser_parse_address_of(Parser* parser) {
    if (!parser_match(parser, TOKEN_AMPERSAND)) {
        return NULL;
    }

    int line = parser->current_token.line;   // Fixed: use . instead of ->
    int column = parser->current_token.column;

    parser_advance(parser); // consume '&'

    ASTNode* operand = parser_parse_unary_expression(parser);  // Fixed: use existing function
    return ast_create_address_of(operand, line, column);
}

// Parse pointer dereference
ASTNode* parser_parse_pointer_dereference(Parser* parser) {
    if (!parser_match(parser, TOKEN_MULTIPLY)) {
        return NULL;
    }

    int line = parser->current_token.line;   // Fixed: use . instead of ->
    int column = parser->current_token.column;

    parser_advance(parser); // consume '*'

    ASTNode* operand = parser_parse_unary_expression(parser);  // Fixed: use existing function
    return ast_create_pointer_dereference(operand, line, column);
}

// Enhanced primary expression parsing to include arrays
ASTNode *parser_parse_primary_expression_with_arrays(Parser *parser) {
    ASTNode *primary = parser_parse_primary_expression(parser);

    // Handle array access after primary expression
    if (primary) {
        primary = parser_parse_array_access(parser, primary);
    }

    return primary;
}

// Update unary expression parsing to include address-of and dereference
ASTNode *parser_parse_unary_expression_with_pointers(Parser *parser) {
    // Check for address-of operator
    if (parser_match(parser, TOKEN_AMPERSAND)) {
        return parser_parse_address_of(parser);
    }

    // Check for dereference operator
    if (parser_match(parser, TOKEN_MULTIPLY)) {
        return parser_parse_pointer_dereference(parser);
    }

    // Fall back to existing unary expression parsing
    return parser_parse_unary_expression(parser);
}

