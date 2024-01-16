#!/bin/bash

library("sdr")

set.seed(666)
nobs <- 1e6
p    <- 1e3
#nobs <- 1e6
#p    <- 1e3
d    <- matrix(runif(nobs * p, -1, 1), ncol = p)

colnames(d) <- paste("x", 1:p, sep = "")
d <- round(as.data.frame(d), 5)

## Create additive predictors.
d$eta.mu <- d$x1 + 2 * d$x2 + 0.5 * d$x3 - 1*d$x4 
d$eta.sigma <- 0.5 * d$x3 + 0.25 * d$x4 -
  0.25 * d$x5  - 0.5 * d$x6 

d$y <- rNO(nobs, mu = d$eta.mu,
           sigma = exp(d$eta.sigma))

## Model formula.
f <- as.formula(paste("y ~ ", paste0("x", 1:p, collapse = "+")))
f <- list(f, update(f, sigma ~ .))

## Standardize (for quick_ffdf)
idx <- grep("^x[0-9]+$", names(d))
for (i in idx) d[[i]] <- (d[[i]] - mean(d[[i]])) / sd(d[[i]])
round(sapply(d[, idx], mean), 10)
round(sapply(d[, idx], sd), 3)

## Remove mu and sigma (quick_ffdf)
d$eta.mu <- NULL
d$eta.sigma <- NULL


fformat <- function(x) {
    x <- paste(format(x), collapse = " ")
    gsub("\\s+", " ", x)
}
#fformat(f[[1]])

meta <- c(sprintf("# nobs: %d", nobs),
          sprintf("# p:    %d", p),
          sprintf("# formula_mu: %s",    fformat(f[[1]])),
          sprintf("# formula_sigma: %s", fformat(f[[2]])))

CSVFILE <- sprintf("_test_nobs%d_p%d.csv", nobs, p)
writeLines(paste(meta, collapse = "\n"), CSVFILE)
write.table(d, CSVFILE, row.names = FALSE,
            sep = ",", dec = ".", append = TRUE)

