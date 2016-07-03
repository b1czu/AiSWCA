#TOOLCHAIN CFG
TOOLCHAIN = /home/piotr/gcc-arm-none-eabi/bin/arm-none-eabi-
#(eg. TOOLCHAIN = /home/xxx/gcc-arm-none-eabi-5_3-2016q1/bin/arm-none-eabi-)

# TOOLCHAIN DEF
CC = $(TOOLCHAIN)gcc
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE = $(TOOLCHAIN)size

# PROJECT PARM
PROJ_NAME = halSTM32F0
OUT_DIR = out
CPU_MODEL_GENERAL = STM32F051x8

# INCLUDE
INCLUDEDIRS = ./
INCLUDEDIRS += ./Inc
INCLUDEDIRS += ./CMSIS/Device/ST/STM32F0xx/Include
INCLUDEDIRS += ./CMSIS/Include/

# auto-generated project paths
LIBSOURCES = $(shell find ./Src/ -name *.c)
LIBOBJS = $(LIBSOURCES:.c=.o)

# "-O0" - no optimization, 
# "-O1" - optimize, 
# "-O2" - optimize even more
# "-Os" - optimize for size 
# "-O3" - optimize yet more 
OPTIMIZATION = -O0

# C language standard
# "c89" / "iso9899:1990" 
# "iso9899:199409"
# "c99" / "iso9899:1999"
# "gnu89" - default 
# "gnu99"
# "c11" - 
C_STD = c11

EXTRA_DEFS = 
EXTRA_DEFS += -D LSE_STARTUP_TIMEOUT=100
EXTRA_DEFS += -D HSE_STARTUP_TIMEOUT=100


CFLAGS = -Wall -g -std=$(C_STD) $(OPTIMIZATION) -D $(CPU_MODEL_GENERAL)
CFLAGS += -mlittle-endian -mcpu=cortex-m0  -march=armv6-m -mthumb 
CFLAGS += -ffunction-sections -fdata-sections 
CFLAGS += -Wl,--gc-sections -Wl,-Map=main.map 
CFLAGS += $(addprefix -I ,$(INCLUDEDIRS))
CFLAGS += $(EXTRA_DEFS)

.PHONY: all

all: proj

proj: $(PROJ_NAME).a

$(PROJ_NAME).a: $(LIBOBJS)
	@echo
	$(AR) -r $@ $(LIBOBJS)

%.o: %.c
	@echo
# $(eval AUTOINCLUDES = $(addprefix -include ,$(shell find $(dir $<) -name *.h)))
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm ./Src/*.o || true
	rm $(PROJ_NAME).a || true
