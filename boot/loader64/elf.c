//
// Created by bear on 9/7/2023.
//

//Relocation becomes handy when you need to load, for example, modules or drivers.
// It's possible to use the "-r" option to ld to permit you to have multiple object files linked into one big one,
// which means easier coding and faster testing.
//
//The basic outline of things you need to do for relocation:
//
//Check the object file header (it has to be ELF, not PE, for example)
//Get a load address (e.g. all drivers start at 0xA0000000, need some method of keeping track of driver locations)
//Allocate enough space for all program sections (ST_PROGBITS)
//Copy from the image in RAM to the allocated space
//Go through all sections resolving external references against the kernel symbol table
//If all succeeded, you can use the "e_entry" field of the header as the offset from the load address to call the entry point (if one was specified),
// or do a symbol lookup, or just return a success error code.
//Once you can relocate ELF objects you'll be able to have drivers loaded when needed instead of at startup
// - which is always a Good Thing (tm).
// See:
// https://wiki.osdev.org/ELF#Relocation