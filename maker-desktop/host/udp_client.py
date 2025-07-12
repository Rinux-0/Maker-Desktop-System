import socket
from PyQt5.QtCore import pyqtSignal, QObject, QThread

class UDPClient(QObject):
    data_received = pyqtSignal(str)  # 用于传递接收到的数据
    connection_status = pyqtSignal(str)  # 用于传递连接状态消息

    def __init__(self, host='0.0.0.0', port=8888, target_ip="192.168.225.33", target_port=64482):
        super().__init__()
        self.host = host
        self.port = port
        self.target_ip = target_ip
        self.target_port = target_port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.receive_thread = None  # 初始化 receive_thread

        # 绑定端口
        try:
            self.socket.bind((self.host, self.port))
            self.connection_status.emit(f"🎉 UDP服务已启动 {self.host}:{self.port}")
            print(f"UDP 服务已成功绑定到 {self.host}:{self.port}")
        except OSError as e:
            self.connection_status.emit(f"❌ 端口绑定失败: {str(e)}")
            print(f"端口绑定失败: {str(e)}")
            return

    def start_receiving(self):
        """启动接收线程"""
        if self.receive_thread is None:
            self.receive_thread = ReceiveDataThread(self.socket, self.data_received, self.connection_status)
        try:
            self.receive_thread.start()  # 启动线程
            self.connection_status.emit("🟢 接收线程已启动")
            print("线程启动成功")
        except Exception as e:
            self.connection_status.emit(f"❌ 启动接收线程失败: {str(e)}")
            print(f"启动接收线程失败: {str(e)}")

    def send_data(self, message):
        """发送数据到目标 IP 和端口"""

        try:
            self.socket.sendto(
                message.encode('utf-8'),
                (self.target_ip, self.target_port)
            )
            self.connection_status.emit(f"📤 已发送指令: {message} ➔ {self.target_ip}:{self.target_port}")
            print(f"已发送指令: {message} ➔ {self.target_ip}:{self.target_port}")
        except Exception as e:
            self.connection_status.emit(f"❌ 发送失败: {str(e)}")
            print(f"发送失败: {str(e)}")


class ReceiveDataThread(QThread):
    data_received_signal = pyqtSignal(str)  # 用于传递接收到的数据
    connection_status_signal = pyqtSignal(str)  # 用于传递连接状态消息
    update_db_signal = pyqtSignal(dict)  # 用于传递数据到主线程更新数据库

    def __init__(self, socket, data_received_signal, connection_status_signal):
        super().__init__()
        self.socket = socket
        self.data_received_signal = data_received_signal
        self.connection_status_signal = connection_status_signal

    def run(self):
        """接收数据线程"""
        print("接收线程启动成功...")
        while True:
            try:
                print("等待接收数据...")
                data, addr = self.socket.recvfrom(1024)
                print(f"接收到原始数据: {data}")
                
                # 尝试UTF-8解码
                try:
                    message = data.decode('utf-8').strip()
                    print(f"解码后的消息: {message}")
                except UnicodeDecodeError:
                    message = str(data)
                    print(f"使用字符串表示原始数据: {message}")
                
                # 发出信号传递原始字符串
                self.data_received_signal.emit(f"{addr[0]}:{addr[1]} - {message}")
                
                # 处理数据
                self._process_data(message)

            except Exception as e:
                self.connection_status_signal.emit(f"⚠️ 数据接收错误: {str(e)}")
                print(f"数据接收错误: {str(e)}")
                break

    def _process_data(self, data):
        """处理接收到的数据"""
        try:
            print(f"处理数据: {data}")
            # 初始化所有变量
            parsed_data = {
                'temp': None,
                'heart_rate': None,
                'distance': None
            }

            # 解析数据
            if data.startswith("t+"):  # 温度数据
                parsed_data['temp'] = data[2:]  # 去掉前缀 "t+"
            elif data.startswith("h"):  # 心率数据
                parsed_data['heart_rate'] = data[1:]  # 去掉前缀 "h"
            elif data.startswith("d"):  # 距离数据
                parsed_data['distance'] = data[1:]  # 去掉前缀 "d"
            else:
                return  # 其他格式直接忽略

            # 发出信号传递数据到主线程
            self.update_db_signal.emit(parsed_data)

        except Exception as e:
            self.connection_status_signal.emit(f"⚠️ 数据处理失败: {str(e)}")
            print(f"数据处理失败: {str(e)}")