; case: selecao por valor
(define (dia-da-semana n)
  (case n
    ((1) "segunda")
    ((2) "terca")
    ((3) "quarta")
    ((4) "quinta")
    ((5) "sexta")
    (else "fim de semana")))

(display (dia-da-semana 1))
(display (dia-da-semana 3))
(display (dia-da-semana 6))
