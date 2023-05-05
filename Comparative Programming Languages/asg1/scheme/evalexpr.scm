#!/afs/cats.ucsc.edu/courses/cmps112-wm/usr/racket/bin/mzscheme -qr
;; $Id: evalexpr.scm,v 1.3 2019-01-16 13:54:30-08 - - $

;; Example showing how to evaluate an expression.

(define fnhash (make-hash))
(for-each
    (lambda (item) (hash-set! fnhash (car item) (cadr item)))
    `((+ ,+)
      (- ,-)
      (* ,*)
      (/ ,/)))

(define (evalexpr expr)
    (cond ((number? expr) (+ 0.0 expr))
          (else (let ((fn (hash-ref fnhash (car expr)))
                      (args (map evalexpr (cdr expr))))
                     (apply fn args)))))

(define example '(+ (* 2 3) (* 4 5)))

(printf "expr = ~s~n" example)
(printf "value = ~s~n" (evalexpr example))

