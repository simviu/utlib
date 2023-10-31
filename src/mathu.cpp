/*
   Author: Sherman Chen
   Create Time: 2023-10-31
   Email: schen@simviu.com
   Copyright(c): Simviu Inc.
   Website: https://www.simviu.com
 */

#include "ut/cutil.h"

using namespace ut;

float PolyCurve::calc(double t)const
{
    float r=0;
    float mi = 1.0;
    for(auto& ki : k_)
    {
        r += mi * ki;
        mi *= t;
    }
    return r;        
}  
