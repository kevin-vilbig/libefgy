/**\file
 * \brief Ranged sequences
 *
 * Contains class and iterator templates for ranged sequences with arbitrary
 * types. These sequences are intended to replace the more common 3-clause for
 * loops with ranged for loops, thus hopefully increasing the legibility of
 * certain types of code.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_RANGE_H)
#define EF_GY_RANGE_H

#include <array>
#include <iterator>

namespace efgy {
/**\brief Range iterator template
 *
 * This template specifies a random access iterator for arbitrary, ranged
 * sequences. You should use the range class template to get instances of
 * this class.
 *
 * Unlike some other iterators, this iterator in particular is rather hard
 * to invalidate. That means you probably won't have to worry about most
 * operations on these iterators in tight loops.
 *
 * \tparam T Numeric base type for the sequences. Basic types such as int
 *           or double should work fine with this template, as should
 *           classes that imitate these types.
 */
template <typename T>
class rangeIterator : public std::iterator<std::random_access_iterator_tag, T> {
public:
  constexpr rangeIterator(const T &pStart, const T &pStep,
                          std::size_t pPosition)
      : start(pStart), stride(pStep), position(pPosition) {}

  constexpr bool operator==(const rangeIterator &b) const {
    return (start == b.start) && (stride == b.stride) &&
           (position == b.position);
  }

  constexpr bool operator!=(const rangeIterator &b) const {
    return (start != b.start) || (stride != b.stride) ||
           (position != b.position);
  }

  constexpr T operator*(void) const { return start + stride * T(position); }

  rangeIterator &operator++(void) {
    ++position;
    return *this;
  }

  rangeIterator operator++(int) {
    rangeIterator r = *this;
    ++(*this);
    return r;
  }

  rangeIterator &operator--(void) {
    --position;
    return *this;
  }

  rangeIterator operator--(int) {
    rangeIterator r = *this;
    --(*this);
    return r;
  }

  rangeIterator &operator+=(const std::ptrdiff_t &b) {
    position += b;
    return *this;
  }

  constexpr rangeIterator operator+(const std::ptrdiff_t &b) const {
    return rangeIterator(start, stride, position + b);
  }

  rangeIterator &operator-=(const std::ptrdiff_t &b) {
    position -= b;
    return *this;
  }

  constexpr rangeIterator operator-(const std::ptrdiff_t &b) const {
    return rangeIterator(start, stride, position - b);
  }

  constexpr std::ptrdiff_t operator-(const rangeIterator &b) const {
    return std::ptrdiff_t(position) - std::ptrdiff_t(b.position);
  }

  constexpr T &operator[](const std::ptrdiff_t &b) const {
    return *((*this) + b);
  }

  constexpr bool operator<(const rangeIterator &b) const {
    return position < b.position;
  }

  constexpr bool operator<=(const rangeIterator &b) const {
    return position <= b.position;
  }

  constexpr bool operator>(const rangeIterator &b) const {
    return position > b.position;
  }

  constexpr bool operator>=(const rangeIterator &b) const {
    return position >= b.position;
  }

protected:
  T start;
  T stride;
  std::size_t position;
};

template <typename T>
constexpr rangeIterator<T> operator+(const std::ptrdiff_t &a,
                                     const rangeIterator<T> &b) {
  return b + a;
}

/**\brief Generic range class
 *
 * Generic range class implementation; think of it as a C++ version of R's
 * seq function.
 *
 * \tparam T The data type for the range elements.
 * \tparam n The amount of elements in the range.
 */
template <typename T, std::size_t n = 0> class range {
public:
  constexpr range(const T &pStart) : start(pStart), stride(T(1)) {}

  constexpr range(const T &pStart, const T &pEnd, const bool inclusive = true)
      : start(pStart), stride((pEnd - pStart) / T(n - (inclusive ? 1 : 0))) {}

  using iterator = rangeIterator<T>;

  constexpr iterator begin(void) const { return iterator(start, stride, 0); }
  constexpr iterator end(void) const { return iterator(start, stride, n); }
  constexpr std::size_t size(void) const { return n; }

  /**\brief Get range array
   *
   * Returns a std::array of the range. This method is both constexpr
   * and static, meaning that for most data types it will be
   * evaluated at compile time.
   *
   * \param start  The start of the range.
   * \param stride The step size of the array.
   * \param p      The array that is filled with the range.
   * \param c      Used internally to remember where in the array we
   *               currently are.
   *
   * One example:
   * \code{.cpp}
   * range<int, 5>::get(2, 2)
   * {2, 4, 6, 8, 10}
   * \endcode
   *
   * \returns A std::array of the range.
   */
  constexpr static std::array<T, n> get(T start = 0, T stride = 1,
                                        std::array<T, n> p = {{}},
                                        const std::size_t c = n) {
    return (p[c] = (start + (stride * c))),
           c == 0 ? p : get(start, stride, p, c - 1);
  }

  T start;
  T stride;
};

/**\brief Generic range class; n=0 fix point
 * \copydetails range
 *
 * This is the instance of the class that is used when no length is
 * explicitly specified.
 */
template <typename T> class range<T, 0> {
public:
  constexpr range(const T &pEnd, const bool inclusive)
      : start(0), stride((pEnd) / T((pEnd) - (inclusive ? 1 : 0))),
        steps(pEnd) {}

  constexpr range(const T &pStart, const T &pEnd, const bool inclusive)
      : start(pStart),
        stride((pEnd - pStart) /
               T(((pEnd - pStart) < 0 ? (pStart - pEnd) : (pEnd - pStart)) -
                 (inclusive ? 1 : 0))),
        steps((pEnd - pStart) < 0 ? (pStart - pEnd) : (pEnd - pStart)) {}

  constexpr range(const T &pStart, const T &pEnd, const std::size_t pSteps,
                  const bool inclusive)
      : start(pStart),
        stride((pEnd - pStart) / T(pSteps - (inclusive ? 1 : 0))),
        steps(pSteps) {}

  typedef rangeIterator<T> iterator;

  constexpr iterator begin(void) const { return iterator(start, stride, 0); }
  constexpr iterator end(void) const { return iterator(start, stride, steps); }
  constexpr std::size_t size(void) const { return steps; }

  /**\copydoc range::get
   *
   * With n = 0 this function becomes rather trivial, as it'll simply
   * return a zero-length array.
   */
  constexpr static std::array<T, 0> get(T start = 0, T stride = 1,
                                        std::array<T, 0> p = {{}},
                                        const std::size_t c = 0) {
    return std::array<T, 0>{};
  }

  T start;
  T stride;
  std::size_t steps;
};
}

#endif
