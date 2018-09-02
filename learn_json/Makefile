

SHELL = /bin/sh
RM = rm -f

srcdir = .
VPATH = .:.
top_srcdir = ..

DEFS =

PREX = arm-linux-
CC = $(PREX)gcc
AR = $(PREX)ar
LD = $(PREX)ld
STRIP = $(PREX)strip
CFLAGS =-O3 -std=c99
DEFS =
LOCAL_CFLAGS = -Wall
CPPFLAGS =

# make in 2.11
LIB_DIR = /home/public/inc_lib_danbing/lib
INCLUDE_BASE = /home/public/inc_lib_danbing/include

# make in 55
#LIB_DIR = /home/public01_resource/emlas/emlas3b/source/emlas3b/lib
#INCLUDE_BASE = /usr/include

INCLUDES = -I$(INCLUDE_BASE)/libxml2/  -I./ -I$(LIB_DIR)/lib_static_arm_modify_2k_statis/
CCFLAGS = $(DEFS) $(LOCAL_CFLAGS) $(CPPFLAGS) $(INCLUDES) $(CFLAGS)
LDFLAGS = -L./
ARFLAGS = -ru

rl_OBJECTS =

rl_SOURCES =

OBJECTS = lte.o fpga_resource.o  view.o  api.o  rf_485.o\
		l3_msg.o statistic.o lte_param.o internal_dispatch.o auto_adjust_phase.o smart_ant.o smart_eng.o sys_timing.o smart_active_identify.o\
		product_info.o smart_weaken.o sys_state.o smart_pin.o smart_sche.o analyze_sms.o sys_debug.o accessanalyze.o smart_sync.o feature_list.o data_transmit.o pos_adjust.o

lte_control:$(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) -lpthread -lm -lrt libxml2.so.6 $(LIB_DIR)/lib_static_arm_modify_2k_statis/libstatic.a ./libdesauth.a $(LIB_DIR)/dtzip/dtzip.a
	@$(STRIP) lte_control
	@chmod +x lte_control

.c.o:
	${RM} $@
	$(CC) $(CCFLAGS) -o $@ -lrt -c $<
#	$(CC) $(CCFLAGS) -o $@ -c -std=c99 $<

clean:
	$(RM) *.o
	$(RM) ./lte_control
