// If you change this, remember to also change glibc_version.redef
#pragma once

#ifdef __x86_64__
    #ifndef __ASSEMBLER__
        __asm__(".symver memcmp,memcmp@GLIBC_2.2.5");
        __asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
        __asm__(".symver powf,powf@GLIBC_2.2.5");
        __asm__(".symver log,log@GLIBC_2.2.5");
        __asm__(".symver log2,log2@GLIBC_2.2.5");
        __asm__(".symver pow,pow@GLIBC_2.2.5");
        __asm__(".symver exp,exp@GLIBC_2.2.5");
        __asm__(".symver fcntl64,fcntl@GLIBC_2.2.5");
    #else
        .symver memcmp,memcmp@GLIBC_2.2.5
        .symver memcpy,memcpy@GLIBC_2.2.5
        .symver powf,powf@GLIBC_2.2.5
        .symver log,log@GLIBC_2.2.5
        .symver log2,log2@GLIBC_2.2.5
        .symver pow,pow@GLIBC_2.2.5
        .symver exp,exp@GLIBC_2.2.5
        .symver fcntl64,fcntl@GLIBC_2.2.5
    #endif
#else
    #ifndef __ASSEMBLER__
        __asm__(".symver memcmp,memcmp@GLIBC_2.0");
        __asm__(".symver memcpy,memcpy@GLIBC_2.0");
        __asm__(".symver powf,powf@GLIBC_2.0");
        __asm__(".symver log,log@GLIBC_2.0");
        __asm__(".symver log2,log2@GLIBC_2.1");
        __asm__(".symver pow,pow@GLIBC_2.0");
        __asm__(".symver exp,exp@GLIBC_2.0");
        __asm__(".symver fcntl64,fcntl@GLIBC_2.0");
        __asm__(".symver fcntl,fcntl@GLIBC_2.0");
    #else
        .symver memcmp,memcmp@GLIBC_2.0
        .symver memcpy,memcpy@GLIBC_2.0
        .symver powf,powf@GLIBC_2.0
        .symver log,log@GLIBC_2.0
        .symver log2,log2@GLIBC_2.1
        .symver pow,pow@GLIBC_2.0
        .symver exp,exp@GLIBC_2.0
        .symver fcntl64,fcntl@GLIBC_2.0
        .symver fcntl,fcntl@GLIBC_2.0
    #endif

#endif
