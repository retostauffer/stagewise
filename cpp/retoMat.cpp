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
                                   Named("nrow") = ncol,
                                   Named("ncol") = nrow,
                                   Named("colnames") = colnames);
    return rval;
}


