/*******************************************************
* Creator: Martin Rudefelt
* Component: none
* Portability: non-native
*-------------------------------------------------------
* This file contains some global typedefs used by many
* parts of the system.
* The typedefs that should be supported by including
* this file are:
* int8, uint8, int16, uint16, int32, uint32, 
* int64, uint64, NULL, bool, true, false, (status_t?), 
* char, uchar
* It also containes #define:s that should be global and
* other useful stuff.
* All headers must include this file!
*******************************************************/

#ifndef __EM_GLOBALS
#define __EM_GLOBALS

/*************************************************************************/
// Misc stuff
/*************************************************************************/

#define EM_BIG_ENDIAN // Used by x86 architectures
//#define EM_LITTLE_ENDIAN // Used by PowerPC architectures


#define PLATFORM_BEOS 1

/*************************************************************************/
// End of misc stuff
/*************************************************************************/

/*************************************************************************/
// BeOS stuff
/*************************************************************************/

#ifdef PLATFORM_BEOS

#include <SupportDefs.h>  // many nice BeOS typedefs

#define EM_FILE_SEPARATOR "/"

#define EM_DEBUG // If debug code should be used

#ifdef EM_DEBUG
#include <Debug.h>
#define EM_new new // No special new here as of this moment
#else
#define EM_new new
#endif

#endif // PLATFORM_BEOS

/*************************************************************************/
// End of BeOS stuff
/*************************************************************************/

#include <new> // Needed for bad_alloc exception handling

#include "EMDebugOutput.h" // This one needs to be included last in the file

#endif // __EM_GLOBALS
