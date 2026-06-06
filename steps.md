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
- Lexer — `COMMENT ;[^\n]*` (comentários vazios e sem `\n` final)

---

## Pendente

### P8. `AST_LAMBDA` — `(lambda (params) body)`
Corpo simples (1 expressão, não-`begin`): `lambda p1, p2: expr`.
Corpo com múltiplas expressões ou `begin`: função auxiliar numerada
gerada inline (padrão similar ao `let_named`).

### P9. `let` / `let*` com múltiplas expressões no corpo
Hoje emite `"NÃO SUPORTADO"`. O corpo precisa virar uma função auxiliar
imediatamente invocada, como já é feito no `let_named`.

### P10. `AST_COND`
Traduzir como cadeia `if / elif / else`.
Cada cláusula `(test body)` → `if test:\n body`.
Cláusula `else` → `else:\n body`.
Forma `=>` (recipient): avaliar o teste uma vez, passar para o recipiente —
pode precisar de variável temporária numerada.

### P11. `AST_CASE`
Traduzir como cadeia `if / elif / else` checando pertinência.
`(case key ((d1 d2) body) (else e))` → `if key in (d1, d2):\n body\nelse:\n e`

### P12. `AST_DO`
Laço `while`. Inicializar variáveis antes, atualizar ao fim de cada iteração,
sair quando o teste for verdadeiro e emitir o resultado.

### P13. `AST_LETREC`
Semelhante ao `let`, mas os bindings se referenciam mutuamente.
Em Python, closures já enxergam funções do mesmo escopo, então o padrão
de função auxiliar do `let_named` provavelmente funciona diretamente.

### P14. `AST_QUOTE` / `AST_VECTOR` / `AST_CHARACTER`
Menor prioridade.
- Quote de símbolo → string Python
- Vetor `#(1 2 3)` → lista Python `[1, 2, 3]`
- Caractere `#\a` → string de 1 char `"a"`

### P15. `AST_DELAY`
Menor prioridade. Emular com `lambda: expr` (thunk).
