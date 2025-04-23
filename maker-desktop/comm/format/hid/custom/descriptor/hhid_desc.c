#include "hhid_desc.h"

#include "ddef.h"



bool hid_desc_is_inited = false;


const u8 hid_desc_device[] = {
	0x12,            //bLenght
	0x01,            //bDescriptorType
	0x00,0x02,       //bcdUSB
	0x00,            //bDeviceClass
	0x00,            //bDeviceSubClass
	0x00,            //bDeviceProtocol
	0x40,            //bMaxPacketSize0
	0x3d,0x41,       //idVendor
	0x07,0x21,       //idProduct
	0x00,0x01,       //bcdDevice
	0x01,            //iManufacturer
	0x02,            //iProduct
	0x03,            //iSerialNumber
	0x01             //bNumConfigurations
};


const u8 hid_desc_config[] = {
	//配置描述符
	0x09,                                   //bLenght
	0x02,                                   //bDescriptorType
	0x34, 0x00,                             //wTotalLength
	0x01,                                   //bNumInterfaces
	0x01,                                   //bConfigurationValue
	0x00,                                   //iConfiguration
	0xA0,                                   //bmAttributes
	0x32,                                   //MaxPower
	//接口描述符 (键盘功能)
	0x09,                                   //bLenght
	0x04,                                   //bDescriptorType
	0x00,                                   //bInterfaceNumber
	0x00,                                   //bAlternateSetting
	0x01,                                   //bNumbEndpoints
	0x03,                                   //bInterfaceClass
	0x01,                                   //bInterfaceSubClass
	0x01,                                   //bInterfaceProtocol
	0x00,                                   //iInterface
	//HID类描述符
	0x09,                                   //bLenght
	0x21,                                   //bDescriptorType
	0x11, 0x01,                             //bcdHID
	0x00,                                   //bCountryCode
	0x01,                                   //bNumDescriptors
	0x22,                                   //bDescriptorType
	0x39, 0x00,                             //wDescriptorLength
	//端点描述符
	0x07,                                   //bLenght
	0x05,                                   //bDescriptorType
	0x81,                                   //bEndpointAddress
	0x03,                                   //bmAttributes
	0x0A, 0x00,                             //wMaxPacketSize
	0x01                                    //bInterval
};


const u8 hid_desc_hid[] = {
	0x05, 0x01,       //   Usage Page (Generic Desktop),
	0x09, 0x06,       //   Usage (Keyboard),
	0xA1, 0x01,       //   Collection (Application),
	// bitmap of modifiers(修饰键)
	0x05, 0x07,       //   Usage Page (Keyboard),
	0x95, 0x08,       //   Report Count (8),
	0x75, 0x01,       //   Report Size  (1),
	0x15, 0x00,       //   Logical Minimum (0),
	0x25, 0x01,       //   Logical Maximum (1),
	0x19, 0xE0,       //   Usage Minimum (Keyboard LeftControl),
	0x29, 0xE7,       //   Usage Maximum (Keyboard Right GUI),
	0x81, 0x02,       //   Input (Data, Variable, Absolute),
	// bitmap of keys(普通键)
	0x05, 0x07,       //   Usage Page (Keyboard),
	0x95, 0x78,       //   Report Count (120),
	0x75, 0x01,       //   Report Size  (1),
	0x15, 0x00,       //   Logical Minimum (0),
	0x25, 0x01,       //   Logical Maximum (1),
	0x19, 0x00,       //   Usage Minimum (0),
	0x29, 0x65,       //   Usage Maximum (101),
	0x81, 0x02,       //   Input (Data, Variable, Absolute),
	// LED output report
	0x05, 0x08,       //   Usage Page (LEDs)
	0x95, 0x03,       //   Report Count (3)
	0x75, 0x01,       //   Report Size  (1)
	0x19, 0x01,       //   Usage Minimum (Num Lock   1)
	0x29, 0x03,       //   Usage Maximum (Scroll Lock   3)
	0x91, 0x02,       //   Output (Data,Var,Abs)
	//output凑共1byte(无实际用处)
	0x95, 0x05,       //   Report Count (5)
	0x75, 0x01,       //   Report Size  (1)
	0x91, 0x01,       //   Output (Cnst,Var,Abs)

	0xC0              //   End Collection
};
