/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_mem_Span_h_INCLUDED_
#define CODA_OSS_mem_Span_h_INCLUDED_
#pragma once

#include "sys/CPlusPlus.h"
#include "gsl/gsl_span_.h"

namespace mem
{
    template <typename T>
    using Span = gsl::span<T>;

     template <typename T>
    inline Span<T> make_Span(T* d, size_t sz)
    {
        return gsl::make_span<T>(d, sz);
    }

     template <typename TContainer>
    inline Span<typename TContainer::value_type> make_Span(TContainer& c)
    {
        return gsl::make_span(c);
    }
    template <typename TContainer>
    inline Span<typename TContainer::value_type> make_Span(const TContainer& c)
    {
        return make_Span(const_cast<TContainer&>(c));
    }

#define CODA_OSS_mem_Span 202002L  // c.f., __cpp_lib_span
}



#endif  // CODA_OSS_mem_Span_h_INCLUDED_
