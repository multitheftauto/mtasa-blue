// If you change this, remember to also change glibc_version.redef
#pragma once

#ifdef __x86_64__
    #ifndef __ASSEMBLER__
        // __asm__(".symver memcmp,memcmp@GLIBC_2.2.5");
    #else
        // .symver memcmp,memcmp@GLIBC_2.2.5
    #endif
#else
    #ifndef __ASSEMBLER__
        // __asm__(".symver memcmp,memcmp@GLIBC_2.0");
    #else
        // .symver memcmp,memcmp@GLIBC_2.0
    #endif
#endif
