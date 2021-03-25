
CC=arm-linux-gnueabi-gcc
ROOT=../..
SBD=$(ROOT)/scewl_bus_driver

IPATH=-I../common
IPATH+=-I$(ROOT)

check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1)))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1))

all:
	$(call check_defined, SCEWL_ID)
	$(CC) -o main main.c ../common/common.c $(SBD)/sbd.o $(IPATH) -Wall -Wextra -Werror -DSCEWL_ID=$(SCEWL_ID)

clean:
	-rm main 2>/dev/null
