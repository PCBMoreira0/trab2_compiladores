;  CODIGOS ERRADOS QUE O PARSER DEVE PEGAR  
; ---------------------------------------------------------------------
;  Cada linha abaixo eh um codigo Scheme SINTATICAMENTE INVALIDO. Um
;  parser correto tambem rejeitaria todos eles -- ou seja, o erro
;  apontado pelo nosso parser esta certo, nao eh artefato de conflito.
;
;   o parser para no PRIMEIRO erro (nao ha recuperacao de erro).
;  Por isso cada linha deve ser testada SEPARADAMENTE. Comando pronto
;  para rodar uma linha de cada vez e ver o erro de cada uma:


; --- define mal formado ---
(define)
(define x)
(define 5 10)

; --- if com numero errado de partes ---
(if)
(if #t 1 2 3)

; --- set! com alvo invalido ou incompleto ---
(set!)
(set! x)
(set! 5 x)

; --- formas especiais incompletas ---
(cond)
(let)
(lambda)
(and

; --- parenteses desbalanceados ---
(+ 1 2
(+ 1 2))
)


