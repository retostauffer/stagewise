


Rcpp::sourceCpp("binMat.cpp")
#file <- "minimal.csv"
#if (file.exists("test.bin")) file.remove("test.bin")
#
##x <- binMat(file2, skip = 4, header = TRUE)
#
######################
#x <- binMat(file, skip = 0, header = TRUE)
#print(data <- read.csv(file))
#
#print(readBin("test.bin", what = 'double', n = 10))
#print(y <- read_binMat("test.bin", nrow(data), ncol(data)))


######
cat(" --------------------------------- ");
Rcpp::sourceCpp("binMat.cpp")
file2 <- "../_test_nobs100000_p100.csv"

if (file.exists("test.bin")) file.remove("test.bin")
cat("Parsing file via binmat\n")
print(system.time(x2 <- binMat(file2, skip = 4, header = TRUE)))
print(x2$dim)
cat("Reading data\n");
nr = x2$dim$nrow
nc = x2$dim$ncol
cat("Want to read ", nr, " x ", nc, "\n")
print(system.time(y2 <- read_binMatFull("test.bin", nr, nc)))
print(dim(y2$data))
print(head(y2$data, n = 2))
print(tail(y2$data, n = 2))

cat("\n\n\n")
cat(" ------------ demo foo file -----------------\n")
cat("Parsing file via binmat\n")
file4 <- "foo.csv"
write.csv(as.data.frame(matrix(1:35, ncol = 5, byrow = TRUE,
                     dimnames = list(NULL, LETTERS[1:5]))),
              row.names = FALSE, file = file4)
print(read.csv(file4))
print(dim(read.csv(file4)))
print(system.time(x2 <- binMat(file4, skip = 0, header = TRUE)))
print(x2$dim)
system.time(y2 <- read_binMatFull("test.bin", x2$dim$nrow, x2$dim$ncol))
print(y2$data)


cat("\n\n\n")
cat(" ------------ demo foo file -----------------\n")
cat("Parsing file via binmat\n")
file4 <- "foo.csv"
write.table(as.data.frame(matrix(1:35, ncol = 5, byrow = TRUE,
                     dimnames = list(NULL, LETTERS[1:5]))),
            sep = ",",
             col.names = FALSE, row.names = FALSE, file = file4)
print(read.csv(file4))
print(dim(read.csv(file4)))
print(system.time(x2 <- binMat(file4, skip = 0, header = FALSE)))
print(x2$dim)
print(x2$colnames)
system.time(y2 <- read_binMatFull("test.bin", x2$dim$nrow, x2$dim$ncol))
print(y2$data)
