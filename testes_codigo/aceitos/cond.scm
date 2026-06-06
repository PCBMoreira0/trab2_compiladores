; Classificacao com cond
(define (classifica n)
  (cond
    ((< n 0)  "negativo")
    ((= n 0)  "zero")
    ((< n 10) "pequeno")
    (else     "grande")))

(display (classifica -5))
(display (classifica 0))
(display (classifica 7))
(display (classifica 100))
