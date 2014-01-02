/**\file
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

#if !defined(EF_GY_PARAMETRIC_H)
#define EF_GY_PARAMETRIC_H

#include <ef.gy/polytope.h>

namespace efgy
{
    namespace geometry
    {
        template <typename Q, unsigned int od, unsigned int d, unsigned int f, typename render>
        using parametric = polytope<Q,od,d,f,render>;

        template <typename Q, unsigned int od, typename render, unsigned int d = 3>
        class moebiusStrip : public parametric<Q,od,d,4,render>
        {
            public:
                typedef parametric<Q,od,d,4,render> parent;

                moebiusStrip (render &pRenderer, const parameters<Q> &pParameter)
                    : parent(pRenderer, pParameter)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::faces;

                static const unsigned int modelDimensionMaximum = 2;

                static const char *id (void) { return "moebius-strip"; }

                void calculateObject(void)
                {
                    Q radius = parameter.polarRadius;
                    Q precision = parameter.polarPrecision;

                    usedRadius = radius;
                    usedPrecision = precision;

                    precision = ceil(precision);
                    const Q stepU = Q(M_PI/(precision*Q(2)));
                    const Q stepV = Q((2*radius)/precision);

                    faces.clear();

                    for (Q u = 0; u < Q(2*M_PI); u += stepU)
                    {
                        for (Q v = -radius; v <= radius; v += stepV)
                        {
                            Q un = (u + stepU) <= Q(2*M_PI) ? (u + stepU) : Q(2*M_PI);
                            Q vn = (v + stepV) <= Q(radius) ? (v + stepV) : radius;

                            faces.push_back
                                ({{ getCoordinates(radius, u, v),
                                    getCoordinates(radius, un, v),
                                    getCoordinates(radius, un, vn),
                                    getCoordinates(radius, u, vn) }});
                        }
                    }
                }

                Q getCoordinate (unsigned int i, Q r, Q u, Q v) const
                {
                    switch (i)
                    {
                        case 0:
                            return (r + v/Q(2) * cos(u/Q(2))) * cos(u);
                        case 1:
                            return (r + v/Q(2) * cos(u/Q(2))) * sin(u);
                        case 2:
                            return v/Q(2) * sin(u/Q(2));
                    }
                    return Q(0);
                }

                typename euclidian::space<Q,d>::vector getCoordinates (Q r, Q u, Q v) const
                {
                    typename euclidian::space<Q,d>::vector res;

                    res[0] = getCoordinate(0, r, u, v);
                    res[1] = getCoordinate(1, r, u, v);
                    res[2] = getCoordinate(2, r, u, v);

                    return res;
                }

                Q usedRadius;
                Q usedPrecision;
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = 3>
        class kleinBagel : public parametric<Q,od,d,4,render>
        {
            public:
                typedef parametric<Q,od,d,4,render> parent;

                kleinBagel (render &pRenderer, const parameters<Q> &pParameter)
                    : parent(pRenderer, pParameter)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::faces;

                static const unsigned int modelDimensionMaximum = 2;

                static const char *id (void) { return "klein-bagel"; }

                void calculateObject(void)
                {
                    Q radius = parameter.polarRadius;
                    Q precision = parameter.polarPrecision;

                    usedRadius = radius;
                    usedPrecision = precision;

                    precision = ceil(precision*Q(2));
                    const Q stepU = Q(M_PI/(precision));
                    const Q stepV = stepU;

                    faces.clear();

                    for (Q u = 0; u < Q(2*M_PI); u += stepU)
                    {
                        for (Q v = 0; v <= Q(2*M_PI); v += stepV)
                        {
                            Q un = (u + stepU) <= Q(2*M_PI) ? (u + stepU) : Q(2*M_PI);
                            Q vn = (v + stepV) <= Q(2*M_PI) ? (v + stepV) : Q(2*M_PI);

                            faces.push_back
                                ({{ getCoordinates(radius, u, v),
                                    getCoordinates(radius, un, v),
                                    getCoordinates(radius, un, vn),
                                    getCoordinates(radius, u, vn) }});
                        }
                    }
                }

                Q getCoordinate (unsigned int i, Q r, Q u, Q v) const
                {
                    switch (i)
                    {
                        case 0:
                            return (r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * cos(u);
                        case 1:
                            return (r + cos(u/Q(2))*sin(v) - sin(u/Q(2))*sin(Q(2)*v)) * sin(u);
                        case 2:
                            return sin(u/Q(2))*sin(v) - cos(u/Q(2))*sin(Q(2)*v);
                    }
                    return Q(0);
                }

                typename euclidian::space<Q,d>::vector getCoordinates (Q r, Q u, Q v) const
                {
                    typename euclidian::space<Q,d>::vector res;

                    res[0] = getCoordinate(0, r, u, v);
                    res[1] = getCoordinate(1, r, u, v);
                    res[2] = getCoordinate(2, r, u, v);

                    return res;
                }

                Q usedRadius;
                Q usedPrecision;
        };
    };
};

#endif
