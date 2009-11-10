// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl ReliableMulticast
// ------------------------------
#ifndef RELIABLEMULTICAST_EXPORT_H
#define RELIABLEMULTICAST_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (RELIABLEMULTICAST_HAS_DLL)
#  define RELIABLEMULTICAST_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && RELIABLEMULTICAST_HAS_DLL */

#if !defined (RELIABLEMULTICAST_HAS_DLL)
#  define RELIABLEMULTICAST_HAS_DLL 1
#endif /* !RELIABLEMULTICAST_HAS_DLL */

#if defined (RELIABLEMULTICAST_HAS_DLL) && (RELIABLEMULTICAST_HAS_DLL == 1)
#  if defined (RELIABLEMULTICAST_BUILD_DLL)
#    define ReliableMulticast_Export ACE_Proper_Export_Flag
#    define RELIABLEMULTICAST_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define RELIABLEMULTICAST_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* RELIABLEMULTICAST_BUILD_DLL */
#    define ReliableMulticast_Export ACE_Proper_Import_Flag
#    define RELIABLEMULTICAST_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define RELIABLEMULTICAST_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* RELIABLEMULTICAST_BUILD_DLL */
#else /* RELIABLEMULTICAST_HAS_DLL == 1 */
#  define ReliableMulticast_Export
#  define RELIABLEMULTICAST_SINGLETON_DECLARATION(T)
#  define RELIABLEMULTICAST_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* RELIABLEMULTICAST_HAS_DLL == 1 */

// Set RELIABLEMULTICAST_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (RELIABLEMULTICAST_NTRACE)
#  if (ACE_NTRACE == 1)
#    define RELIABLEMULTICAST_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define RELIABLEMULTICAST_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !RELIABLEMULTICAST_NTRACE */

#if (RELIABLEMULTICAST_NTRACE == 1)
#  define RELIABLEMULTICAST_TRACE(X)
#else /* (RELIABLEMULTICAST_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define RELIABLEMULTICAST_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (RELIABLEMULTICAST_NTRACE == 1) */

#endif /* RELIABLEMULTICAST_EXPORT_H */

// End of auto generated file.
