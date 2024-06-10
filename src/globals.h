//
// Created by aa8123 on 10/06/24.
//

#ifndef ARMV8_2_GLOBALS_H
#define ARMV8_2_GLOBALS_H

#define MAXLINELEN 1024
extern int NERRORS;
extern char CUR_LINE[MAXLINELEN];
extern int CUR_LINENO; // Line number with labels as well (for debugging only!!)
void error( char *line, int lineno, char *msg );
void error_glob_vals(char *msg);
bool isalpha_str(const char *str);

#endif //ARMV8_2_GLOBALS_H
