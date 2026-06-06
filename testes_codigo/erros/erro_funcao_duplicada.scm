; ERRO SEMANTICO: redeclaracao de funcao no mesmo escopo
; O analisador semantico deve reportar: "identificador duplicado"

(define (soma a b) (+ a b))
(define (soma x y) (- x y))
