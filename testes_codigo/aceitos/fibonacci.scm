; Fibonacci recursivo
(define (fib n)
  (if (< n 2)
      n
      (+ (fib (- n 1)) (fib (- n 2)))))

(display (fib 0))
(display (fib 1))
(display (fib 7))
(display (fib 10))
