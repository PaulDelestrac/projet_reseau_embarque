/*
 * main.h
 *
 *  Created on: 16 août 2018
 *      Author: Laurent
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

/* Global functions */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);

void printchar(char **str, int c);




#endif /* APP_INC_MAIN_H_ */
#ifdef __cplusplus
}
#endif
