#include "General.h"
#include "ConsoleOutput.h"
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
