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
#include <fstream>
#include <errno.h>

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
	if(port > 0) return false;

	printf("Pokopom(%d) -> Recheck\n", port);

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

void StopRumble(u8 port, bool &gamepadPlugged)
{
	return;
}

bool CheckAnalogToggle(u8 port)
{
	return joy_state[port].button[LIN_BTN_BIGX] ? true : false;
}

void SetAnalogLed(u8 port, bool digital)
{
	//printf("Pokopom -> [%d] Mode: (%s).\n", port, digital? "Digital" : "Analog");
}

////////////////////////////////////////////////////////////////////////
// DualShock
////////////////////////////////////////////////////////////////////////

void DualshockPoll(u16 * bufferOut, _Settings &set, bool &gamepadPlugged)
{
	u16 buttons, buttonsStick, analogL, analogR;
	u8 triggerL, triggerR;

	buttons = buttonsStick = 0xFFFF;
	analogL = analogR = 0x7F7F;
	triggerL = triggerR = 0;

	if(JoyPoll(set.xinputPort))
	{
		jstate &js = joy_state[set.xinputPort];

		buttons = 0;
		buttons |= (js.button[LIN_BTN_BACK] ? 0:1) << 0x0; // Select
		buttons |= (js.button[LIN_BTN_LS] ? 0:1) << 0x1; // L3
		buttons |= (js.button[LIN_BTN_RS] ? 0:1) << 0x2; // R3
		buttons |= (js.button[LIN_BTN_START] ? 0:1) << 0x3; // Start
		buttons |= (js.axis[LIN_AXIS_DY] < 0? 0:1) << 0x4; // Up
		buttons |= (js.axis[LIN_AXIS_DX] > 0? 0:1) << 0x5; // Right
		buttons |= (js.axis[LIN_AXIS_DY] > 0? 0:1) << 0x6; // Down
		buttons |= (js.axis[LIN_AXIS_DX] < 0? 0:1) << 0x7; // Left

		buttons |= (js.axis[LIN_AXIS_LT] > -30000 ? 0:1) << 0x8; // L2
		buttons |= (js.axis[LIN_AXIS_RT] > -30000 ? 0:1) << 0x9; // R2

		buttons |= (js.button[LIN_BTN_LB] ? 0:1) << 0xA; // L1
		buttons |= (js.button[LIN_BTN_RB] ? 0:1) << 0xB; // R1

		buttons |= (js.button[LIN_BTN_Y] ? 0:1) << 0xC; // Triangle
		buttons |= (js.button[LIN_BTN_B] ? 0:1) << 0xD; // Circle
		buttons |= (js.button[LIN_BTN_A] ? 0:1) << 0xE; // Cross
		buttons |= (js.button[LIN_BTN_X] ? 0:1) << 0xF; // Square

		buttonsStick = buttons | 0x06;
		const s32 threshold = 16384;

		set.axisValue[GP_AXIS_LY] = js.axis[LIN_AXIS_LY] * (set.axisInverted[GP_AXIS_LY] ? -1 : 1);
		set.axisValue[GP_AXIS_LX] = js.axis[LIN_AXIS_LX] * (set.axisInverted[GP_AXIS_LX] ? -1 : 1);
		set.axisValue[GP_AXIS_RY] = js.axis[LIN_AXIS_RY] * (set.axisInverted[GP_AXIS_RY] ? -1 : 1);
		set.axisValue[GP_AXIS_RX] = js.axis[LIN_AXIS_RX] * (set.axisInverted[GP_AXIS_RX] ? -1 : 1);

		if(js.axis[LIN_AXIS_LY] < -threshold) buttonsStick &= ~(1 << 0x4);
		if(js.axis[LIN_AXIS_LX] >  threshold) buttonsStick &= ~(1 << 0x5);
		if(js.axis[LIN_AXIS_LY] >  threshold) buttonsStick &= ~(1 << 0x6);
		if(js.axis[LIN_AXIS_LX] < -threshold) buttonsStick &= ~(1 << 0x7);

		if(js.axis[LIN_AXIS_RY] < -threshold) buttonsStick &= ~(1 << 0xC);
		if(js.axis[LIN_AXIS_RX] >  threshold) buttonsStick &= ~(1 << 0xD);
		if(js.axis[LIN_AXIS_RY] >  threshold) buttonsStick &= ~(1 << 0xE);
		if(js.axis[LIN_AXIS_RX] < -threshold) buttonsStick &= ~(1 << 0xF);

		analogL = ConvertAnalog(set.axisValue[set.axisRemap[GP_AXIS_LX]],
								-set.axisValue[set.axisRemap[GP_AXIS_LY]], set, 0);

		analogR = ConvertAnalog(set.axisValue[set.axisRemap[GP_AXIS_RX]],
								-set.axisValue[set.axisRemap[GP_AXIS_RY]], set, 0);

		triggerL = (js.axis[LIN_AXIS_LT] + 32767)/256;
		triggerR = (js.axis[LIN_AXIS_RT] + 32767)/256;

		//printf("Pokopom: %04X %04X\n", analogL, analogR);
	}
	else
		gamepadPlugged = false;

	bufferOut[0] = buttons;
	bufferOut[1] = buttonsStick;
	bufferOut[2] = analogL;
	bufferOut[3] = analogR;
	bufferOut[4] = (triggerL << 8) | triggerR;
}

void DualshockRumble(u8 smalldata, u8 bigdata, _Settings &set, bool &gamepadPlugged)
{
	if(!gamepadPlugged) return;

	ffinfo &rumble = rumble_info[set.xinputPort];

	if(rumble.fd == 0)
		rumble.fd = open(rumble.path, O_RDWR | O_NONBLOCK, 0);

	if(rumble.fd > 0)
	{
		u16 &smallMotor = rumble.effect.u.rumble.weak_magnitude;
		u16 &bigMotor = rumble.effect.u.rumble.strong_magnitude;

		smallMotor = smalldata ? Clamp(0xFFFF * set.rumble) : 0;

		if(bigdata)
		{
			f64 broom = 0.006474549734772402 * pow(bigdata, 3.0) -
					1.258165252213538 *  pow(bigdata, 2.0) +
					156.82454281087692 * bigdata +
					3.637978807091713e-11;

			bigMotor = Clamp(broom * set.rumble);
		}
		else
			bigMotor  = 0;

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

		//printf("Vibrate! [%2X] %04X : [%2X] %04X\n", smalldata, smallMotor, bigdata, bigMotor);
	}

}

////////////////////////////////////////////////////////////////////////
// Dreamcast
////////////////////////////////////////////////////////////////////////

void DreamcastPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged)
{
	u16* buffer = (u16*) buffer_out;

	// Some magic number...
	buffer[0] = 0x0000;
	buffer[1] = 0x0100;

	u16 buttons = 0xFFFF;
	u16 analog = 0x8080;
	u16 triggers = 0x0000;

	// Buttons
	buffer[2] = buttons | 0xF901;

	// Triggers
	buffer[3] = triggers;

	// Left Stick
	buffer[4] = analog;

	// Right Stick... not present.
	buffer[5] = 0x8080;
}

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

enum
{
	N64_RIGHT = 0,
	N64_LEFT,
	N64_DOWN,
	N64_UP,
	N64_START,
	N64_TRIGGERZ,
	N64_B,
	N64_A,
	N64_CRIGHT,
	N64_CLEFT,
	N64_CDOWN,
	N64_CUP,
	N64_TRIGGERR,
	N64_TRIGGERL
};

void N64rumbleSwitch(u8 port, bool &rumble, bool &gamepadPlugged)
{
	return;
}

void N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged)
{
	u16 buttons = 0;
	u16 analog = 0x0000;

	u16 * outBig = (u16*)outBuffer;

	outBig[0] = buttons;
	outBig[1] = analog;
}

void N64rumble(bool on, _Settings &set, bool &gamepadPlugged)
{
	return;
}

} // Input namespace
#endif // Linux
