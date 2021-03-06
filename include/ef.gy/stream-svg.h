/**\file
 * \brief SVG stream tag
 *
 * Contains the SVG stream tag, used to differentiate between plain output
 * streams and SVG ones.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_STREAM_SVG_H)
#define EF_GY_STREAM_SVG_H

#include <string>
#include <ostream>

namespace efgy {
namespace svg {
/**\brief std::ostream SVG tag
 *
 * Used to distinguish between a plain std::ostream, and one where the
 * output should be in SVG format.
 *
 * \tparam C Character type for the basic_ostream reference.
 */
template <typename C> class ostream {
public:
  /**\brief Construct with stream reference
   *
   * Initialises a new ostream SVG tag instance.
   *
   * \param[out] pStream The stream to write to.
   */
  ostream(std::basic_ostream<C> &pStream) : stream(pStream) {}

  /**\brief Output stream reference
   *
   * This is the stream where the output is written to.
   */
  std::basic_ostream<C> &stream;
};

/**\brief SVG tag
 *
 * Write this to an ostream to turn it into an ostream. Like this:
 *
 * \code{.cpp}
 * cout << svg::tag();
 * \encode
 */
class tag {};

/**\brief Convert std::ostream to SVG
 *
 * Converts the given stream to an SVG stream so that write operations
 * after that will produce SVG instead of plain text.
 *
 * \tparam C Character type for the basic_ostream reference.
 *
 * \param[out] stream The stream to write to.
 */
template <typename C>
constexpr inline ostream<C> operator<<(std::basic_ostream<C> &stream,
                                       const tag &) {
  return ostream<C>(stream);
}
}
}

#endif
