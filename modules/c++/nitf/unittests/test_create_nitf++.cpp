/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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


/**
 * This test serves as an example to show how one can use CompressedByteProvider
 * to create a NITF with J2K compression.
 *
 * While this demonstrates use of CompressedByteProvider, the module for
 * actually compressing the image data is not a part of NITRO. Actual data will
 * be uncompressed. Search for COMPRESSION comments to see an explanation
 * of what will change with a compressor.
 */

#include <string>
#include <sstream>
#include <thread>
#include <array>
#include <memory>

#include <import/nitf.hpp>
#include <nitf/CompressedByteProvider.hpp>

#include "TestCase.h"

#include "../c/nitf/unittests/nitro_image_.c"

void populateFileHeader(nitf::Record& record, const std::string& title)
{
    /* the file header is already created, so just grab it */
    nitf::FileHeader header = record.getHeader();

    header.getOriginStationID().set("github.com");
    header.getFileTitle().set(title);
}


void setCornersFromDMSBox(nitf::ImageSubheader& header)
{
    /*
     *  You could do this in degrees as easily
     *  but this way we get to show off some new utilities
     */
    int latTopDMS[3] = { 42, 17, 50 };
    int latBottomDMS[3] = { 42, 15, 14 };
    int lonEastDMS[3] = { -83, 42, 12 };
    int lonWestDMS[3] = { -83, 45, 44 };

    double latTopDecimal =
        nitf::Utils::geographicToDecimal(latTopDMS[0],
            latTopDMS[1],
            latTopDMS[2]);

    double latBottomDecimal =
        nitf::Utils::geographicToDecimal(latBottomDMS[0],
            latBottomDMS[1],
            latBottomDMS[2]);

    double lonEastDecimal =
        nitf::Utils::geographicToDecimal(lonEastDMS[0],
            lonEastDMS[1],
            lonEastDMS[2]);


    double lonWestDecimal =
        nitf::Utils::geographicToDecimal(lonWestDMS[0],
            lonWestDMS[1],
            lonWestDMS[2]);

    double corners[4][2];
    corners[0][0] = latTopDecimal;     corners[0][1] = lonWestDecimal;
    corners[1][0] = latTopDecimal;     corners[1][1] = lonEastDecimal;
    corners[2][0] = latBottomDecimal;  corners[2][1] = lonEastDecimal;
    corners[3][0] = latBottomDecimal;  corners[3][1] = lonWestDecimal;

    header.setCornersFromLatLons(NITF_CORNERS_DECIMAL, corners);
}

namespace test_create_nitf_with_byte_provider
{
    void addImageSegment(nitf::Record& record,
        bool shouldCompress = false)
    {
        nitf::ImageSegment segment = record.newImageSegment();
        nitf::ImageSubheader header = segment.getSubheader();

        header.getImageId().set("NITRO-TEST");
        header.getImageDateAndTime().set("20080812000000");

        if (shouldCompress)
        {
            header.getImageCompression().set("C8");
        }

        /* Set the geo-corners to Ann Arbor, MI */
        setCornersFromDMSBox(header);

        const size_t NUM_BANDS = 1;
        std::vector<nitf::BandInfo> bands(NUM_BANDS, nitf::BandInfo());
        for (size_t ii = 0; ii < bands.size(); ++ii)
        {
            bands[ii].init(nitf::Representation::M,   /* The band representation, Nth band */
                " ",       /* The band subcategory */
                "N",       /* The band filter condition */
                "   ");     /* The band standard image filter code */

        }

        const std::string iRep = "MONO";
        header.setPixelInformation("INT",     /* Pixel value type */
            8,         /* Number of bits/pixel */
            8,         /* Actual number of bits/pixel */
            "R",       /* Pixel justification */
            iRep,      /* Image representation */
            "VIS",     /* Image category */
            bands);    /* Band information object list */



/* for fun, let's add a comment */
        header.insertImageComment("NITF generated by NITRO", 0);

        // The image mode P is part of an awful hack to workaround us not
        // having a compression plugin for blocking mode P.
        // If you run this test, it will throw an error. The error will reference
        // a file in the j2k plugin. To make this test run, go to the file
        // and disable the check for blocking mode B.
        // To the best of my knowledge, nothing bad happens as a result.
        header.setBlocking(NITRO_IMAGE.height, /*!< The number of rows */
            NITRO_IMAGE.width,  /*!< The number of columns */
            NITRO_IMAGE.height, /*!< The number of rows/block */
            NITRO_IMAGE.width,  /*!< The number of columns/block */
            "P");               /*!< Image mode */
    }


    void writeNITF(nitf::Record& record, const std::string& filename)
    {
        const size_t NUM_BANDS = 1;
        /*
         * COMPRESSION:
         * Right now, this is writing a single-segment single-block uncompressed
         * NITF. If you want to compress, you would compress the data here.
         *
         * bytesPerBlock has an element for each image segment. Each image segment
         * element has an element for each block. If you're compressing into a
         * single tile, you can simple populate the single element with the
         * compressed size of the image.
         *
         * If you are compressing to multiple tiles (blocks), each tile will have
         * a different size, hence the need for a vector.
         * Once you have CompressedByteProvider constructed, everything else
         * should work the same
         */
        std::vector<std::vector<size_t> > bytesPerBlock(1);
        bytesPerBlock[0].push_back(NITRO_IMAGE.width * NITRO_IMAGE.height * NUM_BANDS);
        nitf::CompressedByteProvider byteProvider(record,
            bytesPerBlock);
        nitf::Off fileOffset;
        nitf::NITFBufferList buffers;
        byteProvider.getBytes(NITRO_IMAGE.data, 0, NITRO_IMAGE.height,
            fileOffset, buffers);
        io::FileOutputStream outputStream(filename);
        for (size_t ii = 0; ii < buffers.mBuffers.size(); ++ii)
        {
            outputStream.write(
                static_cast<const std::byte*>(buffers.mBuffers[ii].mData),
                buffers.mBuffers[ii].mNumBytes);
        }
    }

    void testCreate(const std::string& outname,
        bool shouldCompress = false)
    {
        nitf::Record record;
        populateFileHeader(record, outname);
        addImageSegment(record, shouldCompress);
        writeNITF(record, outname);
    }

    bool testRead(const std::string& pathname)
    {
        const size_t NUM_BANDS = 1;
        nitf::IOHandle handle(pathname, NITF_ACCESS_READONLY, NITF_OPEN_EXISTING);
        nitf::Reader reader;
        nitf::Record record = reader.read(handle);

        for (int ii = 0; ii < static_cast<int>(record.getNumImages()); ++ii)
        {
            nitf::ImageReader imageReader = reader.newImageReader(ii);
            uint64_t blockSize;
            // Read one block. It should match the first blockSize points of the
            // image. If it does, we got the blocking mode right.
            auto block = reinterpret_cast<const unsigned char*>(imageReader.readBlock(0, &blockSize));
            const size_t imageLength = NITRO_IMAGE.width * NITRO_IMAGE.height;

            for (size_t jj = 0; jj < imageLength * NUM_BANDS; ++jj)
            {
                if (block[jj] != NITRO_IMAGE.data[jj])
                {
                    std::cerr << "Image data doesn't match" << std::endl;
                    return false;
                }
            }
        }
        return true;
    }
}

TEST_CASE(test_create_nitf_with_byte_provider_test)
{
    // We can't actually compress. This is just for illustration.
    const bool shouldCompress = false;
    const std::string outname("test_create.nitf");

    test_create_nitf_with_byte_provider::testCreate(outname, shouldCompress);
    const auto result = test_create_nitf_with_byte_provider::testRead(outname);
    TEST_ASSERT(result);
}

namespace test_create_nitf
{
    static const nitf::Representation RGB[] = { nitf::Representation::R,  nitf::Representation::G,  nitf::Representation::B };

    void addImageSegment(nitf::Record& record, bool isMono = false,
        bool shouldCompress = false)
    {
        nitf::ImageSegment segment = record.newImageSegment();
        nitf::ImageSubheader header = segment.getSubheader();

        header.getImageId().set("NITRO-TEST");
        header.getImageDateAndTime().set("20080812000000");

        if (shouldCompress)
        {
            header.getImageCompression().set("C8");
        }

        /* Set the geo-corners to Ann Arbor, MI */
        setCornersFromDMSBox(header);

        const auto NUM_BANDS = static_cast<size_t>(isMono ? 1 : 3);
        std::vector<nitf::BandInfo> bands(NUM_BANDS, nitf::BandInfo());
        for (size_t ii = 0; ii < bands.size(); ++ii)
        {
            bands[ii].init(RGB[ii],   /* The band representation, Nth band */
                " ",       /* The band subcategory */
                "N",       /* The band filter condition */
                "   ");     /* The band standard image filter code */

        }

        const std::string iRep = isMono ? "MONO" : "RGB";
        header.setPixelInformation("INT",     /* Pixel value type */
            8,         /* Number of bits/pixel */
            8,         /* Actual number of bits/pixel */
            "R",       /* Pixel justification */
            iRep,      /* Image representation */
            "VIS",     /* Image category */
            bands);    /* Band information object list */



/* for fun, let's add a comment */
        header.insertImageComment("NITF generated by NITRO", 0);

        // The image mode P is part of an awful hack to workaround us not
        // having a compression plugin for blocking mode P.
        // If you run this test, it will throw an error. The error will reference
        // a file in the j2k plugin. To make this test run, go to the file
        // and disable the check for blocking mode B.
        // To the best of my knowledge, nothing bad happens as a result.
        header.setBlocking(NITRO_IMAGE.height, /*!< The number of rows */
            NITRO_IMAGE.width,  /*!< The number of columns */
            NITRO_IMAGE.height, /*!< The number of rows/block */
            NITRO_IMAGE.width,  /*!< The number of columns/block */
            "P");               /*!< Image mode */
    }


    void writeNITF(nitf::Record& record, const std::string& filename,
        bool isMono = false)
    {
        const int NUM_BANDS = isMono ? 1 : 3;
        nitf::IOHandle out(filename, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        nitf::Writer writer;
        writer.prepare(out, record);

        nitf::ImageWriter imageWriter = writer.newImageWriter(0);
        nitf::ImageSource imageSource;

        /* make one bandSource per band */
        for (int ii = 0; ii < NUM_BANDS; ++ii)
        {
            nitf::BandSource bandSource = nitf::MemorySource(
                (char*)NITRO_IMAGE.data,
                NITRO_IMAGE.width * NITRO_IMAGE.height,
                ii, 1, 2);
            imageSource.addBand(bandSource);
        }

        imageWriter.setWriteCaching(1);
        imageWriter.attachSource(imageSource);
        writer.write();
    }

    void testCreate(const std::string& outname, bool isMono = false,
        bool shouldCompress = false)
    {
        nitf::Record record;
        populateFileHeader(record, outname);
        addImageSegment(record, isMono, shouldCompress);
        writeNITF(record, outname, isMono);
    }

    bool testRead(const std::string& pathname, bool isMono = false,
        bool shouldCompress = false)
    {
        const int NUM_BANDS = isMono ? 1 : 3;
        nitf::IOHandle handle(pathname, NITF_ACCESS_READONLY, NITF_OPEN_EXISTING);
        nitf::Reader reader;
        nitf::Record record = reader.read(handle);

        for (int ii = 0; ii < static_cast<int>(record.getNumImages()); ++ii)
        {
            nitf::ImageReader imageReader = reader.newImageReader(ii);
            uint64_t blockSize;
            // Read one block. It should match the first blockSize points of the
            // image. If it does, we got the blocking mode right.
            auto block = reinterpret_cast<const unsigned char*>(imageReader.readBlock(0, &blockSize));
            const size_t imageLength = NITRO_IMAGE.width * NITRO_IMAGE.height;

            // The image data is interleaved by pixel. When feeding it to the
            // writer, we unpack to interleave by block. Now that we're reading
            // it back in, we have to interleave by pixel again to compare.
            // imageLength is the pixel length of a single band of image data
            for (size_t jj = 0; jj < imageLength * NUM_BANDS; ++jj)
            {
                size_t offset = jj / imageLength;
                size_t index = jj % imageLength;

                // Even though there's only one band, the pixel skip still
                // applies during the write
                size_t imageIndex = (offset)+(3 * index);

                // For this case, NITRO will have already undone our interleaving
                // while writing, so we can ignore the stuff above and just
                // compare directly
                if (!shouldCompress && !isMono)
                {
                    imageIndex = jj;
                }
                if (block[jj] != NITRO_IMAGE.data[imageIndex])
                {
                    std::cerr << "Image data doesn't match" << std::endl;
                    return false;
                }
            }
        }
        return true;
    }
}

TEST_CASE(test_create_nitf_test)
{
    const std::string outname("test_create.nitf");


    bool shouldCompress = false;

    bool isMono = true;
    test_create_nitf::testCreate(outname, isMono, shouldCompress);
    bool result = test_create_nitf::testRead(outname, isMono, shouldCompress);
    TEST_ASSERT(result);

    isMono = false;
    test_create_nitf::testCreate(outname, isMono, shouldCompress);
    result = test_create_nitf::testRead(outname, isMono, shouldCompress);
    TEST_ASSERT(result);

    // If we're compressing, we're using the J2K plugin, so please ensure
    // that it is on your NITF_PLUGIN_PATH
    std::string nitf_plugin_path;
    if (sys::OS().getEnvIfSet("NITF_PLUGIN_PATH", nitf_plugin_path))
    {
        TEST_ASSERT_FALSE(nitf_plugin_path.empty());
        shouldCompress = false; // TODO: true

        test_create_nitf::testCreate(outname, isMono, shouldCompress);
        result = test_create_nitf::testRead(outname, isMono, shouldCompress);
        TEST_ASSERT(result);

        isMono = true;
        test_create_nitf::testCreate(outname, isMono, shouldCompress);
        result = test_create_nitf::testRead(outname, isMono, shouldCompress);
        TEST_ASSERT(result);
    }
    else
    {
        TEST_ASSERT_TRUE(true);
    }
}


static void RecordThread_run()
    {
        nitf::Record record(NITF_VER_21);
        nitf::Writer writer;
        nitf::FileHeader header = record.getHeader();
        header.getFileHeader().set("NITF");
        header.getComplianceLevel().set("09");
        header.getSystemType().set("BF01");
        header.getOriginStationID().set("Bckyd");
        header.getFileTitle().set("FTITLE");
        header.getClassification().set("U");
        header.getMessageCopyNum().set("00000");
        header.getMessageNumCopies().set("00000");
        header.getEncrypted().set("0");
        header.getBackgroundColor().setRawData((char*)"000", 3);
        header.getOriginatorName().set("");
        header.getOriginatorPhone().set("");
        const std::string name = "ACFTB";
        //m.lock();
        (void) new nitf::TRE(name, name);

        std::string file;
        {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            file = ss.str() + ".ntf";
        }

        nitf::IOHandle output(file, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        writer.prepare(output, record);

        writer.write();
    }

TEST_CASE(test_mt_record)
{
    const int NTHR = 2;
    
    std::array<std::thread, NTHR> thrs;
    try
    {
        for (auto& thrs_i : thrs)
        {
            thrs_i = std::thread(RecordThread_run);
        }

        for (auto& thrs_i : thrs)
        {

            thrs_i.join();
        }
    }
    catch (const std::exception&)
    {
        TEST_ASSERT_TRUE(false);
    }

    TEST_ASSERT_TRUE(true);
}




TEST_MAIN(
    (void)argc;
    (void)argv;
    TEST_CHECK(test_create_nitf_with_byte_provider_test);
    TEST_CHECK(test_create_nitf_test);
    TEST_CHECK(test_mt_record);
)