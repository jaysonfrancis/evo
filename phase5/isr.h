// isr.h, 159
// Team Evo
#ifndef _ISR_H_
#define _ISR_H_

void CreateISR(int);
void TerminateISR();
void TimerISR();
void GetPidISR();
void SleepISR(int);
void SemWaitISR();
void SemPostISR();
int SemGetISR();
void MsgSndISR();
void MsgRcvISR();
void IRQ7ISR(v);
#endif
