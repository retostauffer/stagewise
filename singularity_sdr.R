


cat("\n --- installing R packages ---\n")

local({
  r <- getOption("repos")
  r["CRAN"] <- "https://stat.ethz.ch/CRAN/"
  options(repos = r)
})

# Devtools
install.packages("systemfonts")
install.packages("textshaping")
install.packages("pkgdown")
install.packages("devtools")

## CRAN
install.packages("gamlss.dist")
install.packages("Formula")
install.packages("Matrix")
install.packages("colorspace")
install.packages("knitr")
install.packages("scoringRules")
install.packages("parallel")
install.packages("ff")
install.packages("bigmemory")


## IF needed ...
install.packages("remotes")


## Install custom package; copied via %files on build
install.packages("/data/sdr", repos = NULL)
