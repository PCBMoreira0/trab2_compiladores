typedef enum {
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_ASSIGN_STMT,    
    AST_VAR_DECL,       

    AST_BINOP,        
    AST_UNARYOP,      

    AST_IDENTIFIER,  
    AST_LITERAL_INT,
    AST_LITERAL_STRING
} NodeType;