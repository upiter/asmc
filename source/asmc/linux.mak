# Makefile for Asmc Linux using Open Watcom

watcom = \watcom

all: asmc asmc64

asmc:
	asmc -D_WATCOM -Isrc/inc -nologo -elf -nolib -Zp4 -Cs src\*.asm
	wlink @<<
name	asmc.
format	elf runtime linux
libpath $(watcom)/lib386
libpath $(watcom)/lib386/linux
lib	clib3s.lib
lib	../../lib/elf/quadmath.lib
option	norelocs, quiet, stack=0x20000
file { *.obj }
<<
	del *.obj

asmc64:
	asmc -D__ASMC64__ -D_WATCOM -Isrc/inc -nologo -elf -nolib -Zp4 -Cs src\*.asm
	wlink @<<
name	asmc64.
format	elf runtime linux
libpath $(watcom)/lib386
libpath $(watcom)/lib386/linux
lib	clib3s.lib
lib	../../lib/elf/quadmath.lib
option	norelocs, quiet, stack=0x20000
file { *.obj }
<<
	del *.obj
