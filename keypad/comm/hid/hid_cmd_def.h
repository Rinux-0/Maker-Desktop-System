#ifndef _HID_CMD_DEF_H_
#define _HID_CMD_DEF_H_



/** 命令码-状态值 **/
#define CMD_SUCCESS		0x00 // 命令执行成功
#define CMD_ERR_TIMEOUT 0xE1 // 串口接收一个字节超时
#define CMD_ERR_HEAD	0xE2 // 串口接收包头字节出错
#define CMD_ERR_CMD		0xE3 // 串口接收命令码错误
#define CMD_ERR_SUM		0xE4 // 累加和检验值不匹配
#define CMD_ERR_PARA	0xE5 // 参数错误
#define CMD_ERR_OPERATE 0xE6 // 帧正常，执行失败

/** 命令码 **/
#define CMD_GET_INFO				0x01 // 获取芯片版本等信息
#define CMD_SEND_KB_GENERAL_DATA	0x02 // 发送USB键盘普通数据
#define CMD_SEND_KB_MEDIA_DATA		0x03 // 发送USB键盘多媒体数据
#define CMD_SEND_MS_ABS_DATA		0x04 // 发送USB绝对鼠标数据
#define CMD_SEND_MS_REL_DATA		0x05 // 发送USB相对鼠标数据
#define CMD_SEND_MY_HID_DATA		0x06 // 发送USB自定义HID设备数据
#define CMD_READ_MY_HID_DATA		0x87 // 读取USB自定义HID设备数据（无回复）
#define CMD_GET_PARA_CFG			0x08 // 获取参数配置
#define CMD_SET_PARA_CFG			0x09 // 设置参数配置
#define CMD_GET_USB_STRING			0x0A // 获取USB字符串描述符配置
#define CMD_SET_USB_STRING			0x0B // 设置USB字符串描述符配置
#define CMD_SET_DEFAULT_CFG			0x00 // 恢复出厂默认配置
#define CMD_RESET					0x0F // 复位芯片



#endif // _HID_CMD_DEF_H_
