GX_PTHREAD_LEGACY := 1
STATIC_LINKING    := 0
AR                := ar

ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
else ifneq ($(findstring win,$(shell uname -a)),)
   platform = win
endif
endif

# system platform
system_platform = unix
ifeq ($(shell uname -a),)
   EXE_EXT = .exe
   system_platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   system_platform = osx
   arch = intel
ifeq ($(shell uname -p),powerpc)
   arch = ppc
endif
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   system_platform = win
endif

TARGET_NAME := zc250x
LIBM= -lm

ifeq ($(ARCHFLAGS),)
ifeq ($(archs),ppc)
   ARCHFLAGS = -arch ppc -arch ppc64
else
   ARCHFLAGS = -arch i386 -arch x86_64
endif
endif

ifeq ($(platform), osx)
ifndef ($(NOUNIVERSAL))
   CFLAGS += $(ARCHFLAGS)
   LFLAGS += $(ARCHFLAGS)
endif
endif

ifeq ($(STATIC_LINKING), 1)
EXT := a
endif

ifeq ($(platform), unix)
   EXT ?= so
   TARGET := $(TARGET_NAME)_libretro.$(EXT)
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   LDFLAGS := -lpthread
else ifeq ($(platform), linux-portable)
   TARGET := $(TARGET_NAME)_libretro.$(EXT)
   fpic := -fPIC -nostdlib
   SHARED := -shared -Wl,--version-script=link.T
   LIBM :=
else ifneq (,$(findstring osx,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
else ifneq (,$(findstring ios,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
   fpic := -fPIC
   SHARED := -dynamiclib

ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
endif

DEFINES := -DIOS
ifeq ($(platform), ios-arm64)
   CC = cc -arch arm64 -isysroot $(IOSSDK)
else
   CC = cc -arch armv7 -isysroot $(IOSSDK)
endif
ifeq ($(platform),$(filter $(platform),ios9 ios-arm64))
   CC     += -miphoneos-version-min=8.0
   CFLAGS += -miphoneos-version-min=8.0
else
   CC     += -miphoneos-version-min=5.0
   CFLAGS += -miphoneos-version-min=5.0
endif
else ifneq (,$(findstring qnx,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
else ifeq ($(platform), emscripten)
   TARGET := $(TARGET_NAME)_libretro_emscripten.bc
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
else ifeq ($(platform), vita)
   TARGET := $(TARGET_NAME)_libretro_vita.a
   CC = arm-vita-eabi-gcc
   AR = arm-vita-eabi-ar
   CFLAGS += -Wl,-q -Wall -O3
   STATIC_LINKING = 1
# CTR (3DS)
else ifeq ($(platform), ctr)
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(DEVKITARM)/bin/arm-none-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITARM)/bin/arm-none-eabi-g++$(EXE_EXT)
   AR = $(DEVKITARM)/bin/arm-none-eabi-ar$(EXE_EXT)
   CFLAGS += -D_3DS -DARM11 -march=armv6k -mtune=mpcore -mfloat-abi=hard
   STATIC_LINKING = 1
# Nintendo Switch (libnx)
else ifeq ($(platform), libnx)
   include $(DEVKITPRO)/libnx/switch_rules
   EXT=a
   TARGET := $(TARGET_NAME)_libretro_$(platform).$(EXT)
   DEFINES := -DSWITCH=1 -D__SWITCH__=1 -DHAVE_LIBNX=1
   CFLAGS := $(DEFINES) -g -fPIE -I$(LIBNX)/include/ -I$(PORTLIBS)/include/ -ffunction-sections -fdata-sections -ftls-model=local-exec
   CFLAGS += $(INCDIRS) -I$(CORE_DIR)/zlib -I$(CORE_DIR)/libogg/include -I$(CORE_DIR)/libvorbis/include \
         -I$(CORE_DIR)/jpeg-8c -fvisibility=hidden
   CFLAGS += -march=armv8-a -mtune=cortex-a57 -mtp=soft -mcpu=cortex-a57+crc+fp+simd -Dstricmp=strcasecmp -Dstrnicmp=strncasecmp
   STATIC_LINKING = 1
else ifeq ($(platform), psp1)
   TARGET := $(TARGET_NAME)_libretro_psp1.a
   CC = psp-gcc
   CXX = psp-g++
   AR = psp-ar
   CFLAGS += -DPSP -G0
   CXXFLAGS += -DPSP -G0
   DISABLE_ADLIB = 1
   CFLAGS += -I$(shell psp-config --pspsdk-path)/include
   STATIC_LINKING = 1
else ifeq ($(platform), ps2)
   TARGET := $(TARGET_NAME)_libretro_ps2.a
   CC = ee-gcc
   CXX = ee-g++
   AR = ee-ar
   CFLAGS += -DPS2 -G0 -D__sun
   CXXFLAGS += -DPS2 -G0 -D__sun
   STATIC_LINKING = 1
else ifeq ($(platform), psl1ght)
   EXT=a
   TARGET := $(TARGET_NAME)_libretro_$(platform).$(EXT)
   CC = $(PS3DEV)/ppu/bin/ppu-gcc$(EXE_EXT)
   CXX = $(PS3DEV)/ppu/bin/ppu-g++$(EXE_EXT)
   AR = $(PS3DEV)/ppu/bin/ppu-ar$(EXE_EXT)
   CFLAGS += -D__ppc__ -D__CELLOS_LV2__ -D__PSL1GHT__ -I$(PSL1GHT)/ppu/include
   STATIC_LINKING = 1
# SF2000
else ifeq ($(platform), sf2000)
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   MIPS:=/opt/mips32-mti-elf/2019.09-03-2/bin/mips-mti-elf-
   CC = $(MIPS)gcc
   CXX = $(MIPS)g++
   AR = $(MIPS)ar
   CFLAGS = -EL -march=mips32 -mtune=mips32 -msoft-float -G0 -mno-abicalls -fno-pic
   CFLAGS += -ffast-math -fomit-frame-pointer -ffunction-sections -fdata-sections 
   CFLAGS += -DSF2000
   CXXFLAGS = $(CFLAGS)
   STATIC_LINKING = 1
else ifneq (,$(filter $(platform), ngc wii wiiu))
   TARGET := $(TARGET_NAME)_libretro_$(platform).a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
   FLAGS += -DGEKKO -mcpu=750 -meabi -mhard-float -malign-natural 
   FLAGS += -fsingle-precision-constant -mno-bit-align
   FLAGS += -DHAVE_STRTOUL
   FLAGS += -I$(DEVKITPRO)/libogc/include
   ENDIANNESS_DEFINES = -DMSB_FIRST
   ifneq (,$(findstring wiiu,$(platform)))
      FLAGS += -DWIIU -DHW_RVL -mwup
   else ifneq (,$(findstring wii,$(platform)))
      FLAGS += -DHW_RVL -mrvl
   else ifneq (,$(findstring ngc,$(platform)))
      FLAGS += -DHW_DOL -mogc
   endif
   CFLAGS += $(FLAGS)
   CXXFLAGS += $(FLAGS) -fno-merge-constants 
   STATIC_LINKING = 1
   ifeq ($(GX_PTHREAD_LEGACY), 1)
      CFLAGS   += -DGX_PTHREAD_LEGACY
      CXXFLAGS += -DGX_PTHREAD_LEGACY
   endif
else
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
endif

LDFLAGS += $(LIBM)

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
   CXXFLAGS += -O0 -g
else
   CFLAGS += -O3
   CXXFLAGS += -O3
endif

CORE_DIR := .

include Makefile.common

OBJECTS := $(SOURCES_CXX:.cpp=.o) $(SOURCES_C:.c=.o)

CFLAGS += -Wall -pedantic $(ENDIANNESS_DEFINES)
CXXFLAGS += -Wall -pedantic -std=gnu++11 $(ENDIANNESS_DEFINES)

ifneq (,$(findstring qnx,$(platform)))
CFLAGS += -Wc,-std=c99
else
CFLAGS += -std=gnu99
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CXX) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LDFLAGS)
endif

%.o: %.c
	$(CC) $(INCFLAGS) $(CFLAGS) $(fpic) -c -o $@ $<
%.o: %.cpp
	$(CXX) $(INCFLAGS) $(CXXFLAGS) $(fpic) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean

