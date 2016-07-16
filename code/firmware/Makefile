#TOOLCHAIN CFG
TOOLCHAIN = /opt/gcc-arm-none-eabi/bin/arm-none-eabi-
#(eg. TOOLCHAIN = /home/xxx/gcc-arm-none-eabi-5_3-2016q1/bin/arm-none-eabi-)

ifndef TOOLCHAIN
$(error TOOLCHAIN variable is not set)
endif

#OPENOCD CFG
OPENOCD_INTERFACE = interface/stlink-v2.cfg
OPENOCD_TARGET    = target/stm32f0x.cfg

# TOOLCHAIN DEF
CC = $(TOOLCHAIN)gcc
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE = $(TOOLCHAIN)size

# PROJECT PARM
PROJ_NAME = main
OUT_DIR = out
CPU_MODEL_GENERAL = STM32F070x6

# INCLUDE
INCLUDEDIRS =
INCLUDEDIRS += ./inc
INCLUDEDIRS += ./CMSIS/Include
INCLUDEDIRS += ./CMSIS/Device/ST/STM32F0xx/Include

INCLUDEDIRS_LIBS += ./../lib/STM32F0xx_HAL_Driver/Inc
INCLUDEDIRS_LIBS += ./lib

SOURCES = $(shell find . -name *.c)
# auto-generated project paths
SOURCES += CMSIS/startup_stm32f070x6.s # add assembly startup template

LDSCRIPT = ./CMSIS/STM32F070x6_FLASH.ld

# extra external lib src
SOURCES += $(shell find ./ext_lib/itoa-1.0.0-090713/ -name *.c)
SOURCES += $(shell find ./ext_lib/printf-1.0.0-091002/ -name *.c)
# extra external lib inc 
INCLUDEDIRS += ./ext_lib/itoa-1.0.0-090713/
INCLUDEDIRS += ./ext_lib/printf-1.0.0-091002/

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
EXTRA_DEFS += USE_HAL_DRIVER

CFLAGS_LIBS = 
CFLAGS_LIBS += -L ./../lib/STM32F0xx_HAL_Driver
CFLAGS_LIBS += -l :halSTM32F0.a

CFLAGS = -Wall -g -std=$(C_STD) $(OPTIMIZATION) -D$(CPU_MODEL_GENERAL)
CFLAGS += -mlittle-endian -mcpu=cortex-m0  -march=armv6-m -mthumb 
CFLAGS += -ffunction-sections -fdata-sections 
CFLAGS += -Wl,--gc-sections -Wl,-Map=main.map 
CFLAGS += $(addprefix -I ,$(INCLUDEDIRS))
CFLAGS += $(addprefix -D ,$(EXTRA_DEFS))
CFLAGS += $(addprefix -I ,$(INCLUDEDIRS_LIBS))
CFLAGS += $(CFLAGS_LIBS)


.PHONY: all

all: proj

proj: $(PROJ_NAME).elf

%.o: %.c
	@echo
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROJ_NAME).elf: $(SOURCES)
	@echo
	$(CC) $(CFLAGS) $^ -o $@ $(CFLAGS_LIBS) -T $(LDSCRIPT) -MD
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	$(OBJDUMP) -St $(PROJ_NAME).elf >$(PROJ_NAME).lst
	$(SIZE) $(PROJ_NAME).elf

program: proj
	openocd -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c "program main.elf verify reset exit"

openocd:
	openocd -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET)

clean:
	rm $(PROJ_NAME).elf || true
	rm $(PROJ_NAME).hex || true
	rm $(PROJ_NAME).bin || true
	rm $(PROJ_NAME).map || true
	rm $(PROJ_NAME).lst || true
	rm $(PROJ_NAME).d || true