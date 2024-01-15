
library("argparse")

parser <- ArgumentParser("Allows console args for testing")
parser$add_argument("-b", "--batchsize", type = "integer", default = 100,
                    help = "Size of the batch; if < 100 it takes nobs / batchsize!")
parser$add_argument("-m", "--maxit", type = "integer", default = 10,
                    help = "Number of iterations, defaults to 10")
parser$add_argument("-n", "--nobs", type = "integer", default = 1e5,
                    help = "Number of observations, defaults to 1e5")
parser$add_argument("-p", type = "integer", default = 1e2,
                    help = "Number of parameters (covariates), defaults to 1e2")
parser$add_argument("--ff", action = "store_true",
                    help = "Should ff be used (ffdf instead of data.frame)?")

args <- parser$parse_args()
print(args)
if (is.null(args$batchsize)) {
    args$print_help()
    stop("-b/--batchsize missing")
} else if (args$batchsize <= 0) {
    stop("batchsize must be positive")
} else if (args$batchsize < 100) {
    args$batchsize <- args$nobs / args$batchsize
}
if (args$maxit < 5) stop("-m/--maxit must be 5 or larger")


# Re-loading the package (for development purposes)
message("Loading sdr package via devtools")
library("devtools")
load_all("sdr")

message("Sourcing testfunctions (to load test data)")
source("testfunctions.R")
xxx <- get_testdata(args$nobs, args$p, args$ff)

## Variable selection with correlation filtering
## and best subset updating.
t <- Sys.time()
set.seed(666)
#library("peakRAM")
#pk <- peakRAM({
    b <- sdr(formula   = xxx$formula,
             data      = xxx$data,
             CF        = TRUE,
             updating  = "bs",
             family    = NO,
             batch_ids = args$batchsize,
             maxit     = args$maxit)
#})
t <- as.numeric(Sys.time() - t, units = "mins")
message("\n\nFull estimation took me ", round(t, 2), " minutes in total")

cat("\n\n")
print(pk)



