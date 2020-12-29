// If you change this, remember to also change glibc_version.redef
#pragma once

#ifdef __x86_64__
    #ifndef __ASSEMBLER__
        __asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
        __asm__(".symver memcmp,memcmp@GLIBC_2.2.5");
        __asm__(".symver powf,powf@GLIBC_2.2.5");
    #else
        .symver memcpy,memcpy@GLIBC_2.2.5
        .symver memcmp,memcmp@GLIBC_2.2.5
        .symver powf,powf@GLIBC_2.2.5
    #endif
#else
    #ifndef __ASSEMBLER__
        __asm__(".symver memcpy,memcpy@GLIBC_2.0");
        __asm__(".symver memcmp,memcmp@GLIBC_2.0");
        __asm__(".symver powf,powf@GLIBC_2.0");
    #else
        .symver memcpy,memcpy@GLIBC_2.0
        .symver memcmp,memcmp@GLIBC_2.0
        .symver powf,powf@GLIBC_2.0
    #endif

#endif
