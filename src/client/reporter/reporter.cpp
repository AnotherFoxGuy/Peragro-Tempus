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

#include "reporter.h"

#include <iutil/objreg.h>

#include <csutil/ansicommand.h>
#include <csutil/sysfunc.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace FS = boost::filesystem;

namespace PT
{
	Reporter* Reporter::reporter;

	Reporter::Reporter(iObjectRegistry* obj_reg) 
	{ 
		reporter = this; 
		Reporter::obj_reg = obj_reg;
		vfs = csQueryRegistry<iVFS> (obj_reg);
	}

	bool Reporter::Initialize ()
	{
		GetFile ("peragro.log");

		return true;
	}

	void Reporter::GetFile (std::string fileName)
	{
    printf("Creating log file '%s'!\n", fileName.c_str());

    boost::shared_ptr<FS::ofstream> file(new FS::ofstream (fileName.c_str()));
		logFile = file;
	}

	void Reporter::Report(SeverityLevel severity, const char* msg, va_list arg)
	{
		// Return when the severity is ignored on this logging level.
		if (loggingLevel == PT::Errors) 
		{
			if (severity > PT::Error) return;
		}
		else if (loggingLevel == PT::Standard) 
		{
			if (severity > PT::Warning) return;
		}
		else if (loggingLevel == PT::Informative) 
		{
			if (severity > PT::Notify) return;
		}

		// Print a prefix based on the severity.
		std::string prefix;
		switch (severity)
		{
		case PT::Bug:
			csPrintf (CS_ANSI_FM CS_ANSI_TEXT_BOLD_ON "BUG: " CS_ANSI_RST);
			prefix = "BUG: ";
			break;
		case PT::Error:
			csPrintf (CS_ANSI_FR CS_ANSI_TEXT_BOLD_ON "ERROR: " CS_ANSI_RST);
			prefix = "ERROR: ";
			break;
		case PT::Warning:
			csPrintf (CS_ANSI_FY CS_ANSI_TEXT_BOLD_ON "WARNING: " CS_ANSI_RST);
			prefix = "WARNING: ";
			break;
		case PT::Notify:
			csPrintf (CS_ANSI_TEXT_BOLD_ON "NOTIFY: " CS_ANSI_RST);
			prefix = "NOTIFY: ";
			break;
		case PT::Debug:
			csPrintf (CS_ANSI_FW CS_ANSI_TEXT_BOLD_ON "DEBUG: " CS_ANSI_RST);
			prefix = "DEBUG: ";
			break;
		}

		// Print the message to the console.
		csPrintfV(msg, arg);

		// Reset colors and do a new line.
		csPrintf("\n");
		csPrintf(CS_ANSI_RST);

		if (logFile)
		{
			// Prepare a string for file output.
			boost::shared_ptr<char> buffer(new char[1024]);
			vsprintf(buffer.get(), msg, arg);
			std::string line = prefix + buffer.get();
			line += "\n";

			// Write to file.
			logFile->write (line.c_str (), line.size ());
			logFile->flush();
		}

	}

	bool Report(SeverityLevel severity, const char* msg, ...)
	{
		va_list arg;
		va_start (arg, msg);

		Reporter::GetInstance()->Report(severity, msg, arg);

		va_end (arg);

		return false;
	}

} // PT namespace