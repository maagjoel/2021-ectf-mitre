
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
	$(call check_defined, SCEWL_ID \
		C2_ID \
		ALT_FLOOR \
		ALT_CEIL \
		UAVID_FLAG \
		RECOVERY_FLAG \
		DROP_FLAG \
		NO_FLY_ZONE_FLAG)
	$(CC) -o main main.c ../common/common.c $(SBD)/sbd.o $(IPATH) -Wall -Wextra -Werror -DSCEWL_ID=$(SCEWL_ID) \
		-DC2_ID=$(C2_ID) \
		-DALT_FLOOR=$(ALT_FLOOR) \
		-DALT_CEIL=$(ALT_CEIL) \
		-DUAVID_FLAG=$(UAVID_FLAG) \
		-DRECOVERY_FLAG=$(RECOVERY_FLAG) \
		-DDROP_FLAG=$(DROP_FLAG) \
		-DNO_FLY_ZONE_FLAG=$(NO_FLY_ZONE_FLAG)

clean:
	-@rm main
