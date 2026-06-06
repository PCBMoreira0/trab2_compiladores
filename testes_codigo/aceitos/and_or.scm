; Operadores logicos and e or
(define (entre? x lo hi)
  (and (>= x lo) (<= x hi)))

(define (fora? x lo hi)
  (or (< x lo) (> x hi)))

(display (entre? 5 1 10))
(display (entre? 0 1 10))
(display (fora? 15 1 10))
(display (fora? 5 1 10))
