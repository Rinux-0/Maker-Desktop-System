# Maker-Desktop 创客桌面

## 1. 项目简介

`Maker-Desktop` 是一个基于海思小熊派 `Hi3870` [[memory:2791424]] 开发板的、用于 **快速原型开发与验证** 的桌面创客项目框架。

与将所有功能集成在一个固件中的传统项目不同，本项目设计的核心理念是 **“单一功能、分时开发”**。在任意时刻，系统都只专注于某一个特定的功能模块（例如“智慧旋钮”或“星闪接收器”）。开发者可以通过修改配置，轻松切换当前要运行的“应用”，从而实现对不同硬件原型和软件功能的独立开发、调试与验证。

目前，项目内置了 **星闪接收器**、**智慧旋钮**、**健康监测**、**键盘**、**小键盘** 和 **桌面助手** 等多个可独立运行的应用模块。

## 2. 快速上手

### 2.1. 环境准备
确保您已根据 [BearPi__ws63-SDK 官方文档](https://www.bearpi.cn/core_board/bearpi/pico/h3870/) 搭建好基础的开发环境。

### 2.2. 项目放置
将本项目 `maker-desktop` 文件夹放置于 SDK 的 `application/samples/` 目录下。

### 2.3. 切换与编译
1.  **选择运行模式**: 打开 `application/samples/maker-desktop/Kconfig` 文件。
    *   确保选中 `device` 模式，而非 `test` 模式。
2.  **选择功能模块**: 打开 `application/samples/maker-desktop/device/Kconfig` 文件。
    *   在 `choice "device"` 中，选择您希望本次编译运行的具体设备，例如 `DEVICE_DYNAMIC` (智慧旋钮)。
3.  **执行编译**: 回到 SDK 根目录，执行编译命令。编译系统将仅编译您所选定的功能模块及其依赖。

## 3. 软件架构：分层与分发

本项目的软件架构设计精良，通过分层、分发和宏定义，实现了高度的灵活性和可扩展性。

### 3.1. 项目结构
```
.
├── comm
│   ├── format
│   │   └── hid
│   ├── i2c
│   ├── sle
│   │   ├── client
│   │   └── server
│   ├── spi
│   ├── uart
│   └── wifi
│       └── udp_client
├── device
│   ├── deskaide
│   │   ├── asrpro
│   │   ├── sound
│   │   └── vfd
│   ├── dynamic
│   │   ├── fingerprint
│   │   ├── knob
│   │   └── nfc
│   ├── health
│   │   ├── lamp
│   │   ├── rate
│   │   └── temperature
│   ├── keyboard
│   ├── keypad
│   └── receiver
├── test
│   ├── keytest
│   └── tmptest
│       ├── multi_thread
│       └── receiver
└── util
    ├── color
    │   ├── core
    │   ├── color_ctrl.c
    │   └── mode
    ├── ddef.h        - 公用定义
    └── tool
        ├── ttimer.c  - 定时器
        └── ttool.c   - 通用工具库（delay / sleep / gpio）
```

### 3.2. 架构分层
整个项目框架被清晰地划分为四个层次：
- **应用入口层 (`demo.c`)**: 项目的最高层和总入口。它不包含任何业务逻辑，只负责创建 `MakerDesktop-System` 主线程，并依次调用下一层的初始化和主循环函数。
- **设备应用层 (`device/`)**: 项目的核心业务逻辑层。该层包含所有可独立运行的具体设备应用。在任意一次编译中，只有被选中的那一个设备应用是活跃的。
- **通信协议层 (`comm/`)**: 为设备层提供通信能力的公共服务层。它封装了 `SLE`, `UART`, `SPI`, `I2C` 等多种通信协议的底层实现。
- **公用工具层 (`util/`)**: 提供与业务无关的、最底层的通用服务，如日志打印、定时器、看门狗等。

### 3.3. “宏分发链”设计模式
为了实现“单一应用切换”，设备层采用了一种巧妙的“宏分发链”模式：
1.  **初级分发 (`demo.c`)**: `demo.c` 中的 `INIT(DEMO_NAME)()` 会被预处理器替换为 `device_init()`，将执行权交给设备层分发器。
2.  **次级分发 (`device.c`)**: `device.c` 中再次调用 `INIT(DEVICE_NAME)()`，此时宏会被替换为具体设备的初始化函数，如 `dynamic_init()`，从而将执行权精确地交给最终选定的设备应用。
整个流程 `demo.c -> device.c -> dynamic.c` 完全在编译时确定，运行时零开销。

### 3.4. 通信层设计模式
与设备层“一次只跑一个”不同，通信层采用“多路并发”的设计。
- **按需编译**: `comm.c` 通过 `#if defined(CONFIG_COMM_...)` 条件编译，将设备层所依赖的所有通信模块全部编译进来。
- **统一初始化**: `comm_init()` 会依次调用所有需要的通信模块的初始化函数。
- **核心枢纽**: `comm.c` 中的 `sle_r_int_uart_handler` 等回调函数，是实现 `SLE` 无线与 `UART` 有线之间数据透传和协议转换的核心，是整个框架数据交互的灵魂。

## 4. 功能模块详解
以下是项目当前支持的、可独立运行的设备应用。

### 4.1. 星闪接收器 (Receiver)
- **Kconfig 选项**: `DEVICE_RECEIVER`
- **角色**: `SLE` 客户端 (Client)
- **核心功能**: 作为一个专用的无线数据接收器。它通过 `SLE` 协议接收来自各个服务端设备的数据，并通过 `comm.c` 中的核心枢纽函数，将接收到的数据从 `UART` 接口打印或转发出去。
- **关键源码**: `device/receiver/receiver.c`
- **主要函数**:
    - `receiver_init()`: 初始化 `Receiver` 特有的配置。
    - `receiver_oneloop()`: 在主循环中执行的轮询任务，可用于未来扩展。

### 4.2. 智慧旋钮 (Dynamic Knob)
- **Kconfig 选项**: `DEVICE_DYNAMIC`
- **角色**: `SLE` 服务端 (Server)
- **核心功能**: 驱动一个带按键的EC11编码器旋钮。通过 `GPIO` 中断检测旋钮的旋转和按压事件，并将这些事件封装成特定格式的数据，通过 `SLE` 网络广播出去。
- **关键源码**: `device/dynamic/dynamic.c`, `device/dynamic/knob/knob.c`
- **主要函数**:
    - `dynamic_init()`: 初始化旋钮所需的 `GPIO` 管脚和中断。
    - `dynamic_report()`: 上报旋钮状态的核心函数。
- **可配置项**: `device/dynamic/dynamic_def.h` 中定义了旋钮使用的具体 `GPIO` 引脚。

### 4.3. 健康监测 (Health)
- **Kconfig 选项**: `DEVICE_HEALTH`
- **角色**: `SLE` 服务端 (Server)
- **核心功能**: 通过 `I2C` 协议与外部健康传感器（如心率、温度、距离传感器）通信，定时读取传感器数据，并通过 `SLE` 网络广播。
- **关键源码**: `device/health/health.c`
- **主要函数**:
    - `health_init()`: 初始化 `I2C` 总线以及用于定时采样的定时器。
    - `health_report_timer()`: 定时器的回调函数，负责触发一次数据采集和上报。
- **可配置项**: `device/health/health_def.h` 中定义了采样周期等参数。

### 4.4. 键盘 / 小键盘 (Keyboard / Keypad)
- **Kconfig 选项**: `DEVICE_KEYBOARD` / `DEVICE_KEYPAD`
- **角色**: `SLE` 服务端 (Server)
- **核心功能**: 驱动一个矩阵键盘。通过 `SPI` 协议与矩阵扫描芯片（或通过软件模拟）通信，获取按键状态，并将按键事件封装成 `HID` 指令，通过 `SLE` 网络广播。
- **关键源码**: `device/keypad/keypad.c`, `device/keyboard/keyboard.c`
- **主要函数**:
    - `keypad_init()`: 初始化用于键盘扫描的 `SPI` 总线。
    - `keypad_scan()`: 执行一次全键盘扫描的核心函数。
- **可配置项**: `device/keypad/keypad_def.h` 中定义了键盘的行列布局等。

### 4.5. 桌面助手 (Desk Aide)
- **Kconfig 选项**: `DEVICE_DESKAIDE`
- **角色**: `SLE` 服务端 (Server)
- **核心功能**: 一个功能丰富的 `SLE` 服务端应用集合，用于演示复杂的HCI（人机交互）场景。它本身也包含多个可配置的子模块。
- **关键源码**: `device/deskaide/deskaide.c`
- **子模块**:
    - `asrpro`: 语音识别模块，可通过 `UART` 接收指令，通过 `SLE` 上报结果。
    - `sound`: 声音播放模块。
    - `vfd`: 真空荧光显示屏驱动模块。

---
*本 README 最后更新于 YYYY-MM-DD。*
