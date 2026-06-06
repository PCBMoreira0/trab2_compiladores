; Fatorial recursivo
(define (fatorial n)
  (if (= n 0)
      1
      (* n (fatorial (- n 1)))))

(display (fatorial 0))
(display (fatorial 1))
(display (fatorial 5))
(display (fatorial 10))
