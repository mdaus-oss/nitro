#pragma once

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1
#ifndef CODA_OSS_LIBRARY_SHARED
#define CODA_OSS_LIBRARY_SHARED 1
#endif

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 6385) // Reading invalid data from '...':  the readable size is '...' bytes, but '...' bytes may be read.
#pragma warning(disable: 6386) // Buffer overrun while writing to '...':  the writable size is '...' bytes, but '...' bytes might be written.

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <memory.h>
#include <stdint.h>

#pragma warning(push)
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#if defined(_WIN32)
#undef BIGENDIAN
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32")
#else
#include <netinet/in.h>
#endif

#include <jpeglib.h>
#include <jerror.h>
#pragma comment(lib, "openjpeg")
#pragma comment(lib, "jpeg")

#pragma warning(pop)


#include <sys/types.h>

#pragma warning(disable: 4505) // '...': unreferenced function with internal linkage has been removed
#pragma warning(disable: 4774) // '...' : format string expected in argument 3 is not a string literal
// We don't care about any padding added to structs
#pragma warning(disable: 4820) //  '...': '...' bytes padding added after data member '...'

// Assume any unreferenced functions will be used in other code
#pragma warning(disable: 4514) // '...': unreferenced inline function has been removed

// ???
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'

// ???
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

#if __cplusplus
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed/unsigned mismatch
#pragma warning(disable: 4800) // Implicit conversion from '...' to bool. Possible information loss
#pragma warning(disable: 4804) // '...: unsafe use of type '...' in operation
#pragma warning(disable: 5219) // implicit conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4191) // '...': unsafe conversion from '...' to '...'
#pragma warning(disable: 4244) // '...': conversion from '...' to '...', possible loss of data
#endif

#define OPJ_STATIC
