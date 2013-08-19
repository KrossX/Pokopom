/*  Pokopom - Input Plugin for PSX/PS2 Emulators
 *  Copyright (C) 2012  KrossX
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "General.h"
#include "Input.h"
#include "Input_Shared.h"

#ifdef __linux__

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <stdio.h>
#include <fstream>
#include <errno.h>
#include <time.h>

enum
{
	LIN_BTN_A = 0,
	LIN_BTN_B,
	LIN_BTN_X,
	LIN_BTN_Y,
	LIN_BTN_LB,
	LIN_BTN_RB,
	LIN_BTN_BACK,
	LIN_BTN_START,
	LIN_BTN_BIGX,
	LIN_BTN_LS,
	LIN_BTN_RS,
	LIN_BTN_NUM
};

enum
{
	LIN_AXIS_LX = 0, // -left, +right
	LIN_AXIS_LY,     // -top, +bottom
	LIN_AXIS_LT,     // -32767 to 32767(pressed)
	LIN_AXIS_RX,
	LIN_AXIS_RY,
	LIN_AXIS_RT,
	LIN_AXIS_DX, // DPAD
	LIN_AXIS_DY, // DPAD
	LIN_AXIS_NUM
};

namespace Input
{

u8 xpads = 0;
const char xpad_name[] = "Microsoft X-Box 360";

struct devinfo
{
	s32 fd;
	char path[32];

	devinfo()
	{
		fd = 0;
		memset(path, 0, 32);
	}

} joy_info[4];

struct ffinfo : public devinfo
{
	ff_effect effect;
	input_event play;

	ffinfo()
	{
		effect.type = FF_RUMBLE;
		effect.id = -1;
		effect.u.rumble.strong_magnitude = 0;
		effect.u.rumble.weak_magnitude = 0;
		effect.replay.length = 150;
		effect.replay.delay  = 0;

		play.type = EV_FF;
		play.value = 1;
	}
} rumble_info[4];



struct jstate
{
	s32 axis[LIN_AXIS_NUM];
	u8 button[LIN_BTN_NUM];
} joy_state[4];

////////////////////////////////////////////////////////////////////////
// General
////////////////////////////////////////////////////////////////////////

bool JoyPoll(u8 port)
{
	bool polled = false;

	if(port < xpads)
	{
		if(joy_info[port].fd == 0)
			joy_info[port].fd = open(joy_info[port].path, O_RDONLY | O_NONBLOCK, 0);

		if(joy_info[port].fd > 0)
		{
			js_event joy;
			jstate &jstate = joy_state[port];

			while(read(joy_info[port].fd, &joy, sizeof(js_event)) != -1)
			{
				switch (joy.type & ~JS_EVENT_INIT)
		        {
		        case JS_EVENT_AXIS:
				    jstate.axis[joy.number] = joy.value;
				    break;
		        case JS_EVENT_BUTTON:
				    jstate.button[joy.number] = joy.value;
				    break;
		        }
			}

			polled = true;
			//close(joy_fd);
		}
		else
			joy_info[port].fd = 0;
	}

	return polled;
}

bool Recheck(u8 port)
{
	if(settings[port].disabled) return false;
	if(port > 0) return false;

	//printf("Pokopom(%d) -> Recheck\n", port);

	s32 joy_fd, count = 0;
	char name_of_joystick[80] = {0};

	xpads = 0;

	while(xpads < 4)
	{
		memset(name_of_joystick, 0, 80);
		sprintf(joy_info[xpads].path, "/dev/input/js%d", count);

		joy_fd = open(joy_info[xpads].path, O_RDONLY | O_NONBLOCK, 0);

		if(joy_fd < 0 && errno == ENOENT)
		{
			memset(joy_info[xpads].path, 0, 32);
			break;
		}
		else
		{
			ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);

			if(memcmp(xpad_name, name_of_joystick, 18) == 0)
				xpads++;

			close(joy_fd);
		}

		count++;
	}

	// Rumble check

	s32 rumble_fd, num_effects = 0;
	u8 rpads = 0; count = 0;

	if(xpads > 0) while(rpads < xpads)
	{
		sprintf(rumble_info[rpads].path, "/dev/input/event%d", count);

		rumble_fd = open(rumble_info[rpads].path, O_RDWR | O_NONBLOCK);

		if(rumble_fd < 0 && errno == ENOENT)
		{
			memset(rumble_info[xpads].path, 0, 32);
			break;
		}
		else
		{
			/* Should probably check for features too. RUMBLE at least. */
			ioctl(rumble_fd, EVIOCGEFFECTS, &num_effects);

			if(num_effects == 16)
				rpads++;

			close(rumble_fd);
		}

		count++;
	}

	return port < xpads ? true : false;
}

void Pause(bool pewpew)
{
	return;
}

void StopRumble(u8 port)
{
	return;
}

bool CheckAnalogToggle(u8 port)
{
	return joy_state[port].button[LIN_BTN_BIGX] ? true : false;
}

void SetAnalogLed(u8 port, bool digital)
{
	//Debug("Pokopom -> [%d] Mode: (%s).\n", port, digital? "Digital" : "Analog");
}

bool InputGetState(_Pad& pad, _Settings &set)
{
	const bool pollSuccess = JoyPoll(set.xinputPort);

	if(pollSuccess)
	{
		jstate &js = joy_state[set.xinputPort];
		
		pad.buttons[X360_DUP] = js.axis[LIN_AXIS_DY] < 0? 1:0;
		pad.buttons[X360_DDOWN] = js.axis[LIN_AXIS_DY] > 0? 1:0;
		pad.buttons[X360_DLEFT] = js.axis[LIN_AXIS_DX] < 0? 1:0;
		pad.buttons[X360_DRIGHT] = js.axis[LIN_AXIS_DX] > 0? 1:0;

		pad.buttons[X360_START] = js.button[LIN_BTN_START] ? 1:0;
		pad.buttons[X360_BACK] = js.button[LIN_BTN_BACK] ? 1:0;

		pad.buttons[X360_LS] = js.button[LIN_BTN_LS] ? 1:0;
		pad.buttons[X360_RS] = js.button[LIN_BTN_RS] ? 1:0;
		pad.buttons[X360_LB] = js.button[LIN_BTN_LB] ? 1:0;
		pad.buttons[X360_RB] = js.button[LIN_BTN_RB] ? 1:0;

		pad.buttons[X360_BIGX] = js.button[LIN_BTN_BIGX] ? 1:0;

		pad.buttons[X360_A] = js.button[LIN_BTN_A] ? 1:0;
		pad.buttons[X360_B] = js.button[LIN_BTN_B] ? 1:0;
		pad.buttons[X360_X] = js.button[LIN_BTN_X] ? 1:0;
		pad.buttons[X360_Y] = js.button[LIN_BTN_Y] ? 1:0;

		pad.analog[X360_STICKLX] =  js.axis[LIN_AXIS_LX];
		pad.analog[X360_STICKLY] = -js.axis[LIN_AXIS_LY];
		pad.analog[X360_STICKRX] =  js.axis[LIN_AXIS_RX];
		pad.analog[X360_STICKRY] = -js.axis[LIN_AXIS_RY];

		pad.analog[X360_TRIGGERL] = (js.axis[LIN_AXIS_LT] + 32767) / 256;
		pad.analog[X360_TRIGGERR] = (js.axis[LIN_AXIS_RT] + 32767) / 256;

		pad.stickL.X = pad.analog[X360_STICKLX];
		pad.stickL.Y = pad.analog[X360_STICKLY];
		pad.stickR.X = pad.analog[X360_STICKRX];
		pad.stickR.Y = pad.analog[X360_STICKRY];

		set.axisValue[GP_AXIS_LY] = pad.analog[X360_STICKLY] * (set.axisInverted[GP_AXIS_LY] ? -1 : 1);
		set.axisValue[GP_AXIS_LX] = pad.analog[X360_STICKLX] * (set.axisInverted[GP_AXIS_LX] ? -1 : 1);
		set.axisValue[GP_AXIS_RY] = pad.analog[X360_STICKRY] * (set.axisInverted[GP_AXIS_RY] ? -1 : 1);
		set.axisValue[GP_AXIS_RX] = pad.analog[X360_STICKRX] * (set.axisInverted[GP_AXIS_RX] ? -1 : 1);

		pad.modL.X = set.axisValue[set.axisRemap[GP_AXIS_LX]];
		pad.modL.Y = set.axisValue[set.axisRemap[GP_AXIS_LY]];
		pad.modR.X = set.axisValue[set.axisRemap[GP_AXIS_RX]];
		pad.modR.Y = set.axisValue[set.axisRemap[GP_AXIS_RY]];

		GetRadius(pad.stickL); GetRadius(pad.stickR);
		GetRadius(pad.modL); GetRadius(pad.modR);
	}
	
	return pollSuccess;
}

////////////////////////////////////////////////////////////////////////
// DualShock
////////////////////////////////////////////////////////////////////////

void DualshockRumble(u8 smalldata, u8 bigdata, _Settings &set, bool &gamepadPlugged)
{
	if(!gamepadPlugged) return;

	static timespec timeS[4], timeB[4];
	ffinfo &rumble = rumble_info[set.xinputPort];

	if(rumble.fd == 0)
		rumble.fd = open(rumble.path, O_RDWR | O_NONBLOCK, 0);

	if(rumble.fd > 0)
	{
		u16 &smallMotor = rumble.effect.u.rumble.weak_magnitude;
		u16 &bigMotor = rumble.effect.u.rumble.strong_magnitude;
		
		timespec timeC;
		clock_gettime(CLOCK_MONOTONIC, &timeC);

		if(smalldata)
		{
			smallMotor = Clamp(0xFFFF * set.rumble);
			clock_gettime(CLOCK_MONOTONIC, &timeS[set.xinputPort]);
		}
		else if(smallMotor)
		{
			const long secDiff = timeC.tv_sec - timeS[set.xinputPort].tv_sec;
			const long nsecDiff = timeC.tv_nsec - timeS[set.xinputPort].tv_nsec;
			
			if(secDiff > 0 || nsecDiff > 150000000)
				smallMotor = 0;
		}

		if(bigdata)
		{
			f64 broom = 0.006474549734772402 * pow(bigdata, 3.0) -
					1.258165252213538 *  pow(bigdata, 2.0) +
					156.82454281087692 * bigdata +
					3.637978807091713e-11;

			bigMotor = Clamp(broom * set.rumble);
			clock_gettime(CLOCK_MONOTONIC, &timeB[set.xinputPort]);
		}
		else if(bigMotor)
		{
			const long secDiff = timeC.tv_sec - timeB[set.xinputPort].tv_sec;
			const long nsecDiff = timeC.tv_nsec - timeB[set.xinputPort].tv_nsec;
			
			if(secDiff > 0 || nsecDiff > 150000000)
				bigMotor  = 0;
		}

		// Update the same effect...
		if (ioctl(rumble.fd, EVIOCSFF, &rumble.effect) == -1)
		{
			perror("Rumble Upload");
			rumble.fd = 0;
			return;
		}

		rumble.play.code = rumble.effect.id;

		// And play it!
		if (write(rumble.fd, (const void*) &rumble.play, sizeof(rumble.play)) == -1)
		{
			perror("Rumble Playback");
			rumble.fd = 0;
			return;
		}

		//Debug("Vibrate! [%2X] %04X : [%2X] %04X\n", smalldata, smallMotor, bigdata, bigMotor);
	}

}

bool FASTCALL DualshockPressure(u8 * bufferOut, u32 mask, _Settings &set, bool &gamepadPlugged)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
// Dreamcast
////////////////////////////////////////////////////////////////////////

void VibrationWatchdog(void* param)
{
	return;
}

void DreamcastRumble(s16 intensity, bool freqH, bool freqL, void* thread,
	_Settings &set, bool &gamepadPlugged)
{
	return;
}

////////////////////////////////////////////////////////////////////////
// Zilmar
////////////////////////////////////////////////////////////////////////

void N64rumbleSwitch(u8 port, bool &rumble, bool &gamepadPlugged)
{
	return;
}

void N64rumble(bool on, _Settings &set, bool &gamepadPlugged)
{
	return;
}

} // Input namespace
#endif // Linux
