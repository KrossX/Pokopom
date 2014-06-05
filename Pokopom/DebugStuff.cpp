/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"

FILE *logfile = NULL;

bool _DebugOpen()
{
	logfile = fopen("pokopom.log", "w");
	if(logfile) fprintf(logfile, "DebugOpen\n");
	return !!logfile;
}

void _DebugClose()
{
	if(logfile) fprintf(logfile, "DebugClose\n");
	if(logfile) fclose(logfile);
	logfile = NULL;
}

void _DebugFunc(const char* func)
{
	if(!logfile && !_DebugOpen()) return;

	fprintf(logfile, "%s\n", func);
	fflush(logfile);
}

void _DebugPrint(const char* func, const char* fmt, ...)
{
	if(!logfile && !_DebugOpen()) return;

	va_list args;
	
	fprintf(logfile, "%s : ", func);
	
	va_start(args,fmt);
	vfprintf(logfile, fmt, args);
	va_end(args);
	
	fprintf(logfile, "\n");
	fflush(logfile);
}







