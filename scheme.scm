(let contagem ((n 3))
  (if (= n 0)
      "Fogo!"
      (begin
        (display n)
        (newline)
        (contagem (- n 1)))))