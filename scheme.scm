(define (compute a b)
  (if (> a b)
      (* a (+ b 1))
      (* b (+ a 1))))

(define (process x)
  (+ (compute x 3)
     (compute 5 x))

(process 4)