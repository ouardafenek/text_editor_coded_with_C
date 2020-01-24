#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H
 
#ifdef DEBUG
#define DEBUG_PRINTF(x) printf("%s\n"(x))
#else
#define DEBUG_PRINTF(x) 
#endif

#endif
