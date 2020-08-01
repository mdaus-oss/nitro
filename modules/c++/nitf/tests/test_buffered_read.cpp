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

#include <iostream>
#include <string>
#include <nitf/Record.hpp>
#include <nitf/Reader.hpp>
#include <nitf/BufferedReader.hpp>
#include <nitf/List.hpp>

namespace
{
void doRead(const std::string& inFile,
            size_t bufferSize)
{
    nitf::Reader reader;
    nitf::BufferedReader io(inFile, bufferSize);
    nitf::Record record = reader.readIO(io);
    std::vector<uint8_t> image;

    /*  Set this to the end, so we'll know when we're done!  */
    nitf::List imageList(record.getImages());
    size_t idx(0);
    for (nitf::ListIterator iter = imageList.begin();
         iter != imageList.end();
         ++iter, ++idx)
    {
        std::cout << "Reading image " << idx << "..." << std::endl;
        nitf::ImageSegment imageSegment(*iter);
        nitf::ImageSubheader subheader(imageSegment.getSubheader());

        nitf::SubWindow subWindow;
        subWindow.setStartRow(0);
        subWindow.setStartCol(0);
        subWindow.setNumRows(subheader.getNumRows());
        subWindow.setNumCols(subheader.getNumCols());
        subWindow.setNumBands(subheader.getBandCount());
        std::vector<nitf::Uint32> bandList;
        for (size_t ii = 0; ii < subWindow.getNumBands(); ++ii)
        {
            bandList.push_back(ii);
        }
        subWindow.setBandList(&bandList[0]);

        // Read in the image
        const size_t numBitsPerPixel(static_cast<nitf::Uint64>(subheader.getActualBitsPerPixel()));
        const size_t numBytesPerPixel = NITF_NBPP_TO_BYTES(numBitsPerPixel);

        const size_t numBytesPerBand =
            subWindow.getNumRows() *
            subWindow.getNumCols() *
            numBytesPerPixel;

        image.resize(numBytesPerBand * subWindow.getNumBands());

        if (!image.empty())
        {
            std::vector<nitf::Uint8 *> imagePtrs;
            nitf::Uint8 *imagePtr(&image[0]);
            for (size_t ii = 0;
                    ii < subWindow.getNumBands();
                    ++ii, imagePtr += numBytesPerBand)
            {
                imagePtrs.push_back(imagePtr);
            }

            nitf::ImageReader imageReader(reader.newImageReader(idx));

            int padded(0);
            imageReader.read(subWindow, &imagePtrs[0], &padded);
        }

        std::cout << "done.\n" << std::endl;
    }

    std::cout << "Read block info: \n------------------------------------"
              << "\nTotal number of blocks read: " << io.getNumBlocksRead()
              << "\nTotal number of bytes read: " << io.getTotalRead()
              << "\nOf those, " << io.getNumPartialBlocksRead()
              << " were less than buffer size " << bufferSize
              << "\nThe total time to read was: " << io.getTotalWriteTime()
              << "\n";
}
}

int main(int argc, char **argv)
{
    try
    {
        //  Check argv and make sure we are happy
        if (argc < 2 || argc > 3)
        {
            std::cout << "Usage: %s <input-file> (block-size - default is 8192)\n" << argv[0] << std::endl;
            exit(EXIT_FAILURE);
        }

        size_t blockSize = 8192;
        if (argc == 3)
            blockSize = str::toType<int>(argv[2]);

        // Check that wew have a valid NITF
        if (nitf::Reader::getNITFVersion(argv[1]) == NITF_VER_UNKNOWN )
        {
            std::cout << "Invalid NITF: " << argv[1] << std::endl;
            exit(EXIT_FAILURE);
        }

        doRead(argv[1], blockSize);

        return 0;
    }
    catch (except::Throwable & t)
    {
        std::cout << t.getMessage() << std::endl;
    }
}
