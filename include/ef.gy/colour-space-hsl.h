/**\file
 * \brief HSL colour vectors
 *
 * Much the like the RGB colour vectors, these templates allow colours to be
 * represented as 3D or 4D vectors, depending on whether an alpha value is
 * added to the vector.
 *
 * \copyright
 * Copyright (c) 2012-2014, ef.gy Project Members
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_COLOUR_SPACE_HSL_H)
#define EF_GY_COLOUR_SPACE_HSL_H

#include <ef.gy/colour-space-rgb.h>

namespace efgy
{
    namespace math
    {
        namespace space
        {
            /**\brief HSL colour space tag
             *
             * This tag indicates that a vector is of the HSL(A) colour space;
             * since all vectors are tagged with a vector space tag class, this
             * makes it possible for vectors of different spaces to be converted
             * to be converted properly.
             */
            class HSL {};
        };

        template <typename Q>
        class vector<Q,3,space::HSL> : public std::array<Q,3>
        {
            public:
                vector (const std::array<Q,3> &v = {{}},
                        const space::HSL & = space::HSL())
                    : std::array<Q,3>(v),
                      hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                    {}
                vector (const Q &pHue, const Q &pSaturation, const Q &pLightness)
                    : std::array<Q,3>({ pHue, pSaturation, pLightness }),
                      hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                    {}

                Q &hue;
                Q &saturation;
                Q &lightness;

                vector (const vector<Q,3,space::RGB> &v)
                    : std::array<Q,3>(),
                      hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2])
                    {
                        Q min = v.red;
                        Q max = v.red;
                        if (v.green < min) { min = v.green; }
                        if (v.blue  < min) { min = v.blue;  }
                        if (v.green > max) { max = v.green; }
                        if (v.blue  > max) { max = v.blue;  }

                        Q C = max - min;
                        Q h = Q(0);
                        if (max == v.red)
                        {
                            h = (v.green - v.blue) / C;
                        }
                        else if (max == v.green)
                        {
                            h = (v.blue - v.red) / C + Q(2);
                        }
                        else if (max == v.blue)
                        {
                            h = (v.red - v.green) / C + Q(4);
                        }
                        while (h >= Q(6))
                        {
                            h -= Q(6);
                        }

                        hue = h / Q(6);
                        lightness = (max + min) / Q(2);
                        if (C == math::numeric::zero())
                        {
                            saturation = Q(0);
                        }
                        else
                        {
                            Q a = Q(2) * lightness - Q(1);
                            if (a < math::numeric::zero())
                            {
                                a = -a;
                            }
                            saturation = C/(Q(1) - a);
                        }
                    }

                operator vector<Q,3,space::RGB> (void) const
                {
                    Q a = Q(2)*lightness - Q(1);
                    if (a < math::numeric::zero())
                    {
                        a = -a;
                    }

                    Q chroma = (Q(1) - a) * saturation;
                    Q h = hue * Q(6);
                    Q b = h - Q(1);
                    while (b >= Q(2))
                    {
                        b -= Q(2);
                    }
                    if (b < math::numeric::zero())
                    {
                        b = -b;
                    }

                    Q x = chroma * (Q(1) - b);
                    Q r1, g1, b1;
                    if (h < Q(1))
                    {
                        r1 = chroma;
                        g1 = x;
                    }
                    else if (h < Q(2))
                    {
                        r1 = x;
                        g1 = chroma;
                    }
                    else if (h < Q(3))
                    {
                        g1 = chroma;
                        b1 = x;
                    }
                    else if (h < Q(4))
                    {
                        g1 = x;
                        b1 = chroma;
                    }
                    else if (h < Q(5))
                    {
                        r1 = x;
                        b1 = chroma;
                    }
                    else if (h < Q(6))
                    {
                        r1 = chroma;
                        b1 = x;
                    }

                    Q m = lightness - chroma / Q(2);

                    return {{ r1 + m, g1 + m, b1 + m }};
                }
        };

        template <typename Q>
        class vector<Q,4,space::HSL> : public std::array<Q,4>
        {
            public:
                vector (const std::array<Q,4> &v = {{}},
                        const space::HSL & = space::HSL())
                    : std::array<Q,4>(v),
                      hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                    {}
                vector (const Q &pHue, const Q &pSaturation, const Q &pLightness, const Q &pAlpha = Q(1))
                    : std::array<Q,4>({ pHue, pSaturation, pLightness, pAlpha }),
                      hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3]) 
                    {}

                Q &hue;
                Q &saturation;
                Q &lightness;
                Q &alpha;

                vector (const vector<Q,4,space::RGB> &v)
                    : std::array<Q,4>(),
                      hue((*this)[0]), saturation((*this)[1]), lightness((*this)[2]), alpha((*this)[3])
                    {
                        vector<Q,3,space::HSL> hv ({ v[0], v[1], v[2] });
                        hue        = hv.hue;
                        saturation = hv.saturation;
                        lightness  = hv.lightness;
                        alpha      = v[3];
                    }

                operator vector<Q,4,space::RGB> (void) const
                {
                    vector<Q,3,space::RGB> rg = vector<Q,3,space::HSL>({ hue, saturation, lightness });
                    return {{ rg[0], rg[1], rg[2], alpha }};
                }
        };
    };

    namespace colour
    {
        template <typename Q>
        class HSLA;

        template <typename Q>
        class HSL
        {
            public:
                typedef Q base;
                typedef Q scalar;
                typedef math::vector<Q,3,math::space::HSL> vector;
        };

        template <typename Q>
        class HSLA
        {
            public:
                typedef Q base;
                typedef Q scalar;
                typedef math::vector<Q,3,math::space::HSL> vector;
        };
    };
};

#endif
