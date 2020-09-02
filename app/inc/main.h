#ifdef __cplusplus
extern "C" {
#endif

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

#include "string.h"
#include "delay.h"

#define INDEX 0
#define NBR_NOEUD 4
#define PERIOD 2

/* Global functions */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);

void printchar(char **str, int c);

void init(void);
void synchro(void);
void waitPart(int);

#endif /* APP_INC_MAIN_H_ */
#ifdef __cplusplus
}
#endif
