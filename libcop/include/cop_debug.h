#ifndef cop_debug_h
#define cop_debug_h

extern int gVerbose;

#ifdef VERBOSE_ON
#define verbose_print(...) { if (gVerbose) {printf("[%s:%d]: ",__func__, __LINE__ );printf(__VA_ARGS__);}}
#else
#define verbose_print(...) ;;
#endif

#ifdef VERBOSE_ON
#define verbose_memprint(name,length) { \
    if (gVerbose) {\
    long i; \
    for (i = 0; i < length; i++) { \
        if ((i % 16) == 0) { \
            if (i) \
                printf("\n"); \
            printf("%p: ", (name + i));  \
        } \
        printf("%02x", *((char *)name + i)); \
        if (((i + 1) % 2) == 0) \
            printf(" "); \
    } \
    printf("\n"); \
    }\
}
#else
#define verbose_memprint(name, length) ;;
#endif

/* assert that a statement must be true otherwise we leave that function*/
#define CHECK(statement, error_message, error_num, error_ret) {\
    if (!(statement)) {\
        errno = error_num;\
        fprintf(stderr, "[%s:%d]: %s\n", __func__, __LINE__, error_message);\
	return error_ret;\
    }\
}
#endif
