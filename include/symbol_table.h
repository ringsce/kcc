#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "types.h"
#include <stdbool.h>

#define SYMBOL_TABLE_SIZE 127

// Symbol types for the symbol table - UNIFIED DEFINITION
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
    SYMBOL_ARRAY,         // Added array support
    SYMBOL_POINTER,       // Added pointer support
    SYMBOL_STRUCT,        // Added struct support
    SYMBOL_UNION,         // Added union support
    SYMBOL_ENUM,          // Added enum support
    SYMBOL_TYPEDEF        // Added typedef support
} SymbolType;

// Array information structure
typedef struct {
    DataType element_type;    // Changed from SymbolType to DataType
    int dimensions[10];       // Max 10 dimensions
    int dimension_count;
    int total_size;          // Total elements across all dimensions
    int is_dynamic;          // 1 for dynamic arrays
    void* data;              // Pointer to array data (for emulation)
} ArrayInfo;

// Pointer information structure
typedef struct {
    DataType pointed_type;   // Type being pointed to
    int indirection_level;   // Number of pointer levels (* vs ** vs ***)
} PointerInfo;

// Struct/Union member information
typedef struct {
    char* member_name;
    DataType member_type;
    int offset;              // Offset within struct/union
    int bitfield_width;      // For bitfields, 0 if not a bitfield
} MemberInfo;

// Complex type information
typedef struct {
    char* type_name;
    MemberInfo* members;     // Array of members (for struct/union)
    int member_count;
    int size;               // Size in bytes
    int alignment;          // Alignment requirements
} ComplexTypeInfo;

// Symbol structure for hash table entries - UNIFIED DEFINITION
typedef struct Symbol {
    char *name;
    SymbolType symbol_type;
    DataType data_type;
    int scope_level;
    int line;               // Line where symbol was declared
    int column;             // Column where symbol was declared
    bool is_initialized;    // Whether variable has been initialized
    bool is_used;           // Whether symbol has been referenced

    // Union for different symbol data types
    union {
        ArrayInfo array_info;           // For arrays
        PointerInfo pointer_info;       // For pointers
        ComplexTypeInfo complex_info;   // For structs/unions/enums
        struct {
            int parameter_index;        // For function parameters
            bool is_variadic;          // For variadic functions
        } param_info;
        struct {
            DataType return_type;       // Function return type
            int param_count;           // Number of parameters
            DataType* param_types;     // Array of parameter types
            bool is_variadic;          // Whether function is variadic
        } func_info;
    } data;

    struct Symbol *next;  // For chaining in hash table
} Symbol;

// Symbol table structure
typedef struct SymbolTable {
    Symbol **table;       // Array of symbol pointers (hash table)
    int current_scope;    // Current scope level
    int max_scope_seen;   // Highest scope level encountered
} SymbolTable;

// Symbol table functions
SymbolTable *symbol_table_create(void);
void symbol_table_destroy(SymbolTable *table);
void symbol_table_enter_scope(SymbolTable *table);
void symbol_table_exit_scope(SymbolTable *table);

// Symbol management functions
bool symbol_table_insert(SymbolTable *table, const char *name, SymbolType symbol_type, DataType data_type);
Symbol *symbol_table_lookup(SymbolTable *table, const char *name);
Symbol *symbol_table_lookup_current_scope(SymbolTable *table, const char *name);
bool symbol_table_remove(SymbolTable *table, const char *name);

// Specialized symbol creation functions
Symbol* create_array_symbol(const char* name, DataType element_type,
                          int* dimensions, int dim_count, int is_dynamic,
                          int line, int column);

Symbol* create_pointer_symbol(const char* name, DataType pointed_type,
                            int indirection_level, int line, int column);

Symbol* create_function_symbol(const char* name, DataType return_type,
                             DataType* param_types, int param_count,
                             bool is_variadic, int line, int column);

Symbol* create_struct_symbol(const char* name, MemberInfo* members,
                           int member_count, int size, int alignment,
                           int line, int column);

Symbol* create_union_symbol(const char* name, MemberInfo* members,
                          int member_count, int size, int alignment,
                          int line, int column);

// Array-specific helper functions
bool symbol_table_insert_array(SymbolTable *table, const char *name,
                              DataType element_type, int* dimensions,
                              int dim_count, bool is_dynamic,
                              int line, int column);

ArrayInfo* symbol_get_array_info(Symbol* symbol);
bool symbol_is_array(Symbol* symbol);
bool symbol_is_dynamic_array(Symbol* symbol);

// Pointer-specific helper functions
bool symbol_table_insert_pointer(SymbolTable *table, const char *name,
                                DataType pointed_type, int indirection_level,
                                int line, int column);

PointerInfo* symbol_get_pointer_info(Symbol* symbol);
bool symbol_is_pointer(Symbol* symbol);

// Type compatibility checking
bool types_are_compatible(DataType type1, DataType type2);
bool can_convert_types(DataType from_type, DataType to_type);
DataType get_promoted_type(DataType type1, DataType type2);

// Symbol validation functions
bool symbol_is_defined_in_scope(SymbolTable *table, const char *name, int scope);
void mark_symbol_as_used(SymbolTable *table, const char *name);
void mark_symbol_as_initialized(SymbolTable *table, const char *name);

// Debug and utility functions
unsigned int symbol_table_hash(const char *name);
void symbol_table_print(SymbolTable *table);
void symbol_table_print_scope(SymbolTable *table, int scope);
void symbol_print_info(Symbol *symbol);

// Statistics functions
int symbol_table_count_symbols(SymbolTable *table);
int symbol_table_count_unused_symbols(SymbolTable *table);
void symbol_table_report_unused(SymbolTable *table);

#endif // SYMBOL_TABLE_H