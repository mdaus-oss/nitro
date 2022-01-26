/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#ifndef CODA_OSS_coda_oss_type_traits_h_INCLUDED_
#define CODA_OSS_coda_oss_type_traits_h_INCLUDED_
#pragma once

#include <type_traits>

#include "coda_oss/type_traits_.h"
#include "bpstd/type_traits.hpp"
#include "coda_oss/bpstd_.h"

namespace coda_oss
{
#if CODA_OSS_coda_oss_USE_BPSTD_
template <typename T>
using is_trivially_copyable = bpstd::is_trivially_copyable<T>;
#else
using is_trivially_copyable = details::is_trivially_copyable<T>;
#endif
}

#endif  // CODA_OSS_coda_oss_type_traits_h_INCLUDED_
