# TARGET = Tutorial
# OBJS = main.o \
# resources\Scripts\callbacks.o \
# resources\Scripts\MapManager.o
# #include "resources\Scripts\callbacks.h"
# #include "resources\Scripts\MapManager.h"

# CFLAGS =
# CXXFLAGS = $(CFLAGS) -std=c++14 -fno-rtti
# ASFLAGS = $(CFLAGS)

# # PSP Stuff
# BULD_PRX = 1
# PSP_FW_VERSION = 500
# PSP_LARGE_MEMORY = 1

# EXTRA_TARGETS = EBOOT.PBP
# PSP_EBOOT_TITLE = Laberinto V2.0
# #PSP_EBOOT_ICON = ICON0.PNG

# PSPSDK=$(shell psp-config --pspsdk-path)
# include $(PSPSDK)/lib/build.mak


TARGET = cube
OBJS = main.o \ resources\Scripts\callbacks.o \ resources\Scripts\MapManager.o  \ resources\Scripts\ControlManager.o  \ resources\Scripts\AudioManagement.o \ glib2d.o


LIBS = -ljpeg -lpng -lz -lpspgum -lpspgu -lpsprtc -lm -lpspvram -lpspaudiolib -lpspaudio

CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = THE RADIO CHASER
PSP_EBOOT_ICON = icon.png
PSP_LARGE_MEMORY = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak 
install:
	cp ./EBOOT.PBP $(MEDIA)$(TARGET)
dir:
	mkdir $(MEDIA)$(TARGET)
lclean:
	rm *.o
