; Exemplos de let e let*
(define resultado1
  (let ((x 5) (y 3))
    (+ x y)))

(define resultado2
  (let* ((x 5) (y (* x 2)))
    (+ x y)))

(display resultado1)
(display resultado2)

; let com expressao no valor da ligacao
(let ((lado (* 4 3)))
  lado)
