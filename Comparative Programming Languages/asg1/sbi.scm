#!/afs/cats.ucsc.edu/courses/cmps112-wm/usr/racket/bin/mzscheme -qr
;; $Id: sbi.scm,v 1.10 2019-01-15 14:10:54-08 - - $
;;
;; NAME
;;    sbi.scm - silly basic interpreter
;;
;; SYNOPSIS
;;    sbi.scm filename.sbir
;;
;; DESCRIPTION
;;    The file mentioned in argv[1] is read and assumed to be an SBIR
;;    program, which is the executed.  Currently it is only printed.
;;

(define *stdin* (current-input-port))
(define *stdout* (current-output-port))
(define *stderr* (current-error-port))

(define *run-file*
    (let-values
        (((dirpath basepath root?)
            (split-path (find-system-path 'run-file))))
        (path->string basepath))
)

(define (die list)
    (for-each (lambda (item) (display item *stderr*)) list)
    (newline *stderr*)
    (exit 1)
)

(define (usage-exit)
    (die `("Usage: " ,*run-file* " filename"))
)

(define (readlist-from-inputfile filename)
    (let ((inputfile (open-input-file filename)))
         (if (not (input-port? inputfile))
             (die `(,*run-file* ": " ,filename ": open failed"))
             (let ((program (read inputfile)))
                  (close-input-port inputfile)
                         program))))

(define (dump-stdin)
    (let ((token (read)))
         (printf "token=~a~n" token)
         (when (not (eq? token eof)) (dump-stdin))))


(define (write-program-by-line filename program)
    (printf "==================================================~n")
    (printf "~a: ~s~n" *run-file* filename)
    (printf "==================================================~n")
    (printf "(~n")
    (for-each (lambda (line) (printf "~s~n" line)) program)
    (printf ")~n"))





;; Table definitions:
;; Function, Variable, Array, and Labels
(define function-table (make-hash))
(define variable-table (make-hash))
(define array-table (make-hash))
(define label-table (make-hash))
;; For every line inside program, check if 
;;there is a label and add it to the Label Table
;; If the cdr is not null but the cadr is a symbol?
;; then it is a label. A statement is always a pair?
;; ^professor's comment on piazza. Thus if cdr is null, do nothing, 
;; if cadr is a pair, do nothing. Else, insert into table
(define (populate-label-table program)
    (for-each (lambda (line)
        (cond 
            [(null? (cdr line))
              (void)]
            [(pair? (cadr line))
              (void)]
            [else 
              (hash-set! label-table (cadr line) line)
            ]
        )
        )program);;end for each
);; end populate-label-table


;;intialize function-table
(for-each 
    (lambda (element) 
        (hash-set! function-table (car element) (cadr element)))
        `( 
            (sqrt_2   1.414213562373095048801688724209698078569671875)
            (div     ,(lambda (x y) (floor (/ x y))))
            (mod     ,(lambda (x y) (- x (* (div x y) y))))
            (quot    ,(lambda (x y) (truncate (/ x y))))
            (rem     ,(lambda (x y) (- x (* (quot x y) y))))
            (+       ,+)
            (-       , -) 
            (*       , *)
            (/       ,(lambda (x y) (/ x  y )))
            (%       ,(lambda (x y) (- (+ x 0.0) )))
            (^       ,expt)
            (=       ,equal?)
            (<       ,<)
            (>       ,>)
            (<>      ,(lambda (x y) (not (equal? x y))))
            (>=      ,>=)
            (<=      ,<=)
            (abs     ,abs)
            (acos    ,acos)
            (asin    ,asin)
            (atan    ,atan)
            (ceil    ,ceiling)
            (cos     ,cos)
            (exp     ,exp)
            (floor   ,floor)
            (log     ,log)
            (log10   (lambda (x) (/ (log x) (log 10.0))))
            (log2     0.301029995663981195213738894724493026768189881)
            (sqrt    ,sqrt)            
            (round   ,round)
            (sin     ,sin)
            (tan     ,tan)
            (trunc   ,truncate)

         );;end list
);;end for-each

;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////


;;interprets statements that are found in the hashtable
(define (interpret-statement statement) 
    ;;(printf "interpret~n")
    (cond
        [(eqv? 'dim (car statement)) 
            (interpret-dim (car (cadr statement)) 
                (cadr (cadr statement)))]
                
        [(eqv? 'let (car statement))
            (interpret-let (cadr statement) 
                (caddr statement))]
              
        [(eqv? 'goto (car statement))
            (interpret-goto (cadr statement))]

        [(eqv? 'if (car statement))
            (interpret-if (cadr statement) 
                (caddr statement))]
        
        [(eqv? 'print (car statement))
            (interprint (cdr statement))]
        
        [(eqv? 'input (car statement))
            (interpret-input (cdr statement))]
        [else (die '("Error statement"))]
    );;end cond                
);;end interpret-statement


;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////

;;evaluates expressions. If the expression is not 
;;found in the function or variable table it returns an error.
(define (evaluate-expression expr)
    (cond 

        [(number? expr) (+ expr 0.0)]
        
        [(symbol? expr) (+ (hash-ref variable-table expr 0) 0.0)]
            
        [(pair? expr)
            (if (hash-has-key? function-table (car expr))
                (+ (apply (hash-ref function-table (car expr)) 
                    (map evaluate-expression (cdr expr))) 0.0)
                ;;else
                (die '("Error eval"))
            );;end if
        ];;end pair?
    );;end cond
);;end define


;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////           


;;sets or updates values in variable or array tables
(define (interpret-let var expr)
    ;;(printf "let~n")
        (hash-set! variable-table var 
            (evaluate-expression expr))
);;end define
    
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////

;;goto label
(define (interpret-goto label)
    (if (hash-has-key? label-table label)
        (interpret-prog (hash-ref label-table label))
    (die '("Error with interpret goto"))
    );;end if
);;end define

;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////

;;inteprets if statements
(define (interpret-if condition label)
        (when ((hash-ref function-table (car condition)) 
            (evaluate-expression (cadr condition)) 
            (evaluate-expression (caddr condition)))
            (interpret-goto label)
        );;end when
);;end define
      
;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////

(define (interprint expr) 
    ;;(printf "printing~n")
    (if (null? expr) (newline)
    
        (begin
            ;; if its a string, we print it as a string. 
            ;; else we evaluate it and then print.
            (if (string? (car expr))
                (display (car expr))
                ( display (evaluate-expression (car expr))))
            ;; if the next isnt null, recursively call print on it. 
            ;; else print a new line.
            (if (not (null? (cdr expr)))
                    (interprint (cdr expr))(newline)
                )
            ;;end begin
        );;end else
    );;end if
);;end define

;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////

;;iterates through lines of program and calls interpret-statement
(define (interpret-program line-num program)
    (when (< line-num (length program))
        (let ((line (list-ref program line-num)))
                ;;(printf ":)~n")
                ;; if length 1, do nothing. 3 call interpret on caddr
                ;; else length 2 call on cadr.
                (cond 
                    [(= (length line) 1)(void)]
                    [(= (length line) 3)(interpret-statement 
                        (caddr line))]
                    [else(interpret-statement (cadr line))]
                    ;;end if
                )
            ;;exits with 0 if eof, else recursively calls
            ;; interpret-program with line-num+1 
            ;;(printf "after cond :)~n")
            (if (null? (cdr program)) (exit 0)
            (interpret-program (+ line-num 1) program)
            
            )
        );;end let
        
    );;end when
    ;;);;end if

);;end define

;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////


(define (main arglist)
    (if (or (null? arglist) (not (null? (cdr arglist))))
        (usage-exit)
        (let* ((sbprogfile (car arglist))
               (program (readlist-from-inputfile sbprogfile)))
            ;; finds labels
            (populate-label-table program)
            ;; interprets the program
            (interpret-program 0 program)))
    )

;;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
;;///////////////////////////////////////////////

;;(printf "terminal-port? *stdin* = ~s~n" (terminal-port? *stdin*))
(if (terminal-port? *stdin*)
    (main (vector->list (current-command-line-arguments)))
    (printf "sbi.scm: interactive mode~n"))


