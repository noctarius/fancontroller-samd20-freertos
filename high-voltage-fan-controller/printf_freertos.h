
/*
 * printf_freertos.h
 */ 

#ifndef PRINTF_FREERTOS_H_
#define PRINTF_FREERTOS_H_

int freertos_sprintf(char *out, const char *format, ...);

int freertos_snprintf( char *buf, unsigned int count, const char *format, ... );

#endif /* PRINTF_FREERTOS_H_ */
