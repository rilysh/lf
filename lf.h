#ifndef LF_H
#define LF_H    1

#ifndef TRUE
    #define TRUE    1
#endif
#ifndef FALSE
    #define FALSE   0
#endif

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x)[0])
#define DEFAULT_UNICODE     "\uf016"
#define INT_DIGITS      19
#define MAX_SIZE        128

#define STOR_SIZE       30
#ifdef IEC_SPEC
    static char *suff[12] = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB", "RB", "QB" };
    #define UNIT    1024.0
#else
    static char *suff[10] = { "B", "K", "M", "G", "T", "P", "E", "Z", "Y" };
    #define UNIT    1000.0
#endif

#define RED             "\033[0;91m"
#define END             "\033[0m"

struct FileName {
    char *ex;
    char *unicode;
};

#endif /* LF_H */
