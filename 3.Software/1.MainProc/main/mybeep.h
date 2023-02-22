#ifndef MY_BEEP_H
#define MY_BEEP_H

#include <asm/ioctl.h>

#define MY_BEEP_MAGIC 'b'

#define MY_BEEP_OFF _IO(MY_BEEP_MAGIC, 0)
#define MY_BEEP_ON _IO(MY_BEEP_MAGIC, 1)

#endif
