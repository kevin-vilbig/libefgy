/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

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

#if !defined(EF_GY_RENDER_SVG_H)
#define EF_GY_RENDER_SVG_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>
#include <sstream>

namespace efgy
{
    namespace render
    {
        template<typename Q, unsigned int d>
        class svg
        {
            public:
                svg
                    (const geometry::transformation::affine<Q,d> &pTransformation,
                     const geometry::projection<Q,d> &pProjection,
                     svg<Q,d-1> &pLowerRenderer)
                    : transformation(pTransformation), projection(pProjection), lowerRenderer(pLowerRenderer)
                    {}

                void frameStart (void)
                {
                    combined = transformation * projection;
                    lowerRenderer.frameStart();
                };
                void frameEnd (void) const {};

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const;

                void reset (void) const
                {
                    lowerRenderer.reset();
                }

            protected:
                const geometry::transformation::affine<Q,d> &transformation;
                const geometry::projection<Q,d> &projection;
                geometry::transformation::projective<Q,d> combined;
                svg<Q,d-1> &lowerRenderer;
        };

        template<typename Q>
        class svg<Q,2>
        {
            public:
                svg
                    (const typename geometry::transformation::affine<Q,2> &pTransformation)
                    : transformation(pTransformation)
                    {}

                void frameStart (void) const {};
                void frameEnd (void) const {};

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV);

                std::stringstream output;

                void reset()
                {
                    output.str("");
                    previousX = Q();
                    previousY = Q();
                }

            protected:
                const geometry::transformation::affine<Q,2> &transformation;
                Q previousX, previousY;
        };

        template<typename Q, unsigned int d>
        template<unsigned int q>
        void svg<Q,d>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const
        {
            math::tuple<q, typename geometry::euclidian::space<Q,d-1>::vector> V;

            for (unsigned int i = 0; i < q; i++)
            {
                V.data[i] = combined * pV.data[i];
            }

            lowerRenderer.drawFace(V);
        }

        template<typename Q>
        template<unsigned int q>
        void svg<Q,2>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV)
        {
            output << "<path d='";
            for (unsigned int i = 0; i < q; i++)
            {
                std::stringstream sbuf1;
                std::stringstream sbuf2;

                const typename geometry::euclidian::space<Q,2>::vector V = transformation * pV.data[i];

                const double a0 =  Q(V.data[0]);
                const double a1 = -Q(V.data[1]);

                if (i == 0)
                {
                    sbuf1 << "M" << a0 << "," << a1;
                    sbuf2 << "M" << a0 << "," << a1;
                }
                else
                {
                    const typename geometry::euclidian::space<Q,2>::vector V1 = transformation * pV.data[(i-1)];

                    const double a0r = a0 - Q(V1.data[0]);
                    const double a1r = a1 + Q(V1.data[1]);

                    if (pV.data[i].data[1] == V1.data[1])
                    {
                        sbuf1 << "H" << a0;
                        sbuf2 << "h" << a0r;
                    }
                    else if (pV.data[i].data[0] == -V1.data[0])
                    {
                        sbuf1 << "V" << a1;
                        sbuf2 << "v" << a1r;
                    }
                    else
                    {
                        sbuf1 << "L" << a0 << "," << a1;
                        sbuf2 << "l" << a0r << "," << a1r;
                    }
                }
                if (sbuf1.str().size() >= sbuf2.str().size())
                {
                    output << sbuf2.str();
                }
                else
                {
                    output << sbuf1.str();
                }
            }
            output << "Z'/>";
        }
    };
};

#endif
