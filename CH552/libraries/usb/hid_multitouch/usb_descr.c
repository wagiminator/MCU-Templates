// ===================================================================================
// USB Descriptors
// ===================================================================================

#include "usb_descr.h"

// ===================================================================================
// Device Descriptor
// ===================================================================================
__code USB_DEV_DESCR DevDescr = {
  .bLength            = sizeof(DevDescr),       // size of the descriptor in bytes: 18
  .bDescriptorType    = USB_DESCR_TYP_DEVICE,   // device descriptor: 0x01
  .bcdUSB             = 0x0110,                 // USB specification: USB 1.1
  .bDeviceClass       = 0,                      // interface will define class
  .bDeviceSubClass    = 0,                      // unused
  .bDeviceProtocol    = 0,                      // unused
  .bMaxPacketSize0    = EP0_SIZE,               // maximum packet size for Endpoint 0
  .idVendor           = USB_VENDOR_ID,          // VID
  .idProduct          = USB_PRODUCT_ID,         // PID
  .bcdDevice          = USB_DEVICE_VERSION,     // device version
  .iManufacturer      = 1,                      // index of Manufacturer String Descr
  .iProduct           = 2,                      // index of Product String Descriptor
  .iSerialNumber      = 3,                      // index of Serial Number String Descr
  .bNumConfigurations = 1                       // number of possible configurations
};

// ===================================================================================
// Configuration Descriptor
// ===================================================================================
__code USB_CFG_DESCR_HID CfgDescr = {

  // Configuration Descriptor
  .config = {
    .bLength            = sizeof(USB_CFG_DESCR),  // size of the descriptor in bytes
    .bDescriptorType    = USB_DESCR_TYP_CONFIG,   // configuration descriptor: 0x02
    .wTotalLength       = sizeof(CfgDescr),       // total length in bytes
    .bNumInterfaces     = 1,                      // number of interfaces: 1
    .bConfigurationValue= 1,                      // value to select this configuration
    .iConfiguration     = 0,                      // no configuration string descriptor
    .bmAttributes       = 0x80,                   // attributes = bus powered, no wakeup
    .MaxPower           = USB_MAX_POWER_mA / 2    // in 2mA units
  },

  // Interface Descriptor
  .interface0 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 0,                      // number of this interface: 0
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 1,                      // number of endpoints used: 1
    .bInterfaceClass    = USB_DEV_CLASS_HID,      // interface class: HID (0x03)
    .bInterfaceSubClass = 1,                      // boot interface
    .bInterfaceProtocol = 2,                      // mouse
    .iInterface         = 4                       // interface string descriptor
  },

  // HID Descriptor
  .hid0 = {
    .bLength            = sizeof(USB_HID_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_HID,      // HID descriptor: 0x21
    .bcdHID             = 0x0111,                 // HID class spec version (BCD: 1.11)
    .bCountryCode       = 0,                      // not supported
    .bNumDescriptors    = 1,                      // number of report descriptors: 1
    .bDescriptorTypeX   = USB_DESCR_TYP_REPORT,   // descriptor type: report (0x22)
    .wDescriptorLength  = sizeof(ReportDescr)     // report descriptor length
  },

  // Endpoint Descriptor: Endpoint 1 (IN, Interrupt)
  .ep1IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP1_IN,   // endpoint: 1, direction: IN (0x81)
    .bmAttributes       = USB_ENDP_TYPE_INTER,    // transfer type: interrupt (0x03)
    .wMaxPacketSize     = EP1_SIZE,               // max packet size
    .bInterval          = 10                      // polling intervall in ms
  }
};

// ===================================================================================
// HID Report Descriptor
// ===================================================================================
__code uint8_t ReportDescr[] ={
  0x05, 0x0D, // USAGE_PAGE(Digitizers)
  0x09, 0x04, // USAGE     (Touch Screen)
  0xA1, 0x01, // COLLECTION(Application)
  // define the actual amount of fingers that are concurrently touching the
  // screen
  0x09, 0x54, //   USAGE (Contact count)
  0x25, 0x7f, //   LOGICAL_MAXIMUM (128)
  0x95, 0x01, //   REPORT_COUNT(1)
  0x75, 0x08, //   REPORT_SIZE (8)
  0x81, 0x02, //   INPUT (Data,Var,Abs)
              // declare a finger collection
  0x05, 0x0D, //   USAGE_PAGE(Digitizers)
  0x09, 0x22, //   USAGE (Finger)
  0xA1, 0x02, //   COLLECTION (Logical)
  // declare an identifier for the finger
  0x09, 0x51, //     USAGE (Contact Identifier)
  0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
  0x75, 0x08, //     REPORT_SIZE (8)
  0x95, 0x01, //     REPORT_COUNT (1)
  0x81, 0x02, //     INPUT (Data,Var,Abs)
  // declare Tip Switch & In range
  0x09, 0x42, //     USAGE (Tip Switch)
  0x09, 0x32, //     USAGE (In Range)
  0x15, 0x00, //     LOGICAL_MINIMUM (0)
  0x25, 0x01, //     LOGICAL_MAXIMUM (1)
  0x75, 0x01, //     REPORT_SIZE (1)
  0x95, 0x02, //     REPORT_COUNT(2)
  0x81, 0x02, //     INPUT (Data,Var,Abs)
  // declare the 6 padding bits as constant so the driver will ignore them
  0x95, 0x06, //     REPORT_COUNT (6)
  0x81, 0x03, //     INPUT (Cnst,Ary,Abs)
  // declare pressure
  0x09, 0x30, //     USAGE (Pressure)
  0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
  0x75, 0x08, //     REPORT_SIZE (8)
  0x95, 0x01, //     REPORT_COUNT (1)
  0x81, 0x02, //     INPUT (Data,Var,Abs)
  // define absolute X and Y coordinates of 16 bit each (percent values
  // multiplied with 100)
  0x05, 0x01,       //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,       //     Usage (X)
  0x09, 0x31,       //     Usage (Y)
  0x16, 0x00, 0x00, //     Logical Minimum (0)
  0x26, 0x10, 0x27, //     Logical Maximum (10000)
  0x36, 0x00, 0x00, //     Physical Minimum (0)
  0x46, 0x10, 0x27, //     Physical Maximum (10000)
  0x66, 0x00, 0x00, //     UNIT (None)
  0x75, 0x10,       //     Report Size (16),
  0x95, 0x02,       //     Report Count (2),
  0x81, 0x02,       //     Input (Data,Var,Abs)
  0xC0,             //   END_COLLECTION
  // define the maximum amount of fingers that the device supports
  0x05, 0x0D, //   USAGE_PAGE(Digitizers)
  0x09, 0x55, //   USAGE (Contact Count Maximum)
  0x25, 0x7f, //   LOGICAL_MAXIMUM (127)
  0xB1, 0x02, //   FEATURE (Data,Var,Abs)
  0xC0 // END_COLLECTION
};

__code uint8_t ReportDescrLen = sizeof(ReportDescr);

// ===================================================================================
// String Descriptors
// ===================================================================================

// Language Descriptor (Index 0)
__code uint16_t LangDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(LangDescr), 0x0409 };  // US English

// Manufacturer String Descriptor (Index 1)
__code uint16_t ManufDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ManufDescr), MANUFACTURER_STR };

// Product String Descriptor (Index 2)
__code uint16_t ProdDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ProdDescr), PRODUCT_STR };

// Serial String Descriptor (Index 3)
__code uint16_t SerDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(SerDescr), SERIAL_STR };

// Interface String Descriptor (Index 4)
__code uint16_t InterfDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(InterfDescr), INTERFACE_STR };
