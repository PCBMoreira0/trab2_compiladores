; Maximo e minimo com builtins e com funcao propria
(define (meu-max a b)
  (if (> a b) a b))

(define (meu-min a b)
  (if (< a b) a b))

(display (meu-max 10 20))
(display (meu-min 10 20))
(display (max 5 3 8 1))
(display (min 5 3 8 1))
