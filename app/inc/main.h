#ifdef __cplusplus
extern "C" {
#endif

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

#include "string.h"
#include "bsp.h"
#include "delay.h"

/* Global functions */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);

void printchar(char **str, int c);

#endif /* APP_INC_MAIN_H_ */
#ifdef __cplusplus
}
#endif
