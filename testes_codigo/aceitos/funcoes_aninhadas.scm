; Funcoes que chamam outras funcoes
(define (quadrado x)
  (* x x))

(define (cubo x)
  (* x (quadrado x)))

(define (soma-quadrados a b)
  (+ (quadrado a) (quadrado b)))

(display (quadrado 5))
(display (cubo 3))
(display (soma-quadrados 3 4))
