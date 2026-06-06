; Laco do: soma de 1 a N
(define (soma-ate n)
  (do ((i 1 (+ i 1))
       (acc 0 (+ acc i)))
      ((> i n) acc)))

(display (soma-ate 5))
(display (soma-ate 10))
(display (soma-ate 100))
