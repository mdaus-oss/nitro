/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
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
#pragma once
#ifndef CODA_OSS_sys_filesystem_h_INCLUDED_
#define CODA_OSS_sys_filesystem_h_INCLUDED_

#include <filesystem>

// always implement sys::filesystem::path
#include "sys/sys_filesystem.h"

#include "coda_oss/namespace_.h"
namespace coda_oss
{
namespace filesystem
{

using std::filesystem::path;
using std::filesystem::file_type;

using std::filesystem::current_path;
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

}
}

#endif  // CODA_OSS_sys_filesystem_h_INCLUDED_
