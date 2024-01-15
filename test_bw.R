
library("devtools")
load_all("sdr")

source("testfunctions.R")
nobs <- 1e5
p    <- 1e2
xxx <- get_testdata(nobs, p, ff = TRUE)

print(class(xxx$data))

## Variable selection with correlation filtering
## and best subset updating.
set.seed(666)
b <- sdr(formula   = xxx$formula,
         data      = xxx$data,
         CF        = TRUE,
         updating  = "bs",
         family    = NO,
         batch_ids = nobs / 2,   # or list of index vectors
         #batch_ids = list(1:1000, 1001:2000),
         maxit     = 10)
