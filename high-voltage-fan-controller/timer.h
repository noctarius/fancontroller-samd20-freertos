
/*
 * timer.h
 */ 

#ifndef __TIMER_H__
#define __TIMER_H__

int xTickToMs();

typedef struct Timer Timer;
struct Timer {
	unsigned long systick_period;
	unsigned long end_time;
};

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

#endif /* __TIMER_H__ */