/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012, ef.gy Project Members
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

#if !defined(EF_GY_RENDER_OPENGL_H)
#define EF_GY_RENDER_OPENGL_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>

#undef GL3D
#define GL3D

namespace efgy
{
    namespace render
    {
        template<typename Q, unsigned int d>
        class opengl
        {
            public:
                opengl
                    (const geometry::transformation<Q,d> &pTransformation,
                     const geometry::perspectiveProjection<Q,d> &pProjection,
                     const opengl<Q,d-1> &pLoweRenderer)
                    : transformation(pTransformation), projection(pProjection), lowerRenderer(pLoweRenderer)
                    {}

                void drawLine
                    (const typename geometry::euclidian::space<Q,d>::vector &pA,
                     const typename geometry::euclidian::space<Q,d>::vector &pB) const;

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const;

            protected:
                const geometry::transformation<Q,d> &transformation;
                const geometry::perspectiveProjection<Q,d> &projection;
                const opengl<Q,d-1> &lowerRenderer;
        };

#if defined(GL3D)
        template<typename Q>
        class opengl<Q,3>
        {
            public:
                opengl
                    (const geometry::transformation<Q,3> &pTransformation)
                    : transformation(pTransformation)
                    {}

                void drawLine
                    (const typename geometry::euclidian::space<Q,3>::vector &pA,
                     const typename geometry::euclidian::space<Q,3>::vector &pB) const;

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,3>::vector> &pV) const;

            protected:
                const geometry::transformation<Q,3> &transformation;
        };
#endif

        template<typename Q>
        class opengl<Q,2>
        {
            public:
                opengl
                    (const geometry::transformation<Q,2> &pTransformation)
                    : transformation(pTransformation)
                    {}

                void drawLine
                    (const typename geometry::euclidian::space<Q,2>::vector &pA,
                     const typename geometry::euclidian::space<Q,2>::vector &pB) const;

                template<unsigned int q>
                void drawFace
                    (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV) const;

            protected:
                const geometry::transformation<Q,2> &transformation;
        };

        template<typename Q, unsigned int d>
        void opengl<Q,d>::drawLine
            (const typename geometry::euclidian::space<Q,d>::vector &pA,
             const typename geometry::euclidian::space<Q,d>::vector &pB) const
        {
            typename geometry::euclidian::space<Q,d-1>::vector A;
            typename geometry::euclidian::space<Q,d-1>::vector B;

            A = projection.project(transformation * pA);
            B = projection.project(transformation * pB);

            lowerRenderer.drawLine(A, B);
        }

        template<typename Q, unsigned int d>
        template<unsigned int q>
        void opengl<Q,d>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,d>::vector> &pV) const
        {
            math::tuple<q, typename geometry::euclidian::space<Q,d-1>::vector> V;

            for (unsigned int i = 0; i < q; i++)
            {
                V.data[i] = projection.project(transformation * pV.data[i]);
            }

            lowerRenderer.drawFace(V);
        }

#if defined(GL3D)
        template<typename Q>
        void opengl<Q,3>::drawLine
            (const typename geometry::euclidian::space<Q,3>::vector &pA,
             const typename geometry::euclidian::space<Q,3>::vector &pB) const
        {
            const typename geometry::euclidian::space<Q,3>::vector A = transformation * pA;
            const typename geometry::euclidian::space<Q,3>::vector B = transformation * pB;

            const GLdouble a0 = Q(A.data[0]);
            const GLdouble a1 = Q(A.data[1]);
            const GLdouble a2 = Q(A.data[2]);
            const GLdouble b0 = Q(B.data[0]);
            const GLdouble b1 = Q(B.data[1]);
            const GLdouble b2 = Q(B.data[2]);

            glBegin(GL_LINES);
            glNormal3f(a0, a1, a2);
            glVertex3d(a0, a1, a2);
            glNormal3f(b0, b1, b2);
            glVertex3d(b0, b1, b2);
            glEnd();
        }

        template<typename Q>
        template<unsigned int q>
        void opengl<Q,3>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,3>::vector> &pV) const
        {
            glBegin(GL_POLYGON);
            for (unsigned int i = 0; i < q; i++)
            {
                const typename geometry::euclidian::space<Q,3>::vector V = transformation * pV.data[i];

                const GLdouble a0 = Q(V.data[0]);
                const GLdouble a1 = Q(V.data[1]);
                const GLdouble a2 = Q(V.data[2]);
                glNormal3f(a0, a1, a2);
                glVertex3d(a0, a1, a2);
            }
            glEnd();
        }
#endif

        template<typename Q>
        void opengl<Q,2>::drawLine
            (const typename geometry::euclidian::space<Q,2>::vector &pA,
             const typename geometry::euclidian::space<Q,2>::vector &pB) const
        {
            const typename geometry::euclidian::space<Q,2>::vector A = transformation * pA;
            const typename geometry::euclidian::space<Q,2>::vector B = transformation * pB;

            const GLdouble a0 = Q(A.data[0]);
            const GLdouble a1 = Q(A.data[1]);
            const GLdouble b0 = Q(B.data[0]);
            const GLdouble b1 = Q(B.data[1]);

            glBegin(GL_LINES);
            glVertex2d(a0, a1);
            glVertex2d(b0, b1);
            glEnd();
        }

        template<typename Q>
        template<unsigned int q>
        void opengl<Q,2>::drawFace
            (const math::tuple<q, typename geometry::euclidian::space<Q,2>::vector> &pV) const
        {
            glBegin(GL_POLYGON);
            for (unsigned int i = 0; i < q; i++)
            {
                const typename geometry::euclidian::space<Q,2>::vector V = transformation * pV.data[i];

                const GLdouble a0 = Q(V.data[0]);
                const GLdouble a1 = Q(V.data[1]);
                glVertex2d(a0, a1);
            }
            glEnd();
        }
    };
};

#endif
