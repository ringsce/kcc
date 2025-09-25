#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "types.h"
#include <stdbool.h>

#define SYMBOL_TABLE_SIZE 127

// Symbol types for the symbol table
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER
} SymbolType;

// Symbol structure for hash table entries
typedef struct Symbol {
    char *name;
    SymbolType symbol_type;
    DataType data_type;
    int scope_level;
    struct Symbol *next;  // For chaining in hash table
} Symbol;

// Symbol table structure
typedef struct SymbolTable {
    Symbol **table;       // Array of symbol pointers (hash table)
    int current_scope;    // Current scope level
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

// Utility functions
unsigned int symbol_table_hash(const char *name);
void symbol_table_print(SymbolTable *table);

#endif // SYMBOL_TABLE_H