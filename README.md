Lisp-language interpreter with support of:

Basic functions:
```scheme
$ (quote 1)
> 1

$ (quote (+ 1 2))
> (+ 1 2)
```

Arithmetic functions:
```scheme
$ (+ 1 2 3)
> 6

$ (max 1 3 2)
> 3
```

Boolean functions:
```scheme
$ (<= 1 2 3)
> true

$ (and (= 2 2) (> 2 1))
> true
```

Variable definition:
```scheme
$ (define x (+ 1 2))
$ x
> 3

$ (define x 1)
$ set! x 10
$ x
> 10
$ set! y 10
> No such variable 'y'
```

Conditional operator:
```scheme
$ (if (= 2 2) (+ 1 10) 5)
> 11

$ (define x 1)
$ (if (<= 1 2 1) (set! x 30) (set! x 20))
$ x 
> 20
```

Lamda-functions:
```scheme
$ ((lambda (x) (+ 1 x)) 5)
> 6

$ (define test (lambda (x) (set! x (* x 2)) (+ 1 x)))
$ (test 20)
> 41

$ (define range
    (lambda (x)
      (lambda ()
        (set! x (+ x 1))
        x)))

$ (define my-range (range 10))

$ (my-range)
> 11

$ (my-range)
> 12
```
