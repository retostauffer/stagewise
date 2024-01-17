


Rcpp::sourceCpp("retoMat.cpp")
#file <- "../_test_nobs1000000_p1000.csv"
file <- "../_test_nobs100000_p100.csv"

system.time(res <- retoMat(file, 4))
print(str(res))

#system.time(res <- retoMat(file, 5, header = FALSE))

system.time(m <- retoMatMean(res))
print(str(m))
