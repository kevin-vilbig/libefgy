/**\file
 * \brief Test cases for s-expression parsing
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
#include <sstream>

#include <ef.gy/test-case.h>
#include <ef.gy/sexp.h>

using namespace efgy::test;
using efgy::atom;
using efgy::cons;

/* \brief Tests construction of s-expressions
 *
 * \test Constructs an atomic s-expression and a cons s-expression
 *
 * \param [out] log Stream for output messages
 *
 * \returns Zero if everything was okay, a nonzero integer otherwise
 */
int testConstruction(std::ostream &log) {
  atom<int> a(23);

  if (a.data != 23) {
    log << "Expected data value: 23. Actual value: " << a.data;
    return next_integer();
  }

  cons<int, int> c(23, 42);
  if (c.car != 23) {
    log << "Expected value of car(cons(23, 42)): 23. Actual value: " << c.car;
    return next_integer();
  }
  if (c.cdr.just != 42) {
    log << "Expected value of cdr(cons(23, 42)): 42. Actual value: " << c.cdr;
    return next_integer();
  }

  return 0;
}

/* \brief Tests stream output of s-expressions
 *
 * \test Constructs atomic and cons s-expressions and inserts them
 * into a std::ostringstream
 *
 * \param [out] log Stream for output messages
 *
 * \returns Zero if everything was okay, a nonzero integer otherwise
 */
int testStreamOutput(std::ostream &log) {
  atom<int> a(23);
  cons<int, int> c(23, 42);

  std::ostringstream test;
  test << a;
  test << "\n" << c;

  std::string expected = "23\n(23 . 42)";
  if (test.str() != expected) {
    log << "Detected error in stream output. Expected: " << expected;
    log << "\nActual: " << test.str();
    return next_integer();
  } else {
    return 0;
  }
}

TEST_BATCH(testConstruction, testStreamOutput)
