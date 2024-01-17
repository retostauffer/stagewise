


Rcpp::sourceCpp("retoMat.cpp")
file <- "../_test_nobs100000_p100.csv"

system.time(res <- retoMat(file, 4))
print(str(res))

#system.time(res <- retoMat(file, 5, header = FALSE))
