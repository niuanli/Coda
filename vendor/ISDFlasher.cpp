// ISDFlasher by https://github.com/kmx360
// Plus some bits from https://github.com/buddyjojo/Xbox360-ISD
#include <xtl.h>
#include <cstring>
#include "ISDFlasher.h"

// Milliseconds. Conservative limits; validate on the supported consoles.
static const DWORD READY_TIMEOUT_MS = 1000;
static const DWORD OPERATION_TIMEOUT_MS = 5000;
static const DWORD ERASE_TIMEOUT_MS = 30000;

BOOL isd_xfer(BYTE* Buf, DWORD Len)
{
    if (!Buf && Len != 0) return FALSE;
    HalGpioControl(GPIO_CLEAR, ISD_SS_N, ISD_SS_N);
    for (; Len > 0; --Len)
    {
        DWORD start = GetTickCount();
        while ((HalGpioControl(GPIO_READ, 0, 0) & ISD_RDY) == 0)
        {
            if ((DWORD)(GetTickCount() - start) >= READY_TIMEOUT_MS)
            {
                HalGpioControl(GPIO_SET, ISD_SS_N | ISD_CLK, ISD_SS_N | ISD_CLK);
                return FALSE;
            }
        }
        BYTE tx = *Buf;
        BYTE rx = 0;
        for (int bit = 0; bit < 8; ++bit, tx <<= 1)
        {
            HalGpioControl(GPIO_CLEAR, ISD_CLK, ISD_CLK);
            rx = (rx << 1) | ((HalGpioControl(GPIO_READ, 0, 0) & ISD_MISO) != 0);
            HalGpioControl((tx & 0x80) ? GPIO_SET : GPIO_CLEAR, ISD_MOSI, ISD_MOSI);
            HalGpioControl(GPIO_SET, ISD_CLK, ISD_CLK);
        }
        *Buf++ = rx;
    }
    HalGpioControl(GPIO_SET, ISD_SS_N, ISD_SS_N);
    return TRUE;
}

DWORD isd_read_device_id(void)
{
    HalGpioControl(GPIO_SET, ISD_SS_N | ISD_CLK | ISD_MOSI, ISD_SS_N | ISD_CLK | ISD_MOSI);
    HalGpioControl(GPIO_DIR, ISD_SS_N | ISD_CLK | ISD_MOSI,
        ISD_SS_N | ISD_CLK | ISD_MOSI | ISD_RDY | ISD_MISO);
    BYTE reset[] = { ISD_RESET };
    BYTE setup[] = { 0x11 }; // Preserve the original device initialization sequence.
    BYTE status[] = { ISD_READ_STATUS, 0 };
    BYTE id[] = { ISD_READ_ID, 0, 0, 0, 0 };
    if (!isd_xfer(reset, sizeof(reset)) || !isd_xfer(setup, sizeof(setup)) ||
        !isd_xfer(status, sizeof(status)) || !isd_xfer(id, sizeof(id))) return 0;
    DWORD deviceId = (DWORD)id[1] << 24 | (DWORD)id[2] << 16 | (DWORD)id[3] << 8 | id[4];
    BYTE down[] = { ISD_PWR_DN };
    reset[0] = ISD_RESET;
    if (!isd_xfer(down, sizeof(down)) || !isd_xfer(reset, sizeof(reset))) return 0;
    return deviceId;
}

static void isd_set_address(BYTE* buf, UINT32 offset)
{
    buf[1] = (BYTE)(offset >> 16);
    buf[2] = (BYTE)(offset >> 8);
    buf[3] = (BYTE)offset;
}

BOOL isd_flash_read(UINT32 page, BYTE* buffer)
{
    if (!buffer || page >= ISD_IMAGE_SIZE / ISD_READ_SIZE) return FALSE;
    BYTE buf[4 + ISD_READ_SIZE] = { ISD_DIGITAL_READ, 0, 0, 0 };
    isd_set_address(buf, page * ISD_READ_SIZE);
    if (!isd_xfer(buf, sizeof(buf))) return FALSE;
    memcpy(buffer, &buf[4], ISD_READ_SIZE);
    return TRUE;
}

static BOOL isd_wait(BYTE command, BYTE mask, BOOL set, DWORD timeout)
{
    DWORD start = GetTickCount();
    for (;;)
    {
        BYTE buf[] = { command, 0 };
        if (!isd_xfer(buf, sizeof(buf))) return FALSE;
        BYTE status = buf[command == ISD_READ_INTERRUPT ? 1 : 0];
        if (((status & mask) != 0) == (set != FALSE)) return TRUE;
        if ((DWORD)(GetTickCount() - start) >= timeout) return FALSE;
        Sleep(1);
    }
}

BOOL isd_power_up()
{
    BYTE buf[] = { ISD_PWR_UP };
    return isd_xfer(buf, sizeof(buf)) &&
        isd_wait(ISD_READ_STATUS, ISD_DBUF_RDY, TRUE, OPERATION_TIMEOUT_MS) &&
        isd_wait(ISD_READ_STATUS, ISD_VM_BSY, FALSE, OPERATION_TIMEOUT_MS);
}

BOOL isd_chip_erase()
{
    BYTE buf[] = { ISD_CHIP_ERASE, 0x01 };
    return isd_xfer(buf, sizeof(buf)) &&
        isd_wait(ISD_READ_STATUS, ISD_CMD_BSY, FALSE, ERASE_TIMEOUT_MS);
}

BOOL isd_flash_write(UINT32 page, const BYTE* buffer)
{
    if (!buffer || page >= ISD_IMAGE_SIZE / ISD_WRITE_SIZE) return FALSE;
    BYTE buf[4 + ISD_WRITE_SIZE] = { ISD_DIGITAL_WRITE, 0, 0, 0 };
    isd_set_address(buf, page * ISD_WRITE_SIZE);
    memcpy(&buf[4], buffer, ISD_WRITE_SIZE);
    return isd_xfer(buf, sizeof(buf)) &&
        isd_wait(ISD_READ_STATUS, ISD_CMD_BSY, FALSE, OPERATION_TIMEOUT_MS) &&
        isd_wait(ISD_READ_INTERRUPT, ISD_WR_FIN, TRUE, OPERATION_TIMEOUT_MS);
}

BOOL isd1200_play_vp(UINT16 index)
{
    BYTE buf[] = { 0xA6, (BYTE)(index >> 8), (BYTE)index };
    return isd_xfer(buf, sizeof(buf));
}

