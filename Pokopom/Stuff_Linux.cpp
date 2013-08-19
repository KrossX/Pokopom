#include "General.h"
#include "Stuff.h"

#ifdef __linux__

#include <X11/XKBlib.h>
#include <X11/keysym.h>

Display *hDisplay = NULL;
Window hWindow = 0;
Atom aProtocols = 0, aDeleteWindow = 0;

void KeyboardOpen()
{
	//Debug("Pokopom -> Keyboard Open\n");
	aProtocols = XInternAtom(hDisplay, "WM_PROTOCOLS", 0);
	aDeleteWindow = XInternAtom(hDisplay, "WM_DELETE_WINDOW", 0);

	XkbSetDetectableAutoRepeat(hDisplay, true, NULL);
}

void KeyboardClose()
{
	//Debug("Pokopom -> Keyboard Close\n");
	XkbSetDetectableAutoRepeat(hDisplay, false, NULL);
}

void KeyboardCheck()
{
	XEvent xEvent;
	keyEvent newEvent;

	while (XPending(hDisplay))
	{
		XNextEvent(hDisplay, &xEvent);
		switch (xEvent.type)
		{
		case KeyPress:
		{
			newEvent.evt = 1;
			newEvent.key = XLookupKeysym((XKeyEvent *)&xEvent, 0);
			keyEventList.push_back(newEvent);
		}
		break;

		case KeyRelease:
		{
			newEvent.evt = 2;
			newEvent.key = XLookupKeysym((XKeyEvent *)&xEvent, 0) | 0x40000000;
			keyEventList.push_back(newEvent);
		}
		break;

		case ClientMessage:
		{
			XClientMessageEvent *xce = (XClientMessageEvent *)&xEvent;
			if (xce->message_type == aProtocols && (Atom)xce->data.l[0] == aDeleteWindow)
			{
				// Fake an ESC key if user clicked the close button on window
				newEvent.evt = 1;
				newEvent.key = XK_Escape;
				keyEventList.push_back(newEvent);
				return;
			}
		}
		break;
		}
	}
}

void GetDisplay(void* pDisplay)
{
	//Debug("Pokopom -> GetDisplay\n");
	hDisplay = (Display*)*(uPointer*)pDisplay;
}

void ShowDialog(const wchar_t* message, const wchar_t* title)
{
}

void SetPriority()
{
}

u8 SwapPorts()
{
	static u8 swapIt = 0; /*
	static bool pressed = false;
	const bool currPress = !!(GetAsyncKeyState(0x30) >> 1);

	if(!pressed && currPress)
	{
		swapIt ^= 1;
		pressed = true;
	}
	else if(pressed && !currPress)
	{
		pressed = false;
	}
*/
	return swapIt;
}

void KeepAwake(u8 mode)
{
	switch(mode)
	{
	case KEEPAWAKE_INIT:
	{
	}
	break;

	case KEEPAWAKE_CLOSE:
	{
	}
	break;
	}
}

#endif //Linux
