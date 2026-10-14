#ifndef __ISDFLASHER_H__
#define __ISDFLASHER_H__
#pragma once

// ISD pins
#define ISD_CLK		(1 << 6)
#define ISD_MOSI	(1 << 7)
#define ISD_RDY		(1 << 11)
#define ISD_SS_N	(1 << 14)
#define ISD_MISO	(1 << 15)

// ISD commands
#define ISD_PWR_UP			0x10
#define ISD_PWR_DN			0x12
#define ISD_RESET			0x14
#define ISD_CHIP_ERASE		0x26
#define ISD_READ_STATUS		0x40
#define ISD_READ_INTERRUPT	0x46
#define ISD_READ_ID			0x48
#define ISD_DIGITAL_WRITE	0xA0
#define ISD_DIGITAL_READ	0xA2

// ISD status bits
#define ISD_CMD_BSY			(1 << 0)
#define ISD_CBUF_FUL		(1 << 1)
#define ISD_VM_BSY			(1 << 2)
#define ISD_INT				(1 << 5)
#define ISD_DBUF_RDY		(1 << 6)
#define ISD_PD				(1 << 7)

// ISD interrupt status bits
#define ISD_WR_FIN			(1 << 5)

// Kernel GPIO and time stamp APIs
extern "C" {

#define GPIO_DIR	0
#define GPIO_SET	4
#define GPIO_CLEAR	5
#define GPIO_READ	6

	DWORD HalGpioControl(DWORD Command, DWORD Set, DWORD Clear);

	/*
	typedef struct _TIME_STAMP_BUNDLE {
		UINT64 InterruptTime;
		UINT64 SystemTime;
		UINT32 TickCount;
		UINT32 Padding;
	} TIME_STAMP_BUNDLE;

	extern TIME_STAMP_BUNDLE KeTimeStampBundle;
	*/

};

#define ISD_IMAGE_SIZE 0xB000
#define ISD_READ_SIZE 512
#define ISD_WRITE_SIZE 16

BOOL isd_xfer(BYTE* Buf, DWORD Len);

extern DWORD isd_read_device_id(void);

BOOL isd_flash_read(UINT32 page, BYTE* buffer);



BOOL isd_power_up();

BOOL isd_chip_erase();

BOOL isd_flash_write(UINT32 page, const BYTE* buffer);







BOOL isd1200_play_vp(UINT16 index);

#endif
