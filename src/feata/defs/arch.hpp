#pragma once


#if defined(_WIN32)
    #define FEATA_SYSTEM_WIN
#elif defined(__unix__)
    #define FEATA_SYSTEM_UNIX
#elif defined(__APPLE__)
    #define FEATA_SYSTEM_APPLE
#endif

#if defined(_WIN64) || defined(__x86_64__)
    #define FEATA_ARCH_64BIT
#else
    #define FEATA_ARCH_32BIT
#endif
