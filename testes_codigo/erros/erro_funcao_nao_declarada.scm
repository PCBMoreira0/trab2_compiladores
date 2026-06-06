; ERRO SEMANTICO: chamada de funcao nao declarada
; O analisador semantico deve reportar: "variavel nao declarada"

(define resultado (funcao-inexistente 10 20))
(display resultado)
