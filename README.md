

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

