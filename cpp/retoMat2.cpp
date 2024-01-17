#include <Rcpp.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace Rcpp;
using namespace std;

// [[Rcpp::export]]
Rcpp::List retoMat2(std::string file, int skip = 0, int nrows = 3, bool header = true) {

    Rcout << "file name is " << file.c_str() << "\n";

    int ncol = 0, nrow = 0;

    // Open file connection
    std::ifstream myfile(file.c_str());
    std::string line;
    std::string val;

    // Skipping lines
    if (skip > 0) {
        for (int i = 0; i < skip; i++) {
            // Will be 'wasted'; do we need to read the full line?
            // Btw: '\n' is crucial ("\n" would not work).
            std::getline(myfile, line, '\n');
        }
    }

    std::stringstream iss(""); // Must be 'globally' available

    // Header?
    std::getline(myfile, line, '\n');
    iss.clear(); iss.str(line);
    //std::stringstream iss(line);
    // First run: count number of columns
    while (std::getline(iss, val, ',')) {
        ncol++;
        //Rcout << " | " << val << "\n";
    }
    Rcout << "Number of columns: " << ncol << "\n";
    
    // Creating colnames vector (length ncol) and fill.
    CharacterVector colnames(ncol);

    // Creating numeric vectors to store row sums and
    // squared row sums to calculate mean and variance (sd).
    // We could re-use some of them, but they are fairly small
    // (numeric vectors of length ncol).
    NumericVector xsum        = rep(0.0, ncol);
    NumericVector xsumsquared = rep(0.0, ncol);
    NumericVector mean        = rep(0.0, ncol);
    NumericVector variance    = rep(0.0, ncol);

    // Extracting header information
    if (header) {
        // Create new CharacterVector and store the result
        // Clear and reset (processing the same line once again)
        iss.clear();
        iss.str(line);
        // First run: count number of columns
        for (int i = 0; i < ncol; i++) {
            std::getline(iss, val, ',');
            // Remove double quotes if there are any
            val.erase(remove(val.begin(), val.end(), '"'), val.end());
            // Remove single quotes if there are any
            val.erase(remove(val.begin(), val.end(), '\''), val.end());
            colnames[i] = val.c_str();
        }
    } else {
        for (int i = 0; i < ncol; i++) {
            Rcout << i;
            colnames[i] = "V" + to_string(i);
        }
    }
    Rcout << "Colnames: " << colnames << "\n";

    // Reading data; read line by line until we find EOF.
    // At the same time we calculate the row sums and
    // squared row sums for mean/variance.
    // TODO(R): Test what happens if we find non-numeric
    //          elements and if the length of the row (number
    //          ov values) is not equal to ncol!
    double x = 0.;
    while (std::getline(myfile, line, '\n')) {
        nrow++;
        iss.clear(); iss.str(line);
        for (int i = 0; i < ncol; i++) {
            std::getline(iss, val, ',');
            x = std::stod(val);
            xsum[i]        = xsum[i] + x;
            xsumsquared[i] = xsumsquared[i] + x * x;
        }
    }
    Rcout << "Number of rows: " << nrow << "\n";

    // Calculate mean and variance
    for (int i = 0; i < ncol; i++)
        variance[i] = (xsumsquared[i] - xsum[i] * xsum[i] / nrow) / nrow;
    for (int i = 0; i < ncol; i++)
        mean[i]     = xsum[i] / nrow;

    //Rcpp::List res = Rcpp::List::create(colnames);
    List rval = Rcpp::List::create(Named("file") = file,
                                   Named("header") = header,
                                   Named("skip") = skip,
                                   Named("nrow") = nrow,
                                   Named("ncol") = ncol,
                                   Named("colnames") = colnames,
                                   Named("mean") = mean,
                                   Named("variance") = variance);
    return rval;
}


