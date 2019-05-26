#ifndef JOSHUA
#define JOSHUA
#include <linux/ioctl.h>

// This is the required size of the buffer.

#define JOSHUA_MAX 40

// This macro will give us the right ioctl code.

#define JOSHUA_GET_QUOTE _IOR(0xff, 1, char *)

#endif
