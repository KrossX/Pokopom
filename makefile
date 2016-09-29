CC = gcc
CFLAGS = -Wall -O2 -flto -std=c++11 -march=i686 -m32 -mwindows -s 
LDLIBS = -lstdc++ -luxtheme

OBJ = ConfigDialog.o             \
      DebugStuff.o               \
      demul.o                    \
      FileIO.o                   \
      Input_Linux.o              \
      Input_Shared.o             \
      Input_XInput.o             \
      playstation_dualshock.o    \
      playstation_dualshock2.o   \
      playstation_guitar.o       \
      playstation_mtap.o         \
      psemupro.o                 \
      regini.o                   \
      Settings.o                 \
      Stuff_Linux.o              \
      Stuff_Shared.o             \
      Stuff_Windows.o            \
      Zilmar.o                   \
      Zilmar_Devices.o


%.o : pokopom/%.cpp
	@$(CC) -o $@ -c $< -DUNICODE $(CFLAGS)

default: $(OBJ)
	@windres pokopom/pokopom.rc pokopom_res.o --target=pe-i386
	@$(CC) -shared -static-libgcc -static-libstdc++ $^ pokopom_res.o pokopom/exports.def $(CFLAGS) $(LDLIBS) -o padPokopom.dll
