#!/bin/bash

library("sdr")

set.seed(666)
nobs <- 1e6
p    <- 1000
d    <- matrix(runif(nobs * p, -1, 1), ncol = p)

colnames(d) <- paste("x", 1:p, sep = "")
cat("Data drawn, convert to data.frame\n")
d <- as.data.frame(d)

cat("data.frame of dimension", paste(dim(d), collapse = " x "), "\n")

## Create additive predictors.
cat("Calculating eta.mu\n")
d$eta.mu <- d$x1 + 2 * d$x2 + 0.5 * d$x3 - 1*d$x4 
cat("Calculating eta.sigma\n")
d$eta.sigma <- 0.5 * d$x3 + 0.25 * d$x4 -
  0.25 * d$x5  - 0.5 * d$x6 

cat("Calculating response y\n")
d$y <- rNO(nobs, mu = d$eta.mu,
           sigma = exp(d$eta.sigma))

## Model formula.
f <- as.formula(paste("y ~ ", paste0("x", 1:p, collapse = "+")))
f <- list(f, update(f, sigma ~ .))

## Standardize (for quick_ffdf)
cat("Standardizing the data ...\n")
idx <- grep("^x[0-9]+$", names(d))
for (i in idx) d[[i]] <- round((d[[i]] - mean(d[[i]])) / sd(d[[i]]), 5)

## Remove mu and sigma (quick_ffdf)
cat("Removing eta\n")
d$eta.mu <- NULL
d$eta.sigma <- NULL


fformat <- function(x) {
    x <- paste(format(x), collapse = " ")
    gsub("\\s+", " ", x)
}
#fformat(f[[1]])

CSVFILE <- sprintf("_test_nobs%d_p%d.csv", nobs, p)

cat("Creating and writing meta\n")
meta <- c(sprintf("# nobs: %d", nobs),
          sprintf("# p:    %d", p),
          sprintf("# formula_mu: %s",    fformat(f[[1]])),
          sprintf("# formula_sigma: %s", fformat(f[[2]])))
writeLines(paste(meta, collapse = "\n"), CSVFILE)

cat("Writing the data (all at once) ...\n")
write.table(d, CSVFILE, row.names = FALSE,
            sep = ",", dec = ".", append = TRUE)


cat("Job done\n")
