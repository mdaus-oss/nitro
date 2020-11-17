/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/SegmentSource.hpp"

#include "gsl/gsl.h"

nitf::SegmentMemorySource::SegmentMemorySource(const char * data, size_t size,
        nitf::Off start, int byteSkip, bool copyData)
{
    setNative(nitf_SegmentMemorySource_construct(data, gsl::narrow<nitf::Off>(size), start, byteSkip,
    		                                     copyData, &error));
    setManaged(false);
}

nitf::SegmentFileSource::SegmentFileSource(nitf::IOHandle & io,
        nitf::Off start, int byteSkip)
{
    setNative(nitf_SegmentFileSource_constructIO(io.getNative(),
                                                 start, byteSkip,
                                                 &error));
    setManaged(false);
    io.setManaged(true); //TODO unmanage on deletion
}

nitf::SegmentReaderSource::SegmentReaderSource(nitf::SegmentReader reader)
{
    setNative(nitf_SegmentReaderSource_construct(reader.getNativeOrThrow(),
                                                 &error));
    setManaged(false);
    reader.setManaged(true); //TODO unmanage on deletion
}
