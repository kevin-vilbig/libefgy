/**\file
 * \brief JSON helpers
 *
 * Contains helper functionality for JSON output.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_JSON_H)
#define EF_GY_JSON_H

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace efgy {
/**\brief JSON helpers
 *
 * Namespace for helper classes and functions used to generate CSS
 * serialisations.
 */
namespace json {
/**\brief JSON value
 *
 * This class is used to represent a JSON value, as defined at
 * http://www.json.org/ - the class basically holds a type tag and a
 * pointer.
 *
 * \see http://www.json.org/
 */
template <typename tNumeric = long double> class value {
public:
  typedef tNumeric numeric;

  value(void) : type(null), payload(0) {}
  value(const bool &pValue) : type(pValue ? yes : no), payload(0) {}
  value(const value &b) : type(null), payload(0) { *this = b; }
  value(const char *b) : type(null), payload(0) { toString() = b; }
  value(const std::string &b) : type(null), payload(0) { toString() = b; }
  value(const numeric &b) : type(null), payload(0) { toNumber() = b; }

  ~value(void) { clear(); }

  enum {
    object,
    array,
    string,
    number,
    yes,
    no,
    null,
    comma,
    colon,
    error,
    endArray,
    endObject
  } type;

  value &operator=(const value &b) {
    clear();
    switch (b.type) {
    case object:
      toObject() = b.asObject();
      break;
    case array:
      toArray() = b.asArray();
      break;
    case string:
      toString() = b.asString();
      break;
    case number:
      toNumber() = b.asNumber();
      break;
    case yes:
    case no:
    case null:
    case comma:
    case colon:
    case error:
    case endObject:
    case endArray:
      type = b.type;
      break;
    }
    return *this;
  }

  const std::size_t size(void) const {
    if (isObject()) {
      const auto &a = *((const std::map<std::string, value<numeric>> *)payload);
      return a.size();
    } else if (isArray()) {
      const auto &a = *((const std::vector<value<numeric>> *)payload);
      return a.size();
    }
    return 0;
  }

  const value<numeric> operator()(const std::string &i) const {
    if (isObject()) {
      return (*((const std::map<std::string, value<numeric>> *)payload))[i];
    }
    return value<numeric>();
  }

  value<numeric> &operator()(const std::string &i) { return toObject()[i]; }

  constexpr const bool isObject(void) const { return type == object; }

  const std::map<std::string, value<numeric>> asObject(void) const {
    if (!isObject()) {
      return std::map<std::string, value<numeric>>();
    }
    return *((const std::map<std::string, value<numeric>> *)payload);
  }

  std::map<std::string, value<numeric>> &toObject(void) {
    if (!isObject()) {
      clear();
      type = object;
      payload = new std::map<std::string, value<numeric>>();
    }
    return *((std::map<std::string, value<numeric>> *)payload);
  }

  value<numeric> &push(const value<numeric> &v) {
    toArray().push_back(v);
    return *this;
  }

  const value<numeric> operator[](const std::size_t &i) const {
    if (isArray()) {
      const auto &a = *((const std::vector<value<numeric>> *)payload);
      if (i < a.size()) {
        return a[i];
      }
    }
    return value<numeric>();
  }

  value<numeric> &operator[](const std::size_t &i) { return toArray()[i]; }

  constexpr const bool isArray(void) const { return type == array; }

  const std::vector<value<numeric>> asArray(void) const {
    if (!isArray()) {
      return std::vector<value<numeric>>();
    }
    return *((const std::vector<value<numeric>> *)payload);
  }

  std::vector<value<numeric>> &toArray(void) {
    if (!isArray()) {
      clear();
      type = array;
      payload = new std::vector<value<numeric>>();
    }
    return *((std::vector<value<numeric>> *)payload);
  }

  constexpr const bool isString(void) const { return type == string; }

  const std::string asString(void) const {
    if (!isString()) {
      return "";
    }
    return *((const std::string *)payload);
  }

  std::string &toString(void) {
    if (!isString()) {
      clear();
      type = string;
      payload = new std::string();
    }
    return *((std::string *)payload);
  }

  constexpr operator const numeric(void) const { return asNumber(); }

  constexpr explicit operator bool(void) const { return type == yes; }

  constexpr const bool isNumber(void) const { return type == number; }

  const numeric asNumber(void) const {
    if (!isNumber()) {
      return 0;
    }
    return *((numeric *)payload);
  }

  numeric &toNumber(void) {
    if (!isNumber()) {
      clear();
      type = number;
      payload = new numeric();
    }
    return *((numeric *)payload);
  }

protected:
  void clear(void) {
    if (payload) {
      switch (type) {
      case object:
        delete ((std::map<std::string, value<numeric>> *)payload);
        break;
      case array:
        delete ((std::vector<value<numeric>> *)payload);
        break;
      case string:
        delete ((std::string *)payload);
        break;
      case number:
        delete ((numeric *)payload);
        break;
      case yes:
      case no:
      case null:
      case comma:
      case colon:
      case error:
      case endObject:
      case endArray:
        break;
      }
      payload = 0;
    }
    type = null;
  }

  void *payload;
};

using json = value<long double>;
}
}

#endif
