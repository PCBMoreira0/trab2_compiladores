; Fibonacci eficiente com named let (iterativo)
(define (fib-iter n)
  (let loop ((i n) (a 0) (b 1))
    (if (= i 0)
        a
        (loop (- i 1) b (+ a b)))))

(display (fib-iter 0))
(display (fib-iter 1))
(display (fib-iter 10))
(display (fib-iter 20))
