


Rcpp::sourceCpp("retoMat.cpp")
file1 <- "../_test_nobs1000000_p1000.csv"
file2 <- "../_test_nobs100000_p100.csv"
file3 <- "minimal.csv"

#system.time(res <- retoMat(file, 4))
#print(str(res))
#
##system.time(res <- retoMat(file, 5, header = FALSE))
#
#system.time(m <- retoMatMean(res))
#print(str(m))

# Create minimal
N <- 100000
N <- 10
d <- data.frame(x = rnorm(N, 10, 5),
                y = rnorm(N, 52, 13),
                foo = rnorm(N, -2, 3),
                bar = rnorm(N, -100, 0.2))
d <- data.frame(x   = sample(1:99, N), 
                y   = sample(1:99 + 100, N), 
                foo = sample(1:99 + 200, N), 
                bar = sample(1:99 + 300, N))
write.csv(d, file = file3, row.names = FALSE)


############################
# sourcing retoMat.cpp
Rcpp::sourceCpp("retoMat.cpp")
cat("sourced cpp, continue ...\n")
system.time(rmt <- retoMat(file3, 0, TRUE, ",", TRUE))

f <- function(...) source("retoMat.R") # Methods

Rcpp::sourceCpp("retoMat.cpp"); source("retoMat.R")
print(rmt[1:10, ])
print(rmt[1:10, , standardize = TRUE])

xx <- rmt[1:10, , standardize = TRUE]
print(round(apply(xx, 2, mean), 2))
print(round(apply(xx, 2, sd), 2))


rmt2 <- retoMat(file2, 4)
xx <- rmt2[1:10000, , standardize = TRUE]
print(round(apply(xx, 2, mean), 2))
print(round(apply(xx, 2, sd), 2))

stop(" ------------- ")

f(); print(rmt, n = 1000)
f(); head(rmt, n = 1000)
f(); tail(rmt, n = 3)

cat(" ---- cpp\n")
ii <- 3:2
jj <- c("foo","x")
f(); print(xx <- rmt[ii, jj])
cat(" ---- R\n")
print(d[3:2, c("foo", "x")])


############################
library("arrow")

a <- open_dataset(file3, format = "csv")
as.data.frame(a[ii, jj])
rmt[ii, jj]





