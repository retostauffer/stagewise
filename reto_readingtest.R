
library("argparse")

parser <- ArgumentParser("Allows console args for testing")
parser$add_argument("-b", "--batchsize", type = "integer", default = 100,
                    help = "Size of the batch; if < 100 it takes nobs / batchsize!")
parser$add_argument("-m", "--maxit", type = "integer", default = 100,
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

cat("Using binmm. Deleting binary file if it already exists for testing purposes!")
(binfile <- paste("_readingtest_", gsub("\\.\\w{0,3}$", "", csvfile), "binmm", sep = "."))
if (file.exists(binfile)) file.remove(binfile)

cat("\n\nCreating binmm first ... measuring time\n")
t <- Sys.time()
data <- read.binmm(csvfile, binfile = binfile, skip = 4)
t <- as.numeric(Sys.time() - t, units = "mins")
cat("\n\nCreating the binary binmm file took ", round(t, 2), " minutes\n")

# Drawing data
cat("\n\nDrawing", args$maxit, "batches with random", args$batchsize, "rows all", args$p, "cols\n", sep = " ")
t <- Sys.time()
for (i in seq_len(args$maxit)) {
    dead_end <- data[sample(1:nrow(data), args$batchsize), ]
}
t <- as.numeric(Sys.time() - t, units = "mins")
cat("\nThis reading task took me ", round(t, 2), " minutes\n")
