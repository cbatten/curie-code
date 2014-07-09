//========================================================================
// curie
//========================================================================
// This is the primary library used by the CURIE Academy. All of the
// components should be contained in other files and included here. The
// only exception is CurieTest.h which should be explicitly included in
// the unit test drivers.

#ifndef CURIE_H
#define CURIE_H

//------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------

#include <CurieLCD.h>
#include <CurieTest.h>
#include <CurieWifi.h>

//------------------------------------------------------------------------
// F() workaround
//------------------------------------------------------------------------
// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734

#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

#endif /* CURIE_H */

