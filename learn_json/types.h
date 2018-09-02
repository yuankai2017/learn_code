#ifndef _H_TYPES_H_
#define _H_TYPES_H_


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


typedef unsigned long long uint64;
typedef unsigned int       uint32;
typedef unsigned short     uint16;
typedef unsigned char      uint8;

typedef char      int8;
typedef short     int16;
typedef int       int32;
typedef long long int64;

typedef unsigned char PlmnValueTag;
typedef unsigned char CellValueTag;

#include "dev_param.h"

#define FC_IOCTL_CODE_BASE 0x0
#define FC_MAGIC           'P'
#define VALID               1
#define INVALID             0


#define IOCTL_MSG( code ) _IOWR(FC_MAGIC, code, uint8)

extern void usleep(int);

#endif /* _H_TYPES_H_ */

