#*******************************************************************************
# 
# Filename: Makefile
# Author  : Zhang Hui 
# Version : V1.0.0
# 
# Last modified: 2021-03-31 14:48
# Description  : 
# 
#*******************************************************************************




PWD:=$(shell pwd)


SRC_PATH:= $(PWD)/../src/
LIB_PATH:= $(PWD)/../lib
INC_PATH:= $(PWD)/../include/
TARGET := avdecodec_demo
TAR_DIR:= $(PWD)/../bin/


CC = g++
 
CFLAGS:=
CFLAGS+= -g
CFLAGS+= -pthread
CFLAGS+= -I$(INC_PATH)/libavcodec
CFLAGS+= -I$(INC_PATH)

LDFLAGS:= -pthread
#LDFLAGS+= -lz
#LDFLAGS+= -lm
#LDFLAGS+= -lbz2

##链接顺序会导致编译不过！！！
LDFLAGS+= -L$(LIB_PATH)
LDFLAGS+= -lavformat
LDFLAGS+= -lavcodec
LDFLAGS+= -lavdevice
LDFLAGS+= -lavfilter
LDFLAGS+= -lavutil
LDFLAGS+= -lswresample
LDFLAGS+= -lswscale
LDFLAGS+= -L$(LIB_PATH)libx264.so
LDFLAGS+= -L$(LIB_PATH)libx265.so
LDFLAGS+= -lx264
LDFLAGS+= -lx265
LDFLAGS+= -std=c++11




all:$(TARGET)

$(TARGET) : main.o
	@echo "zhanghui1"
	$(CC) $^ $(LDFLAGS) -o $(TAR_DIR)$(TARGET) 

main.o: $(SRC_PATH)main.cpp
	@echo "zhanghui2"
	g++ -c $(CFLAGS) $(SRC_PATH)main.cpp  -o main.o	$(LDFLAGS)
	

test:
	@echo "$(LDFLAGS)"
	
test1:
	@echo "$(CFLAGS)"

.PHONY : clean
clean:
	rm -rf $(PWD)/*.o
	rm -rf $(TAR_DIR)/$(TARGET)
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
