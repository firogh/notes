---
tags: kernel
title: file
date: 2015-02-27 15:46:12
category: kernel
---
#Reference
http://www.iecc.com/linker/linker10.html
http://www.linuxjournal.com/article/1060

#ELF executable and linkable format.
Used for Relocatable file(object file, kernel moudle), Executable file, Dynamic library, Core dump. 
##Structure
ELF header, readelf -h
Program header table, readelf -l
Segments, readelf --segments 
Sections, readelf --sections
Section header table, readelf -S 
.symtab, Symbol table store index of string
.strtab, String table is a array of all strings split by \0
.hash, utiliy for quire symbols
.shstrtab, Section header string table store section name for ELF,like .text, .data...
Symbols table, nm, readelf -s 

###Relocatable file(object file, kernel module)
ELF header.
Sections.
Section header table.

### Executeable file, dynamic library
ELF header.
Program header table.
Segments.
Section header table.

