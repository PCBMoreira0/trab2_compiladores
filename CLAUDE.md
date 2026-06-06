# Transpilador Scheme → Python

Projeto acadêmico (UFF - Compiladores). Transpila um subconjunto de Scheme para Python.
Pipeline: Flex/Bison → AST em C → análise semântica → tradução para Python.

## Arquivos principais

- `bottom_up/scheme.l` / `scheme.y` — lexer e parser (Flex/Bison)
- `libs/scheme_ast.h` / `scheme_ast.c` — definição e construção da AST
- `libs/symbol_tab.h` / `symbol_tab.c` — tabela de símbolos e escopos
- `libs/translator.h` / `translator.c` — fase de tradução (foco atual)
- `libs/merge_file.c` — mescla `py_defs.py` + `py_code.py` → `python.py`
- `testes_codigo/aceitos.scm` — casos de teste válidos
- `testes_codigo/erros.scm` — casos de teste com erros semânticos

## Saída da tradução

A tradução gera dois arquivos intermediários que são mesclados em `python.py`:
- `py_defs.py` — funções auxiliares geradas (if_block_N, named let, etc.)
- `py_code.py` — código principal

## O que está traduzido

| Scheme | Python |
|---|---|
| `(define x val)` | `x = val` |
| `(define (f p1 p2) body)` | `def f(p1, p2): ... return ...` |
| `(if cond then else)` — ramos simples | ternário `then if cond else else` |
| `(if cond (begin ...) else)` — ramos com `begin` | função auxiliar `if_block_N()` em `py_defs.py` |
| `(let ((a v)) expr)` — corpo de 1 expressão | `(lambda a: expr)(v)` |
| `(let* ((a v1) (b v2)) expr)` — corpo de 1 expressão | lambdas aninhados |
| `(let nome ((a v)) body)` — named let | função auxiliar `nome_N()` em `py_defs.py` |
| `(proc arg1 arg2)` | `proc(arg1, arg2)` |
| `(+ a b)`, `(- ...)`, `(* ...)`, `(/ ...)`, comparadores | operadores infix |
| Builtins: `display`, `write`, `newline`, `expt`, `abs`, `min`, `max`, `length` | mapeados para Python |

## Limitações e bugs conhecidos

### Não implementados (caem no `default: break` sem gerar código)
- `AST_LAMBDA` — `(lambda (x) ...)` como expressão
- `AST_SET` — `(set! x val)`
- `AST_COND` / `AST_COND_CLAUSE` — `(cond ...)`
- `AST_CASE` / `AST_CASE_CLAUSE` — `(case ...)`
- `AST_AND` / `AST_OR` — `(and ...)` / `(or ...)`
- `AST_LETREC` — `(letrec ...)`
- `AST_BEGIN` — `(begin ...)` isolado
- `AST_DO` — laço `(do ...)`
- `AST_DELAY` — `(delay ...)`
- `AST_QUOTE` — `'a`, `(quote ...)`
- `AST_VECTOR` — `#(1 2 3)`
- `AST_BOOLEAN` — `#t` / `#f`
- `AST_CHARACTER` — `#\a`

### Bugs no que está implementado

**`if_statement` (translator.c:267)** — desreferencia `else_branch->type` sem checar NULL primeiro. `(if #t 10)` sem ramo else crasha.

**`if_block` (translator.c:252)** — o ramo `else` sempre assume que `else_branch` é `AST_LIST` (usa `.list.item`), mas quando o ramo else é um nó simples (número, variável) o comportamento é incorreto.

**`let`/`let*` com múltiplas expressões no corpo (translator.c:83,130)** — emite a string literal `"NÃO SUPORTADO"` no output.

**`call` com operandos complexos (translator.c:206)** — usa `i->list.item->value_str` diretamente em vez de chamar `translate()`. Chamadas aninhadas como `(f (g x))` quebram.

**`named let` (translator.c:144)** — bug de memória: `name` é ponteiro não inicializado ao qual se atribui `value_str`.

**`call_operator` com mais de 2 operandos (translator.c:179)** — também usa `value_str` diretamente, falhando para sub-expressões.
