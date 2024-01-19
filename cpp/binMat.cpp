#include <Rcpp.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace Rcpp;
using namespace std;


// [[Rcpp::export]]
Rcpp::List binMat(std::string file, int skip = 0,
                  bool header = true,
                  char sep = ',', bool verbose = true) {

    if (verbose) Rcout << "[cpp] Reading file " << file.c_str() << "\n";

    int ncol = 0, nrow = 0;
    int bytes_dbl = 8;
    std::string binfile = "test.bin";

    // Open input file connection
    std::ifstream myfile(file.c_str());
    if (!myfile) {
        stop("Whoops, input file not found/problem to open stream.");
    }
    std::string line;
    std::string val;

    // Open output stream for binary data
    std::ofstream outfile(binfile, std::ios::binary);
    if (!outfile) {
        stop("Problems opening output file");
    }

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
    while (std::getline(iss, val, sep)) {
        ncol++;
        //Rcout << " | " << val << "\n";
    }
    if (verbose)
        Rcout << "[cpp] Number of columns: " << ncol << "\n";
    
    // Creating colnames vector (length ncol) and fill.
    CharacterVector colnames(ncol);

    // Creating numeric vectors to store row sums and
    // squared row sums to calculate mean and standard deviation (sd).
    // We could re-use some of them, but they are fairly small
    // (numeric vectors of length ncol).
    NumericVector xsum        = rep(0.0, ncol);
    NumericVector xsumsquared = rep(0.0, ncol);
    NumericVector mean        = rep(0.0, ncol);
    NumericVector sd          = rep(0.0, ncol);

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
    //Rcout << "[cpp] Colnames: " << colnames << "\n";

    // Reading data; read line by line until we find EOF.
    // At the same time we calculate the row sums and
    // squared row sums for mean/sd.
    // TODO(R): Test what happens if we find non-numeric
    //          elements and if the length of the row (number
    //          ov values) is not equal to ncol!
    double x = 0.;
    if (verbose) Rcout << "[cpp] Processing data ...\n";
    while (std::getline(myfile, line, '\n')) {
        nrow++;
        iss.clear(); iss.str(line);
        for (int i = 0; i < ncol; i++) {
            std::getline(iss, val, ',');
            x = std::stod(val);
            outfile.write(reinterpret_cast<const char*>(&x), bytes_dbl);
            xsum[i]        = xsum[i] + x;
            xsumsquared[i] = xsumsquared[i] + x * x;
        }
    }
    outfile.close();

    if (verbose) Rcout << "[cpp] Number of rows: " << nrow << "\n";

    // Calculate mean and standard deviation (sd)
    if (verbose) Rcout << "[cpp] Calculating mean and standard deviation (row-wise)\n";
    for (int i = 0; i < ncol; i++) {
        sd[i]    = sqrt((xsumsquared[i] - xsum[i] * xsum[i] / nrow) / nrow);
        mean[i]  = xsum[i] / nrow;
    }

    if (verbose)
        Rcout << "[cpp] All done, creating return object ...\n";

    mean.attr("names") = colnames;
    sd.attr("names") = colnames;
    List dim = Rcpp::List::create(Named("nrow") = nrow, Named("ncol") = ncol);

    //Rcpp::List res = Rcpp::List::create(colnames);
    List rval = Rcpp::List::create(Named("file") = file,
                                   Named("binfile") = binfile,
                                   Named("header") = header,
                                   Named("skip") = skip,
                                   Named("dim")  = dim,
                                   Named("colnames") = colnames,
                                   // mean and standard deviation for scaling
                                   Named("scale") = Rcpp::List::create(Named("mean") = mean,
                                                                       Named("sd") = sd)
                                  );
    rval.attr("class") = "binMat";
    return rval;
}

// [[Rcpp::export]]
Rcpp::List read_binMat(std::string file, int ncol, int nrow, bool verbose = true) {

    if (verbose) Rcout << "[cpp] Reading file " << file.c_str() << "\n";

    int bytes_dbl = 8, counter = 0;
    double y;
    char buffer[bytes_dbl];

    NumericMatrix rmat(ncol, nrow);
    //NumericMatrix rmat(nrow, ncol);
    NumericVector rvec(nrow * ncol);

    // Open input file connection (binary)
    std::ifstream myfile(file.c_str(), ios::in | std::ios::binary);
    if (!myfile) {
        stop("Whoops, input file not found/problem to open stream.");
    }


    for (int j = 0; j < ncol; j++) {
        for (int i = 0; i < ncol; i++) {
            myfile.read(buffer, bytes_dbl);
            memcpy(&y, buffer, bytes_dbl);
            rmat(j, i) = y;
        }
    }

    
    //for (int i = 0; i < (ncol * nrow); i++) {
    //    myfile.read(buffer, bytes_dbl);
    //    memcpy(&y, buffer, bytes_dbl);
    //    rvec(i) = y;
    //}

    myfile.close();

    // Dummy return
    IntegerVector dummy = rep(0, 3);
    return Rcpp::List::create(rmat, dummy);
}
