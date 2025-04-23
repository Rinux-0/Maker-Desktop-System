#pragma once
#ifndef _HHID_DESC_DEF_H_
#define _HHID_DESC_DEF_H_



#include "ddef.h"

#pragma pack(1)



// USB 设备描述符
typedef struct {
	u8  b_desc_len;					// desc长 (dft=0x18 (B))
	u8  b_desc_type;				// desc类型 (device:0x01)

	/* USB 规范版本号 (BCD码)
	*	【0x0110】1.1
	*	【0x0200】2.0
	*	【0x0210】2.1
	*	【0x0250】2.5
	*	【0x0300】3.0
	*	【0x0310】3.1
	*	【0x0320】3.2
	*/
	u16 w_bcd_usb;

	/* (若不在 接口层 ，而在 设备层 定义功能)
	*	b_device_class =
	*		0x00  规定 不用 接口关联描述符
				b_device_subclass = 0x00
	*		0x02  通信设备 (可改在 接口层 中声明)
	*		0x09  Hub
				b_device_subclass = 0x00
				b_device_protocol = 0x00: Full speed
				b_device_protocol = 0x01: High speed with single Transaction Translator
				b_device_protocol = 0x02: High speed with multiple Transaction Translators
				b_device_protocol = 0x03: SuperSpeed / SuperSpeedPlus
	*		0x0F  个人健康设备 (首选在 接口层 中声明)
	*		0xDC  诊断设备 (可改在 接口层 中声明)
				b_device_subclass = 0x01
				b_device_protocol = 0x01: USB2 Compliance Device
	*		0xE0  无线控制器 (设备层 只支持蓝牙，其他协议只能在 接口层 中声明))
				b_device_subclass = 0x01
				b_device_protocol = 0x01: Bluetooth programming interface (还需在 接口层 声明)
				b_device_subclass = 0x04: Bluetooth AMP controller (还需在 接口层 声明)
	*		0xEF  各种，规定 用 接口关联描述符
				b_device_subclass = 0x01
				b_device_protocol = 0x01: active sync
				b_device_protocol = 0x02: Palm sync
				b_device_subclass = 0x02
				b_device_protocol = 0x01: interface association descriptor
				b_device_protocol = 0x01: wire adapter multifunction peripheral (Wireless USB)
	*		0xFF  厂商专属 (可改在 接口层 中声明)
	*/
	u8  b_device_class;				// 该设备类型
	u8  b_device_subclass;			// 该设备子类型
	u8  b_device_protocol;			// 该设备协议

	/* end0 最大包长
	*	对USB2.0/USB1.x :
			低速( low speed)  8
			全速(full speed)  8 / 16 / 32 / 64
			高速(high speed)  64
		对于增强型超高速，end0的最大包大小为 2^{b_0maxpack_size} : 9 (即end0的最大包大小为512)
	*/
	u8  b_0maxpack_size;

	u16 w_id_vendor;				// 厂商ID
	u16 w_id_product;				// 产品ID
	u16 w_bcd_device;				// 该设备版本号 (BCD码)
	u8  b_str_index_mfr;			// 厂商名称 str desc 索引 (若无: 置0)
	u8  b_str_index_product;		// 产品名称 str desc 索引 (若无: 置0)
	u8  b_str_index_serial_num;		// 序列号索 str desc 索引 (有助于区分相同设备，若无: 置0)
	u8  b_num_cfgs;					// 当前运行速度下，该设备所支持的配置数 (>=1)
}desc_device_t;



// USB 配置描述符
typedef struct {
	u8  b_desc_len;					// desc长 (dft=0x09 (B))
	u8  b_desc_type;				// desc类型 (cfg:0x02)
	u16 w_total_cfg_size;			// 该配置所引出的后续所有描述符的总大小 (单位: B)
	u8  b_total_interface;			// 该配置所支持的接口数 (>=1)
	u8  b_cfg_code;					// set/get 该配置的确认码 (若值为0: set请求 会使该设备进入未配置状态)
	u8  b_cfg_str_index;			// 该配置的名称 str desc 索引 (若无: 置0)
	u8  b_cfg_attr;					// 该配置的属性 (@ref: desc_cfg_attr_t)
	u8  b_maxpower_consumption;		// 最大供电电流，单位是2mA
}desc_cfg_t;
// 配置属性 (@ref: desc_cfg_t::b_cfg_attr)
typedef struct {	// 位域 ("小端模式")
	u8 b_5_reserved : 5;			// 置0: 保留
	u8 b_1_remote_wakeup : 1;		// 置1: 支持远程唤醒
	u8 b_1_selfpowerd : 1;			// 置1: 支持自供电
	u8 b_1_reserved : 1;			// 置1: 保留 (USB1.0: 总线供电)
}desc_cfg_attr_t;



// USB 接口关联描述符
typedef struct {	// 其下接口号必须连续
	u8 b_desc_len;					// desc长 (dft=0x08 (B))
	u8 b_desc_type;					// desc类型 (cfg:0x0B)
	u8 b_first_interface;			// 其下首个接口编号 (@ref: desc_interface_t::b_interface_id)
	u8 b_interface_count;			// 其下接口数
	u8 b_func_class;				// 类型 (首选值: 其下首个接口的 @ref: desc_interface_t::b_interface_class) (不可为0，0xFF为厂商专属，其他值保留)
	u8 b_func_subclass;				// 子类型 (首选值: 其下首个接口的 @ref: desc_interface_t::b_interface_subclass)
	u8 b_func_protocol;				// 协议 (首选值: 其下首个接口的 @ref: desc_interface_t::b_interface_protocol)
	u8 i_func;						// 其 str desc 索引 (若无: 置0)
}desc_interface_assoc_t;



// USB 接口描述符
typedef struct {
	u8 b_desc_len;					// desc长 (dft=0x09 (B))
	u8 b_desc_type;					// desc类型 (cfg:0x04)
	u8 b_interface_id;				// 该接口编号 (dft=0x00) (各接口唯一，但该接口与其备用接口一致)
	u8 b_alternate_setting;			// 备用接口号 (dft=0x00) (对同一接口编号，每次只有一个备用接口可被激活)
	u8 b_num_end;					// 该接口的端点数 (不包含end0) (若值为0: 该接口用默认控制管道，即只用end0)

	/* (与该设备相同 (@ref: desc_device_t)，除非该设备由接口规定)
	*	b_interface_class =
	*		0x00	保留
				b_interface_subclass = 0x00
	*		0x01	音频
	*		0x02	通讯设备：通讯接口
	*		0x03	HID
				b_interface_subclass =
					0x00: 无需支持引导的HID设备
						b_interface_protocol: 无效
					0x01: 支持引导的USB设备（键鼠）
						b_interface_protocol =
							0x01: 键盘
							0x02: 鼠标
					其他: 保留
	*		0x05	物理
	*		0x06	图像
				b_interface_subclass = 0x01
				b_interface_protocol = 0x01: Imaging device
	*		0x07	打印机
	*		0x08	大容量存储
	*		0x09	Hub (还需在 设备层 中声明)
	*		0x0A	通讯设备：数据接口
	*		0x0B	智能卡
	*		0x0D	内容安全
				b_interface_subclass = 0x01
				b_interface_protocol = 0x01: Content security device
	*		0x0E	视频
	*		0x0F	个人健康设备 (可改在 设备层 中声明)
	*		0x10	Audio/Video(AV)
				b_interface_subclass = 0x01
				b_interface_protocol = 0x00: AVControl Interface
				b_interface_subclass = 0x02
				b_interface_protocol = 0x00: AVData Video Streaming Interface
				b_interface_subclass = 0x03
				b_interface_protocol = 0x00: AVData Audio Streaming Interface
	*		0xDC	诊断设备 (可改在 设备层 中声明)
				b_interface_subclass= 0x01
				b_interface_protocol = 0x01: USB2 compliance device
	*		0xE0	无线控制器
				b_interface_subclass = 0x01
				b_interface_protocol = 0x01: Bluetooth programming interface (还需在 设备层 中声明)
				b_interface_protocol = 0x02: UWB Radio control interface (Wireless USB)
				b_interface_protocol = 0x03: RNDIS
				b_interface_protocol = 0x04: Bluetooth AMP controller (还需在 设备层 中声明)
				b_interface_subclass = 0x02: Host and device wire adapters (Wireless USB)
				b_interface_protocol = 0x01: Host wire adapter control/data interface
				b_interface_protocol = 0x02: Device wire adapter control/data interface
				b_interface_protocol = 0x03: Device wire adapter isochronous interface
	*		0xEF	各种
				b_interface_subclass = 0x01
				b_interface_protocol = 0x01: active sync
				b_interface_protocol = 0x02: Palm sync
				b_interface_subclass = 0x02
				b_interface_protocol = 0x01: Interface Association Descriptor
				b_interface_protocol = 0x02: Wire Adapter Multifunction Peripheral programming interface
				b_interface_subclass = 0x03: Cable based association framework (Wireless USB)
				b_interface_protocol = 0x01: Cable based association framework device
				b_interface_subclass = 0x04
				b_interface_protocol = 0x01: RNDIS over Ethernet
				b_interface_protocol = 0x02: RNDIS over WiFi
				b_interface_protocol = 0x03: RNDIS over Maxim
				b_interface_protocol = 0x04: RNDIS over WWAN
				b_interface_protocol = 0x05: RNDIS for Raw IPv4
				b_interface_protocol = 0x06: RNDIS for Raw IPv6
				b_interface_protocol = 0x07: RNDIS for GPRS
				b_interface_subclass = 0x05: Machine vision device (USB3 Vision)
				b_interface_protocol = 0x00: USB3 Vision Control Interface
				b_interface_protocol = 0x01: USB0 Vision Event Interface
				b_interface_protocol = 0x02: USB3 Vision Streaming Interface
	*		0xFE	应用程序专属
				b_interface_subclass = 0x01: Device firmware upgrade
				b_interface_subclass = 0x02: IrDA bridge
				b_interface_subclass = 0x03: Test and measurement (USBTMC)
				b_interface_protocol = 0x00: Complies with USBTMC spec
				b_interface_protocol = 0x01: Complies with USBTMC USB488 subclass
	*		0xFF	厂商专属 (可改在 设备层 中声明)
	*/
	u8 b_interface_class;			// 该接口类型
	u8 b_interface_subclass;		// 该接口子类型
	u8 b_interface_protocol;		// 该接口协议

	u8 b_interface_str_index;		// 该接口名称的 str desc 索引 (若无: 置0)
}desc_interface_t;



// USB HID描述符
typedef struct {	//  放在 接口描述符 中 (要求 b_interface_class = 0x03, b_interface_subclass = 0x01)
	u8  b_desc_len;					// desc长 (>=0x09)(单位 : B)
	u8  b_desc_type;				// desc类型 (cfg:0x21)

	/*	Hid协议版本 (BCD)
	*	【0x0100】1.0
	*	【0x0101】1.1
	*	【0x0200】2.0
	*/
	u16 w_bcd_hid;

	u8  b_country;				// 国家/地区 【5】China 【33】US 【...】...

	/*	附属描述符数量 (>=1)
	*	附属分为两种 :
	*		报文描述符 : 至少1个
	*		物理描述符 : 可选的
	*/
	u8  b_num_desc;

	u8  b_1desc_type;				// 附属1报表描述符的类型
	u16 w_1desc_len;				// 附属1报表描述符的长度

	// optional
	// u8  b_ndesc_type;
	// u16 w_ndesc_len;
	// ...
}desc_hid_t;



// USB 端点描述符
typedef struct {
	u8  b_desc_len;					// desc长 (dft=0x07 (B))
	u8  b_desc_type;				// desc类型 (device:0x05)
	u8  b_end_addr;					// 端点地址 (@ref: desc_end_addr_t)
	u8  b_end_attr;					// 端点属性 (@ref: desc_end_attr_t)

	/*	端点支持的最大包大小
	*		对 USB1.x : 范围0–1023 (余位需置0)
	*		对 USB2.0 :
				范围0–1024 (占用bit(0-10))
				bit(13-15) : 置0，保留
				bit(11-12) :
					对？高速？等时和中断端点 :
						每微帧所支持的额外事务数目
						【0b00】无额外事务(每微帧1个事务)
						【0b01】1个额外事务(每微帧2个事务)
						【0b10】2个额外事务(每微帧3个事务)
						【0b11】保留
					对其他端点 : 置0，保留
	*		对 USB3.x 的 增强型超高速 :
				对控制端点 : 512
				对批量端点 : 1024
				对中断和等时端点 :
					决定于超高速端点伙伴描述符(SuperSpeed Endpoint Companion Descriptor)中的bMaxBurst
					若bMaxBurst > 0 : 1024
					若bMaxBurst = 0 :
						对等时端点 : 取值范围0-1024，
						对中断端点 : 取值范围1-1024
	*/
	u16 w_maxpack_size;

	/*	轮询周期
	*	对中断端点 :
			在这个间隔周期里面，主机必须安排端点事务
			对全速/低速中断端点 :
				该字段为最大等待时间，主机必须在这个周期里安排端点事务
				单位ms，取值范围1-255
				全速端点 : 速度限定1-255ms
				低速端点 : 速度限定10-255ms
 	*	对高速中断端点:
			轮询周期为 2^{b_interval-1} 微帧
			取值范围为1-16
	*	对等时端点：
			在这个间隔周期里面，主机必须安排端点事务
			轮询周期为 2^{b_interval-1} 微帧
				对全速/高速/增强型超高速等时端点 :
					取值范围1-16
					全速端点的时间范围1ms-32.768s
					高速/超高速端点的时间范围125us-4.096s
				对 USB1.x 的等时端点 : 1
	*	对于高速批量和控制OUT端点 :
			此字段表示最大NAK速率，传输中要符合这个字段的要求。
			【0】端点从不发出NAK
			【其他值】用NAK响应OUT处理后，下次重试是在b_interval个微帧后
			范围0-255 (设备常置0)
	*	对其他批量和控制端点 : 保留
	*/
	u8  b_interval;
}desc_end_t;
// 端点地址
typedef struct {	// 位域 ("小端模式")
	u8 b_4_end_num : 4;				// 端点号 (低速设备至多3个(0–2)，其他设备至多16个(0–15))
	u8 b_3_reserved : 3;			// 置0: 保留
	u8 b_1_dir : 1;					// 传输方向 【0】OUT 【1】IN (控制端 忽略此位)
}desc_end_addr_t;
// 端点属性
typedef struct {	// 位域 ("小端模式")
	/* 传输类型
	*	【0b00】控制ctrl
	*	【0b01】等时isochronous
	*	【0b10】批量bulk
	*	【0b11】中断interrupt
	*/
	u8 b_2_transfer_type : 2;

	/*
	*	等时端点 :
			b_2_sync_type 保留
				【0b00】非同步
				【0b01】异步
				【0b10】自适应
				【0b10】同步
			b_2_usage_type
				【0b00】数据端点
				【0b01】反馈端点 (要求: @ref: b_2_transfer_type 为 0b01, @ref: b_2_sync_type 为 0b00)
				【0b10】隐式反馈数据端点
				【0b11】保留
	*	增强型超高速中断端点 :
			b_2_sync_type 保留
			b_2_usage_type
				【0b00】周期
				【0b01】通知
				【0b10】【0b11】保留
	*	其他端点 :
			b_2_sync_type 保留
			b_2_usage_type 保留
	*/
	u8 b_2_sync_type : 2;			// 同步类型 (仅iso传输有效)
	u8 b_2_usage_type : 2;			// 使用类型 (仅iso传输有效)

	u8 b_2_reserved : 2;			// 置0: 保留
}desc_end_attr_t;



/// @todo
// USB 字串描述符 (可选)
typedef struct {
	u8  b_desc_len;					// 字串描述符的长度
	u8  b_desc_type;				// 固定值String(0x03)
	u16 w_unicode_str[];
}desc_str_t;



/// @todo
// USB 设备限定描述符
typedef struct {
	u8  b_desc_len;					// 固定值18B
	u8  b_desc_type;				// 固定值Device(0x01)
	u16 w_bcd_usb;					// USB Spec版本
	u8  b_device_class;				// 设备类型
	u8  b_device_subclass;			// 设备子类型
	u8  b_device_protocol;			// 协议类型
	u8  b_maxpack_size0;			// EP0的最大包长度
	u8  b_num_cfg;					// 配置数量>=1
	u8  b_reserved;					// 保留置0
}desc_device_qualifier_t;



/// @todo
// USB Hub描述符	具体上网查
typedef struct {
	u8  b_desc_len;
	u8  b_desc_type;				// 固定值Hub(0x29)
	u8  b_num_port;					// 端口数量
	u16 w_hub_attr;
	u8  b_pwron2pwrok;				// 从通电正常工作所需的时间，以2ms为单位
	u8  b_hub_ctrl_current[0];		// 0应改为端口数值
	u8  ab_device_removable;		// 各个端口是否可动态移除 【0】可, 【1】不可
	u8  ab_port_pwr_ctrl_mask[64];	// 为了兼容USB1.0所保留字段。各位应全为1，位数应是8的整数倍
}desc_hub_t;
// Hub属性
typedef struct {
	u16 logic_pwr_switch_mode;
	u8  identify_composite_device;
	u16 over_current_protect_mode;
	u16 think_time;
	u8  support_port_led;
	u8  reserved[8];
}desc_hub_attr_t;



/// @todo
// USB 功能描述符
typedef struct {
	u8 b_desc_len;
	u8 b_desc_type;
	u8 b_desc_subtype;
	u8 ab_func_specific_data[];	// data[0] ~ data[N-1]
}desc_func_t;



// USB HID报表描述符
typedef struct {
	/// @todo 待补充
}desc_report_t;



#endif // _HHID_DESC_DEF_H_
