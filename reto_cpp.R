
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

args <- parser$parse_args()
if (is.null(args$batchsize)) {
    args$print_help()
    stop("-b/--batchsize missing")
} else if (args$batchsize <= 0) {
    stop("batchsize must be positive")
} else if (args$batchsize <= 100) {
    args$batchsize <- floor(args$nobs / args$batchsize)
}
if (args$maxit < 5) stop("-m/--maxit must be 5 or larger")
print(str(args))

# Define name of CSV file we will use; will throw an error
# if not existing.
csvfile <- sprintf("_test_nobs%d_p%d.csv", args$nobs, args$p)
cat("Trying to use file ", csvfile, "\n")
stopifnot("file does not exist" = file.exists(csvfile))

# Re-loading the package (for development purposes)
library("devtools")
load_all("sdr")

cat("Using read.retoMat to connect to file\n")
t <- Sys.time()
data <- read.retoMat(csvfile, skip = 4)
t <- as.numeric(Sys.time() - t, units = "mins")
cat("\n\nInitial reading (retoMat) took ", round(t, 2), " minutes\n")

print(data[1:5, 1:5])

# Building formula
cat("Building formula now ...\n")
vars <- data$colnames[!data$colnames == "y"]
f <- list(paste0("y ~", paste(vars, collapse = " + ")),
          paste0("sigma ~", paste(vars, collapse = " + ")))
print(f)

## Variable selection with correlation filtering
## and best subset updating.
t <- Sys.time()
set.seed(666)
library("peakRAM")
cat("Calling sdr now ...\n")
pk <- peakRAM({
    b <- sdr(formula   = f,
             data      = data,
             CF        = TRUE,
             updating  = "bs",
             family    = NO,
             batch_ids = args$batchsize,
             maxit     = args$maxit)
})
t <- as.numeric(Sys.time() - t, units = "mins")
cat("\n\nFull estimation took me ", round(t, 2), " minutes in total\n")

cat("\n\n")
print(pk)



