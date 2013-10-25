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

#if !defined(EF_GY_FLAME_H)
#define EF_GY_FLAME_H

#include <ef.gy/ifs.h>

namespace efgy
{
    namespace geometry
    {
        namespace transformation
        {
            /* These transformations are based on the 'Fractal Flame Algorithm' paper by Scott Draves
               and Eric Reckase.
               See http://flam3.com/flame_draves.pdf for the original paper. */
            template <typename Q, unsigned int d>
            class flame : public affine<Q,d>
            {
                public:
                    flame (void) {}

                    using affine<Q,d>::transformationMatrix;

                    typename euclidian::space<Q,d>::vector operator *
                        (const typename euclidian::space<Q,d>::vector &pV) const
                    {
                        const typename euclidian::space<Q,d>::vector V = affine<Q,d>(*this) * pV;
                        typename euclidian::space<Q,d>::vector rv = V * coefficient[0];

                        for (unsigned int i = 1; i < coefficients; i++)
                        {
                            rv = rv + apply(i, V);
                        }

                        return rv;
                    }

                    static const unsigned int coefficients = 19;
                    Q coefficient[coefficients];

                protected:
                    typename euclidian::space<Q,d>::vector apply
                        (unsigned int f, const typename euclidian::space<Q,d>::vector &V) const
                    {
                        typename euclidian::space<Q,d>::vector rv;

                        if (coefficient[f] <= Q(0.))
                        {
                            return rv;
                        }

                        const Q theta = atan(V.data[0]/V.data[1]);
                        const Q phi   = atan(V.data[1]/V.data[0]);
                        const Q r2    = euclidian::lengthSquared<Q,d>(V);
                        const Q r     = sqrt(r2);
                        const Q omega = Q(std::rand() % 2) * Q(M_PI);
                        const Q delta = (std::rand() % 2) == 1 ? Q(1) : Q(-1);
                        const Q psi   = Q(std::rand() % 10000) / Q(10000);

                        switch (f)
                        {
                            case 0: // "linear"
                                rv = V;
                                break;
                            case 1: // "sinusoidal"
                                for (unsigned int i = 0; i < d; i++)
                                    rv.data[i] = sin(V.data[i]);
                                break;
                            case 2: // "spherical"
                                rv = V / r2;
                                break;
                            case 3: // "swirl"
                            {
                                const Q sinrsq = sin(r2);
                                const Q cosrsq = cos(r2);
                                for (unsigned int i = 0; i < d; i++)
                                    if ((i % 2 == 0) && (i < (d-1)))
                                        rv.data[i] = V.data[i] * sinrsq - V.data[(i+1)] * cosrsq;
                                    else
                                        rv.data[i] = V.data[(i-1)] * cosrsq + V.data[i] * sinrsq;
                            }
                                break;
                            case 4: // "horseshoe"
                                rv = V;
                                rv.data[0] = (V.data[0]-V.data[1])*(V.data[0]+V.data[1]);
                                rv.data[1] = Q(2) * V.data[0] * V.data[1];
                                rv = rv / r;
                                break;
                            case 5: // "polar"
                                rv = V;
                                rv.data[0] = theta/Q(M_PI);
                                rv.data[1] = sqrt(euclidian::lengthSquared<Q,d>(V)) - Q(1);
                                break;
                            case 6: // "handkerchief"
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 4)
                                    {
                                        case 0: rv.data[i] = sin(theta + r);
                                        case 1: rv.data[i] = cos(theta - r);
                                        case 3: rv.data[i] = sin(theta - r);
                                        case 4: rv.data[i] = cos(theta + r);
                                    }
                                rv = rv * r;
                                break;
                            case 7: // "heart"
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 4)
                                    {
                                        case 0: rv.data[i] =  sin(theta * r); break;
                                        case 1: rv.data[i] = -cos(theta * r); break;
                                        case 2: rv.data[i] = -sin(theta * r); break;
                                        case 3: rv.data[i] =  cos(theta * r); break;
                                    }
                                rv = rv * r;
                                break;
                            case 8: // "disc"
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 2)
                                    {
                                        case 0: rv.data[i] = sin(Q(M_PI) * r); break;
                                        case 1: rv.data[i] = cos(Q(M_PI) * r); break;
                                    }
                                rv = rv * theta / Q(M_PI);
                                break;
                            case 9: // "spiral"
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 4)
                                    {
                                        case 0: rv.data[i] = cos(theta) + sin(r); break;
                                        case 1: rv.data[i] = sin(theta) - cos(r); break;
                                        case 2: rv.data[i] = cos(theta) - sin(r); break;
                                        case 3: rv.data[i] = sin(theta) + cos(r); break;
                                    }
                                rv = rv / r;
                                break;
                            case 10: // "hyperbolic"
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 4)
                                    {
                                        case 0: rv.data[i] = sin(theta) / r; break;
                                        case 1: rv.data[i] = cos(theta) * r; break;
                                        case 2: rv.data[i] = sin(theta) * r; break;
                                        case 3: rv.data[i] = cos(theta) / r; break;
                                    }
                                break;
                            case 11: // "diamond"
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 2)
                                    {
                                        case 0: rv.data[i] = sin(theta) * cos(r); break;
                                        case 1: rv.data[i] = cos(theta) * sin(r); break;
                                    }
                                break;
                            case 12: // "ex"
                            {
                                const Q p0 = sin(theta + r),
                                        p1 = cos(theta - r),
                                        p2 = sin(theta - r),
                                        p3 = cos(theta + r);
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 4)
                                    {
                                        case 0: rv.data[i] = p0*p0*p0 + p1*p1*p1; break;
                                        case 1: rv.data[i] = p0*p0*p0 - p1*p1*p1; break;
                                        case 2: rv.data[i] = p2*p2*p2 + p3*p3*p3; break;
                                        case 3: rv.data[i] = p2*p2*p2 - p3*p3*p3; break;
                                    }
                                rv = rv / r;
                            }
                                break;
                            case 13: // "julia"
                            {
                                const Q thpo = theta/Q(2) + omega;
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 2)
                                    {
                                        case 0: rv.data[i] = cos(thpo); break;
                                        case 1: rv.data[i] = sin(thpo); break;
                                    }
                                rv = rv * sqrt(r);
                            }
                                break;
                            case 14: // "bent"
                                for (unsigned int i = 0; i < d; i++)
                                    switch ((i % 2) + ((V.data[0] < Q(0)) << 1) + ((V.data[1] < Q(0)) << 2))
                                    {
                                        case 0: rv.data[i] = V.data[i];        break;
                                        case 1: rv.data[i] = V.data[i];        break;
                                        case 2: rv.data[i] = V.data[i] * Q(2); break;
                                        case 3: rv.data[i] = V.data[i];        break;
                                        case 4: rv.data[i] = V.data[i];        break;
                                        case 5: rv.data[i] = V.data[i] / Q(2); break;
                                        case 6: rv.data[i] = V.data[i] * Q(2); break;
                                        case 7: rv.data[i] = V.data[i] / Q(2); break;
                                    }
                                break;
                            case 15: // "waves"
                                for (unsigned int i = 0; i < d; i++)
                                    if (i < (d-1))
                                        rv.data[i] = V.data[i]
                                                   * transformationMatrix.data[i][0]
                                                   * sin(V.data[(i+1)] / (transformationMatrix.data[d][i] * transformationMatrix.data[d][i]));
                                    else
                                        rv.data[i] = V.data[i]
                                                   * transformationMatrix.data[i][0]
                                                   * sin(V.data[0] / (transformationMatrix.data[d][i] * transformationMatrix.data[d][i]));
                                break;
                            case 16: // "fisheye"
                                for (unsigned int i = 0; i < d; i++)
                                    rv.data[i] = V.data[(d-i)];
                                rv = rv * Q(2) / (r + Q(1));
                                break;
                            case 17: // "popcorn"
                                for (unsigned int i = 0; i < d; i++)
                                    rv.data[i] = V.data[i] + transformationMatrix.data[d][i] * sin(tan(Q(3)*V.data[i]));
                                break;
                            case 18: // "exponential"
                                for (unsigned int i = 0; i < d; i++)
                                    switch (i % 2)
                                    {
                                        case 0: rv.data[i] = cos(M_PI*V.data[1]); break;
                                        case 1: rv.data[i] = sin(M_PI*V.data[1]); break;
                                    }
                                rv = rv * exp(V.data[0]-Q(1));
                                break;
                            default:
                               return rv;
                        }

                        return rv * coefficient[f];
                    }
            };

            template <typename Q, unsigned int d, unsigned int od = d>
            class randomFlame : public flame<Q,d>
            {
                public:
                    randomFlame(const parameters<Q> &pParameter)
                        {
                            transformationMatrix = randomAffine<Q,d,od>(pParameter).transformationMatrix;

                            for (unsigned int i = 0; i < coefficients; i++)
                            {
                                coefficient[i] = Q(std::rand()%10000)/Q(10000);
                            }

                            for (unsigned int nonzero = pParameter.flameCoefficients + 1;
                                 nonzero > pParameter.flameCoefficients; )
                            {
                                nonzero = 0;
                                for (unsigned int i = 0; i < coefficients; i++)
                                {
                                    if (coefficient[i] > Q(0.))
                                    {
                                        nonzero++;
                                    }
                                    else if (coefficient[i] < Q(0.))
                                    {
                                        coefficient[i] = Q(0.);
                                    }
                                }
                                
                                if (nonzero > pParameter.flameCoefficients)
                                {
                                    coefficient[(std::rand()%coefficients)] = Q(0.);
                                }
                            }
                            
                            Q coefficientsum = coefficient[0];

                            for (unsigned int i = 1; i < coefficients; i++)
                            {
                                coefficientsum = coefficientsum + coefficient[i];
                            }
                            
                            for (unsigned int i = 0; i < coefficients; i++)
                            {
                                coefficient[i] = coefficient[i] / coefficientsum;
                            }
                        }

                    using flame<Q,d>::transformationMatrix;
                    using flame<Q,d>::coefficient;
                    using flame<Q,d>::coefficients;
            };
        };

        namespace flame
        {
            /**\brief Random Fractal Flame primitive
             *
             * This class template creates random fractal flame-esque geometry
             * based on the current values of the seed and functions
             * parameters.
             *
             * The geometry created by this class can take all sorts of shapes,
             * depending on the transformations it creates; here's an example:
             *
             * \htmlonly
             * <iframe width="100%" height="600"
             *  src="http://www.youtube.com/embed/XzObtC66g3c"
             *  frameborder="0" allowfullscreen="true"/>
             * \endhtmlonly
             *
             * \tparam Q      The base data type to use for calculations.
             * \tparam od     The depth of the model.
             * \tparam render The renderer to use.
             * \tparam d      The render depth to use.
             */
            template <typename Q, unsigned int od, typename render, unsigned int d = od>
            class random : public ifs<Q,od,render,d,plane,2,transformation::flame>
            {
                public:
                    typedef ifs<Q,od,render,d,plane,2,transformation::flame> parent;

                    random(render &pRenderer, const parameters<Q> &pParameter, const Q &pMultiplier = 1)
                        : parent(pRenderer, pParameter, pMultiplier)
                        {
                            calculateObject();
                        }

                        void calculateObject (void)
                        {
                            functions.clear();

                            std::srand(parameter.seed);

                            const unsigned int nfunctions = parameter.functions;
                        
                            for (unsigned int i = 0; i < nfunctions; i++)
                            {
                                functions.push_back (transformation::randomFlame<Q,d,od>(parameter));
                            }
                        
                            parent::calculateObject();
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

                    using parent::functions;
                    using parent::calculateObject;

                    static unsigned int depth (void) { return od; }
                    static unsigned int renderDepth (void) { return d; }
                    static const char *id (void) { return "random-flame"; }
            };
        };
    };
};

#endif