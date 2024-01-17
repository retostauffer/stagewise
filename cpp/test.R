


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
d <- data.frame(x = rnorm(N, 10, 5),
                y = rnorm(N, 52, 13))
write.csv(d, file = file1, row.names = FALSE)


############################
Rcpp::sourceCpp("retoMat.cpp")
system.time(res2 <- retoMat(file3, 0, TRUE, TRUE, ","))
print(str(res2))
print(res2$scale)
