// isr.h, 159
// Team Evo
#ifndef _ISR_H_
#define _ISR_H_

void CreateISR(int);
void TerminateISR();
void TimerISR();
void GetPidISR();
void SleepISR();
void SemWaitISR(int);
void SemPostISR(int);
int SemGetISR(int);
void IRQ7ISR(void);

#endif
