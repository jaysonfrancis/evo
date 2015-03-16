// syscall.h
// Team evo
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

int GetPid(void);      // no input, 1 return
void Sleep(int);   // 1 input, no return
void SemWait(int SemID);
void SemPost(int SemID);
int SemGet(int count);

#endif
