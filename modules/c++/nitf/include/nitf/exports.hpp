#pragma once

#include "config/compiler_extensions.h"

// Need to specify how this code will be consumed, either NITRO_NITFCPP_LIB (static library)
// or NITRO_NITFCPP_DLL (aka "shared" library).  For DLLs, it needs to be set for BOTH
// "exporting" (building this code) and "importing" (consuming).
//
// Use Windows naming conventions (DLL, LIB) because this really only matters for _MSC_VER, see below.
#if !defined(NITRO_NITFCPP_LIB) && !defined(NITRO_NITFCPP_DLL)
    // **A huge HACK so this file can be commited.**
    // Windows clients must know whether the LIB is an static LIB or an export
    // LIB for a DLL (see comments below); thus, CODA_OSS_DLL/CODA_OSS_LIB
    // must always be set to the same value.  Having to add this to every
    // **vcxproj** project is a pain (and error-prone) ... so, the hack is to 
    // use *_FILE_OFFSET_BITS* and *_LARGEFILE_SOURCE* which are
    // only set by our **waf** and/or **cmake** build tools.
    #if defined(_FILE_OFFSET_BITS) || defined(_LARGEFILE_SOURCE)
        // **cmake** or **waf** build, assume static LIB
        #define NITRO_NITFCPP_LIB 1  // Static library, all symbols visible.
    #else
        // Not building with **cmake** or **waf**, assume we're in Visual Studio
        // and thus are using DLLs.
        #define NITRO_NITFCPP_DLL 1  // Symbols must be exported and imported (see below).
    #endif // defined(_FILE_OFFSET_BITS) || defined(_LARGEFILE_SOURCE)
#endif
#if defined(NITRO_NITFCPP_LIB) && defined(NITRO_NITFCPP_DLL)
    #error "Both NITRO_NITFCPP_LIB and NITRO_NITFCPP_DLL are #define'd'"
#endif
#if defined(NITRO_NITFCPP_EXPORTS) && defined(NITRO_NITFCPP_LIB)
    #error "Can't export from a LIB'"
#endif

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the NITRO_NITFCPP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// NITRO_NITFCPP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
// https://www.gnu.org/software/gnulib/manual/html_node/Exported-Symbols-of-Shared-Libraries.html
#ifdef NITRO_NITFCPP_EXPORTS
    #define NITRO_NITFCPP_API CODA_OSS_library_export
#else
    // Either building a static library (no NITRO_NITFCPP_EXPORTS) or
    // importing (not building) a shared library.

    // We need to know whether we're consuming (importing) a DLL or static LIB
    // The default is a static LIB as that's what existing code/builds expect.
    #ifdef NITRO_NITFCPP_DLL
        // Actually, it seems that the linker is able to figure this out from the .LIB, so 
        // there doesn't seem to be a need for __declspec(dllimport).  Clients don't
        // need to #define NITRO_NITFCPP_DLL ... ?  Well, almost ... it looks
        // like __declspec(dllimport) is needed to get virtual "inline"s (e.g., destructors) correct.
        #define NITRO_NITFCPP_API CODA_OSS_library_import
    #else
        #define NITRO_NITFCPP_API /* "importing" a static LIB */
    #endif
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4251) // '...' : class '...' needs to have dll-interface to be used by clients of struct '...'
#endif

