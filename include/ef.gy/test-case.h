/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include <iostream>
#include <vector>

namespace efgy
{
    namespace test
    {
        typedef int(*testCase)(std::ostream &log);

        int run (int, char **, const std::vector<testCase> &testCases)
        {
            try
            {
                for (std::vector<testCase>::const_iterator it = testCases.begin();
                     it != testCases.end();
                     it++)
                {
                    std::cerr << "running test case " << (std::distance(testCases.begin(), it) + 1)
                              << " in batch of " << testCases.size() << ": ";
                    int res = (*it)(std::cerr);
                    if (res != 0)
                    {
                        std::cerr << "failed; code: " << res << "\n";
                        return res;
                    }
                    std::cerr << "OK\n";
                }
            }
            catch (std::exception &e)
            {
                std::cerr << "Exception: " << e.what() << "\n";
                return -1;
            }
            catch (...)
            {
                std::cerr << "Unknown Exception\n";
                return -1;
            }

            return 0;
        }
    };
};

#if defined(RUN_TEST_CASES)
extern const std::vector<efgy::test::testCase> testCases;

int main (int argc, char **argv)
{
    return efgy::test::run (argc, argv, testCases);
}

#define TEST_BATCH(...)\
static const efgy::test::testCase testCasesArray[] = { __VA_ARGS__ };\
const std::vector<efgy::test::testCase> testCases (testCasesArray, testCasesArray + sizeof(testCasesArray) / sizeof(efgy::test::testCase));

#else
#define TEST_BATCH(...)
#endif