// Includes all relevant components of mlpack.
// modified covariance example code from https://www.mlpack.org/doc/stable/doxygen/sample.html
#include <mlpack/core.hpp>
#include <iostream>
// Convenience.
using namespace mlpack;
int main()
{
  // First, load the data.
  arma::mat data;
  arma::mat A(2,3);
  // Use data::Load() which transposes the matrix.
  data::Load("data.csv", data, true);
  std::cout << data.size() << std::endl;
  std::cout << "Data has " << data.n_rows << " rows and has " << data.n_cols << " columns..." << std::endl;
  // note that the data flips from what the csv actually shows

  for (int row = 0; row<data.n_rows;row++){
    for (int col = 0; col<data.n_cols;col++){
      std::cout << data(row,col) << " ";
    }
    std::cout <<"\n";
  }

  // Now compute the covariance.  We assume that the data is already centered.
  // Remember, because the matrix is column-major, the covariance operation is
  // transposed.
  arma::mat cov = data * trans(data) / data.n_cols;
  // Save the output.
  data::Save("cov.csv", cov, true);
}
