


get_testdata <- function(nobs, p, ff = FALSE, bigmem = FALSE) {
    stopifnot(is.numeric(nobs), is.numeric(p))
    stopifnot(isTRUE(ff) | isFALSE(ff))
    stopifnot(isTRUE(bigmem) | isFALSE(bigmem))
    nobs <- as.integer(nobs)[1]
    p    <- as.integer(p)[1]
    file <- sprintf("_test_nobs%d_p%d.csv", nobs, p)
    cat("Reading ", file, "\n")
    stopifnot(file.exists(file))

    if (ff & bigmem) warning("ff and bigmem are set TRUE, using bigmem")

    if (bigmem) {
        require("bigmemory")
        d <- read.big.matrix(file, header = TRUE, skip = 4, type = "double")
    } else if (ff) {
        require("ff")
        d <- read.table.ffdf(file = file, comment.char = "#", sep = ",", header = TRUE)
    } else {
        d <- read.csv(file, comment.char = "#")
    }
    # Reading meta
    x <- read.table(file, nrow = 4, sep = ":", comment.char = "")
    f <- list(as.formula(x[3, 2]), as.formula(x[4, 2]))
    return(list(data = d, formula = f))

}
#tmp <- get_testdata(100000, 100)
#head(tmp$data, n = 2)
#
#tmp2 <- get_testdata(100000, 100, ff = TRUE)
#head(tmp2$data, n = 2)
#
#library("pryr")
#print(object_size(d))
#print(object_size(d2))

