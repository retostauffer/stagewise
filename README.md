


# `bigmem_vs_ff.R`

```
library("bigmemory")
library("ff")
file <- "_test_nobs1000000_p1000.csv"

message(" -- bigmem")
system.time(x1 <- read.big.matrix(file, skip = 4, type = "double", header = TRUE))

message(" -- ff")
system.time(x2 <- read.table.ffdf(file = file, sep = ",", comment.char = "#", header = TRUE))
```

```
 -- bigmem
   user  system elapsed 
256.019  18.090 276.469 
 -- ff
   user  system elapsed 
482.987  51.439 538.011 
```


# Initial test (minimal)

```
nobs <- 100
p    <- 10
b    <- sdr(formula = f, data = d, CF = TRUE, updating = "bs",
            family = NO, maxit = 300)
```

* 5MB
* 11 seconds

# Test 2

```
nobs <- 10000
p    <- 100
b    <- sdr(formula = f, data = d, CF = TRUE, updating = "bs",
            family = NO, maxit = 300)
```

* 22MB
* 00:01:18

