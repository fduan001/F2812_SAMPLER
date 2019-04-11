/*
 * shellconsole.h
 *
 *  Created on: 2014-8-8
 *      Author:
 */

#ifndef SHELLCONSOLE_H_
#define SHELLCONSOLE_H_
#include "shellctype.h"
#include <stdarg.h>
extern s8 console_buffer[];
void shellputs(const s8 *s);
void shellputc(const s8 c);
s32 shellgetc(void);
s32 shellprintf(const s8 *fmt, ...);
s32 tstc(void);
s32 ctrlc(void);
u8 set_shellprompt (char* p);
s32 parse_line (s8 *line, s8 *argv[]);
s32 readline (const s8 *const prompt);
s32 readline_into_buffer (const s8 *const prompt, s8 *buffer);
void clear_ctrlc(void);
s32 had_ctrlc (void);
s32 run_command (const s8 *cmd, s32 flag);

int shellvsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

void shellsetudpprintmode(u8 enable);
u8  shellgetudprintmode();

#define PRINTF  shellprintf

#endif /* CONSOLE_H_ */
