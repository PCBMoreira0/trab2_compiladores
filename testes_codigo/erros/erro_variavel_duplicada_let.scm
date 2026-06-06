; ERRO SEMANTICO: ligacao duplicada dentro de let
; O analisador semantico deve reportar: "variavel duplicada"

(let ((x 1) (x 2))
  x)
