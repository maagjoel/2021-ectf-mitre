
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
		PRNG_SEED \
	   	ATCK_DZ_ID \
		ATCK_DZ_X \
		ATCK_DZ_Y \
		TRST_DZ_ID \
		PACKAGE_FLAG)
	$(CC) -o main main.c ../common/common.c $(SBD)/sbd.o $(IPATH) -Wall -Wextra -Werror -DSCEWL_ID=$(SCEWL_ID) \
		-DPRNG_SEED=$(PRNG_SEED) \
	   	-DATCK_DZ_ID=$(ATCK_DZ_ID) \
		-DATCK_DZ_X=$(ATCK_DZ_X) \
		-DATCK_DZ_Y=$(ATCK_DZ_Y) \
	   	-DTRST_DZ_ID=$(TRST_DZ_ID) \
		-DPACKAGE_FLAG=$(PACKAGE_FLAG)

clean:
	-@rm main
