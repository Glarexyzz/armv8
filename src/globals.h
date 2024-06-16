//
// Created by aa8123 on 10/06/24.
//

#ifndef ARMV8_2_GLOBALS_H
#define ARMV8_2_GLOBALS_H

#include <stdbool.h>

#define MAXLINELEN 1024
#define MAXERRORLEN 128
#define ZR 31

// Error checking MACROS
#define ERROR_AND_FREE_ON_COND(condition, error_message, file_context, list, num_list) \
  do { \
      if (condition) { \
          FREE_ON_COND(condition, list, num_list); \
          ERROR_ON_COND(condition, error_message, file_context); \
          return false; \
      } \
  } while (0)

#define FREE_ON_COND(condition, list, num_list) \
do { \
    if (condition) { \
        for (int i = 0; i < (num_list); i++) free((list)[i]); \
        return false; \
    } \
  } while (0)

#define ERROR_ON_COND(condition, error_message, file_context) \
do { \
    if (condition) { \
    error((error_message), (file_context)); \
    return false; \
    } \
  } while (0)

#define FREE_LIST(list, num_list) \
  do { \
    for (int i = 0; i < (num_list); i++) free((list)[i]); \
  } while (0)


struct context {
    int nerrors;
    char cur_line[MAXLINELEN];
    int prog_lineno; // Only program instructions
    int file_lineno; // Includes labels, new lines and comments
};
typedef struct context *context;
//extern context GLOBAL_CONTEXT; - decided to define in assemble
context create_context(void);
void reset_linenos(context context);
void free_context(context context);
void error( char *msg, context context );

#endif //ARMV8_2_GLOBALS_H
