// In a new file semantic.c or in your existing analyzer

bool semantic_check_const_assignment(ASTNode *node, SymbolTable *symbols) {
    if (node->type == AST_ASSIGNMENT) {
        // Look up variable in symbol table
        Symbol *sym = symbol_table_lookup(symbols, node->data.assignment.variable);
        if (sym && sym->is_const) {
            fprintf(stderr, "Error: Cannot assign to const variable '%s'\n",
                   node->data.assignment.variable);
            return false;
        }
    }
    return true;
}