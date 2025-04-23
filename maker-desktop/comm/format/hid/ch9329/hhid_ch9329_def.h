#pragma once
#ifndef _HHID_CH9329_DEF_H_
#define _HHID_CH9329_DEF_H_



#include "ddef.h"

/** 命令码-状态值 **/
#define HID_CH9329_CMD_OK			(0x00) // 命令执行成功
#define HID_CH9329_CMD_ERR_TIMEOUT	(0xE1) // 串口接收一个字节超时
#define HID_CH9329_CMD_ERR_HEAD		(0xE2) // 串口接收包头字节出错
#define HID_CH9329_CMD_ERR_CMD		(0xE3) // 串口接收命令码错误
#define HID_CH9329_CMD_ERR_SUM		(0xE4) // 累加和检验值不匹配
#define HID_CH9329_CMD_ERR_PARA		(0xE5) // 参数错误
#define HID_CH9329_CMD_ERR_OPERATE	(0xE6) // 帧正常，执行失败

/** 命令码 **/
#define HID_CH9329_CMD_GET_INFO				 (0x01) // 获取芯片版本等信息
#define HID_CH9329_CMD_SEND_KB_GENERAL_DATA	 (0x02) // 发送USB键盘普通数据
#define HID_CH9329_CMD_SEND_KB_MEDIA_DATA	 (0x03) // 发送USB键盘多媒体数据
#define HID_CH9329_CMD_SEND_MS_ABS_DATA		 (0x04) // 发送USB绝对鼠标数据
#define HID_CH9329_CMD_SEND_MS_REL_DATA		 (0x05) // 发送USB相对鼠标数据
#define HID_CH9329_CMD_SEND_MY_HID_DATA		 (0x06) // 发送USB自定义HID设备数据
#define HID_CH9329_CMD_READ_MY_HID_DATA		 (0x87) // 读取USB自定义HID设备数据（无回复）
#define HID_CH9329_CMD_GET_PARA_CFG			 (0x08) // 获取参数配置
#define HID_CH9329_CMD_SET_PARA_CFG			 (0x09) // 设置参数配置
#define HID_CH9329_CMD_GET_USB_STRING		 (0x0A) // 获取USB字符串描述符配置
#define HID_CH9329_CMD_SET_USB_STRING		 (0x0B) // 设置USB字符串描述符配置
#define HID_CH9329_CMD_SET_DEFAULT_CFG		 (0x00) // 恢复出厂默认配置
#define HID_CH9329_CMD_RESET				 (0x0F) // 复位芯片

#pragma pack(1)
/// @param sum = 无data时的值 + data （手册计算法则有误，需参考不同cmd具体包的例子）
typedef struct {
	u8 head[2];
	u8 addr;
	u8 cmd;
	u8 length;		// length <= 64
	u8 data[64];	// data[length] = sum (可能越界，但更简便)
	u8 sum;			// 包校验和
}hid_ch9329_pack;



#endif // _HHID_CH9329_DEF_H_
