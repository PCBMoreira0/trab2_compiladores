(let loop ((i 1))
  (if (> i 5)
      'fim
      (begin
        (display i)
        (newline)
        (loop (+ i 1)))))