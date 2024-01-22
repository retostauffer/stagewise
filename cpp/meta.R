
Rcpp::sourceCpp("binMat.cpp")

cat("\n\n\n")
cat(" ------------ demo foo file -----------------\n")
cat("Parsing file via binmat\n")
file4 <- "foo.csv"

cnames <- c("A", "retostauffer", paste("X", sample(1:500, 3), sep = ""))
set.seed(666)
vals   <- sample(1:1000, 35) / 10
write.csv(as.data.frame(matrix(vals, ncol = 5, byrow = TRUE,
                     dimnames = list(NULL, cnames))),
              row.names = FALSE, file = file4)
data <- read.csv(file4)
print(system.time(x2 <- create_binmm(file4, "_foo.bin", skip = 0, header = TRUE)))

cat("\n\n ---------- read -----------------\n\n")

ii <- c(2, 5)
jj <- c(2, 4)
print(data[ii, jj])

system.time(y2 <- subset_binmm("_foo.bin", x2$dim$nrow, x2$dim$ncol,
                               as.integer(ii) - 1L, as.integer(jj) - 1L))
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
