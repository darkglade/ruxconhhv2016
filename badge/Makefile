# setup
UART_PORT = /dev/ttyUSB0

PERIPH_LIB_PATH = STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/
PERIPH_DRIVER_PATH = STM32F0xx_StdPeriph_Driver/

COMPILE_OPTS = -mlittle-endian -mcpu=cortex-m0 -march=armv6-m -mthumb -Wall -g -O0 \
 -mfloat-abi=soft -ffunction-sections -fdata-sections -ffreestanding -nostdlib --specs=nano.specs \
 -D STM32F030 -D USE_STDPERIPH_DRIVER -D SINGLE_THREADED
INCLUDE_DIRS = -I . -I $(PERIPH_LIB_PATH)$(PERIPH_DRIVER_PATH)inc -I $(PERIPH_LIB_PATH)CMSIS/Include
LIBRARY_DIRS = -L . -L $(PERIPH_LIB_PATH)$(PERIPH_DRIVER_PATH)src -L linker

STARTUP_FILE = startup_stm32f030.o

SIZE = arm-none-eabi-size

CC = arm-none-eabi-gcc
CFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS)

CXX = arm-none-eabi-g++
CXXFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS)

AS = arm-none-eabi-gcc
ASFLAGS = $(COMPILE_OPTS) -c

LD = arm-none-eabi-gcc
LDFLAGS = -Wl,--gc-sections,-Map=$@.map,-cref -mthumb -mcpu=cortex-m0 $(INCLUDE_DIRS) $(LIBRARY_DIRS) -T stm32.ld 

OBJCP = arm-none-eabi-objcopy
OBJCPFLAGS = -O binary

AR = arm-none-eabi-ar
ARFLAGS = cr

MAIN_ELF = main.elf

LIBSTM32_OUT = libstm32.a

LIBSTM32_SRCS = $(wildcard $(PERIPH_LIB_PATH)$(PERIPH_DRIVER_PATH)src/*.c)
$(info LIBSTM32_SRCS= $(LIBSTM32_SRCS))
LIBSTM32_OBJS = $(LIBSTM32_SRCS:.c=.o)

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OBJS += $(STARTUP_FILE)
OBJS += $(LIBSTM32_OUT)

# all

.PHONY : all

all: 
ifneq ($(wildcard ./$(PERIPH_LIB_PATH).),)
	make bins	
else
	@echo "Standard Peripherals Library not found, do you need to change PERIPH_LIB_PATH?"
endif

bins: $(STARTUP_FILE) $(MAIN_ELF) $(MAIN_ELF:.elf=.bin) $(LIBSTM32_OUT)

# main

$(MAIN_ELF): $(OBJS)  
	$(LD) $(LDFLAGS) $(OBJS) --output $@
	$(SIZE) $(MAIN_ELF)

$(MAIN_ELF:.elf=.bin): $(MAIN_ELF)
	$(OBJCP) $(OBJCPFLAGS) $< $@

$(LIBSTM32_OUT): $(LIBSTM32_OBJS)
	$(AR) $(ARFLAGS) $@ $(LIBSTM32_OBJS)

# flash
flash: $(MAIN_ELF)
#	@cp $(MAIN_OUT_ELF) jtag/flash
#	@cd jtag; openocd -f flash.cfg
#	@rm jtag/flash
#	read -p "Activate bootloader mode, then press [Enter]"
	stm32flash -w $(MAIN_OUT_BIN) -v -g 0x00 $(UART_PORT)

.PHONY : shiny libclean clean

clean:
	-rm -f *.o 
	-rm -f *.map 
	-rm -f $(MAIN_ELF)
	-rm -f $(MAIN_ELF:.elf=.bin)

libclean:
	-rm -f $(LIBSTM32_OUT)
	-rm -f $(PERIPH_LIB_PATH)$(PERIPH_DRIVER_PATH)src/*.o

shiny: libclean clean
