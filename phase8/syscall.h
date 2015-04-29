// syscall.h
// Team evo
#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "type.h"

int GetPid(void);      // no input, 1 return
void Sleep(int);   // 1 input, no return
void SemWait(int SemID);
void SemPost(int SemID);
int SemGet(int count);
void MsgSnd(msg_t *p);
void MsgRcv(msg_t *p);

void TipIRQ3();


#endif
