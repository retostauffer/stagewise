



`[.retoMat` <- function (x, i, j, drop = FALSE) {

    i <- unique(as.integer(i))
    j <- if (missing(j)) seq_len(x$dim$ncol) else unique(j)

    # If character, translate to integer
    if (is.character(j)) {
        idx <- which(!j %in% x$colnames)
        if (length(idx) > 0)
            stop("column names not in data set: ", paste(x$colnames[idx], collapse = ", "))
        j <- match(j, x$colnames)
    }

    # Out of range check
    if (any(i < 0) | any(i > x$dim$nrow))
        stop("index `i` out of range (must be within {1, ", x$dim$nrow, "}")
    if (any(j < 0) | any(j > x$dim$ncol))
        stop("index `i` out of range (must be within {1, ", x$dim$ncol, "}")

    # Calling cpp for getting the data
    res <- retoMat_subset(x, sort(i), sort(j))
    return(res[order(i), order(j)])

}


head.retoMat <- function(x, n = 6, ...) {
    i <- seq_len(n)
    x[i[i <= x$dim$nrow], ]
}

tail.retoMat <- function(x, n = 6, ...) {
    i <- rev(x$dim$nrow - seq_len(n) + 1)
    x[i[i >= 1], ]
}

print.retoMat <- function(x, n = 6, ...) {
    # Estimated size if fully loaded in MB, assuming
    # * 8 bytes for each value (double)
    # * 120 bytes for column names (char)
    # * 2 bytes for row names (int)
    # Rough guess, tough.
    mest <- ceiling((x$dim$nrow * x$dim$ncol * 8 + x$dim$ncol * 100 + x$dim$nrow * 2) * 10) / 10

    cat("Warning: This is not advised.\n")
    cat("Here is some information and the head of the matrix.\n\n")

    cat("Object of class", paste(class(x), collapse = ", "), "\n")
    cat("    Dimension:", x$dim$nrow, "x", x$dim$ncol, "\n")
    cat("    Source file:", sprintf("\"%s\"", x$file), "\n")
    cat("    Estimated size when fully loaded:", sprintf("%.1f MB", mest / 1024^2), "\n")

    if (!file.exists(x$file)) {
        cat("\n[ERROR] File cannot be found\n")
    } else {
        print(head(x, n = n))
    }
    invisible(x)
}

summary.retoMat <- function(x, ...) {
    data.frame(Class = class(x),
               nrow = x$dim$nrow,
               ncol = x$dim$ncol,
               file = x$file)
}
