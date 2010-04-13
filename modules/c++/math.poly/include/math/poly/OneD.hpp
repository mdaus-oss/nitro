/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * math.poly-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <cmath>
#include <import/except.h>
#include <import/sys.h>

namespace math
{
namespace poly
{

template<typename _T>
_T 
OneD<_T>::operator () (double at) const
{
   _T lRet(0.0);
   double lAtPwr = 1.0;
   for (unsigned int lX = 0, lXEnd = mCoef.size() ; lX < lXEnd ; lX++)
   {
      lRet += mCoef[lX]*lAtPwr;
      lAtPwr *= at;
   }
   
   return lRet;
}

template<typename _T>
_T 
OneD<_T>::integrate(double start, double end) const
{
   _T lRet(0.0);
   double lEndAtPwr = end;
   double lStartAtPwr = start;
   double lDiv = 0;
   double lNewCoef;
   for (unsigned int lX = 0, lXEnd = mCoef.size() ; lX < lXEnd ; lX++)
   {
      lDiv = 1.0 / (lX + 1);
      lNewCoef = mCoef[lX] * lDiv;
      lRet += lNewCoef * lEndAtPwr;
      lRet -= lNewCoef * lStartAtPwr;
      lEndAtPwr *= end;
      lStartAtPwr *= start;
   }
   return lRet;
}

template<typename _T>
OneD<_T>
OneD<_T>::derivative() const
{
   OneD<_T> lRet;
   if (order() > 0)
   {
      lRet = OneD<_T>(order()-1);
      for (unsigned int lX = 0 ; lX < mCoef.size()-1 ; lX++)
      {
         lRet[lX] = mCoef[lX+1] * (lX+1);
      }
   }
   return lRet;
}

template<typename _T>
_T& 
OneD<_T>::operator [] (unsigned int idx)
{
   if (idx < mCoef.size())
   {
      return (mCoef[idx]);
   }
   else
   {
      std::stringstream lStr;
      lStr << "idx(" << idx << ") not within range [0..." << mCoef.size() << ")";
      std::string lMsg(lStr.str());
      throw(except::IndexOutOfRangeException(Ctxt(lMsg)));
   }
}


template<typename _T>
_T 
OneD<_T>::operator [] (unsigned int idx) const
{
   _T lRet(0.0);
   if (idx < mCoef.size())
   {
      lRet = mCoef[idx];
   }
   else
   {
      std::stringstream lStr;
      lStr << "idx(" << idx << ") not within range [0..." << mCoef.size() << ")";
      std::string lMsg(lStr.str());
      throw(except::IndexOutOfRangeException(Ctxt(lMsg)));
   }
   return lRet;
}

template<typename _T>
std::ostream& 
operator << (std::ostream& out, const OneD<_T>& p)
{
   for (unsigned int lX = 0 ; lX < p.mCoef.size() ; lX++)
   {
      out << p[lX] << "*y^" << lX << " ";
   }
   return out;
}

template<typename _T>
OneD<_T>& 
OneD<_T>::operator *= (double cv) 
{
   for (unsigned int lX = 0, lXEnd = mCoef.size() ; lX < lXEnd ; lX++)
   {
      mCoef[lX] *= cv;
   }
   return *this;
}

template<typename _T>
OneD<_T> 
OneD<_T>::operator * (double cv) const
{
   OneD<_T> lRet(*this);
   lRet *= cv;
   return lRet;
}

template<typename _T>
OneD<_T> 
operator * (double cv, const OneD<_T>& p) 
{
   return p*cv;
}

template<typename _T>
OneD<_T>& 
OneD<_T>::operator *= (const OneD<_T>& p) 
{
   OneD<_T> lTmp(order()+p.order());
   for (unsigned int lX = 0, lXEnd = mCoef.size() ; lX < lXEnd ; lX++)
   {
      for (unsigned int lY = 0, lYEnd = p.mCoef.size() ; lY < lYEnd ; lY++)
      {
         lTmp.mCoef[lX+lY] += mCoef[lX] * p.mCoef[lY];
      }
   }
   *this = lTmp;
   return *this;
}

template<typename _T>
OneD<_T> 
OneD<_T>::operator * (const OneD<_T>& p) const
{
   OneD<_T> lRet(*this);
   lRet *= p;
   return lRet;
}

template<typename _T>
OneD<_T>& 
OneD<_T>::operator += (const OneD<_T>& p) 
{
   OneD<_T> lTmp(std::max<int>(order(),p.order()));
   for (unsigned int lX = 0, lXEnd = mCoef.size() ; lX < lXEnd ; lX++)
   {
      lTmp.mCoef[lX] = mCoef[lX];
   }
   for (unsigned int lX = 0, lXEnd = p.mCoef.size() ; lX < lXEnd ; lX++)
   {
      lTmp.mCoef[lX] += p.mCoef[lX];
   }
   *this = lTmp;
   return *this;
}

template<typename _T>
OneD<_T> 
OneD<_T>::operator + (const OneD<_T>& p) const
{
   OneD<_T> lRet(*this);
   lRet += p;
   return lRet;
}

template<typename _T>
OneD<_T>& 
OneD<_T>::operator -= (const OneD<_T>& p) 
{
   OneD<_T> lTmp(std::max<int>(order(),p.order()));
   for (unsigned int lX = 0, lXEnd = mCoef.size() ; lX < lXEnd ; lX++)
   {
      lTmp.mCoef[lX] = mCoef[lX];
   }
   for (unsigned int lX = 0, lXEnd = p.mCoef.size() ; lX < lXEnd ; lX++)
   {
      lTmp.mCoef[lX] -= p.mCoef[lX];
   }
   *this = lTmp;
   return *this;
}

template<typename _T>
OneD<_T> 
OneD<_T>::operator - (const OneD<_T>& p) const
{
   OneD<_T> lRet(*this);
   lRet -= p;
   return lRet;
}

template<typename _T>
OneD<_T>& 
OneD<_T>::operator /= (double cv) 
{
   double lRecipCV = 1.0/cv;
   for (unsigned int lX = 0, lXEnd = mCoef.size() ; lX < lXEnd ; lX++)
   {
      mCoef[lX] *= lRecipCV;
   }
   return *this;
}

template<typename _T>
OneD<_T> 
OneD<_T>::operator / (double cv) const
{
   OneD<_T> lRet(*this);
   lRet *= (1.0/cv);
   return lRet;
}
/*
template<typename _T>
bool
OneD<_T>::operator == (const OneD<_T>& p) const
{
    if (this == &p)
        return true;

    unsigned int lMinSize = std::min<unsigned int>(mCoef.size(),
            p.mCoef.size());

    for (unsigned int lX = 0 ; lX < lMinSize ; lX++)
        if (!math::linear::equals(mCoef[lX], p.mCoef[lX]))
            return false;

    _T dflt(0.0);

    // Cover case where one polynomial has more coefficients than the other.
    if (mCoef.size() > p.mCoef.size())
    {
        for (unsigned int lX = lMinSize ; lX < mCoef.size() ; lX++)
            if (!math::linear::equals(mCoef[lX], dflt))
                return false;
    }
    else if (mCoef.size() < p.mCoef.size())
    {
        for (unsigned int lX = lMinSize; lX < p.mCoef.size(); lX++)
            if (!math::linear::equals(p.mCoef[lX], dflt))
                return false;
    }

    return true;
}
*/
template<typename Vector_T> OneD<double> fit(const Vector_T& x,
					     const Vector_T& y,
					     int numCoeffs)
{
        
    math::linear::Vector<double> vy(y);
    // n is polynomial order
    int sizeX = x.size();

    math::linear::Matrix2D<double> A(x.size(), numCoeffs + 1);

    for (int i = 0; i < sizeX; i++)
    {
        // The c0 coefficient is a freebie
        A(i, 0) = 1;
        double v = x[i];
        A(i, 1) = v;
        for (int j = 2; j <= numCoeffs; j++)
        {
            A(i, j) = pow(v, j);
        }
    }
    
    math::linear::Matrix2D<double> At = A.transpose();
    math::linear::Matrix2D<double> inv = inverse(At * A);
    math::linear::Matrix2D<double> B = inv * At;
    math::linear::Vector<double> c(B * vy.matrix());
    return math::poly::OneD<double>(c.vec());
}


} // poly
} // math
