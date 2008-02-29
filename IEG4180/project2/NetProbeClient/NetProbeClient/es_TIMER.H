/*
 * File         : es_timer.h
 * Module       : Timer Class
 * System       : Generic System
 * Project      : Foundation Classes Construction
 * Start Date   : 15th April 1994
 * Version      : 1.1
 * Version Date : 18 Sep 2002
 * Designer     : Jack Lee
 * Programmer   : Jack Lee
 * Copyright    : Jack Lee 1994-2002 All Rights Reserved
 * Descriptions : Header file.
 * Remarks      : Sample Timer Class for use in IEG4180 Projects
 * References   :
 *
 */

#define INTEGER_64_AVAILABLE 1
#ifndef     __ES_TIMER_H
#define     __ES_TIMER_H

///////////////////////////////////
////////// Include Files //////////
///////////////////////////////////



#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <dos.h>
#include <time.h>


///////////////////////////////////
////// Constants Definition ///////
///////////////////////////////////

//#define INTEGER_64_AVAILABLE

///////////////////////////////////
/// Global Variables Definition ///
///////////////////////////////////



///////////////////////////////////
/// Class/Function Declarations ///
///////////////////////////////////


/* Class Name     : ES_FlashTimer
 * Descriptions   : An accurate time-keeping class, supporting stop-watch like
 *                  features.
 *
 *
 * Notes          : This class is OS dependent.
 * Remarks        : All functions are in-lined.
 *
 */

class ES_FlashTimer
{
public:

   ES_FlashTimer()
   {
      pCounter = new LARGE_INTEGER;
      QueryPerformanceFrequency(pCounter);
#ifdef INTEGER_64_AVAILABLE
      NumPerUSec = *((__int64 *)(pCounter)) / 1000000;
      NumPerMSec = *((__int64 *)(pCounter)) / 1000;
#else
      NumPerUSec = C64tLD(*pCounter) / 1000000;
      NumPerMSec = C64tLD(*pCounter) / 1000;
#endif
      Start();
      StartX();
   }

   ~ES_FlashTimer()
   {
      delete pCounter;
   }

   long Start()
   {
      Refresh();
      StartMs = *pCounter;
#ifdef INTEGER_64_AVAILABLE
      return ((*(__int64 *)&StartMs)) / NumPerMSec;
#else
      return long(C64tLD(StartMs) / NumPerMSec);
#endif
   }

   long StartX()
   {
      Refresh();
      StartXMs  = *pCounter;
#ifdef INTEGER_64_AVAILABLE
      return ((*(__int64 *)&StartXMs)) / NumPerMSec;
#else
      return long(C64tLD(StartXMs) / NumPerMSec);
#endif
   }

   long Elapsed()
   {
      Refresh();
#ifdef INTEGER_64_AVAILABLE
      return (*((__int64 *)(pCounter)) - ((*(__int64 *)&StartMs)))/NumPerMSec;
#else
      return (C64tLD(*pCounter) - C64tLD(StartMs))/NumPerMSec;
#endif
   }

   long ElapseduSec()
   {
      Refresh();
#ifdef INTEGER_64_AVAILABLE
      return (*((__int64 *)(pCounter)) - ((*(__int64 *)&StartMs)))/NumPerUSec;
#else
      return (C64tLD(*pCounter) - C64tLD(StartMs))/NumPerUSec;
#endif
   }

   long ElapsedX()
   {
      Refresh();
#ifdef INTEGER_64_AVAILABLE
      return (*((__int64 *)(pCounter)) - ((*(__int64 *)&StartXMs)))/NumPerMSec;
#else
      return (C64tLD(*pCounter) - C64tLD(StartXMs))/NumPerMSec;
#endif
   }

   void Refresh()
   {
      QueryPerformanceCounter(pCounter);
   }

protected:

#ifdef INTEGER_64_AVAILABLE
   __int64        NumPerMSec,NumPerUSec;
#else  // Use long double instead.
   long double    NumPerMSec,NumPerUSec;
   long double    C64tLD(const LARGE_INTEGER &val)
   { return (long double) val.u.HighPart * 0xFFFFFFFF + val.u.LowPart; }
#endif

   LARGE_INTEGER  StartMs, StartXMs;
   LARGE_INTEGER  *pCounter;
};



///////////////////////////////////
////////// End of File ////////////
///////////////////////////////////

#endif
