# Próximos passos — fase de tradução

## Concluído

- `translate_as_block` (TR_RETURN / TR_ASSIGN) — substitui `if_block` e `py_defs.py`
- `translate_as_return` — wrapper sobre `translate_as_block`
- `define_var` — usa `needs_block()` recursiva; detecta `begin`/`if` aninhados
- `define_func` — usa `translate_as_return` na última expressão do corpo
- `let_named` — ponteiro corrigido, params extraídos dos bindings, chamada inicial emitida
- `call_operator` — todos os operandos passam por `translate()`
- `call` — guarda de tipo antes de `get_op`; lambda operador recebe `()` ao redor
- `if_statement` — checa `else_branch == NULL`, emite `None` quando ausente
- `AST_BOOLEAN` — `#t` → `True`, `#f` → `False`
- `AST_SET` — `(set! x val)` → `x = val` com indentação
- `AST_BEGIN` standalone — emite cada sub-expressão como statement
- `AST_AND` / `AST_OR` — `and` / `or` infix via `call_logic`
- `AST_LAMBDA` — caso simples: `lambda params: expr`; complexo emite aviso
- `AST_COND` — cadeia `if/elif/else` via `translate_as_block`
- `AST_CASE` — cadeia `if key in (...)/elif/else` via `translate_as_block`
- `AST_CASE` no switch de `translate` — statement de nível superior
- Lexer — `COMMENT ;[^\n]*` (comentários vazios e sem `\n` final)
- Lexer — `STRING_ELEMENT ([^"\\]|\\.)` (escape sequences em strings)
- `AST_DO` — laço `while` com inicialização, atualização simultânea e resultado
- `AST_QUOTE` — símbolo → `"str"`, lista → `[...]`, `'()` → `[]` via `translate_datum`
- `AST_VECTOR` — `#(1 2 3)` → `[1, 2, 3]` via `translate_datum`
- `AST_CHARACTER` — `#\a` → `'a'`, `#\space` → `' '`, `#\newline` → `'\n'`

---

## Limitações conhecidas (não implementadas)

- `cond` nas formas `(test)` e `(test => recipient)` — `body` é NULL, não suportado
- `let` / `let*` com múltiplas expressões no corpo — emite aviso, não suportado
- `lambda` com corpo complexo (`begin` ou múltiplas expressões) — emite aviso
- `AST_DO` como expressão (valor de `define`, argumento de chamada, etc.) — emite `None`
- `AST_LETREC` — não implementado
- `AST_DELAY` — não implementado
- `AST_QUOTE` aninhado dentro de datum (ex.: `''foo`) — emite `None`
- `AST_CHARACTER` com `#\\` ou `#\'` — gera Python inválido (caso raro)

---

## Pendente

Todos os itens planejados foram implementados ou marcados como limitação.
