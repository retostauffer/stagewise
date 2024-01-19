


Rcpp::sourceCpp("binMat.cpp")
file <- "minimal.csv"
file2 <- "../_test_nobs100000_p100.csv"
if (file.exists("test.bin")) file.remove("test.bin")

x <- binMat(file2, skip = 4, header = TRUE)
print(readBin("test.bin", what = 'double', n = 10))
print(read.csv(file))

