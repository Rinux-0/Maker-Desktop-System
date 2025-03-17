# Maker-Desktop

创客桌面

## 预备

###### 1. 开发环境搭建

- [BearPi__ws63-SDK](https://www.bearpi.cn/core_board/bearpi/pico/h3863/)

###### 2. ws63-SDK

- [Gitee__BearPi-Pico-H3863-SDK](https://gitee.com/bearpi/bearpi-pico_h3863)

###### 3. 键盘

- 小键盘（测试）：16键
![keypad实物图](./RES/keypad.jpg "keypad")

- 主键盘（正式）：87键
![keyboard实物图](./RES/keyboard.jpg "keyboard")

###### 4. 通信模块

![通信用_UART-HID实物图](./RES/UART-HID.jpg "通信用_UART-HID")

![调试用_UART-USB实物图](./RES/UART-USB.jpg "调试用_UART-USB")

---

## 配置

###### 1. 下载 本仓库

- 下载后，直接放到 ***SDK/application/samples/*** 目录下

###### 2. **配置 *Kconfig*** （若不希望直接覆盖）

- 打开 ***SDK/application/samples/Kconfig*** ，添加

```kconfig

config ENABLE_KEYBOARD_SAMPLE
    bool "Enable the Sample of keyboard."
    default y
    depends on SAMPLE_ENABLE
    help This option means enable the sample of keyboard.
if ENABLE_KEYBOARD_SAMPLE
    osource "application/samples/keyboard/Kconfig"
endif

config ENABLE_KEYPAD_SAMPLE
    bool "Enable the Sample of keypad."
    default n
    depends on SAMPLE_ENABLE
    help This option means enable the sample of keypad.
if ENABLE_KEYPAD_SAMPLE
    osource "application/samples/keypad/Kconfig"
endif

```

###### 3. **配置 *CMakeLists.txt*** （若不希望直接覆盖）

- 打开 ***SDK/application/samples/CMakeLists.txt*** ，添加

```cmake

if(DEFINED CONFIG_ENABLE_KEYBOARD_SAMPLE)
    add_subdirectory_if_exist(keyboard)
endif()

if(DEFINED CONFIG_ENABLE_KEYPAD_SAMPLE)
    add_subdirectory_if_exist(keypad)
endif()

```

---

## 说明

###### 1. 调试代码

- 默认不包含 调试代码 ，**编译前** 可在 *Kconfig* 对应子项目中开启，例如：
 ![调试代码](./RES/debug-config.png)

- 可用的调试函数: debug_uart_print()

- 取消 调试代码 后，调用调试函数之处将自动被 **宏替换** 为空语句

> keypad中，详见 `keypad/comm/uart/uart.h`

- 调试代码所用 UART 与 项目不同，不会冲突，默认情况下：

> debug_uart_bus_id = 0
> uart_bus_id = 1

- 若将 uart_bus_id 设为 0，debug_uart_bus_id会自动改用 1

> keypad中，详见 `keypad/comm/uart/uart_cfg_def.h`

---

## 其他

1. keypad：键盘扫描为 **硬编码** ，不可移植
2. keyboard：暂无代码
