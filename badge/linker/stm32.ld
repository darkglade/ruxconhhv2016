/*  STM32F0xx Series Linker Script 
    By Morgan Reed
    Based on work from http://www.embedds.com/programming-stm32-discovery-using-gnu-tools-linker-script

    Script updated to match CMSIS definitions in the Standard Peripherals Library.

    Content licensed under Creative Commons Attribution-ShareAlike 4.0 International [CC BY-SA 4.0]
*/

MEMORY
{
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 4K
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K
}

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} > FLASH

  .ARM.exidx :
  {
    *(.ARM.exidx*)
    *(.gnu.linkonce.armexidx.*)
  } > FLASH

	_sidata = .;

	.data : AT ( _sidata )
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data)
		*(.data*)
		. = ALIGN(4);
		_edata = .;	
	} > RAM

	.bss : 
	{
    . = ALIGN(4);
		_sbss = .;
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} > RAM

	_stack_size = 1024;
	_estack = ORIGIN(RAM)+LENGTH(RAM);
	_bstack = _estack - _stack_size;
	. = _bstack;
	._stack : 
	{
		. = . + _stack_size;
	} > RAM 

  end = .;
}

ENTRY(Reset_Handler);

GROUP(libnosys.a)
