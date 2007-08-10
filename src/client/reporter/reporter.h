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

#ifndef REPORTER_H_
#define REPORTER_H_

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/objreg.h>
#include <iutil/vfs.h>

#include <string>
#include <fstream>

namespace PT
{

	enum LoggingLevel
	{
		Errors,			  // Bugs and errors are logged.
		Standard,		  // Bugs, errors and warnings are logged.
		Informative,	// Bugs, errors, warnings and notify are logged.
		Insane			  // Everything is logged.
	};

	enum SeverityLevel
	{
		Bug,			  
		Error,		  
		Warning,	
		Notify,
		Debug
	};

	class Reporter
	{
	private:
		static Reporter* reporter;

		iObjectRegistry* obj_reg;
		csRef<iVFS> vfs;

		LoggingLevel loggingLevel;

		std::ofstream logFile;
		//boost::shared_ptr<FS::ofstream> logFile;

	private:
		void GetFile (std::string fileName);

	public:
		Reporter(iObjectRegistry* obj_reg);
		virtual ~Reporter();

		static Reporter* GetInstance() { return reporter; }

		bool Initialize ();

		void SetLoggingLevel(LoggingLevel level) { Reporter::loggingLevel = level; }

		void Report(SeverityLevel severity, const char* msg, va_list arg);
	};

	bool Report(SeverityLevel severity, const char* msg, ...);

} // PT namespace

#endif // REPORTER_H_
