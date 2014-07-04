/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        animation/EasingEquations.h
*  PURPOSE:     Easing equations for non-linear time interpolation
*  DEVELOPERS:  Kayl
*
*****************************************************************************/

/*
Disclaimer for Robert Penner's Easing Equations license:

TERMS OF USE - EASING EQUATIONS

Open source under the BSD License.

Copyright © 2001 Robert Penner
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 (M_PI / 2)
#endif

static double easeNone(double progress)
{
    return progress;
}

static double easeInQuad(double t)
{
    return t*t;
}

static double easeOutQuad(double t)
{
    return -t*(t-2);
}

static double easeInOutQuad(double t)
{
    t*=2.0;
    if (t < 1) {
        return t*t/double(2);
    } else {
        --t;
        return -0.5 * (t*(t-2) - 1);
    }
}

static double easeOutInQuad(double t)
{
    if (t < 0.5) return easeOutQuad (t*2)/2;
    return easeInQuad((2*t)-1)/2 + 0.5;
}

static double easeInElastic_helper(double t, double b, double c, double d, double a, double p)
{
    if (t==0) return b;
    double t_adj = (double)t / (double)d;
    if (t_adj==1) return b+c;

    double s;
    if(a < fabs(c)) {
        a = c;
        s = p / 4.0f;
    } else {
        s = p / (2 * M_PI) * asin(c / a);
    }

    t_adj -= 1.0f;
    return -(a*pow(2.0,10*t_adj) * sin( (t_adj*d-s)*(2*M_PI)/p )) + b;
}

static double easeInElastic(double t, double a, double p)
{
    return easeInElastic_helper(t, 0, 1, 1, a, p);
}

static double easeOutElastic_helper(double t, double /*b*/, double c, double /*d*/, double a, double p)
{
    if (t==0) return 0;
    if (t==1) return c;

    double s;
    if(a < c) {
        a = c;
        s = p / 4.0f;
    } else {
        s = p / (2 * M_PI) * asin(c / a);
    }

    return (a*pow(2.0,-10*t) * sin( (t-s)*(2*M_PI)/p ) + c);
}

static double easeOutElastic(double t, double a, double p)
{
    return easeOutElastic_helper(t, 0, 1, 1, a, p);
}

static double easeInOutElastic(double t, double a, double p)
{
    if (t==0) return 0.0;
    t*=2.0;
    if (t==2) return 1.0;

    double s;
    if(a < 1.0) {
        a = 1.0;
        s = p / 4.0f;
    } else {
        s = p / (2 * M_PI) * asin(1.0 / a);
    }

    if (t < 1) return -.5*(a*pow(2.0,10*(t-1)) * sin( (t-1-s)*(2*M_PI)/p ));
    return a*pow(2.0,-10*(t-1)) * sin( (t-1-s)*(2*M_PI)/p )*.5 + 1.0;
}

static double easeOutInElastic(double t, double a, double p)
{
    if (t < 0.5) return easeOutElastic_helper(t*2, 0, 0.5, 1.0, a, p);
    return easeInElastic_helper(2*t - 1.0, 0.5, 0.5, 1.0, a, p);
}

static double easeInBack(double t, double s)
{
    return t*t*((s+1)*t - s);
}

static double easeOutBack(double t, double s)
{
    t-= double(1.0);
    return t*t*((s+1)*t+ s) + 1;
}

static double easeInOutBack(double t, double s)
{
    t *= 2.0;
    if (t < 1) {
        s *= 1.525f;
        return 0.5*(t*t*((s+1)*t - s));
    } else {
        t -= 2;
        s *= 1.525f;
        return 0.5*(t*t*((s+1)*t+ s) + 2);
    }
}

static double easeOutInBack(double t, double s)
{
    if (t < 0.5) return easeOutBack (2*t, s)/2;
    return easeInBack(2*t - 1, s)/2 + 0.5;
}

static double easeOutBounce_helper(double t, double c, double a)
{
    if (t == 1.0) return c;
    if (t < (4/11.0)) {
        return c*(7.5625*t*t);
    } else if (t < (8/11.0)) {
        t -= (6/11.0);
        return -a * (1. - (7.5625*t*t + .75)) + c;
    } else if (t < (10/11.0)) {
        t -= (9/11.0);
        return -a * (1. - (7.5625*t*t + .9375)) + c;
    } else {
        t -= (21/22.0);
        return -a * (1. - (7.5625*t*t + .984375)) + c;
    }
}

static double easeOutBounce(double t, double a)
{
    return easeOutBounce_helper(t, 1, a);
}

static double easeInBounce(double t, double a)
{
    return 1.0 - easeOutBounce_helper(1.0-t, 1.0, a);
}

static double easeInOutBounce(double t, double a)
{
    if (t < 0.5) return easeInBounce (2*t, a)/2;
    else return (t == 1.0) ? 1.0 : easeOutBounce (2*t - 1, a)/2 + 0.5;
}

static double easeOutInBounce(double t, double a)
{
    if (t < 0.5) return easeOutBounce_helper(t*2, 0.5, a);
    return 1.0 - easeOutBounce_helper (2.0-2*t, 0.5, a);
}

static double easeSineCurve(double t)
{
    return (sin(((t * M_PI * 2)) - M_PI_2) + 1) / 2;
}

static double easeCosineCurve(double t)
{
    return (cos(((t * M_PI * 2)) - M_PI_2) + 1) / 2;
}
