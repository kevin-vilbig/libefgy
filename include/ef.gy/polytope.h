/**\file
 * \brief Basic primitives
 *
 * This file contains code to create the meshes for basic primitives and to
 * present the results with the renderers provided by this library.
 *
 * 'Basic' primitives includes cubes, simplices, planes and spheres.
 *
 * \see parametric.h, ifs.h and flame.h for more complicated shapes, some of
 *      which are based on these primitives.
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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

#if !defined(EF_GY_POLYTOPE_H)
#define EF_GY_POLYTOPE_H

#include <ef.gy/euclidian.h>
#include <ef.gy/polar.h>
#include <vector>

namespace efgy
{
    namespace geometry
    {
        /**\brief Geometry parameters
         *
         * Contains parameters used by the shape-creating templates. You need
         * to have at least one of these for each data type you intend to use.
         *
         * \tparam Q Base data type for calculations.
         */
        template <typename Q>
        class parameters
        {
            public:
                Q polarRadius;
                Q polarPrecision;
                unsigned int iterations;
                unsigned int functions;
                unsigned int seed;
                bool preRotate;
                bool postRotate;
                unsigned int flameCoefficients;
        };

        template <typename Q, unsigned int d, unsigned int f, typename render>
        class polytope
        {
            public:
                polytope (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier)
                    : renderer(pRenderer), parameter(pParameter), precisionMultiplier(pMultiplier)
                    {}

                void renderSolid () const
                {
                    typename std::vector<math::tuple<f,typename euclidian::space<Q,d>::vector> >::const_iterator it = faces.begin();
                    typename std::vector<Q>::const_iterator itIndex = indices.begin();

                    while (it != faces.end())
                    {
                        if (itIndex == indices.end())
                        {
                            renderer.render::template drawFace<f> (*it);
                        }
                        else
                        {
                            renderer.render::template drawFace<f> (*it, *itIndex);
                            itIndex++;
                        }
                        it++;
                    }
                }

                static const unsigned int modelDimensionMinimum  = 2;
                static const unsigned int modelDimensionMaximum  = 0;
                static const unsigned int renderDimensionMinimum = 3;
                static const unsigned int renderDimensionMaximum = 0;

                static const unsigned int faceVertices = f;

            protected:
                render &renderer;
                const parameters<Q> &parameter;
                const Q &precisionMultiplier;

                std::vector<math::tuple<f,typename euclidian::space<Q,d>::vector> > faces;
                std::vector<Q> indices;
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class simplex : public polytope<Q,d,3,render>
        {
            public:
                typedef polytope<Q,d,3,render> parent;

                simplex (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = d;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                static unsigned int depth (void) { return od; }
                static unsigned int renderDepth (void) { return d; }
                static const char *id (void) { return "simplex"; }

                void recurse (const int r, typename polar::space<Q,d>::vector v, std::vector<typename euclidian::space<Q,d>::vector> &points)
                {
                    if (r == 0)
                    {
                        typename euclidian::space<Q,d>::vector A = v;
                        points.push_back(A);
                    }
                    else
                    {
                        const int q = r-1;

                        v.data[r] = 0;
                        recurse (q, v, points);
                        v.data[r] = Q(M_PI)/Q(1.5);
                        recurse (q, v, points);
                    }
                }

                void calculateObject (void)
                {
                    Q radius = parameter.polarRadius;

                    faces = std::vector<math::tuple<3,typename euclidian::space<Q,d>::vector> >();

                    std::vector<typename euclidian::space<Q,d>::vector> points;

                    typename polar::space<Q,d>::vector v;
                    v.data[0] = radius;
                    
                    const int r = od-1;
                    const int q = r-1;

                    v.data[r] = Q(-M_PI)/Q(1.5);
                    recurse (q, v, points);
                    v.data[r] = 0;
                    recurse (q, v, points);
                    v.data[r] = Q(M_PI)/Q(1.5);
                    recurse (q, v, points);

                    std::vector<typename euclidian::space<Q,d>::vector> points2;

                    for (typename std::vector<typename euclidian::space<Q,d>::vector>::iterator it1 = points.begin();
                         it1 != points.end(); it1++)
                    {
                        bool pointInSet = false;

                        for (typename std::vector<typename euclidian::space<Q,d>::vector>::iterator it2 = points2.begin();
                             it2 != points2.end(); it2++)
                        {
                            if (*it1 == *it2)
                            {
                                pointInSet = true;
                                break;
                            }
                        }

                        if (!pointInSet)
                        {
                            points2.push_back (*it1);
                        }
                    }

                    points = points2;

                    std::vector<typename euclidian::space<Q,d>::vector> usedPoints;

                    for (typename std::vector<typename euclidian::space<Q,d>::vector>::iterator it1 = points.begin();
                         it1 != points.end(); it1++)
                    {
                        const typename euclidian::space<Q,d>::vector A = *it1;

                        std::vector<typename euclidian::space<Q,d>::vector> usedPoints2;

                        for (typename std::vector<typename euclidian::space<Q,d>::vector>::iterator it2 = usedPoints.begin();
                             it2 != usedPoints.end(); it2++)
                        {
                            const typename euclidian::space<Q,d>::vector B = *it2;

                            for (typename std::vector<typename euclidian::space<Q,d>::vector>::iterator it3 = usedPoints2.begin();
                                 it3 != usedPoints2.end(); it3++)
                            {
                                const typename euclidian::space<Q,d>::vector C = *it3;

                                math::tuple<3,typename euclidian::space<Q,d>::vector> newTriangle;
                                newTriangle.data[0] = A;
                                newTriangle.data[1] = B;
                                newTriangle.data[2] = C;
                                faces.push_back(newTriangle);
                            }

                            usedPoints2.push_back(B);
                        }

                        usedPoints.push_back(A);
                    }
                }
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class cube : public polytope<Q,d,4,render>
        {
            public:
                typedef polytope<Q,d,4,render> parent;

                cube (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = d;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                static unsigned int depth (void) { return od; }
                static unsigned int renderDepth (void) { return d; }
                static const char *id (void) { return "cube"; }

                void calculateObject (void)
                {
                    Q diameter = parameter.polarRadius * Q(0.5);
                    
                    std::vector<math::tuple<2,typename euclidian::space<Q,d>::vector> > lines;
                    faces = std::vector<math::tuple<4,typename euclidian::space<Q,d>::vector> >();
                    
                    std::vector<typename euclidian::space<Q,d>::vector> points;
                    
                    typename euclidian::space<Q,d>::vector A;
                    
                    points.push_back (A);
                    
                    for (unsigned int i = 0; i < od; i++)
                    {
                        std::vector<typename euclidian::space<Q,d>::vector> newPoints;
                        std::vector<math::tuple<2,typename euclidian::space<Q,d>::vector> > newLines;
                        std::vector<math::tuple<4,typename euclidian::space<Q,d>::vector> > newFaces;
                        
                        for (typename std::vector<math::tuple<2,typename euclidian::space<Q,d>::vector> >::iterator it = lines.begin();
                             it != lines.end(); it++)
                        {
                            it->data[0].data[i] = -diameter;
                            it->data[1].data[i] = -diameter;
                            
                            math::tuple<2,typename euclidian::space<Q,d>::vector> newLine = *it;
                            
                            newLine.data[0].data[i] = diameter;
                            newLine.data[1].data[i] = diameter;
                            
                            newLines.push_back(newLine);
                            
                            math::tuple<4,typename euclidian::space<Q,d>::vector> newFace;
                            newFace.data[0] = newLine.data [0];
                            newFace.data[1] = newLine.data [1];
                            newFace.data[2] = it->data     [1];
                            newFace.data[3] = it->data     [0];
                            newFaces.push_back(newFace);
                        }
                        
                        for (typename std::vector<math::tuple<4,typename euclidian::space<Q,d>::vector> >::iterator it = faces.begin();
                             it != faces.end(); it++)
                        {
                            it->data[0].data[i] = -diameter;
                            it->data[1].data[i] = -diameter;
                            it->data[2].data[i] = -diameter;
                            it->data[3].data[i] = -diameter;
                            
                            math::tuple<4,typename euclidian::space<Q,d>::vector> newFace = *it;
                            newFace.data[0].data[i] = diameter;
                            newFace.data[1].data[i] = diameter;
                            newFace.data[2].data[i] = diameter;
                            newFace.data[3].data[i] = diameter;
                            newFaces.push_back(newFace);
                        }
                        
                        for (typename std::vector<typename euclidian::space<Q,d>::vector>::iterator it = points.begin();
                             it != points.end(); it++)
                        {
                            math::tuple<2,typename euclidian::space<Q,d>::vector> newLine;
                            
                            it->data[i] = -diameter;
                            
                            newLine.data[0] = *it;
                            newLine.data[1] = *it;
                            newLine.data[1].data[i] = diameter;
                            
                            newPoints.push_back(newLine.data[1]);
                            
                            lines.push_back(newLine);
                        }
                        
                        for (typename std::vector<typename euclidian::space<Q,d>::vector>::iterator it = newPoints.begin();
                             it != newPoints.end(); it++)
                        {
                            points.push_back(*it);
                        }
                        
                        for (typename std::vector<math::tuple<2,typename euclidian::space<Q,d>::vector> >::iterator it = newLines.begin();
                             it != newLines.end(); it++)
                        {
                            lines.push_back(*it);
                        }
                        
                        for (typename std::vector<math::tuple<4,typename euclidian::space<Q,d>::vector> >::iterator it = newFaces.begin();
                             it != newFaces.end(); it++)
                        {
                            faces.push_back(*it);
                        }
                    }
                }
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class plane : public polytope<Q,d,4,render>
        {
            public:
                typedef polytope<Q,d,4,render> parent;

                plane (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier)
                    {
                        calculateObject();
                    }

                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = 2;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                static unsigned int depth (void) { return od; }
                static unsigned int renderDepth (void) { return d; }
                static const char *id (void) { return "plane"; }

                void calculateObject (void)
                {
                    const Q s = parameter.polarRadius * Q(2);
                    const Q q = s/parameter.polarPrecision;

                    faces.clear();

                    for (Q i = -s; i <= s; i+=q)
                    {
                        for (Q j = -s; j <= s; j+=q)
                        {
                            math::tuple<4,typename euclidian::space<Q,d>::vector> newFace;

                            newFace.data[0].data[0] = i;
                            newFace.data[0].data[1] = j;
                            
                            newFace.data[1].data[0] = i+q;
                            newFace.data[1].data[1] = j;
                            
                            newFace.data[2].data[0] = i+q;
                            newFace.data[2].data[1] = j+q;
                            
                            newFace.data[3].data[0] = i;
                            newFace.data[3].data[1] = j+q;

                            faces.push_back(newFace);
                        }
                    }
                }
        };

        template <typename Q, unsigned int od, typename render, unsigned int d = od>
        class sphere : public polytope<Q,d,3,render>
        {
            public:
                typedef polytope<Q,d,3,render> parent;

                sphere (render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                    : parent(pRenderer, pParameter, pMultiplier),
                      step(Q(M_PI) / (parameter.polarPrecision * precisionMultiplier))
                    {
                        calculateObject();
                    }

                using parent::precisionMultiplier;
                using parent::parameter;
                using parent::renderSolid;
                using parent::renderer;
                using parent::faces;

                using parent::modelDimensionMinimum;
                static const unsigned int modelDimensionMaximum = d - 1;
                using parent::renderDimensionMinimum;
                using parent::renderDimensionMaximum;

                using parent::faceVertices;

                static unsigned int depth (void) { return od; }
                static unsigned int renderDepth (void) { return d; }
                static const char *id (void) { return "sphere"; }

                void recurse (const int r, typename polar::space<Q,d>::vector v)
                {
                    if (r == 0)
                    {
                        const typename euclidian::space<Q,d>::vector A = v;

                        for (unsigned int i = 1; i <= od; i++)
                        {
                            typename polar::space<Q,d>::vector v1 = v;

                            v1.data[i] += step;

                            const typename euclidian::space<Q,d>::vector B = v1;

                            math::tuple<3,typename euclidian::space<Q,d>::vector> newFace;

                            newFace.data[0] = A;
                            newFace.data[1] = B;

                            for (unsigned int j = 1; j <= od; j++)
                            {
                                if (i != j)
                                {
                                    v1 = v;
                                    v1.data[j] -= step;

                                    const typename euclidian::space<Q,d>::vector C = v1;
                                    newFace.data[2] = C;

                                    faces.push_back(newFace);
                                }
                            }
                        }
                    }
                    else
                    {
                        const int q = r-1;

                        for (Q i = Q(-M_PI); i < Q(M_PI); i+= step)
                        {
                            v.data[r] = i;
                            recurse (q, v);
                        }
                    }
                }

                void calculateObject (void)
                {
                    Q radius = parameter.polarRadius;
                    step = (Q(M_PI) / (parameter.polarPrecision * precisionMultiplier));

                    Q usedRadius = radius;

                    faces = std::vector<math::tuple<3,typename euclidian::space<Q,d>::vector> >();

                    typename polar::space<Q,d>::vector v;
                    v.data[0] = radius;

                    const int r = od;
                    const int q = r-1;

                    for (Q i = Q(-M_PI); i < Q(M_PI); i+= step)
                    {
                        v.data[r] = i;
                        recurse (q, v);
                    }
                }

                Q usedRadius;

            protected:
                Q step;
        };
    };
};

#endif
