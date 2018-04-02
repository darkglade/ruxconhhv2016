STM32F0xx Series Microcontroller Build Environment
==================================================

1. Install arm-none-eabi-gcc, arm-none-eabi-gdb and openocd from your distro's 
  package repository, if they don't supply one you'll have to go get the 
  toolchain from CodeSourcery or similar.
2. Install make (or build-essential if you're on a Debian-ish distro).
3. Download the STM32F0xx Standard Peripherals Library from the ST website, the
  package is designated STST-STM32048 (They'll whinge at you about switching to
  STM32CubeMX, ignore that crap).
4. Extract the STM32F0xx Standard Peripherals Library, by default the Makefile
  expects it to be extracted to the root of the project, if you want to put it 
  elsewhere you'll need to update the PERIPH_LIB_PATH variable in the Makefile.
5. Write some code.
6. Build your code;
  - The project bundles all the crap from the Standard Peripherals Driver into a
  static library called libstm32.a, you can change what modules are included in 
  that can be changed by commenting out whatever you don't need from 
  LIBSTM32_OBJS in the Makefile, but anything unused ought to get optimised out
  anyway.
  - The "all" target will bundle the necessary startup code, the library and 
  your code into main.bin (if you prefer your targets in hex change OBJCPFLAGS
  to "-O ihex").
  - There's a separate libstm32.a target should you wish to rebuild only the 
  library.
  - Other targets in the make file;
    - libclean - removes the library and the objects that make up the library.
    - clean - only cleans up the main binary, you'll want to use this most of
    the time, as it will avoid rebuilding libstm32 all the time.
    - shiny - cleans up the library and the main binaries.

Some notes: 
This code assumes you want to use the internal 8MHz RC oscillator (FWIW, you can 
use the PLL to ramp that up to 48MHz), if you want to use a different oscillator 
configuration you have a few options;
  - Spend hours pawing through the doco to try to understand the clock tree, and
   update system_stm32f0xx.c manually.
  - Dig up a copy of AN4055 and a copy of Excel to run it in (if somebody's 
  REALLY bored, it'd be cool to see an updated version of the Macros in that 
  which will run in LibreOffice...)
  - Get a copy of STM32CubeMX, do your clock config in there, take 
  system_stm32f0xx.c from there and throw away the rest of the project.

If you're the register-banging greybeardy type who "don't need no stinkin'
library" you're welcome to remove all the LIBSTM32 related stuff from the
Makefile and have at it.

This project targets the STM32F030 processor, if you want to target a different
Cortex-F0 device you'll want to tweak the linker script with your target's 
memory sizes, switch startup_stm32f030.s out for the appropriate one for your
target (you can find this under Libraries/CMSIS/Device... in the Standard
Peripherals library), and update STARTUP_FILE in the Makefile to match the name
of your new startup_<blah>.s file.

The above point also goes for using this project for other STM32 Cortex devices;
though you will obviously need to get the appropriate Standard Peripherals
Library, retrieve the template bits and pieces from it (anything with stm32f0xx 
in the name) update the paths PERIPH_LIB_PATH and PERIPH_DRIVER_PATH variables 
in the Makefile, and probably the last path listed in INCLUDE_DIRS as ST fail at
consistent structure (the CMSIS/Include part should be replaced by the path to
the core_<whatever>.h files), change the target define in the Makefile, and 
you'll need to change your COMPILE_OPTS to reflect the new architecture (you 
will likely get WEIRD behaviour if you don't, ask me how I know...).

The linker script included here was handwritten for the project, due to the 
Standard Peripherals Library lacking a linker script under an appropriate 
license for redistribution, this script is CC BY-SA v4 licensed so you can 
reuse it at will (yes, I know CC was never intended for code, but for the 
purposes of a linker script most of the objections go away).

As a curiosity, I've included a C version of the startup code at misc/startup.c
in case you want to know what this stuff actually does and don't grok assembly,
should you want to use that instead of the Standard Peripherals Library startup
code you can do so by copying it to the root and setting STARTUP_FILE in the 
Makefile to be startup.o, make will figure out the rest.

This codebase is the complete codebase including the flag in case anybody is 
interested in how I implemented that.