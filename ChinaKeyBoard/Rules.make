#Note:

#	1.core board module define - Modified for local native build

MODULE=NATIVE

# Binary file output path
export DEST_PATH		:= ~/tftpboot

ifeq ($(MODULE), NATIVE)
	export ARCH					:=x86_64
	export PLATFORM				:=native
	export CROSS_COMPILE_PATH	:=
	export CROSS_COMPILE		:=
	MINIGUI_ROOT_DIR			:=/home/shkwon/Projects/LVGL/ChinaKeyBoard/install
	MINIGUILIB 					:=-lpng -lfreetype -lanimate -lcommon_animates -lz -ljpeg -lminigui_sa
	
	CFLAGS	+= -DNATIVE -D_STAND_ALONE

endif

# Use native tools for compilation
LD		:= ld
CC		:= gcc
CPP		:= c++
AR		:= ar
STRIP	:= strip





