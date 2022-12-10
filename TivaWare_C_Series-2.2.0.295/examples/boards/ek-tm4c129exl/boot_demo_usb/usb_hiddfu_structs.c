//*****************************************************************************
//
// usb_hiddfu_structs.c - Data structures defining the USB mouse device.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C129EXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidmouse.h"
#include "usblib/device/usbddfu-rt.h"
#include "usblib/device/usbdcomp.h"
#include "usb_hiddfu_structs.h"

//*****************************************************************************
//
// The languages supported by this device.
//
//*****************************************************************************
const uint8_t g_pui8LangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//*****************************************************************************
//
// The manufacturer string.
//
//*****************************************************************************
const uint8_t g_pui8ManufacturerString[] =
{
    (17 + 1) * 2,
    USB_DTYPE_STRING,
    'T', 0, 'e', 0, 'x', 0, 'a', 0, 's', 0, ' ', 0, 'I', 0, 'n', 0, 's', 0,
    't', 0, 'r', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 's', 0,
};

//*****************************************************************************
//
// The product string.
//
//*****************************************************************************
#if 1
const uint8_t g_pui8ProductString[] =
{
    (34 + 1) * 2,
    USB_DTYPE_STRING,
    'M', 0, 'o', 0, 'u', 0, 's', 0, 'e', 0, ' ', 0, 'w', 0, 'i', 0, 't', 0,
    'h', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0, ' ', 0,
    'F', 0, 'i', 0, 'r', 0, 'm', 0, 'w', 0, 'a', 0, 'r', 0, 'e', 0, ' ', 0,
    'U', 0, 'p', 0, 'g', 0, 'r', 0, 'a', 0, 'd', 0, 'e', 0
};
#else
const uint8_t g_pui8ProductString[] =
{
    (37 + 1) * 2,
    USB_DTYPE_STRING,
    'K', 0, 'e', 0, 'y', 0, 'b', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0,
    'w', 0, 'i', 0, 't', 0, 'h', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0,
    'c', 0, 'e', 0, ' ', 0, 'F', 0, 'i', 0, 'r', 0, 'm', 0, 'w', 0, 'a', 0,
    'r', 0, 'e', 0, ' ', 0, 'U', 0, 'p', 0, 'g', 0, 'r', 0, 'a', 0, 'd', 0,
    'e', 0
};
#endif


//*****************************************************************************
//
// The serial number string.
//
//*****************************************************************************
const uint8_t g_pui8SerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The interface description string.
//
//*****************************************************************************
#if 1
const uint8_t g_pui8HIDInterfaceString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'M', 0, 'o', 0, 'u', 0, 's', 0,
    'e', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0,
    'a', 0, 'c', 0, 'e', 0
};
#else
const uint8_t g_pui8HIDInterfaceString[] =
{
    (22 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'K', 0, 'e', 0, 'y', 0, 'b', 0,
    'o', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0,
    'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0

};
#endif

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
#if 1
const uint8_t g_pui8ConfigString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'M', 0, 'o', 0, 'u', 0, 's', 0,
    'e', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0,
    'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0
};
#else
const uint8_t g_pui8ConfigString[] =
{
    (26 + 1) * 2,
    USB_DTYPE_STRING,
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'K', 0, 'e', 0, 'y', 0, 'b', 0,
    'o', 0, 'a', 0, 'r', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0,
    'f', 0, 'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0,
    'o', 0, 'n', 0
};
#endif

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const uint8_t * const g_ppui8StringDescriptors[] =
{
    g_pui8LangDescriptor,
    g_pui8ManufacturerString,
    g_pui8ProductString,
    g_pui8SerialNumberString,
    g_pui8HIDInterfaceString,
    g_pui8ConfigString
};

#define NUM_STRING_DESCRIPTORS  (sizeof(g_ppui8StringDescriptors) /           \
                                 sizeof(g_ppui8StringDescriptors[0]))

#if 1
//*****************************************************************************
//
// The HID mouse device initialization and customization structures.
//
//*****************************************************************************
tUSBDHIDMouseDevice g_sMouseDevice =
{
    USB_VID_TI_1CBE,
    USB_PID_COMP_HID_DFU,
    500,
    USB_CONF_ATTR_SELF_PWR,
    MouseHandler,
    (void *)&g_sMouseDevice,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS
};
#else
//*****************************************************************************
//
// The HID keyboard device initialization and customization structures.
//
//*****************************************************************************
tUSBDHIDKeyboardDevice g_sKeyboardDevice =
{
    USB_VID_TI_1CBE,
    USB_PID_KEYBOARD,
    500,
    USB_CONF_ATTR_SELF_PWR | USB_CONF_ATTR_RWAKE,
    KeyboardHandler,
    (void *)&g_sKeyboardDevice,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS
};
#endif


//*****************************************************************************
//
// The DFU runtime interface initialization and customization structures
//
//*****************************************************************************
tUSBDDFUDevice g_sDFUDevice =
{
    DFUDetachCallback,
    (void *)&g_sDFUDevice
};

//****************************************************************************
//
// The number of device class instances that this composite device will
// use.
//
//****************************************************************************
#define NUM_DEVICES             2

//****************************************************************************
//
// The array of devices supported by this composite device.
//
//****************************************************************************
tCompositeEntry g_psCompDevices[NUM_DEVICES];

//****************************************************************************
//
// Additional workspace required by the composite driver to hold a lookup
// table allowing mapping of composite interface and endpoint numbers to
// individual device class instances.
//
//****************************************************************************
uint32_t g_pui32CompWorkspace[NUM_DEVICES];

//****************************************************************************
//
// The instance data for this composite device.
//
//****************************************************************************
tCompositeInstance g_sCompInstance;

//****************************************************************************
//
// Allocate the Device Data for the top level composite device class.
//
//****************************************************************************
tUSBDCompositeDevice g_sCompDevice =
{
    //
    // Stellaris VID.
    //
    USB_VID_TI_1CBE,

    //
    // Stellaris PID for composite HID/DFU device.
    //
    USB_PID_COMP_HID_DFU,

    //
    // This is in milliamps.
    //
    500,

    //
    // Bus powered device.
    //
    USB_CONF_ATTR_BUS_PWR,

    //
    // Device event handler function pointer (receives connect, disconnect
    // and other device-level notifications).
    //
#if 1
    MouseHandler,
#else
    KeyboardHandler,
#endif

    //
    // The string table.
    //
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS,

    //
    // The Composite device array.
    //
    NUM_DEVICES,
    g_psCompDevices,
};

//****************************************************************************
//
// A buffer into which the composite device can write the combined config
// descriptor.
//
//****************************************************************************
uint8_t g_pui8DescriptorBuffer[DESCRIPTOR_BUFFER_SIZE];
