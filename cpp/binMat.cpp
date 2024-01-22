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
void write_magic_number(std::ofstream& ofile, int bytes = 4) {
    int m = my_magic_number();
    ofile.write(reinterpret_cast<const char*>(&m), bytes);
}
    
// Reading magic number from current position of ifstream
int read_magic_number(std::ifstream& ifile, int bytes = 4) {
    int m;
    char buffer[bytes];
    ifile.read(buffer, bytes);
    memcpy(&m, &buffer, bytes);
    return m;
}

void write_string(std::ofstream& ofile, const std::string& val, const int bytes) {
    ofile.write(val.c_str(), bytes);
}

// Reading std::string of length nchar from ifstream
std::string read_string(std::ifstream& ifile, const int nchar) {
    std::string result(nchar, '\0'); // Create string with 'nchar' chars
    ifile.read(&result[0], nchar);
    return result;
}

void write_double(std::ofstream& ofile, double val, int &bytes) {
    ofile.write(reinterpret_cast<const char*>(&val), bytes); //sizeof(val));
}

double read_double(std::ifstream& ifile) {
    double res;
    ifile.read(reinterpret_cast<char*>(&res), sizeof(res));
    return res;
}


void write_int(std::ofstream& ofile, int val) {
    ofile.write(reinterpret_cast<const char*>(&val), sizeof(int));
}

int read_int(std::ifstream& ifile) {
    int res;
    ifile.read(reinterpret_cast<char*>(&res), sizeof(res));
    return res;
}

int get_bytes(std::string& type) {

    int res = -999;
    if (type == "float") {
        res = 4;
    } else if (type == "double") {
        res = 8;
    } else {
        stop("Unrecognized type!");
    }
    return res;
}


// Small helper function; checks if the maximum absolute value
// found in the data exceeds what we can store with X bytes
// (controlled by the type argument).
// If exceeding, throw an error.
void check_data_range(std::string& type, double& maxabs) {

    bool error = false;
    if (type == "float") {
        float limit = std::numeric_limits<float>::max();
        if (maxabs > limit) error = true;
    } else if (type == "double") {
        double limit = std::numeric_limits<double>::max();
        if (maxabs > limit) error = true;
    }

    if (error) {
        std::string errormsg = "Max absolute value (" + to_string(maxabs) + ") exceeds what can be stored as " + type + "; exit (change type)";
        stop(errormsg);
    }
}


// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
//
// Parameters
// ----------
// file : Path/name to the csv file to be read.
// binfile: Name of the binary output file created by this function.
// type: Can be 'float' or 'double'. Will write the
//       data in 4 bytes or 8 bytes respectively.
// skip: Number of lines to skip at the start of the CSV file.
// header: true if there is a header line with variable description, else false.
// sep: Single character, value separator.
// verbose: true/false, increases verbosity if set true.

// [[Rcpp::export]]
Rcpp::List create_binmm(std::string file,
                        std::string binfile,
                        std::string type = "double",
                        int skip = 0,
                        bool header = true,
                        char sep = ',', bool verbose = true) {

    int ncol = 0, nrow = 0, nchar;
    int nchar_file;
    int nchar_filename = file.size(); // Number of character of input file name
    std::string line;
    std::string val;

    // Getting bytes to write the data later on (float, double)
    int bytes = get_bytes(type);
    Rcout << "[cpp] Bytes for numerics: " << bytes << " (" << type << ")\n";

    Rcout << "[cpp] Range limit for " << type << ": " << std::numeric_limits<double>::max() << "  " << std::numeric_limits<double>::min() << "\n";


    // Open input file connection
    if (verbose) Rcout << "[cpp] Reading file " << file.c_str() << " (filename len = " << nchar_filename << ")\n";
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
    write_magic_number(ofile);
    write_int(ofile, -999);       // nrow, will be replaced at the end as soon as we know
    write_int(ofile, ncol);       // ncol
    write_int(ofile, nchar_filename); // Length of original file name
    write_string(ofile, file.c_str(), nchar_filename); // Original file name
    
    // Creating vector for column names as well as for the length
    // of the colnames (number of characters of each name).
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
            colnames[i] = "V" + to_string(i);
        }
        // Revert to first line as the first line (we already used
        // once to count number of columns) as a data line.
        if (verbose) Rcout << "[cpp] Revert line\n";
        ifile.seekg(line1);
    }

    // Store number of characters for longest colname; used
    // for byte length for binary writing/reading later.
    if (verbose) Rcout << "[cpp] Writing length and content of column names\n";
    for (int i = 0; i < ncol; i++)
        write_int(ofile, colnames[i].size());
    for (int i = 0; i < ncol; i++)
        write_string(ofile, Rcpp::as<std::string>(colnames[i]), colnames[i].size());

    // Creating numeric vectors to store row sums and
    // squared row sums to calculate mean and standard deviation (sd).
    // We could re-use some of them, but they are fairly small
    // (numeric vectors of length ncol).
    NumericVector xsum        = rep(0.0, ncol);
    NumericVector xsumsquared = rep(0.0, ncol);
    NumericVector mean        = rep(0.0, ncol);
    NumericVector sd          = rep(0.0, ncol);

    // Reading data; read line by line until we find EOF.
    // At the same time we calculate the row sums and
    // squared row sums for mean/sd.
    // TODO(R): Test what happens if we find non-numeric
    //          elements and if the length of the row (number
    //          ov values) is not equal to ncol!

    // Keeping track of the max(abs(value)) to do a range check
    // to see if we can accurately store this given the type
    // used.

    double x, xabs, maxabs = 0.0;

    if (verbose) Rcout << "[cpp] Processing data ...\n";
    while (std::getline(ifile, line, '\n')) {
        nrow++;
        iss.clear(); iss.str(line);
        for (int i = 0; i < ncol; i++) {
            std::getline(iss, val, ',');
            x = std::stod(val);
            xabs = std::abs(x);
            if (xabs > maxabs) maxabs = xabs;
            write_double(ofile, x, bytes);
            xsum[i]        = xsum[i] + x;
            xsumsquared[i] = xsumsquared[i] + x * x;
        }
    }

    if (verbose) Rcout << "[cpp] Maximum absolute value: " << maxabs << "\n";
    check_data_range(type, maxabs);

    if (verbose) Rcout << "[cpp] Dimension found/read: " << nrow << " x " << ncol << "\n";
    if (verbose) Rcout << "[cpp] Closing file connections\n";

    ifile.close();

    // Setting number of rows (after 4 bytes; after magic number)
    ofile.seekp(4); 
    write_int(ofile, nrow);

    ofile.close();

    if (verbose) Rcout << "[cpp] Number of rows: " << nrow << "\n";

    // Calculate mean and standard deviation (sd)
    if (verbose) Rcout << "[cpp] Calculating mean and standard deviation (row-wise)\n";
    for (int i = 0; i < ncol; i++) {
        sd[i]    = sqrt((xsumsquared[i] - xsum[i] * xsum[i] / nrow) / nrow);
        mean[i]  = xsum[i] / nrow;
    }

    //Rcout << mean << "\n";
    //Rcout << sd << "\n";

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


// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

// [[Rcpp::export]]
Rcpp::NumericMatrix subset_binmm(std::string file, int nrow, int ncol,
                        Rcpp::IntegerVector& ii,
                        Rcpp::IntegerVector& jj,
                        bool verbose = true) {


    Rcout << "ii = " << ii << "\n";
    Rcout << "jj = " << jj << "\n";
    if (verbose) Rcout << "[cpp] Reading file " << file.c_str() << "\n";

    int i, j, counter = 0;
    int nrow_total, ncol_total, nchar_filename;

    if (verbose) Rcout << "Dimension of object to read/return: " << nrow << "x" << ncol << "\n";

    // Open input file connection (binary)
    std::ifstream ifile(file.c_str(), ios::in | std::ios::binary);
    if (!ifile) {
        stop("Whoops, input file not found/problem to open stream.");
    }

    // Reading binary file meta
    // - magic_number: 4 byte int
    // - file_name_bytes: 4 byte int
    // - ncol: 4 byte int
    int magic_number = read_magic_number(ifile);
    if (magic_number != my_magic_number())
        stop("Wrong magic number; content of binary file not what is expected");
    if (verbose) Rcout << "[cpp] Got magic number " << magic_number << "\n";

    nrow_total    = read_int(ifile);
    ncol_total    = read_int(ifile);
    if (verbose) Rcout << "[cpp] Total dim: " << nrow_total << " x " << ncol_total << "\n";

    nchar_filename = read_int(ifile);
    std::string original_file = read_string(ifile, nchar_filename);

    if (verbose) Rcout << "[cpp] Original file: " << original_file << " (" << nchar_filename << ")\n";

    // Reading column names
    // First nchar of each of the colum names, then string (name)
    IntegerVector nchar_colnames = rep(-9, ncol);
    CharacterVector colnames(ncol);
    for (j = 0; j < ncol; j++)
        nchar_colnames[j] = read_int(ifile);
    for (j = 0; j < ncol; j++)
        colnames[j] = read_string(ifile, nchar_colnames[j]);
    

    // Create numeric matrix of the dimension requested
    // by the user; loop over i/j indices (0 based) and
    // read the data.
    // Sets pointers to jump to the value of interest
    // (long stream of doubles).
    NumericMatrix rmat(ii.size(), jj.size());

    // Appending dimension names
    CharacterVector rmat_colnames(ii.size());
    for (j =  0; j < jj.size(); ++j) rmat_colnames[j] = colnames[jj[j]];
    rmat.attr("dimnames") = Rcpp::List::create(R_NilValue, rmat_colnames);

    // Pointer position start of data
    std::streampos data_pos0 = ifile.tellg();

    int pos;
    double val;
    for (i = 0; i < ii.size(); ++i) {
        for (j = 0; j < jj.size(); ++j) {
            // Calculate pointer position
            pos = data_pos0 + ((ii[i] * ncol) + jj[j]) * sizeof(val);
            // Setting pointer; read and store double
            ifile.seekg(pos);
            rmat(i, j) = read_double(ifile);
        }
    }

    if (verbose) Rcout << "[cpp] Closing file connection\n";
    ifile.close();

    rmat.attr("original_file")  = original_file;
    rmat.attr("cols_available") = colnames;

    // Dummy return
    return rmat;
}

