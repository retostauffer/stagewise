#include <Rcpp.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace Rcpp;
using namespace std;

// [[Rcpp::export]]
Rcpp::List retoMat(std::string file, int skip = 0, int nrows = 3, bool header = true) {

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

    // Now we need to figure out how many rows we have (excluding
    // header and skipped rows as we re-use our file handler w/ current position).
    while (getline(myfile, line, '\n')) {
        nrow++;
    }
    Rcout << "Number of rows: " << nrow << "\n";

    //Rcpp::List res = Rcpp::List::create(colnames);
    List rval = Rcpp::List::create(Named("file") = file,
                                   Named("header") = header,
                                   Named("skip") = skip,
                                   Named("nrow") = nrow,
                                   Named("ncol") = ncol,
                                   Named("colnames") = colnames);
    return rval;
}


// [[Rcpp::export]]
Rcpp::List retoMatMean(const List & X) {

    int ncol = as<int>(X["ncol"]);
    int nrow = as<int>(X["nrow"]);
    int skip = as<int>(X["skip"]);
    bool header = as<bool>(X["header"]);
    std::string file = X["file"];

    Rcout << ncol << "x" << nrow << "  -  " << file << "\n";

    NumericVector mean = rep(0.0, ncol);
    NumericVector var  = rep(0.0, ncol);
    NumericVector xsquared = rep(0.0, ncol);

    Rcout << file << " (file)\n";
    Rcout << nrow << " " << ncol << " " << header << "\n";

    // Open file connection, skip first 'skip + header' lines.
    std::ifstream myfile(file);
    std::string line;
    std::string val;

    // Skipping lines
    if (skip > 0) {
        for (int i = 0; i < skip; i++) std::getline(myfile, line, '\n');
    }
    if (header) {
        std::getline(myfile, line, '\n'); // Skipping header
    }

    std::stringstream iss(""); // Must be 'globally' available

    // Reading data
    int test_counter = 0;
    double x = 0.;
    while (std::getline(myfile, line, '\n')) {
        test_counter++;
        iss.clear(); iss.str(line);
        for (int i = 0; i < ncol; i++) {
            std::getline(iss, val, ',');
            x = std::stod(val);
            mean[i] = mean[i] + x;
            xsquared[i] = xsquared[i] + x * x;
        }
    }
    for (int i = 0; i < ncol; i++) {
        var[i] = (xsquared[i] - mean[i] * mean[i] / nrow) / nrow;
    }
    for (int i = 0; i < ncol; i++) mean[i] = mean[i] / nrow;
    Rcout << "Number of lines processed: " << test_counter << "\n";

    return Rcpp::List::create(Named("mean") = mean,
                              Named("var") = var);

}

