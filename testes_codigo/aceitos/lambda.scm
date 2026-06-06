; Lambda como valor e chamada imediata
(define dobro (lambda (x) (* x 2)))
(define triplo (lambda (x) (* x 3)))

(display (dobro 5))
(display (triplo 4))

; Chamada imediata de lambda (IIFE)
((lambda (x y) (+ x y)) 10 20)
