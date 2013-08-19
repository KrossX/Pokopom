// Based on the following blog post
// 	http://justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/
// 
// Quite the useful thing.
//

#include "General.h"
#include "ConsoleOutput.h"

#ifdef _WIN32

#include <io.h>
#include <fcntl.h>

void GimmeConsole()
{
	BOOL gotConsole = AllocConsole();

	if(gotConsole != 0)
	{
		HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		//HANDLE hConsoleError = GetStdHandle(STD_ERROR_HANDLE);

		int hCrt = _open_osfhandle((long) hConsoleOutput, _O_TEXT);
		FILE* hf_out = _fdopen(hCrt, "w");
		setvbuf(hf_out, NULL, _IONBF, 1);
		*stdout = *hf_out;
	}
}

#endif //WIN32

#ifdef __linux__

void GimmeConsole()
{
}

#endif //Linux
