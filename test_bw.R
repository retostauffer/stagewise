
library("devtools")
load_all("sdr")

## Variable selection with correlation filtering
## and best subset updating.
b <- sdr(formula   = f,
         data      = d,
         CF        = TRUE,
         updating  = "bs",
         family    = NO,
         batch_ids = nobs / 2,   # or list of index vectors
         batch_ids = list(1:1000, 1001:2000),
         maxit     = 10)
