/**\file
 * \brief Test cases for vectors
 *
 * Contains test cases that test libefgy's generic vectors.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#include <iostream>

#include <ef.gy/test-case.h>
#include <ef.gy/vector.h>
#include <ef.gy/polar.h>

using namespace efgy::math;

/**\brief Tests real vectors
 * \test Performs some basic arithmetic on vectors.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testRealVectors(std::ostream &log) {
  vector<int, 3> v1 = {{1, 2, 3}};
  vector<int, 3> v2 = {{2, 3, 4}};
  vector<int, 3> r = v1 + v2;

  log << v1 << " + " << v2 << " = " << r << "\n";
  log << "sizeof(int) = " << sizeof(int) << "\n";
  log << "sizeof(vector<int,3>) = " << sizeof(vector<int, 3>) << "\n";

  if (r != vector<int, 3>({3, 5, 7})) {
    log << "unexpected result after vector addition; expected ([real] 3, 5, 7) "
           "but have " << r << "\n";
    return 1;
  }

  vector<double, 3, format::polar> vp1 = {{1, 2, 3}};
  vector<double, 3, format::polar> vp2 = {{2, -1, 1}};
  vector<double, 3, format::polar> rp = vp1 + vp2;

  log << vp1 << " + " << vp2 << " = " << rp << "\n";
  log << "sizeof(double) = " << sizeof(double) << "\n";
  log << "sizeof(vector<double,3,format::polar>) = "
      << sizeof(vector<double, 3, format::polar>) << "\n";

  if (rp != vector<double, 3, format::polar>({3, 1, 4})) {
    log << "unexpected result after vector addition; expected ([polar:10] 3, "
           "1, 4) but have " << rp << "\n";
    return 2;
  }

  vector<double, 3> rpc = rp;

  log << rp << " = " << rpc << "\n";

  vector<double, 3, format::polar> rpcr = rpc;

  log << rpc << " = " << rpcr << "\n";

  return 0;
}

TEST_BATCH(testRealVectors)
