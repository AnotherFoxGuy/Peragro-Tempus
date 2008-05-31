/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#if (_MSC_VER >= 1300)

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "time.h"

#define snprintf _snprintf

#include "dbghelp.h"

const char* prefix;

LONG WriteDump(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
#ifndef _DEBUG
  if (pExceptionInfo->ExceptionRecord->ExceptionFlags == EXCEPTION_NONCONTINUABLE)
  {
    int rv = MessageBox(0, "We appology for this ruff interruption of your gameplay "
      "but we think you played long enough now and should take a short break.....\n\n"
      "Ok, ok, I admit it, it was a bug that crashed the game :/\n\n"
      "May we write a crash report into a file and ask you to send it to the "
      "Peragro Tempus Development Team?\nI will help us fixing the game and "
      "allows you to play with less 'Interruptions' :)\n\n"
      "Thanks in advance.",
      "Unexpected Interruption of Game Experience", MB_YESNO);

    if (rv == IDYES)
    {
#endif // _DEBUG
      HINSTANCE hinst = LoadLibraryA("dbghelp.dll");

      char dmpfile[256];
      snprintf(dmpfile, 256, "%s-%d.dmp", prefix, time(0));

      typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
        HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
        CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
        );

      HANDLE file = CreateFileA(dmpfile, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

      MINIDUMPWRITEDUMP dumper = (MINIDUMPWRITEDUMP)GetProcAddress(hinst, "MiniDumpWriteDump");

      MINIDUMP_EXCEPTION_INFORMATION mdExcInf;
      mdExcInf.ClientPointers = true;
      mdExcInf.ExceptionPointers = pExceptionInfo;
      mdExcInf.ThreadId = GetCurrentThreadId();

      if (dumper)
        dumper(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithIndirectlyReferencedMemory, &mdExcInf, 0, 0);
#ifndef _DEBUG
    }
  }
#endif // _DEBUG

  return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void setWinCrashDump(const char* file_prefix)
{
#if (_MSC_VER >= 1300)
  prefix = file_prefix;
  PTOP_LEVEL_EXCEPTION_FILTER dmp = (PTOP_LEVEL_EXCEPTION_FILTER)WriteDump;
  SetUnhandledExceptionFilter( dmp );
#endif
}
