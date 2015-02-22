/**\file
 * \brief Test cases for the 'maybe' template
 *
 * This file contains test cases for the 'maybe' template.
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#include <iostream>
#include <sstream>
#include <ef.gy/test-case.h>
#include <ef.gy/maybe.h>

using namespace efgy;
using namespace std;
using efgy::test::next_integer;

/**\brief 'Maybe' assignment and initialisation tests
 * \test Initialises several instances of the 'maybe' template of different
 *       numeric types to test if they're working as expected.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testMaybe(std::ostream &log) {
  maybe<int> nothing;

  if (nothing) {
    log << "value should have been 'nothing'\n";
    return 1;
  }

  maybe<int> something(42);

  if (!something) {
    log << "value should have been something other than 'nothing'\n";
    return 2;
  }

  if (int(something) != 42) {
    log << "value should have been '42', but is '" << int(something) << "'\n";
    return 3;
  }

  something = nothing;

  if (something) {
    log << "'something' was changed to 'nothing', but still thinks otherwise.\n";
    return 4;
  }

  maybe<double> somethingElse(something);

  if (somethingElse) {
    log << "value should have been 'nothing'\n";
    return 5;
  }

  somethingElse = maybe<int>(42);

  if (!somethingElse) {
    log << "value should have been something other than 'nothing'\n";
    return 6;
  }

  if (double(somethingElse) != 42) {
    log << "value should have been '42', but is '" << double(somethingElse)
        << "'\n";
    return 7;
  }

  return 0;
}

/**\brief Nontrivial example class
 *
 * Used in testMaybeNontrivial to test whether the (constexpr) maybe template
 * works as intended with a nontrivial class.
 */
class outputInDestructor {
public:
  /**\brief Initialise with output stream
   *
   * Initialises the class with an output stream, which is used in the
   * destructor.
   *
   * \param[in,out] pLog The stream to log to upon destruction of the
   *                     object
   */
  outputInDestructor(std::ostream &pLog) : log(pLog) {}

  /**\brief Destructor
   *
   * This destructor will log a message to the ostream passed to the
   * constructor in order to visualise when copies of an object are being
   * created an destroyed when using the maybe template.
   */
  ~outputInDestructor(void) {
    log << "outputInDestructor::~outputInDestructor()\n";
  }

protected:
  /**\brief Output stream reference
   *
   * The output stream passed to the constructor; used in the destructor.
   */
  std::ostream &log;
};

/**\brief 'Maybe' usage with nontrivial types test
 * \test Initialises a maybe with a nontrivial type to see if it behaves as
 *       expected. This test case also demonstrates how and when the contained
 *       objects are copied or re-initialised.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testMaybeNontrivial(std::ostream &log) {
  // maybe<outputInDestructor> nontrivial; // should not compile

  maybe<outputInDestructor> nontrivial((outputInDestructor(log)));

  const outputInDestructor &dest = nontrivial;

  return 0;
}

/** \brief Stream output test for maybes
 *  \test Tests stream output for both nontrivial maybes and nothings.
 *
 *  \param[out] log A stream for test cases to log messages to.
 *
 *  \return Zero when everything went as expected, nonzero otherwise.
 */
int testMaybeStreamOutput(std::ostream &log) {
  maybe<int> trivial;
  maybe<int> nontrivial(42);

  std::ostringstream test;
  test << trivial;
  test << "\n";
  test << nontrivial;

  std::stringstream expected;
  expected << "nothing"
           << "\n"
           << "42";

  if (test.str() != expected.str()) {
    log << "Unexpected characters in stream output.\n";
    log << "Expected: \n";
    log << expected.str();
    log << "\n\nActual:\n";
    log << test.str();
    return next_integer();
  }
  return 0;
}

TEST_BATCH(testMaybe, testMaybeNontrivial, testMaybeStreamOutput)
