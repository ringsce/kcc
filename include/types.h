/**
 * @file types.h
 * @brief Type definitions for KCC compiler with Objective-C support
 * @author KCC Development Team
 * @version 1.10.0
 *
 * This file contains all type definitions, enums, and structures used throughout
 * the KCC compiler, including support for both C and Objective-C language constructs.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>      // Add this for FILE type

// Forward declarations to avoid circular dependencies - MOVED TO TOP
struct ASTNode;
typedef struct ASTNode ASTNode;
struct SymbolTable;
typedef struct SymbolTable SymbolTable;
struct Symbol;
typedef struct Symbol Symbol;

/**
 * @brief Token types for lexical analysis
 *
 * Enumeration of all token types supported by the lexer, including
 * C language tokens and Objective-C extensions.
 */
typedef enum {
    TOKEN_EOF = -1,
    TOKEN_UNKNOWN = 0,

    // C Keywords
    TOKEN_INT,
    TOKEN_CHAR_KW,
    TOKEN_VOID,
    TOKEN_FLOAT,
    TOKEN_DOUBLE,         // Add double support
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_RETURN,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_STRUCT,         // Add struct support
    TOKEN_UNION,          // Add union support
    TOKEN_ENUM,           // Add enum support
    TOKEN_TYPEDEF,        // Add typedef support
    TOKEN_STATIC,         // Add static support
    TOKEN_EXTERN,         // Add extern support
    TOKEN_CONST,          // Add const support

    // Objective-C Keywords and Directives
    TOKEN_AT,             // @
    TOKEN_INTERFACE,      // @interface (also used without @)
    TOKEN_IMPLEMENTATION, // @implementation (also used without @)
    TOKEN_PROTOCOL,       // @protocol (also used without @)
    TOKEN_END,            // @end (also used without @)
    TOKEN_PROPERTY,       // @property (also used without @)
    TOKEN_SYNTHESIZE,     // @synthesize (also used without @)
    TOKEN_DYNAMIC,        // @dynamic (also used without @)
    TOKEN_SELECTOR,       // @selector (also used without @)
    TOKEN_CLASS,          // @class (forward declaration, also used without @)
    TOKEN_PRIVATE,        // @private
    TOKEN_PROTECTED,      // @protected
    TOKEN_PUBLIC,         // @public
    TOKEN_PACKAGE,        // @package
    TOKEN_OPTIONAL,       // @optional
    TOKEN_REQUIRED,       // @required
    TOKEN_AUTORELEASEPOOL,// @autoreleasepool
    TOKEN_TRY,            // @try (also used without @)
    TOKEN_CATCH,          // @catch (also used without @)
    TOKEN_FINALLY,        // @finally (also used without @)
    TOKEN_THROW,          // @throw (also used without @)
    TOKEN_SYNCHRONIZED,   // @synchronized (also used without @)

    // Objective-C @ prefixed tokens (specific to @ usage)
    TOKEN_AT_INTERFACE,
    TOKEN_AT_IMPLEMENTATION,
    TOKEN_AT_PROTOCOL,
    TOKEN_AT_PROPERTY,
    TOKEN_AT_SYNTHESIZE,
    TOKEN_AT_DYNAMIC,
    TOKEN_AT_CLASS,
    TOKEN_AT_SELECTOR,
    TOKEN_AT_ENCODE,
    TOKEN_AT_SYNCHRONIZED,
    TOKEN_AT_TRY,
    TOKEN_AT_CATCH,
    TOKEN_AT_FINALLY,
    TOKEN_AT_THROW,
    TOKEN_AT_END,
    TOKEN_AT_IDENTIFIER,

    // Objective-C literals and expressions
    TOKEN_NSSTRING_LITERAL,    // @"string"
    TOKEN_NSARRAY_LITERAL,     // @[...]
    TOKEN_NSDICTIONARY_LITERAL, // @{...}
    TOKEN_BOXED_EXPRESSION,    // @(...)
    TOKEN_BOXED_NUMBER,        // @123

    // Objective-C Memory Management
    TOKEN_RETAIN,         // retain
    TOKEN_RELEASE,        // release
    TOKEN_AUTORELEASE,    // autorelease
    TOKEN_STRONG,         // __strong
    TOKEN_WEAK,           // __weak
    TOKEN_UNSAFE_UNRETAINED, // __unsafe_unretained
    TOKEN_AUTORELEASING,  // __autoreleasing

    // Property attributes
    TOKEN_ATOMIC,         // atomic
    TOKEN_NONATOMIC,      // nonatomic
    TOKEN_ASSIGN_ATTR,    // assign (property attribute, different from =)
    TOKEN_COPY,           // copy
    TOKEN_READONLY,       // readonly
    TOKEN_READWRITE,      // readwrite
    TOKEN_GETTER,         // getter
    TOKEN_SETTER,         // setter

    // Objective-C Types
    TOKEN_ID,             // id
    TOKEN_CLASS_KW,       // Class
    TOKEN_SEL,            // SEL
    TOKEN_IMP,            // IMP
    TOKEN_BOOL_KW,        // BOOL
    TOKEN_YES,            // YES
    TOKEN_NO,             // NO
    TOKEN_NIL,            // nil
    TOKEN_NULL,           // NULL
    TOKEN_SELF,           // self
    TOKEN_SUPER,          // super
    TOKEN_INSTANCETYPE,   // instancetype

    // Foundation framework types
    TOKEN_NSSTRING,       // NSString
    TOKEN_NSARRAY,        // NSArray
    TOKEN_NSDICTIONARY,   // NSDictionary
    TOKEN_NSOBJECT,       // NSObject

    // Additional Objective-C constructs
    TOKEN_ENCODE,         // encode (used with @encode)

    // Identifiers and Literals
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_STRING_OBJC,    // @"Objective-C string" (alternative naming)
    TOKEN_CHAR,           // 'c'

    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_BITWISE_AND,    // &
    TOKEN_BITWISE_OR,     // |
    TOKEN_BITWISE_XOR,    // ^
    TOKEN_BITWISE_NOT,    // ~
    TOKEN_LEFT_SHIFT,     // <<
    TOKEN_RIGHT_SHIFT,    // >>
    TOKEN_INCREMENT,      // ++
    TOKEN_DECREMENT,      // --
    TOKEN_PLUS_ASSIGN,    // +=
    TOKEN_MINUS_ASSIGN,   // -=
    TOKEN_ARROW,          // ->
    TOKEN_DOT,            // .

    // Delimiters
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,       // [ (also used for message sends)
    TOKEN_RBRACKET,       // ] (also used for message sends)
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_COLON,          // : (used in Objective-C method names)
    TOKEN_QUESTION,       // ? (ternary operator)
    TOKEN_NEWLINE,
    TOKEN_HASH,
    TOKEN_AMPERSAND,      // &
    TOKEN_PIPE,           // |
    TOKEN_LESS

} TokenType;

/**
 * @brief Data types supported by the compiler
 */
typedef enum {
    TYPE_UNKNOWN,
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_ID,              // Objective-C id type
    TYPE_CLASS,           // Objective-C Class type
    TYPE_SEL,             // Objective-C SEL type
    TYPE_POINTER,         // Pointer types
    TYPE_STRUCT,          // Structure types
    TYPE_UNION,           // Union types
    TYPE_ENUM,
    TYPE_TYPEDEF,
    TYPE_ARRAY            // Array types
} DataType;

/**
 * @brief AST Node types
 *
 * Enumeration of all AST node types, including C and Objective-C constructs.
 */
typedef enum {
    // C Declarations
    AST_FUNCTION_DECLARATION,
    AST_VARIABLE_DECLARATION,
    AST_PARAMETER,
    AST_STRUCT_DECLARATION,
    AST_UNION_DECLARATION,
    AST_ENUM_DECLARATION,
    AST_TYPEDEF_DECLARATION,

    // Objective-C Declarations
    AST_OBJC_INTERFACE,
    AST_OBJC_IMPLEMENTATION,
    AST_OBJC_PROTOCOL,
    AST_OBJC_CATEGORY,
    AST_OBJC_METHOD_DECLARATION,
    AST_OBJC_PROPERTY_DECLARATION,
    AST_OBJC_SYNTHESIZE,
    AST_OBJC_DYNAMIC,

    // C Statements
    AST_COMPOUND_STATEMENT,
    AST_EXPRESSION_STATEMENT,
    AST_IF_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_FOR_STATEMENT,
    AST_RETURN_STATEMENT,
    AST_BREAK_STATEMENT,
    AST_CONTINUE_STATEMENT,

    // Objective-C Statements
    AST_OBJC_TRY_STATEMENT,
    AST_OBJC_CATCH_STATEMENT,
    AST_OBJC_FINALLY_STATEMENT,
    AST_OBJC_THROW_STATEMENT,
    AST_OBJC_SYNCHRONIZED_STATEMENT,
    AST_OBJC_AUTORELEASEPOOL_STATEMENT,

    // C Expressions
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_IDENTIFIER,
    AST_NUMBER_LITERAL,
    AST_STRING_LITERAL,
    AST_CHAR_LITERAL,
    AST_MEMBER_ACCESS,    // . and -> operators
    AST_ARRAY_ACCESS,     // [] operator - ONLY DEFINED ONCE
    AST_TERNARY_OP,       // ? : operator

    // Objective-C Expressions
    AST_OBJC_MESSAGE_SEND,
    AST_OBJC_STRING_LITERAL,  // @"string"
    AST_OBJC_SELECTOR_EXPR,   // @selector()
    AST_OBJC_PROTOCOL_EXPR,   // @protocol()
    AST_OBJC_ENCODE_EXPR,     // @encode()
    AST_OBJC_BOOLEAN_LITERAL, // YES/NO

    // Complex type definitions
    AST_TYPEDEF,
    AST_STRUCT,
    AST_UNION,
    AST_ENUM,
    AST_ENUM_CONSTANT,
    AST_STRUCT_MEMBER,
    AST_BASIC_TYPE,
    AST_VAR_DECL,

    // Array support
    AST_ARRAY_DECLARATION,
    AST_ARRAY_LITERAL,
    AST_POINTER_DEREFERENCE,
    AST_ADDRESS_OF,

    // Special
    AST_PROGRAM
} ASTNodeType;

/**
 * @brief Objective-C method type
 */
typedef enum {
    OBJC_INSTANCE_METHOD,  // - (void)method
    OBJC_CLASS_METHOD      // + (void)method
} ObjCMethodType;

/**
 * @brief Objective-C property attributes
 */
typedef enum {
    OBJC_PROPERTY_ASSIGN    = 1 << 0,
    OBJC_PROPERTY_RETAIN    = 1 << 1,
    OBJC_PROPERTY_COPY      = 1 << 2,
    OBJC_PROPERTY_READONLY  = 1 << 3,
    OBJC_PROPERTY_READWRITE = 1 << 4,
    OBJC_PROPERTY_NONATOMIC = 1 << 5,
    OBJC_PROPERTY_ATOMIC    = 1 << 6,
    OBJC_PROPERTY_STRONG    = 1 << 7,
    OBJC_PROPERTY_WEAK      = 1 << 8
} ObjCPropertyAttributes;

/**
 * @brief Token structure
 */
typedef struct Token {
    TokenType type;
    char *value;              // Token text (lexeme)
    char *lexeme;            // Alternative name for compatibility
    int line;                // Line number
    int column;              // Column number
    union {
        int int_value;
        float float_value;
        double double_value;
        char char_value;
        bool bool_value;
    } literal;               // For literal values
} Token;

/**
 * @brief Objective-C method parameter
 */
typedef struct ObjCMethodParam {
    char *selector_part;     // The selector part (e.g., "with" in "withString:")
    DataType param_type;     // Parameter type
    char *param_name;        // Parameter name
} ObjCMethodParam;

// Array-specific AST node structures - DEFINED BEFORE ASTNode
typedef struct {
    ASTNode* element_type;    // Type of array elements
    ASTNode* size_expr;       // Size expression (can be NULL for unsized arrays)
    int is_dynamic;           // 1 for dynamic arrays, 0 for static
    int dimension_count;      // Number of dimensions
    ASTNode** dimensions;     // Array of dimension expressions
} ArrayDeclaration;

typedef struct {
    ASTNode* array_expr;      // Expression that evaluates to array
    ASTNode* index_expr;      // Index expression
} ArrayAccess;

typedef struct {
    ASTNode** elements;       // Array of element expressions
    int element_count;        // Number of elements
    ASTNode* element_type;    // Type of elements (inferred)
} ArrayLiteral;

typedef struct {
    ASTNode* operand;         // Expression to take address of
} AddressOf;

typedef struct {
    ASTNode* operand;         // Expression to dereference
} PointerDereference;

/**
 * @brief AST Node structure
 *
 * Main AST node structure supporting both C and Objective-C constructs.
 */
struct ASTNode {
    ASTNodeType type;
    DataType data_type;
    int line;
    int column;

    union {
        struct {
            struct ASTNode **declarations;
            int declaration_count;
        } program;

        struct {
            DataType return_type;
            char *name;
            struct ASTNode **parameters;
            int parameter_count;
            struct ASTNode *body;
        } function_decl;

        struct {
            DataType var_type;
            char *name;
            struct ASTNode *initializer;
            struct ASTNode *type_node;  // Add this field for complex types
        } var_decl;

        struct {
            DataType param_type;
            char *name;
        } parameter;

        struct {
            struct ASTNode **statements;
            int statement_count;
        } compound_stmt;

        struct {
            struct ASTNode *expression;
        } expression_stmt;

        struct {
            struct ASTNode *expression;
        } return_stmt;

        struct {
            struct ASTNode *condition;
            struct ASTNode *then_stmt;
            struct ASTNode *else_stmt;
        } if_stmt;

        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_stmt;

        struct {
            struct ASTNode *init;
            struct ASTNode *condition;
            struct ASTNode *update;
            struct ASTNode *body;
        } for_stmt;

        struct {
            TokenType operator;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_expr;

        struct {
            TokenType operator;
            struct ASTNode *operand;
        } unary_expr;

        struct {
            char *function_name;
            struct ASTNode **arguments;
            int argument_count;
        } call_expr;

        struct {
            char *name;
        } identifier;

        struct {
            int value;
        } number;

        struct {
            char *value;
        } string;

        struct {
            char *variable;
            struct ASTNode *value;
        } assignment;

        // Objective-C specific nodes
        struct {
            char *class_name;
            char *superclass_name;
            struct ASTNode **protocols;     // Adopted protocols
            int protocol_count;
            struct ASTNode **methods;       // Method declarations
            int method_count;
            struct ASTNode **properties;   // Property declarations
            int property_count;
        } objc_interface;

        struct {
            char *class_name;
            char *category_name;            // NULL if not a category
            struct ASTNode **methods;       // Method implementations
            int method_count;
            struct ASTNode **ivars;         // Instance variables
            int ivar_count;
        } objc_implementation;

        struct {
            ObjCMethodType method_type;     // + or -
            DataType return_type;
            char *selector;                 // Full selector string
            ObjCMethodParam *params;        // Method parameters
            int param_count;
            struct ASTNode *body;           // Method body (NULL for declarations)
        } objc_method;

        struct {
            struct ASTNode *receiver;       // Object receiving the message
            char *selector;                 // Method selector
            struct ASTNode **arguments;     // Arguments
            int argument_count;
        } objc_message;

        struct {
            DataType property_type;
            char *property_name;
            ObjCPropertyAttributes attributes;
            char *getter_name;              // Custom getter name (optional)
            char *setter_name;              // Custom setter name (optional)
        } objc_property;

        struct {
            char *protocol_name;
            struct ASTNode **methods;       // Required/optional methods
            int method_count;
            struct ASTNode **properties;   // Protocol properties
            int property_count;
        } objc_protocol;

        struct {
            char *value;                    // @"string content"
        } objc_string;

        struct {
            char *selector_name;            // @selector(methodName:)
        } objc_selector;

        struct {
            bool value;                     // YES or NO
        } objc_boolean;

        // Complex type definitions
        struct {
            struct ASTNode *base_type;
            char *alias_name;
        } typedef_decl;

        struct {
            char *name;
            struct ASTNode **members;
            int member_count;
        } struct_decl;

        struct {
            char *name;
            struct ASTNode **members;
            int member_count;
        } union_decl;

        struct {
            char *name;
            struct ASTNode **constants;
            int constant_count;
        } enum_decl;

        struct {
            char *name;
            int value;
        } enum_constant;

        struct {
            DataType type;
            char *name;
            int bitfield_width;
            struct ASTNode *type_node;
        } struct_member;

        struct {
            DataType type;
        } basic_type;

        // Array support structures
        ArrayDeclaration array_decl;
        ArrayAccess array_access;
        ArrayLiteral array_literal;
        AddressOf address_of;
        PointerDereference pointer_deref;

    } data;  // This closes the union - ONLY ONE data union
};

/**
 * @brief Lexer structure
 */
typedef struct Lexer {
    const char *source;
    size_t position;
    size_t current;
    int line;
    int column;
    bool has_error;
    char *error_message;
    char *input;
    size_t pos;
    size_t input_length;
    bool objc_mode;              // Enable Objective-C syntax
} Lexer;

/**
 * @brief Parser structure
 */
typedef struct Parser {
    Lexer *lexer;
    Token current_token;
    Token peek_token;
    bool has_error;
    char *error_message;
    bool objc_mode;              // Enable Objective-C parsing
} Parser;

/**
 * @brief CodeGenerator structure
 */
typedef struct CodeGenerator {
    FILE *output_file;
    int label_counter;
    int temp_counter;
    bool objc_mode;              // Enable Objective-C code generation
    SymbolTable *symbol_table;   // Now properly forward declared
} CodeGenerator;

// Utility function declarations
const char* token_type_to_string(TokenType type);
const char* data_type_to_string(DataType type);
const char* ast_node_type_to_string(ASTNodeType type);
const char* objc_method_type_to_string(ObjCMethodType type);
DataType token_to_data_type(TokenType type);

// PreProcessor definitions
#define MAX_MACRO_NAME 64
#define MAX_MACRO_BODY 512
#define MAX_MACRO_PARAMS 32
#define MAX_MACROS 256
#define MAX_INCLUDE_DEPTH 32
#define MAX_LINE_LENGTH 1024

/**
 * @brief Macro types
 */
typedef enum {
    MACRO_OBJECT,       // #define NAME value
    MACRO_FUNCTION      // #define NAME(params) body
} MacroType;

/**
 * @brief Macro parameter
 */
typedef struct MacroParam {
    char name[MAX_MACRO_NAME];
} MacroParam;

/**
 * @brief Macro definition
 */
typedef struct Macro {
    char name[256];
    char body[1024];
    MacroType type;
    MacroParam params[MAX_MACRO_PARAMS];
    int param_count;
    bool is_predefined;
    int line_defined;
    char *file_defined;
} Macro;

/**
 * @brief Conditional compilation state
 */
typedef enum {
    COND_NONE,
    COND_IF,
    COND_IFDEF,
    COND_IFNDEF,
    COND_ELIF,
    COND_ELSE
} ConditionalType;

/**
 * @brief Conditional state tracking
 */
typedef struct ConditionalState {
    ConditionalType type;
    bool condition_met;
    bool else_taken;
    int line_number;
} ConditionalState;

/**
 * @brief Include file tracking
 */
typedef struct IncludeFile {
    char *filename;
    char *content;
    int line;
    int pos;
} IncludeFile;

// Forward declaration only - full definition in preprocessor.h
typedef struct Preprocessor Preprocessor;

#endif // TYPES_H