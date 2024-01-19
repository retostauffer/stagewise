cat("Sourcing testfunctions (to load test data)\n")
#source("sdr/inst/tinytest/get_testdata.R")
#data <- get_testdata(100, 10, TRUE)
#
#library("sdr")
#devtools::load_all("sdr")
#
#f <- formula(paste("y ~", paste(names(data)[!grepl("^y$", names(data))], collapse = "+")))
#f <- list(f, update(f, "sigma ~ ."))
#batch_ids <- list(1:10, 11:20)
##mod <- sdr(f, data = data, updating = "bs",
##           batch_ids = batch_ids, maxit = 10)
#mod <- sdr(f, data = data, updating = "bs",
#           batch_ids = 100, maxit = 10)
#####################################

source("sdr/inst/tinytest/get_testdata.R")
data <- get_testdata(100000, 100, TRUE)

library("sdr")
devtools::load_all("sdr")

f <- formula(paste("y ~", paste(names(data)[!grepl("^y$", names(data))], collapse = "+")))
f <- list(f, update(f, "sigma ~ ."))
mod <- sdr(f, data = data, updating = "bs",
           batch_ids = 10000, maxit = 200, scalex = TRUE)

plot(mod)
#str(mod)
#
#names(mod)
#sort(names(mod))
#
#mod$coefficients
#mod$logLik
#deparse(mod$logLik)
#tail(mod$coefficients$mu, 1)
