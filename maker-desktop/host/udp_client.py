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

    def sector_letter_to_number(self, letter):
        """将扇区字母转换为数字: 'a'=0, 'b'=1, ..., 'p'=15"""
        letter = letter.lower()
        if 'a' <= letter <= 'p':
            return ord(letter) - ord('a')
        return None

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
        """处理接收到的数据 - 按照新规范解析
        
        数据格式规范:
        - na{用户ID}: NFC用户ID (1字节)
        - fi{用户ID}: 指纹用户ID (1字节)  
        - nb-np{数据}: NFC扇区数据 (暂时忽略，保留接口)
        - h{心率}: 心率数据
        - b{呼吸}: 呼吸数据
        - t{体温}: 体温数据
        - d{距离}: 距离数据
        - fg{分数}: 指纹相似分数
        """
        try:
            print(f"处理数据: {data}")
            
            # 检查数据是否为空
            if not data or len(data) < 2:
                print(f"数据格式无效: {data}")
                return
            
            # 解析数据 - 按照首字符判断数据类型
            parsed_data = {}
            
            # 获取首字符
            first_char = data[0].lower()
            
            if first_char == 'n':
                # NFC扇区数据: n{字母扇区号}{数据}
                if len(data) >= 3:
                    sector_letter = data[1].lower()
                    nfc_data = data[2:]
                    
                    if sector_letter == 'a':
                        # na{用户ID} - 用户ID数据
                        if len(nfc_data) >= 1:
                            user_id = nfc_data[0]  # 取第1个字节作为用户ID
                            parsed_data['nfc_user_id'] = user_id
                            print(f"NFC用户ID: {user_id}")
                        else:
                            print(f"NFC用户ID数据格式错误: {data}")
                    elif 'b' <= sector_letter <= 'p':
                        # nb-np 扇区数据暂时不处理，但保留接口
                        sector_num = self.sector_letter_to_number(sector_letter)
                        if sector_num is not None:
                            parsed_data['nfc_sector'] = str(sector_num)
                            parsed_data['nfc_data'] = nfc_data
                            parsed_data['nfc_ignored'] = True  # 标记为暂时忽略
                            print(f"NFC扇区数据(暂时忽略): 扇区{sector_num}({sector_letter}), 数据{nfc_data}")
                    else:
                        print(f"NFC扇区字母无效: {sector_letter}")
                else:
                    print(f"NFC数据格式错误: {data}")
                    
            elif first_char == 'f':
                # 指纹相关数据: f{类型}{数据}
                if len(data) >= 3:
                    finger_type = data[1].lower()
                    finger_data = data[2:]
                    
                    if finger_type == 'i':
                        # fi{用户ID} - 用户ID数据
                        if len(finger_data) >= 1:
                            user_id = finger_data[1]  # 取第2个字节作为用户ID
                            parsed_data['finger_id'] = user_id
                            print(f"指纹用户ID: {user_id}")
                        else:
                            print(f"指纹用户ID数据格式错误: {data}")
                    elif finger_type == 'g':
                        # 指纹相似分数
                        parsed_data['finger_score'] = finger_data
                        print(f"指纹相似分数: {finger_data}")
                    else:
                        print(f"未知的指纹数据类型: {finger_type}")
                else:
                    print(f"指纹数据格式错误: {data}")
                    
            elif first_char == 'h':
                # 心率数据: h{心率值}
                heart_rate = data[1:]
                parsed_data['heart'] = heart_rate
                print(f"心率数据: {heart_rate}")
                
            elif first_char == 'b':
                # 呼吸数据: b{呼吸值}
                breath_rate = data[1:]
                parsed_data['breath'] = breath_rate
                print(f"呼吸数据: {breath_rate}")
                
            elif first_char == 't':
                # 体温数据: t{体温值}
                temperature = data[1:]
                parsed_data['temp'] = temperature
                print(f"体温数据: {temperature}")
                
            elif first_char == 'd':
                # 距离数据: d{距离值}
                distance = data[1:]
                parsed_data['distance'] = distance
                print(f"距离数据: {distance}")
                
            else:
                # 未知数据类型，记录日志
                print(f"未知数据类型: {first_char}, 完整数据: {data}")
                # 仍然传递给UI处理，以防是其他格式
                self.data_received_signal.emit(data)
                return
            
            # 如果有解析到数据，传递给主线程处理
            if parsed_data:
                # 发出信号传递解析后的数据到主线程
                self.update_db_signal.emit(parsed_data)
                # 同时传递原始数据给UI显示
                self.data_received_signal.emit(data)
            else:
                # 没有解析到有效数据，直接传递给UI
                self.data_received_signal.emit(data)

        except Exception as e:
            self.connection_status_signal.emit(f"⚠️ 数据处理失败: {str(e)}")
            print(f"数据处理失败: {str(e)}")