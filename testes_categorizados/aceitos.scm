
;  CODIGOS QUE O PARSER ACEITA  (Scheme valido, sem erros)

; ---- Definicao de variavel e atribuicao ----
(define limite 100)
(set! limite 200)
(set! x (+ 1 2))

; ---- Chamadas de procedimento (simples, aninhadas, sem operandos) ----
(iniciar)
(+ 10 20 limite)
(f (g (h x)))
(* (+ 1 2) (- 5 3) (/ 8 4))

; ---- Definicao com expressao composta no valor ----
(define total (* (+ 1 2) (- 5 3)))

; ---- Condicional if: completo, sem alternate, e aninhado ----
(if (> limite 50) "maior" "menor")
(if #t 10)
(if (and (> x 0) (< x 10)) (f x) (g x))
(set! r (if #t (+ 1 2) (- 3 4)))
(if (if #t #f #t) (a) (b))

; ---- and / or, inclusive profundamente aninhados ----
(and #t #f)
(or #t #f)
(and (or #t #f) (or #f #t) (not #f))
(or (and #t (or #f #t)) (and #f #t))

; ---- cond com a flecha => (forma (<test> => <recipient>)) ----
(cond ((testa) => processa))
(cond ((= x 1) => f) ((= x 2) => g) ((= x 3) => h))

; ---- do (iteracao): um e varios iteration-specs ----
(do ((i 0 (+ i 1))) ((= i 10) i) (print i))
(do ((i 0 (+ i 1)) (acc 0 (+ acc i))) ((= i 5) acc))

; ---- delay, com expressao composta dentro ----
(delay (+ 10 20))
(delay (if #t (soma a b) (sub a b)))

; ---- Quotation: forma ', forma (quote ...), aninhada, e vetor quotado ----
(quote a)
'a
''a
'#(1 2 3)

; ---- let / let* cujo corpo eh um atomo (variavel) ----
(let ((a 1)) a)
(let ((x (+ 1 2))) x)
(let* ((x 1) (y 2)) y)

; ---- Literais auto-avaliaveis ----
"texto"
#t
#f
42
#\a
