
/*
 * subsystems.h
 */ 

#ifndef __SUBSYSTEMS_H__
#define __SUBSYSTEMS_H__

#ifdef __cplusplus
extern "C" {
#endif

// Enabled subsystems
#define TASK_ENABLE_UART		1
#define TASK_ENABLE_DS18B20		1
#define TASK_ENABLE_I2C			1
#define TASK_ENABLE_WATCHDOG	0
#define TASK_ENABLE_MONITOR		0
#define TASK_ENABLE_SWITCH		0
#define TASK_ENABLE_MQTT		1
#define TASK_ENABLE_SELFTEST	1

#ifdef __cplusplus
}
#endif

#endif /* __SUBSYSTEMS_H__ */
