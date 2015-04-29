// tool.h, 159
// Team Evo

#ifndef _TOOL_H
#define _TOOL_H

#include "type.h" // q_t needs be defined in code below

void MyBZero(char *, int);
int DeQ(q_t *);
void EnQ(int, q_t *);
void MsgEnQ(msg_t *p, msg_q_t *q);
msg_t *MsgDeQ(msg_q_t *p);
void MyStrcpy(char *dest, char *src);
int MyStrcmp(char *, char *);
int MyStrlen(char *);

#endif

