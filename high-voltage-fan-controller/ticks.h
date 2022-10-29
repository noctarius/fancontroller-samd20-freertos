/*
 * ticks.h
 */ 

#ifndef __TICKS_H__
#define __TICKS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ticks10s	pdMS_TO_TICKS(10000)
#define ticks1s		pdMS_TO_TICKS(1000)
#define ticks750ms	pdMS_TO_TICKS(750)
#define ticks250ms	pdMS_TO_TICKS(250)
#define ticks100ms	pdMS_TO_TICKS(100)
#define ticks20ms	pdMS_TO_TICKS(20)

#ifdef __cplusplus
}
#endif

#endif /* __TICKS_H__ */