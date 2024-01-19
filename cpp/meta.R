
Rcpp::sourceCpp("binMat.cpp")

cat("\n\n\n")
cat(" ------------ demo foo file -----------------\n")
cat("Parsing file via binmat\n")
file4 <- "foo.csv"
write.csv(as.data.frame(matrix(1:35, ncol = 5, byrow = TRUE,
                     dimnames = list(NULL, LETTERS[1:5]))),
              row.names = FALSE, file = file4)
print(system.time(x2 <- create_binmm(file4, "_foo.bin", skip = 0, header = TRUE)))
system.time(y2 <- meta_binmm("_foo.bin", x2$dim$nrow, x2$dim$ncol))
print(y2)


##cat("\n\n\n")
##cat(" ------------ demo foo file -----------------\n")
##cat("Parsing file via binmat\n")
##file4 <- "foo.csv"
##write.table(as.data.frame(matrix(1:35, ncol = 5, byrow = TRUE,
##                     dimnames = list(NULL, LETTERS[1:5]))),
##            sep = ",",
##             col.names = FALSE, row.names = FALSE, file = file4)
##print(read.csv(file4))
##print(dim(read.csv(file4)))
##print(system.time(x2 <- binMat(file4, skip = 0, header = FALSE)))
##print(x2$dim)
##print(x2$colnames)
##system.time(y2 <- read_binMatFull("test.bin", x2$dim$nrow, x2$dim$ncol))
##print(y2$data)
