; Maximo divisor comum (algoritmo de Euclides subtrativo)
(define (mdc a b)
  (cond
    ((= b 0) a)
    ((> a b) (mdc (- a b) b))
    (else    (mdc a (- b a)))))

(display (mdc 12 8))
(display (mdc 100 75))
(display (mdc 17 5))
