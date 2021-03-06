/**\file
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_TRACER_H)
#define EF_GY_TRACER_H

#include <string>
#include <sstream>
#include <memory>

namespace efgy {
namespace math {
/**\brief Numeric operation tracer
 *
 * Implements a template class that acts like a numeric data type, but
 * instead of performing any calculations it will log any operations
 * performed on it. This would allow you to, e.g. find out which
 * calculations a template function actually performs at runtime.
 */
namespace tracer {
template <typename T, typename S, char op, typename format> class tracer;

/**\brief Output formatter base
 *
 * Generic output formatter base template. Only creates a typedef
 * for the result type.
 *
 * \tparam R The result type that the data needs to be formatted to.
 */
template <typename R> class formatter {
public:
  /**\brief Result type
   *
   * Convenient typedef for the result type, which was set as
   * a template parameter.
   */
  typedef R result;
};

/**\brief Standard string output formatter
 *
 * The default output formatter for the tracer; formats content as
 * a std::string for output or further processing.
 */
template <> class formatter<std::string> {
public:
  /**\brief Result type
   *
   * Convenient typedef for the result type, which was set as
   * a template parameter.
   */
  typedef std::string result;

  /**\brief Format two-parameter operation
   *
   * Formats an operation with two parameters. The output is
   * always fully-paranthesised to avoid ambiguities.
   *
   * \tparam T       Type of the first argument.
   * \tparam S       Type of the second argument.
   * \tparam op      The applicable operator.
   *
   * \param[in] t The operation to format.
   */
  template <typename T, typename S, char op>
  static result format(const tracer<T, S, op, formatter> &t) {
    std::stringstream s("");
    if (op) {
      s << "(" << t.value1 << " " << op << " " << t.value2 << ")";
    } else {
      s << "(" << t.value1 << " " << t.value2 << ")";
    }
    return s.str();
  }

  /**\brief Format single-parameter operation
   *
   * Formats an operation with one parameter. The output is
   * always fully-paranthesised to avoid ambiguities.
   *
   * \tparam T       Type of the argument.
   * \tparam op      The applicable operator.
   *
   * \param[in] t The operation to format.
   */
  template <typename T, char op>
  static result format(const tracer<T, void, op, formatter> &t) {
    std::stringstream s("");
    if (op) {
      s << "(" << op << t.value1 << ")";
    } else {
      s << t.value1;
    }
    return s.str();
  }
};

/**\brief Tracer base class
 *
 * The base class for the numeric tracer. Sets up virtual attributes
 * as required by the tracer.
 *
 * \tparam format  The formatter for the tracer.
 */
template <typename format = formatter<std::string> > class base {
public:
  /**\brief Virtual base destructor
   *
   * Empty, but defined so that derived classes can have their
   * destructors called when necessary.
   */
  virtual ~base(void) {}

  /**\brief Format the tracer result
   *
   * Cast the tracer result to the formatter's return type to
   * format the result.
   *
   * \returns The formatted result.
   */
  virtual operator typename format::result(void) const { return "unknown"; }
};

/**\brief Runtime tracer type
 *
 * Convenient typedef for a runtime tracer with the standard output
 * formatter.
 */
typedef std::shared_ptr<base<> > runtime;

/**\brief Numeric tracer two-parameter operation
 *
 * Wraps a two-parameter tracer operation.
 *
 * \tparam T       Type of the left-hand value for the operation.
 * \tparam S       Type of the right-hand value for the operation.
 * \tparam op      The operator for the operation.
 * \tparam format  The formatter for the tracer.
 */
template <typename T, typename S = void, char op = 0,
          typename format = formatter<std::string> >
class tracer : public base<format> {
public:
  /**\brief Construct with values
   *
   * Construct a tracer operation with two parameters.
   */
  constexpr tracer(const T &p1, const S &p2) : value1(p1), value2(p2) {}

  /**\brief Format the tracer result
   *
   * Cast the tracer result to the formatter's return type to
   * format the result.
   *
   * \returns The formatted result.
   */
  operator typename format::result(void) const { return format::format(*this); }

  /**\brief Upcast to shared pointer
   *
   * This is used to turn a tracer into a shared pointer,
   * which is necessary when using a runtime tracer to avoid
   * memory leaks.
   */
  operator std::shared_ptr<tracer>(void) {
    return std::shared_ptr<tracer>(new tracer(*this));
  }

  /**\brief First/left-hand parameter
   *
   * This is the first - or left-hand - parameter to the
   * traced operation.
   */
  const T value1;

  /**\brief Second/right-hand parameter
   *
   * This is the second - or right-hand - parameter to the
   * traced operation.
   */
  const S value2;
};

/**\brief Numeric tracer single-parameter operation
 *
 * Wraps a single-parameter tracer operation.
 *
 * \tparam T       Type of the value used with the operator.
 * \tparam op      The operator for the operation.
 * \tparam format  The formatter for the tracer.
 */
template <typename T, char op, typename format>
class tracer<T, void, op, format> : public base<format> {
public:
  /**\brief Construct with value
   *
   * Construct a tracer operation with one parameter.
   */
  constexpr tracer(const T &p1) : value1(p1) {}

  /**\brief Format the tracer result
   *
   * Cast the tracer result to the formatter's return type to
   * format the result.
   *
   * \returns The formatted result.
   */
  operator typename format::result(void) const { return format::format(*this); }

  /**\brief Upcast to shared pointer
   *
   * This is used to turn a tracer into a shared pointer,
   * which is necessary when using a runtime tracer to avoid
   * memory leaks.
   */
  operator std::shared_ptr<tracer>(void) {
    return std::shared_ptr<tracer>(new tracer(*this));
  }

  /**\brief First/left-hand parameter
   *
   * This is the first - or left-hand - parameter to the
   * traced operation.
   *
   * Since this is a single-parameter operation this
   */
  const T value1;
};

/**\brief Numeric tracer variable name
 *
 * Wraps a std::string variable name for use in a runtime tracer.
 *
 * \tparam format  The formatter for the tracer.
 */
template <typename format>
class tracer<void, void, 0, format> : public base<format> {
public:
  /**\brief Construct with name
   *
   * Construct a tracer variable name using a std::string.
   */
  tracer(std::string pName) : name(pName) {}

  /**\brief Format the tracer result
   *
   * Cast the tracer result to the formatter's return type to
   * format the result.
   *
   * \returns The formatted result.
   */
  operator std::string(void) const { return name; }

  /**\brief Upcast to shared pointer
   *
   * This is used to turn a tracer into a shared pointer,
   * which is necessary when using a runtime tracer to avoid
   * memory leaks.
   */
  operator std::shared_ptr<tracer>(void) {
    return std::shared_ptr<tracer>(new tracer(*this));
  }

  /**\brief Variable name
   *
   * Name of the variable contained in this object.
   */
  const std::string name;
};

/**\brief Tracer stream output
 *
 * Used to write an instance of the tracer to a standard output
 * stream. This operator has several overloads, depending on the
 * kind of tracer operation to write out.
 *
 * This is the runtime tracer overload.
 *
 * \tparam T    Type of the left-hand value for the operation.
 * \tparam S    Type of the right-hand value for the operation.
 * \tparam op   The operator for the operation.
 *
 * \param[in] s The stream to write to.
 * \param[in] t The traced operation to write to the stream.
 *
 * \returns A reference to the stream that the traced operation was
 *          written to.
 */
template <typename T, typename S, char op>
    static inline std::ostream &operator<<(std::ostream &s,
                                           const tracer<T, S, op> &t) {
  return (s << std::string(t));
}

/**\brief Tracer stream output
 *
 * Used to write an instance of the tracer to a standard output
 * stream. This operator has several overloads, depending on the
 * kind of tracer operation to write out.
 *
 * This overload writes a tracer that has been wrapped in a shared
 * pointer to the stream.
 *
 * \tparam T    Type of the left-hand value for the operation.
 * \tparam S    Type of the right-hand value for the operation.
 * \tparam op   The operator for the operation.
 *
 * \param[in] s The stream to write to.
 * \param[in] t The traced operation to write to the stream.
 *
 * \returns A reference to the stream that the traced operation was
 *          written to.
 */
template <typename T, typename S, char op>
    static inline std::ostream &operator<<(std::ostream &s,
                                           const std::shared_ptr<
                                               tracer<T, S, op> > &t) {
  return t ? (s << *t) : (s << "0");
}

/**\brief Tracer stream output
 *
 * Used to write an instance of the tracer to a standard output
 * stream. This operator has several overloads, depending on the
 * kind of tracer operation to write out.
 *
 * This is the overload for the standard runtime type.
 *
 * \param[in] s The stream to write to.
 * \param[in] t The traced operation to write to the stream.
 *
 * \returns A reference to the stream that the traced operation was
 *          written to.
 */
static inline std::ostream &operator<<(std::ostream &s, const runtime &t) {
  return t ? (s << std::string(*t)) : (s << "0");
}

/**\brief Numeric tracer addition operator
 *
 * Implements the addition operator for a runtime tracer with
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of adding things,
 * i.e. with the tracer variable being on either side, with or
 * without a runtime tracer, etc.
 *
 * \tparam R    Type of the value that is added to the tracer.
 *
 * \param[in] p1 The tracer that a value is added to.
 * \param[in] p2 The value that is added to the tracer.
 *
 * \returns A new two-operator tracer with both operands added
 *          together.
 */
template <typename R>
    static inline std::shared_ptr<
        tracer<runtime, R, '+'> > operator+(const runtime &p1, const R &p2) {
  return std::shared_ptr<tracer<runtime, R, '+'> >(
      new tracer<runtime, R, '+'>(p1, p2));
}

/**\brief Numeric tracer addition operator
 *
 * Implements the addition operator for a runtime tracer with
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of adding things,
 * i.e. with the tracer variable being on either side, with or
 * without a runtime tracer, etc.
 *
 * \tparam R    Type of the value that is added to the tracer.
 *
 * \param[in] p1 The value that is added to the tracer.
 * \param[in] p2 The tracer that a value is added to.
 *
 * \returns A new two-operator tracer with both operands added
 *          together.
 */
template <typename R>
    static inline std::shared_ptr<
        tracer<R, runtime, '+'> > operator+(const R &p1, const runtime &p2) {
  return std::shared_ptr<tracer<R, runtime, '+'> >(
      new tracer<R, runtime, '+'>(p1, p2));
}

/**\brief Numeric tracer addition operator
 *
 * Implements the addition operator for two tracer values.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of adding things,
 * i.e. with the tracer variable being on either side, with or
 * without a runtime tracer, etc.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer with both operands added
 *          together.
 */
static inline std::shared_ptr<
    tracer<runtime, runtime, '+'> > operator+(const runtime &p1,
                                              const runtime &p2) {
  return std::shared_ptr<tracer<runtime, runtime, '+'> >(
      new tracer<runtime, runtime, '+'>(p1, p2));
}

/**\brief Numeric tracer subtraction operator
 *
 * Implements the subtraction operator for a runtime tracer and
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of subtracting
 * things, i.e. with the tracer variable being on either side, with
 * or without a runtime tracer, etc.
 *
 * \tparam R    Type of the value that is subtracted from the
 *              tracer.
 *
 * \param[in] p1 The tracer that a value is subtracted from.
 * \param[in] p2 The value that is subtracted from the tracer.
 *
 * \returns A new two-operator tracer with the right-hand side
 *          subtracted from the left-hand side.
 */
template <typename R>
    static inline std::shared_ptr<
        tracer<runtime, R, '-'> > operator-(const runtime &p1, const R &p2) {
  return std::shared_ptr<tracer<runtime, R, '-'> >(
      new tracer<runtime, R, '-'>(p1, p2));
}

/**\brief Numeric tracer subtraction operator
 *
 * Implements the subtraction operator for a runtime tracer and
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of subtracting
 * things, i.e. with the tracer variable being on either side, with
 * or without a runtime tracer, etc.
 *
 * \tparam R    Type of the value that the tracer is subtracted
 *              from.
 *
 * \param[in] p1 The value that the tracer is subtracted from.
 * \param[in] p2 The tracer that is subtracted from the value.
 *
 * \returns A new two-operator tracer with the right-hand side
 *          subtracted from the left-hand side.
 */
template <typename R>
    static inline std::shared_ptr<
        tracer<R, runtime, '-'> > operator-(const R &p1, const runtime &p2) {
  return std::shared_ptr<tracer<R, runtime, '-'> >(
      new tracer<R, runtime, '-'>(p1, p2));
}

/**\brief Numeric tracer subtraction operator
 *
 * Implements the subtraction operator for two tracer values.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of subtracting
 * things, i.e. with the tracer variable being on either side, with
 * or without a runtime tracer, etc.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer with the right-hand side
 *          subtracted from the left-hand side.
 */
static inline std::shared_ptr<
    tracer<runtime, runtime, '-'> > operator-(const runtime &p1,
                                              const runtime &p2) {
  return std::shared_ptr<tracer<runtime, runtime, '-'> >(
      new tracer<runtime, runtime, '-'>(p1, p2));
}

/**\brief Numeric tracer multiplication operator
 *
 * Implements the multiplication operator for a runtime tracer with
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of multiplying
 * things, i.e. with the tracer variable being on either side, with
 * or without a runtime tracer, etc.
 *
 * \tparam R    Type of the factor to multiply with the tracer.
 *
 * \param[in] p1 The tracer that a value is multiplied with.
 * \param[in] p2 The factor to multiply the tracer with.
 *
 * \returns A new two-operator tracer containing the product of the
 *          two parameters.
 */
template <typename R>
static inline std::shared_ptr<tracer<runtime, R, '*'> > operator*(
    const runtime &p1, const R &p2) {
  return std::shared_ptr<tracer<runtime, R, '*'> >(
      new tracer<runtime, R, '*'>(p1, p2));
}

/**\brief Numeric tracer multiplication operator
 *
 * Implements the multiplication operator for a runtime tracer with
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of multiplying
 * things, i.e. with the tracer variable being on either side, with
 * or without a runtime tracer, etc.
 *
 * \tparam R    Type of the factor to multiply with the tracer.
 *
 * \param[in] p1 The factor to multiply the tracer with.
 * \param[in] p2 The tracer that a value is multiplied with.
 *
 * \returns A new two-operator tracer containing the product of the
 *          two parameters.
 */
template <typename R>
static inline std::shared_ptr<tracer<R, runtime, '*'> > operator*(
    const R &p1, const runtime &p2) {
  return std::shared_ptr<tracer<R, runtime, '*'> >(
      new tracer<R, runtime, '*'>(p1, p2));
}

/**\brief Numeric tracer multiplication operator
 *
 * Implements the multiplication operator for two tracer values.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of multiplying
 * things, i.e. with the tracer variable being on either side, with
 * or without a runtime tracer, etc.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer containing the product of the
 *          two former tracers.
 */
static inline std::shared_ptr<tracer<runtime, runtime, '*'> > operator*(
    const runtime &p1, const runtime &p2) {
  return std::shared_ptr<tracer<runtime, runtime, '*'> >(
      new tracer<runtime, runtime, '*'>(p1, p2));
}

/**\brief Numeric tracer division operator
 *
 * Implements the multiplication operator for a runtime tracer with
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of dividing things,
 * i.e. with the tracer variable being on either side, with or
 * without a runtime tracer, etc.
 *
 * \tparam R    Type of the divisor.
 *
 * \param[in] p1 The tracer that is to be divided.
 * \param[in] p2 The divisor to divide by.
 *
 * \returns A new two-operator tracer containing the quotient of the
 *          two parameters.
 */
template <typename R>
static inline std::shared_ptr<
    tracer<runtime, R, '/'> > operator/(const runtime &p1, const R &p2) {
  return std::shared_ptr<tracer<runtime, R, '/'> >(
      new tracer<runtime, R, '/'>(p1, p2));
}

/**\brief Numeric tracer division operator
 *
 * Implements the multiplication operator for a runtime tracer with
 * another value.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of dividing things,
 * i.e. with the tracer variable being on either side, with or
 * without a runtime tracer, etc.
 *
 * \tparam R    Type of the dividend.
 *
 * \param[in] p2 The dividend to divide by the tracer.
 * \param[in] p1 The tracer to divide by.
 *
 * \returns A new two-operator tracer containing the quotient of the
 *          two parameters.
 */
template <typename R>
static inline std::shared_ptr<
    tracer<R, runtime, '/'> > operator/(const R &p1, const runtime &p2) {
  return std::shared_ptr<tracer<R, runtime, '/'> >(
      new tracer<R, runtime, '/'>(p1, p2));
}

/**\brief Numeric tracer division operator
 *
 * Implements the divison operator for two tracer values.
 *
 * There is a host of conceptually similar operators the implement
 * this feature, so as to support different ways of dividing things,
 * i.e. with the tracer variable being on either side, with or
 * without a runtime tracer, etc.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer containing the quotient of the
 *          two original tracers.
 */
static inline std::shared_ptr<
    tracer<runtime, runtime, '/'> > operator/(const runtime &p1,
                                              const runtime &p2) {
  return std::shared_ptr<tracer<runtime, runtime, '/'> >(
      new tracer<runtime, runtime, '/'>(p1, p2));
}

/**\brief Numeric tracer add-and-assign operator
 *
 * Implements the add-and-assign operator for two tracer values.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer with the right-hand side added
 *          to the left-hand side.
 */
static inline runtime operator+=(runtime &p1, const runtime &p2) {
  return p1 = (p1 + p2);
}

/**\brief Numeric tracer subtract-and-assign operator
 *
 * Implements the subtract-and-assign operator for two tracer
 * values.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer with the right-hand side
 *          subtracted from the left-hand side.
 */
static inline runtime operator-=(runtime &p1, const runtime &p2) {
  return p1 = (p1 - p2);
}

/**\brief Numeric tracer multiply-and-assign operator
 *
 * Implements the multiply-and-assign operator for two tracer values.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer with the product of the two
 *          tracers.
 */
static inline runtime operator*=(runtime &p1, const runtime &p2) {
  return p1 = (p1 * p2);
}

/**\brief Numeric tracer divide-and-assign operator
 *
 * Implements the divide-and-assign operator for two tracer values.
 *
 * \param[in] p1 The left-hand tracer.
 * \param[in] p2 The right-hand tracer.
 *
 * \returns A new two-operator tracer with the quotion of the two
 *          tracers.
 */
static inline runtime operator/=(runtime &p1, const runtime &p2) {
  return p1 = (p1 / p2);
}
}
}
}

#endif
