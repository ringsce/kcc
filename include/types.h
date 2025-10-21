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
    TOKEN_SWITCH,        // ADD THIS
    TOKEN_CASE,          // ADD THIS
    TOKEN_DEFAULT,       // ADD THIS

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
    TOKEN_LESS,
    // New type keywords
    TOKEN_LONG,           // long
    TOKEN_SHORT,          // short
    TOKEN_UNSIGNED,       // unsigned
    TOKEN_SIGNED,         // signed
    TOKEN_SIZEOF,         // sizeof operator
    TOKEN_VOLATILE,       // volatile
    TOKEN_RESTRICT,       // restrict (optional, C99)

    // ARC-specific keywords
    TOKEN_BRIDGE,           // __bridge
    TOKEN_BRIDGE_RETAINED,  // __bridge_retained
    TOKEN_BRIDGE_TRANSFER  // __bridge_transfer

} TokenType;

// Add this enum after DataType enum in types.h (around line 250)
typedef enum {
    QUAL_NONE = 0,
    QUAL_CONST = 1 << 0,      // const qualifier
    QUAL_VOLATILE = 1 << 1,   // volatile qualifier
    QUAL_RESTRICT = 1 << 2    // restrict qualifier (C99)
} TypeQualifier;

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
    // New complex types
    TYPE_LONG,              // long int
    TYPE_LONG_LONG,         // long long int
    TYPE_UNSIGNED_INT,      // unsigned int
    TYPE_UNSIGNED_LONG,     // unsigned long
    TYPE_SHORT,             // short int
    TYPE_UNSIGNED_SHORT,    // unsigned short
    TYPE_SIGNED_CHAR,       // signed char
    TYPE_UNSIGNED_CHAR,     // unsigned char
    TYPE_LONG_DOUBLE,       // long double

    // Function pointer type
    TYPE_FUNCTION_POINTER,  // Function pointer type

    // String types
    TYPE_STRING,            // char[] or char*
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
// Replace your ASTNodeType enum in types.h with this updated version:

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
    AST_MEMBER_ACCESS,
    AST_ARRAY_ACCESS,
    AST_TERNARY_OP,

    // Objective-C Expressions
    AST_OBJC_MESSAGE_SEND,
    AST_OBJC_STRING_LITERAL,
    AST_OBJC_SELECTOR_EXPR,
    AST_OBJC_PROTOCOL_EXPR,
    AST_OBJC_ENCODE_EXPR,
    AST_OBJC_BOOLEAN_LITERAL,

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

    // NEW: Complex types - ADD THESE
    AST_FUNCTION_POINTER,
    AST_SIZEOF_EXPR,
    AST_CAST_EXPR,
    AST_FLOAT_LITERAL,
    AST_DOUBLE_LITERAL,
    AST_LONG_LITERAL,
    AST_ULONG_LITERAL,

    // In the statements section
    AST_SWITCH_STATEMENT,
    AST_CASE_STATEMENT,
    AST_DEFAULT_STATEMENT,

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

// Add ARC ownership qualifiers
typedef enum {
    ARC_QUALIFIER_NONE = 0,
    ARC_QUALIFIER_STRONG,           // Default for object types
    ARC_QUALIFIER_WEAK,             // Zeroing weak reference
    ARC_QUALIFIER_UNSAFE_UNRETAINED, // Non-zeroing weak reference
    ARC_QUALIFIER_AUTORELEASING     // Used for out parameters
} ARCQualifier;

// ARC bridge casting types
typedef enum {
    ARC_BRIDGE_NONE = 0,
    ARC_BRIDGE,                     // __bridge
    ARC_BRIDGE_RETAINED,            // __bridge_retained (CF to ObjC)
    ARC_BRIDGE_TRANSFER             // __bridge_transfer (ObjC to CF)
} ARCBridgeCast;

// ARC memory management metadata
typedef struct {
    ARCQualifier qualifier;
    bool is_objc_object;            // Is this an ObjC object type?
    bool needs_retain;              // Should retain on assignment
    bool needs_release;             // Should release on scope exit
    bool is_parameter;              // Is this a function/method parameter?
    bool is_return_value;           // Is this a return value?
    int retain_count;               // Track retain count (for analysis)
} ARCInfo;

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

    // Add ARC metadata
    ARCInfo arc_info;



    union {

        // Enhanced variable declaration with ARC
        struct {
            DataType var_type;
            char *name;
            struct ASTNode *initializer;
            struct ASTNode *type_node;
            ARCQualifier arc_qualifier;  // Ownership qualifier
        } var_decl_arc;

        // ARC-aware assignment
        struct {
            char *variable;
            struct ASTNode *value;
            bool needs_retain;           // Insert retain call
            bool needs_release;          // Insert release call
            ARCQualifier source_qualifier;
            ARCQualifier dest_qualifier;
        } assignment_arc;

        // ARC bridge cast
        struct {
            ARCBridgeCast bridge_type;
            struct ASTNode *operand;
            DataType target_type;
        } bridge_cast;

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
            struct ASTNode *type_node;
            TypeQualifier qualifiers;  // ADD THIS LINE
            bool is_const;             // ADD THIS LINE
            bool is_volatile;          // ADD THIS LINE
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
            struct ASTNode **protocols;
            int protocol_count;
            struct ASTNode **methods;
            int method_count;
            struct ASTNode **properties;
            int property_count;
        } objc_interface;

        struct {
            char *class_name;
            char *category_name;
            struct ASTNode **methods;
            int method_count;
            struct ASTNode **ivars;
            int ivar_count;
        } objc_implementation;

        struct {
            ObjCMethodType method_type;
            DataType return_type;
            char *selector;
            ObjCMethodParam *params;
            int param_count;
            struct ASTNode *body;
        } objc_method;

        struct {
            struct ASTNode *receiver;
            char *selector;
            struct ASTNode **arguments;
            int argument_count;
        } objc_message;

        struct {
            DataType property_type;
            char *property_name;
            ObjCPropertyAttributes attributes;
            char *getter_name;
            char *setter_name;
        } objc_property;

        struct {
            char *protocol_name;
            struct ASTNode **methods;
            int method_count;
            struct ASTNode **properties;
            int property_count;
        } objc_protocol;

        struct {
            char *value;
        } objc_string;

        struct {
            char *selector_name;
        } objc_selector;

        struct {
            bool value;
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
            TypeQualifier qualifiers;  // ADD THIS LINE
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

        // NEW: Complex type structures - ADD THESE
        struct {
            DataType return_type;
            char *name;
            struct ASTNode **param_types;
            int param_count;
            bool is_variadic;
        } function_ptr;

        struct {
            struct ASTNode *operand;
        } sizeof_expr;

        struct {
            DataType target_type;
            struct ASTNode *operand;
        } cast_expr;

        struct {
            char value;
        } char_literal;

        struct {
            float value;
        } float_literal;

        struct {
            double value;
        } double_literal;

        struct {
            long value;
        } long_literal;

        struct {
            unsigned long value;
        } ulong_literal;

        // Switch statement
        struct {
            struct ASTNode *expression;      // Switch expression
            struct ASTNode **cases;          // Array of case/default statements
            int case_count;                  // Number of cases
        } switch_stmt;

        // Case statement
        struct {
            struct ASTNode *value;           // Case value (NULL for default)
            struct ASTNode **statements;     // Statements in this case
            int statement_count;             // Number of statements
            bool is_default;                 // Is this a default case?
        } case_stmt;

    } data;  // This closes the union
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

// ARC context for tracking scope and cleanup
typedef struct ARCContext {
    SymbolTable* symbol_table;
    ASTNode** cleanup_vars;      // Variables needing cleanup on scope exit
    int cleanup_count;
    int cleanup_capacity;
    ASTNode** weak_refs;         // Weak references for zeroing
    int weak_ref_count;
    int scope_depth;
} ARCContext;

// Forward declaration only - full definition in preprocessor.h
typedef struct Preprocessor Preprocessor;

#endif // TYPES_H
