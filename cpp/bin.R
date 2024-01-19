


Rcpp::sourceCpp("binMat.cpp")
file <- "minimal.csv"
if (file.exists("test.bin")) file.remove("test.bin")

#x <- binMat(file2, skip = 4, header = TRUE)

#####################
x <- binMat(file, skip = 0, header = TRUE)
print(data <- read.csv(file))

print(readBin("test.bin", what = 'double', n = 10))
print(y <- read_binMat("test.bin", nrow(data), ncol(data)))


######
Rcpp::sourceCpp("binMat.cpp")
file2 <- "../_test_nobs100000_p100.csv"
if (file.exists("test.bin")) file.remove("test.bin")
print(system.time(x2 <- binMat(file2, skip = 4, header = TRUE)))
print(x2$dim)
print(system.time(y2 <- read_binMat("test.bin", x2$dim$nrow, x2$dim$ncol)))
