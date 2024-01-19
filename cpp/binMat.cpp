#include <Rcpp.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace Rcpp;
using namespace std;

// Small function which returns our magic number
int my_magic_number() {
    return 74143467; // Reto's credit card number
}

// Writes magic number into current ifstream
void set_magic_number(std::ofstream& ofile, int bytes = 4) {
    int m = my_magic_number();
    ofile.write(reinterpret_cast<const char*>(&m), bytes);
}
    
// Reading magic number from current position of ifstream
int get_magic_number(std::ifstream& infile, int bytes = 4) {
    int m;
    char buffer[bytes];
    infile.read(buffer, bytes);
    memcpy(&m, &buffer, bytes);
    return m;
}

void set_char(std::ofstream& ofile, char val, int bytes) {
    ofile.write(reinterpret_cast<const char*>(&val), bytes);
}
char get_char(std::ifstream& infile, int bytes) {
    char* res = new char[bytes + 1];
    infile.read(res, bytes);
    return res;
    //char buffer[bytes];
    ////char* res = new char[bytes + 1];
    //char res[bytes + 1];
    //Rcout << " :: " << bytes << "\n";
    //Rcout << " ??? inline.read()\n";
    //infile.read(buffer, bytes);
    //Rcout << " ??? memcpy()\n";
    //strcpy(res, buffer);
    //Rcout << " ??? print\n";
    //Rcout << res << "\n";
    //return res;
}
void set_int4(std::ofstream& ofile, int val, int bytes = 4) {
    ofile.write(reinterpret_cast<const char*>(&val), bytes);
}
int get_int4(std::ifstream& infile, int bytes = 4) {
    char buffer[bytes];
    int res;
    infile.read(buffer, bytes);
    memcpy(&res, &buffer, bytes);
    return res;
}

// [[Rcpp::export]]
Rcpp::List create_binmm(std::string file,
                        std::string binfile,
                        int skip = 0,
                        bool header = true,
                        char sep = ',', bool verbose = true) {

    int ncol = 0, nrow = 0, nchar;
    int bytes_dbl = 8, nchar_colnames = 0, nchar_file;
    int bytes_colnames = 4; // We'll take bytes_colnames * nchar_colnames later;
    int bytes_file     = 8 * file.size(); // 8 bytes per char
    std::string line;
    std::string val;

    // file from std::string -> const char -> char
    //char *file_char;
    //Rcout << " xxxxxxxxxxxx " << file_char << " xxxxxxx\n";
    //std::strcpy(file_char, file.c_str());

    if (verbose) Rcout << "[cpp] Reading file " << file.c_str() << " (" << bytes_file << ")\n";

    // Open input file connection
    std::ifstream ifile(file.c_str());
    if (!ifile) stop("Whoops, input file not found/problem to open stream.");

    // Skipping lines
    if (skip > 0) {
        for (int i = 0; i < skip; i++) {
            // Will be 'wasted'; do we need to read the full line?
            // Btw: '\n' is crucial ("\n" would not work).
            std::getline(ifile, line, '\n');
        }
    }

    std::stringstream iss(""); // Must be 'globally' available

    // Reading first line to count number of columns.
    // This can be a header line (if header = true we will extract
    // the column names from it) or a data line. To be able to catch
    // both cases we need to store the file pointer to revert in case
    // header = false to re-read the same line as 'data'.
    std::streampos line1 = ifile.tellg(); // Store position of first line
    std::getline(ifile, line, '\n'); // Reading line
    iss.clear(); iss.str(line);

    //std::stringstream iss(line);
    // First run: count number of columns
    while (std::getline(iss, val, sep)) {
        ncol++;
    }
    if (verbose) Rcout << "[cpp] Number of columns: " << ncol << "\n";

    // Open output stream for binary data
    std::ofstream ofile(binfile.c_str(), std::ios::binary);
    if (!ofile) stop("Problems opening output file");

    // Writing binary file 'meta'
    // - magic_number: 4 byte int
    // - nrow:            4 byte int
    // - ncol:            4 byte int
    // - file_name_bytes: 4 byte int
    // - file name:       8 * file_name_bytes char
    set_magic_number(ofile);
    set_int4(ofile, -999);       // nrow, will be replaced at the end as soon as we know
    set_int4(ofile, ncol);       // ncol
    set_int4(ofile, bytes_file); // file name bytes
    set_char(ofile, *file.c_str(), bytes_file); // file name
    ofile.write(reinterpret_cast<const char*>(file.c_str()), bytes_dbl);
    
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
            colnames[i] = "V" + to_string(i);
        }
        // Revert to first line as the first line (we already used
        // once to count number of columns) as a data line.
        Rcout << "[cpp] Revert line\n";
        ifile.seekg(line1);
    }

    // Store number of characters for longest colname; used
    // for byte length for binary writing/reading later.
    for (int i = 0; i < ncol; i++) {
        nchar = colnames[i].size();
        if (nchar > nchar_colnames) nchar_colnames = nchar;
        bytes_colnames = bytes_colnames * nchar_colnames;
    }
    if (verbose) Rcout << "[cpp] Longest colname (nchar): " << nchar << "\n";
    if (verbose) Rcout << "[cpp] Bytes used for colnames: " << bytes_colnames << "\n";

    // Reading data; read line by line until we find EOF.
    // At the same time we calculate the row sums and
    // squared row sums for mean/sd.
    // TODO(R): Test what happens if we find non-numeric
    //          elements and if the length of the row (number
    //          ov values) is not equal to ncol!
    double x = 0.;
    if (verbose) Rcout << "[cpp] Processing data ...\n";
    while (std::getline(ifile, line, '\n')) {
        nrow++;
        iss.clear(); iss.str(line);
        for (int i = 0; i < ncol; i++) {
            std::getline(iss, val, ',');
            x = std::stod(val);
            ofile.write(reinterpret_cast<const char*>(&x), bytes_dbl);
            xsum[i]        = xsum[i] + x;
            xsumsquared[i] = xsumsquared[i] + x * x;
        }
    }

    if (verbose) Rcout << "[cpp] Dimension found/read: " << nrow << " x " << ncol << "\n";
    if (verbose) Rcout << "[cpp] Closing file connections\n";

    ifile.close();

    // Setting number of rows (after 4 bytes; after magic number)
    ofile.seekp(4); 
    set_int4(ofile, nrow);

    ofile.close();

    if (verbose) Rcout << "[cpp] Number of rows: " << nrow << "\n";

    // Calculate mean and standard deviation (sd)
    if (verbose) Rcout << "[cpp] Calculating mean and standard deviation (row-wise)\n";
    for (int i = 0; i < ncol; i++) {
        sd[i]    = sqrt((xsumsquared[i] - xsum[i] * xsum[i] / nrow) / nrow);
        mean[i]  = xsum[i] / nrow;
    }

    if (verbose) Rcout << "[cpp] All done, creating return object ...\n";

    mean.attr("names") = colnames;
    sd.attr("names") = colnames;
    List dim = Rcpp::List::create(Named("nrow") = nrow, Named("ncol") = ncol);

    List rval = Rcpp::List::create(Named("file")     = file,
                                   Named("binfile")  = binfile,
                                   Named("header")   = header,
                                   Named("skip")     = skip,
                                   Named("dim")      = dim,
                                   Named("colnames") = colnames,
                                   // mean and standard deviation for scaling
                                   Named("scale") = Rcpp::List::create(Named("mean") = mean,
                                                                       Named("sd") = sd)
                                  );
    rval.attr("class") = "binmm";
    return rval;
}





// [[Rcpp::export]]
Rcpp::List meta_binmm(std::string file, int nrow, int ncol, bool verbose = true) {

    if (verbose) Rcout << "[cpp] Reading file " << file.c_str() << "\n";

    int bytes_dbl = 8, counter = 0;
    int nrow_total, ncol_total, bytes_file;

    double y;
    char buffer[bytes_dbl];

    
    if (verbose) Rcout << "Dimension of object to read/return: " << nrow << "x" << ncol << "\n";

    // Open input file connection (binary)
    std::ifstream myfile(file.c_str(), ios::in | std::ios::binary);
    if (!myfile) {
        stop("Whoops, input file not found/problem to open stream.");
    }

    // Reading binary file meta
    // - magic_number: 4 byte int
    // - file_name_bytes: 4 byte int
    // - ncol: 4 byte int
    int magic_number = get_magic_number(myfile);
    if (magic_number != my_magic_number())
        stop("Wrong magic number; content of binary file not what is expected");
    Rcout << "[cpp] Got magic number " << magic_number << "\n";

    nrow_total    = get_int4(myfile);
    ncol_total    = get_int4(myfile);
    Rcout << "[cpp] Total dim: " << nrow_total << " x " << ncol_total << "\n";

    bytes_file    = get_int4(myfile);
    Rcout << bytes_file << "\n";
    char original_file = get_char(myfile, bytes_file);

    Rcout << "[cpp] Original file: " << original_file << "(" << bytes_file << ")\n";

    


    IntegerVector res = rep(9, 2);
    return Rcpp::List::create(Named("dummy"), res);

    NumericMatrix rmat(nrow, ncol);

    counter = 0;
    for (int i = 0; i < nrow; i++) {
        for (int j = 0; j < ncol; j++) {
            counter++;
            myfile.read(buffer, bytes_dbl);
            memcpy(&y, buffer, bytes_dbl);
            rmat(i, j) = y;
        }
    }

    if (verbose) Rcout << "[cpp] Closing file connection\n";

    myfile.close();

    // Dummy return
    return Rcpp::List::create(Named("data") = rmat);
}





// [[Rcpp::export]]
Rcpp::List read_binMatFull(std::string file, int nrow, int ncol, bool verbose = true) {

    if (verbose) Rcout << "[cpp] Reading file " << file.c_str() << "\n";

    int bytes_dbl = 8, counter = 0;
    double y;
    char buffer[bytes_dbl];
    
    if (verbose) Rcout << "Dimension of object to read/return: " << nrow << "x" << ncol << "\n";

    NumericMatrix rmat(nrow, ncol);

    // Open input file connection (binary)
    std::ifstream myfile(file.c_str(), ios::in | std::ios::binary);
    if (!myfile) {
        stop("Whoops, input file not found/problem to open stream.");
    }

    counter = 0;
    for (int i = 0; i < nrow; i++) {
        for (int j = 0; j < ncol; j++) {
            counter++;
            myfile.read(buffer, bytes_dbl);
            memcpy(&y, buffer, bytes_dbl);
            rmat(i, j) = y;
        }
    }

    if (verbose) Rcout << "[cpp] Closing file connection\n";

    myfile.close();

    // Dummy return
    return Rcpp::List::create(Named("data") = rmat);
}
