import sys
from datetime import datetime
from threading import Thread
import numpy as np
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PyQt5.QtCore import Qt, pyqtSlot, pyqtSignal, QSize, QPropertyAnimation, QEasingCurve, QThread, QTimer
from chart_update_thread import ChartUpdateThread
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                             QTextEdit, QPushButton, QLineEdit, QLabel, QComboBox, QDialog,
                             QFormLayout, QDialogButtonBox, QSpinBox, QGroupBox, QSizePolicy,
                             QScrollArea, QFrame, QSplitter, QSlider)
from PyQt5.QtGui import QPainter, QColor, QPen
import markdown
from api_client import query_deepseek
from udp_client import UDPClient
from database import user_db
import pygame
import speech_recognition as sr
import requests  # 用于调用文字转语音API
from io import BytesIO
import wave
from vosk import Model, KaldiRecognizer
import pyaudio
import json  # 导入 json 模块
from time import sleep

vosk_model = None
vosk_recognizer = None

# 导入自定义控件
from modern_text_edit import ModernTextEdit
from loading_spinner import LoadingSpinner
from user_dialog import UserDialog
from expandable_chart_panel import ExpandableChartPanel
from voice_recognizer import VoiceRecognitionThread

# ========================= 主窗口 =========================
class MainWindow(QMainWindow):
    system_signal = pyqtSignal(str, bool)  # 系统消息信号
    chat_signal = pyqtSignal(str, bool)    # 聊天消息信号

    heart_rate = 0.0
    breath_rate = 0.0
    temperature = 0.0
    distance = 0.0

    def __init__(self):
        super().__init__()
        self.system_signal.connect(self._append_system_message)
        self.chat_signal.connect(self._append_chat_message)
        self.setWindowTitle('智能医疗监测系统')
        self.setGeometry(200, 200, 1200, 800)
        self.current_user = None
        self.is_recording = False
        self.loading_spinner = None
        # 初始化语音识别线程
        self.voice_thread = VoiceRecognitionThread()
        self.voice_thread.recognition_result.connect(self.on_voice_recognition_result)
        self.voice_thread.recognition_error.connect(self.on_voice_recognition_error)
        self.voice_thread.recognition_finished.connect(self.on_voice_recognition_finished)
        self.voice_thread.recording_status.connect(self.on_recording_status_changed)
        self.setup_ui()
        self.init_udp_client()

    @pyqtSlot(str, bool)
    def _append_system_message(self, content, is_markdown=False):
        timestamp = datetime.now().strftime("%H:%M:%S")
        formatted_content = f"<div style='color: #6b7280;'>{timestamp}  系统</div>{content}"
        self.system_display.append(formatted_content, is_markdown)
        self.system_display.verticalScrollBar().setValue(
            self.system_display.verticalScrollBar().maximum()
        )

    @pyqtSlot(str, bool)
    def _append_chat_message(self, content, is_markdown=False):
        timestamp = datetime.now().strftime("%H:%M:%S")
        formatted_content = f"<div style='color: #6b7280;'>{timestamp} 👤 用户</div>{content}"
        self.chat_display.append(formatted_content, is_markdown)
        self.chat_display.verticalScrollBar().setValue(
            self.chat_display.verticalScrollBar().maximum()
        )

    def setup_ui(self):
        # 主分割布局
        main_splitter = QSplitter(Qt.Horizontal)
        left_widget = QWidget()
        left_layout = QVBoxLayout(left_widget)
        left_layout.setContentsMargins(10, 10, 10, 10)
        right_widget = QWidget()
        right_layout = QVBoxLayout(right_widget)
        right_layout.setContentsMargins(10, 10, 10, 10)

        # === 左侧内容 ===
        # 用户管理区域
        user_group = QGroupBox("用户管理")
        user_group.setMaximumHeight(100)
        user_group.setStyleSheet("""
            QGroupBox { font: bold 14px '微软雅黑'; border: 1px solid #d1d5db; border-radius: 8px; margin-top: 5px; padding-top: 20px; background: #f9fafb; }
            QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }
        """)
        user_layout = QVBoxLayout(user_group)
        user_select_layout = QHBoxLayout()
        self.user_label = QLabel("当前用户: 未选择")
        self.user_combo = QComboBox()
        self.user_combo.setMinimumWidth(200)

        self.user_combo.setStyleSheet("""
            QComboBox { font: 14px '微软雅黑'; padding: 8px; border: 1px solid #d1d5db; border-radius: 4px; }
        """)
        self.user_combo.currentIndexChanged.connect(self.on_user_selected)
        self.btn_manage = QPushButton("管理用户")
        self.btn_manage.setStyleSheet("""
            QPushButton { font: bold 12px; padding: 6px 12px; background: #8b5cf6; color: white; border-radius: 6px; }
            QPushButton:hover { background: #7c3aed; }
        """)
        self.btn_manage.clicked.connect(self.manage_user)
        self.btn_refresh_users = QPushButton("刷新用户")
        self.btn_refresh_users.setStyleSheet("""
            QPushButton { font: bold 12px; padding: 6px 12px; background: #3b82f6; color: white; border-radius: 6px; }
            QPushButton:hover { background: #2563eb; }
        """)
        self.btn_refresh_users.clicked.connect(self.load_users)
        user_select_layout.addWidget(QLabel("选择用户:"))
        user_select_layout.addWidget(self.user_combo)
        user_select_layout.addWidget(self.btn_refresh_users)
        user_select_layout.addWidget(self.btn_manage)
        user_select_layout.addStretch()
        user_layout.addLayout(user_select_layout)
        user_layout.addWidget(self.user_label)
        left_layout.addWidget(user_group)
        # 数据可视化面板
        self.chart_panel = ExpandableChartPanel()
        left_layout.addWidget(self.chart_panel)
        # 系统监控区域
        system_group = QGroupBox("系统监控")
        system_group.setMaximumHeight(250)
        system_group.setStyleSheet("""
            QGroupBox { font: bold 14px '微软雅黑'; border: 1px solid #d1d5db; border-radius: 8px; margin-top: 10px; padding-top: 20px; background: #f9fafb; }
            QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }
        """)
        system_layout = QVBoxLayout(system_group)
        self.system_display = ModernTextEdit()
        self.system_display.setReadOnly(True)
        self.system_display.setStyleSheet("""
            QTextEdit { font: 13px '微软雅黑'; border: 2px solid #d1d5db; border-radius: 12px; padding: 12px; background: #ffffff; color: #2c3e50; }
        """)
        # self.system_display.setMinimumHeight(200)
        system_layout.addWidget(self.system_display)
        btn_layout = QHBoxLayout()
        self.btn_request = QPushButton("请求设备数据")
        self.btn_request.setStyleSheet("""
            QPushButton { font: bold 12px; padding: 8px 16px; background: #10b981; color: white; border-radius: 6px; }
            QPushButton:hover { background: #059669; }
        """)
        self.btn_request.clicked.connect(self.send_request)
        # 清屏按钮
        self.btn_clear_system = QPushButton("清屏")
        self.btn_clear_system.setStyleSheet("""
            QPushButton { font: bold 12px; padding: 8px 16px; background: #ef4444; color: white; border-radius: 6px; }
            QPushButton:hover { background: #dc2626; }
        """)
        self.btn_clear_system.clicked.connect(self.clear_system_display)
        btn_layout.addWidget(self.btn_request)
        btn_layout.addWidget(self.btn_clear_system)
        btn_layout.addStretch()
        system_layout.addLayout(btn_layout)
        left_layout.addWidget(system_group)

        # === 右侧内容 ===
        chat_group = QGroupBox("AI健康助手")
        chat_group.setStyleSheet("""
            QGroupBox { font: bold 14px '微软雅黑'; border: 1px solid #d1d5db; border-radius: 8px; margin-top: 5px; padding-top: 20px; background: #f9fafb; }
            QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }
        """)
        chat_layout = QVBoxLayout(chat_group)
        self.chat_display = ModernTextEdit()
        self.chat_display.setReadOnly(True)
        self.chat_display.setStyleSheet("""
            QTextEdit { font: 14px '微软雅黑'; border: 2px solid #e5e7eb; border-radius: 12px; padding: 15px; background: #f9fafb; color: #1f2937; min-height: 300px; }
        """)
        chat_layout.addWidget(self.chat_display)
        input_layout = QHBoxLayout()
        self.input_field = QLineEdit()
        self.input_field.setPlaceholderText("输入健康问题...")
        self.input_field.setStyleSheet("""
            QLineEdit { font: 14px '微软雅黑'; border: 2px solid #d1d5db; border-radius: 8px; padding: 12px; }
        """)
        self.input_field.returnPressed.connect(self.on_query)
        self.btn_voice = QPushButton("🎤")
        self.btn_voice.setStyleSheet("""
            QPushButton { font: bold 14px '微软雅黑'; padding: 12px 15px; background: #8b5cf6; color: white; border-radius: 8px; }
            QPushButton:hover { background: #7c3aed; }
            QPushButton:checked { background: #dc2626; animation: pulse 1s infinite; }
        """)
        self.btn_voice.setToolTip("点击开始录音，再点击停止录音")
        self.btn_voice.setMaximumWidth(60)
        self.btn_voice.setCheckable(True)
        self.btn_voice.clicked.connect(self.toggle_voice_input)
        self.btn_query = QPushButton("发送")
        self.btn_query.setStyleSheet("""
            QPushButton { font: bold 14px '微软雅黑'; padding: 12px 20px; background: #3b82f6; color: white; border-radius: 8px; }
            QPushButton:hover { background: #2563eb; }
        """)
        self.btn_query.setMaximumWidth(100)
        self.btn_query.clicked.connect(self.on_query)
        self.loading_spinner = LoadingSpinner(self, size=30)
        self.loading_spinner.hide()
        input_layout.addWidget(self.input_field)
        input_layout.addWidget(self.btn_voice)
        input_layout.addWidget(self.loading_spinner)
        input_layout.addWidget(self.btn_query)
        chat_layout.addLayout(input_layout)
        audio_control_layout = QVBoxLayout()
        volume_layout = QHBoxLayout()
        self.volume_label = QLabel("音量:")
        self.volume_slider = QSlider(Qt.Horizontal)
        self.volume_slider.setRange(0, 100)
        self.volume_slider.setValue(50)
        self.volume_slider.setStyleSheet("""
            QSlider { height: 20px; background: #f5f5f5; margin: 10px 0; }
            QSlider::groove:horizontal { height: 5px; background: #d1d5db; border-radius: 2px; }
            QSlider::handle:horizontal { width: 15px; height: 15px; background: #3b82f6; border-radius: 7px; margin: -5px 0; }
        """)
        self.volume_slider.valueChanged.connect(self.on_volume_changed)
        volume_layout.addWidget(self.volume_label)
        volume_layout.addWidget(self.volume_slider)
        audio_control_layout.addLayout(volume_layout)
        chat_layout.addLayout(audio_control_layout)
        tips = QLabel(
            "<div style='color: #6b7280; font-size: 12px; margin-top: 10px;'>"
            "💡 您可以询问：<br>"
            "- 我的心率正常吗？<br>"
            "- 分析我的健康状况<br>"
            "- 如何改善睡眠质量？"
            "</div>"
        )
        tips.setAlignment(Qt.AlignCenter)
        chat_layout.addWidget(tips)
        right_layout.addWidget(chat_group)
        main_splitter.addWidget(left_widget)
        main_splitter.addWidget(right_widget)
        main_splitter.setSizes([800, 200])
        self.setCentralWidget(main_splitter)
        self.load_users()
        pygame.mixer.init()
        self.system_signal.emit("系统已启动，等待设备连接...", False)

    def load_users(self):
        """从数据库加载用户列表"""
        self.user_combo.clear()
        # 添加默认用户选项
        self.user_combo.addItem("默认用户", "default_user")
        users = user_db.get_all_users()
        for user_id, name in users:
            # 跳过default_user的重复添加
            if user_id == "default_user":
                continue
            display_name = f"{name} ({user_id})" if name else f"用户 {user_id}"
            self.user_combo.addItem(display_name, user_id)
        self.system_signal.emit("✅ 用户列表已刷新", False)
    
    def start_chart_update_thread(self):
        user_id = self.current_user if self.current_user else "default_user"
        self.chart_thread = ChartUpdateThread(user_id)
        self.chart_thread.data_ready.connect(self.on_chart_data_ready)
        self.chart_thread.start()

    def on_chart_data_ready(self, health_data):
        if not health_data:
            self.system_signal.emit("⚠️ 没有可用的健康数据", False)
            self.chart_panel.update_chart([], [], [], [], [])
            return
        timestamps = []
        heart_rates = []
        breaths = []
        temperatures = []
        distances = []
        for data in health_data:
            timestamps.append(data[0])
            heart_rates.append(data[1])
            breaths.append(data[2])
            temperatures.append(data[3])
            distances.append(data[4])
        self.chart_panel.update_chart(timestamps, heart_rates, breaths, temperatures, distances)

    def on_user_selected(self, index):
        """用户选择变更"""
        user_id = self.user_combo.itemData(index)
        if user_id:
            self.current_user = user_id
            if user_id == "default_user":
                self.user_label.setText("当前用户: 默认用户")
                self.start_chart_update_thread()
                return
            user_info = user_db.get_user(user_id)
            if user_info:
                name = user_info[4] if user_info[4] else f"用户 {user_id}"
                self.user_label.setText(f"当前用户: {name}")
                user_db.set_current_user(user_id)
                self.system_signal.emit(f"👤 切换用户: {name}", False)
                self.start_chart_update_thread()
            else:
                self.user_label.setText(f"当前用户: {user_id}")

    def manage_user(self):
        """管理当前用户信息"""
        if not self.current_user:
            self.system_signal.emit("⚠️ 请先选择一个用户", False)
            return
            
        dialog = UserDialog(self.current_user, self)
        if dialog.exec_() == QDialog.Accepted:
            user_info = dialog.get_user_info()
            user_db.update_user_info(
                self.current_user,
                name=user_info["name"],
                age=user_info["age"],
                gender=user_info["gender"]
            )
            self.system_signal.emit("✅ 用户信息已更新", False)
            self.load_users()  # 刷新用户列表

    def init_udp_client(self):
        # UDP 客户端初始化
        self.udp_client = UDPClient()
        self.udp_client.data_received.connect(self.handle_udp_data)
        self.udp_client.connection_status.connect(self.handle_udp_status)
        self.udp_client.start_receiving()  # 调用 start_receiving 方法启动接收线程
        
        # 确保 receive_thread 已初始化并连接信号
        if self.udp_client.receive_thread:
            self.udp_client.receive_thread.update_db_signal.connect(self.handle_parsed_data)
    
    @pyqtSlot(str)
    def handle_udp_status(self, msg):
        """处理UDP状态消息"""
        self.system_signal.emit(f"⚙️ {msg}", False)

    @pyqtSlot(str)
    def handle_udp_data(self, data):
        try:
            pass
            # 显示接收到的原始数据
            # self.system_signal.emit(f"📥 接收数据: {data}", False)
            
            # 数据已经在UDP客户端中按照新规范解析过了
            # 这里主要处理显示和日志记录
            # 实际的数据库更新通过update_database方法处理
            
        except Exception as e:
            self.system_signal.emit(f"⚠️ 数据处理错误: {str(e)}", False)

    @pyqtSlot(dict)
    def handle_parsed_data(self, parsed_data):
        """处理UDP客户端解析后的数据"""
        try:
            # 调用update_user_info方法处理解析后的数据
            self.update_user_info(parsed_data)
        except Exception as e:
            self.system_signal.emit(f"⚠️ 解析数据处理错误: {str(e)}", False)

    def send_request(self):
        message = "1"
        self.udp_client.send_data(message)  # 发送纯文本
        self.system_signal.emit("📤 请求设备数据", False)

    def on_query(self):
        """处理查询（使用数据库中的健康数据）"""
        query = self.input_field.text()
        if not query:
            self.system_signal.emit("⚠️ 请输入问题内容", False)
            return
        self.input_field.clear()
        
        # 在聊天区域显示用户问题
        self.chat_signal.emit(
            f"<div style='color: #10b981;'>💬 {query}</div>", 
            True
        )
        
        # 添加当前用户上下文
        user_context = ""
        health_data_context = ""
        
        if self.current_user:
            # 获取用户信息
            user_details = user_db.get_user_details(self.current_user)
            if user_details:
                name, age, gender = user_details
                user_context = (
                    f" [用户: {name or '未知'}, "
                    f"年龄: {age if age else '未知'}, "
                    f"性别: {gender if gender else '未知'}]"
                )
            
            # 获取最近健康数据
            health_data = user_db.get_health_data(self.current_user, 5)  # 获取最近5条记录
            if health_data:
                health_data_context = "\n\n近期健康数据（按时间倒序）：\n"
                for i, data in enumerate(reversed(health_data)):
                    timestamp, heart, breath, temp, distance = data
                    health_data_context += (
                        f"记录 {i+1} ({timestamp}):\n"
                        f"- 心率: {heart} BPM\n"
                        f"- 呼吸: {breath} BPM\n"
                        f"- 体温: {temp}°C\n"
                        f"- 屏幕距离: {distance} cm\n\n"
                    )
        
        # 组合查询内容
        full_query = (
            f"用户问题{user_context}: {query}"
            f"{health_data_context if health_data_context else '（无健康数据记录）'}"
        )
        
        # 显示加载动画
        self.loading_spinner.start()
        self.btn_query.setEnabled(False)  # 禁用发送按钮
        
        # 调用API分析
        Thread(target=self.call_api, args=(full_query,)).start()

    def call_api(self, prompt):
        """调用API并处理响应（同时显示文字和播放语音）"""
        try:
            response = query_deepseek(prompt)
            
            # 显示文字回复
            self.chat_signal.emit(
                f"<div style='color: #8b5cf6;'>🤖 AI回复: {markdown.markdown(response)}</div>。",
                True
            )
            
            # 播放语音回复
            self.play_response(response)
            
        except Exception as e:
            self.system_signal.emit(f"❌ API调用失败: {str(e)}", False)
        finally:
            # 停止加载动画并恢复按钮
            self.loading_spinner.stop()
            self.btn_query.setEnabled(True)

    def parse_data(self, data):
        """解析收到的字符串数据"""
        parsed = {}
        try:
            if isinstance(data, str):
                items = data.split(';')
                for item in items:
                    item = item.strip()
                    if ':' in item:
                        key, value = item.split(':', 1)
                        parsed[key.strip()] = value.strip()
                return parsed
            else:
                return {}
        except Exception as e:
            self.system_signal.emit(f"⚠️ 数据解析失败: {str(e)}", False)
            return {}
    
    def update_database(self, parsed_data):
        """在主线程中更新数据库"""
        try:
            # 调用update_user_info方法处理解析后的数据
            self.update_user_info(parsed_data)
        except Exception as e:
            self.system_signal.emit(f"⚠️ 数据库更新失败: {str(e)}", False)

    def toggle_voice_input(self):
        """切换语音输入状态"""
        if not self.is_recording:
            # 开始录音
            self.start_voice_input()
        else:
            # 停止录音
            self.stop_voice_input()

    def start_voice_input(self):
        """开始语音输入"""
        if self.is_recording:
            return
            
        # 启动语音识别线程（如果未运行）
        if not self.voice_thread.isRunning():
            self.voice_thread.start()
        
        # 开始录音
        self.voice_thread.start_recording()
        self.system_signal.emit("🎤 开始录音，请说话...", False)

    def stop_voice_input(self):
        """停止语音输入"""
        if not self.is_recording:
            return
            
        self.voice_thread.stop_recording()
        # self.system_signal.emit("⏹️ 停止录音，正在识别...", False)

    def on_recording_status_changed(self, is_recording):
        """录音状态改变时的处理"""
        self.is_recording = is_recording
        self.btn_voice.setChecked(is_recording)
        
        if is_recording:
            self.btn_voice.setText("⏹️")
            self.btn_voice.setToolTip("点击停止录音")
        else:
            self.btn_voice.setText("🎤")
            self.btn_voice.setToolTip("点击开始录音")

    def on_voice_recognition_result(self, query):
        """处理语音识别结果"""
        self.input_field.setText(query)  # 将识别结果填入输入框，方便修改
        self.system_signal.emit(f"🎤 识别结果: {query}", False)
        # 不再自动触发查询，让用户可以修改文本后再发送

    def on_voice_recognition_error(self, error_msg):
        """处理语音识别错误"""
        self.system_signal.emit(error_msg, False)

    def on_voice_recognition_finished(self):
        """语音识别完成"""
        # self.system_signal.emit("✅ 语音识别已完成", False)

    def play_response(self, text):
        """将文字转换为语音并播放"""
        try:
            # 调用语音合成API
            url = "http://127.0.0.1:9001"
            data = {
                "text": text,
                "text_language": "zh"
            }
            response = requests.post(url, json=data)
            
            if response.status_code == 200:
                # 返回的是音频流，直接播放或保存
                self.system_signal.emit("🔊 正在播放语音回复...", False)
                self.audio_data = response.content
                pygame.mixer.music.load(BytesIO(self.audio_data))
                
                # 设置音量
                volume = self.volume_slider.value() / 100.0
                pygame.mixer.music.set_volume(volume)
                
                # 获取音频长度
                self.audio_length = pygame.mixer.Sound(BytesIO(self.audio_data)).get_length()
                
                pygame.mixer.music.play()
            else:
                error_info = response.json()
                self.system_signal.emit(f"⚠️ 语音合成失败: {error_info.get('message', '未知错误')}", False)
        except Exception as e:
            self.system_signal.emit(f"⚠️ 文字转语音失败: {str(e)}", False)

    def on_volume_changed(self, value):
        """音量滑块值改变时的处理"""
        volume = value / 100.0
        pygame.mixer.music.set_volume(volume)

    def sector_letter_to_number(self, letter):
        """将扇区字母转换为数字: 'a'=0, 'b'=1, ..., 'p'=15"""
        letter = letter.lower()
        if 'a' <= letter <= 'p':
            return ord(letter) - ord('a')
        return None
    
    def sector_number_to_letter(self, number):
        """将扇区数字转换为字母: 0='a', 1='b', ..., 15='p'"""
        if 0 <= number <= 15:
            return chr(ord('a') + number)   ### !!!
        return None

    def update_user_info(self, data):
        """更新用户信息并检查距离安全（支持新的数据格式）"""
        try:
            # 处理 NFC 扇区数据
            if 'nfc_sector' in data and 'nfc_data' in data:
                # 检查是否为暂时忽略的扇区数据
                if data.get('nfc_ignored', False):
                    self.system_signal.emit(f"📥 NFC扇区数据(暂时忽略): {data['nfc_data']}", False)
                    return  # 不进行进一步处理
                
                # 初始化 NFC 缓冲区（如果尚未存在）
                if not hasattr(self, 'nfc_buffer'):
                    self.nfc_buffer = {}
                
                sector_num = data['nfc_sector']
                nfc_data = data['nfc_data']
                
                # 存储扇区数据
                self.nfc_buffer[sector_num] = nfc_data
                
                # 检查是否收集到完整的 NFC ID（16个扇区，0-15）
                if len(self.nfc_buffer) >= 16:
                    # 组合完整 NFC ID（按扇区号排序）
                    sorted_sectors = sorted(self.nfc_buffer.keys(), key=int)
                    full_nfc = ''.join([self.nfc_buffer[s] for s in sorted_sectors])
                    
                    # 将完整 NFC ID 添加到数据中
                    data['nfc'] = full_nfc
                    
                    # 发出 NFC 收集完成信号
                    self.system_signal.emit(f"🔑 NFC ID 收集完成: {full_nfc}", False)
                else:
                    # 显示当前收集进度
                    sector_letter = self.sector_number_to_letter(int(sector_num))
                    self.system_signal.emit(f"📥 NFC扇区{sector_letter}({sector_num})数据: {nfc_data} [{len(self.nfc_buffer)}/16]", False)
            
            # 处理 NFC 用户ID数据
            elif 'nfc_user_id' in data:
                nfc_user_id = data['nfc_user_id']
                data['nfc'] = nfc_user_id  # 直接使用用户ID作为NFC标识
                self.system_signal.emit(f"🔑 NFC用户ID: {nfc_user_id}", False)
            
            # 处理指纹数据
            elif 'finger_id' in data:
                finger_user_id = data['finger_id']
                data['finger'] = finger_user_id
                self.system_signal.emit(f"👆 指纹用户ID: {finger_user_id}", False)
            
            elif 'finger_score' in data:
                self.system_signal.emit(f"📊 指纹相似分数: {data['finger_score']}", False)
            
            # 处理健康数据 - 数据已经在UDP客户端中解析过了
            # 这里只需要进行数值转换和验证
            
            # 处理心率数据
            elif 'heart' in data:
                try:
                    data['heart'] = float(data['heart'])
                except ValueError:
                    self.system_signal.emit(f"⚠️ 心率数据格式错误: {data['heart']}", False)
                    data.pop('heart', None)
            
            # 处理呼吸数据
            elif 'breath' in data:
                breath_str = str(data['breath']).strip()
                # 只允许数字和小数点，且不能是空
                if breath_str.replace('.', '', 1).isdigit() and breath_str.count('.') <= 1 and breath_str != '':
                    try:
                        data['breath'] = float(breath_str)
                    except ValueError:
                        self.system_signal.emit(f"⚠️ 呼吸数据格式错误: {data['breath']}", False)
                        data.pop('breath', None)
                else:
                    self.system_signal.emit(f"⚠️ 呼吸数据格式错误: {data['breath']}", False)
                    data.pop('breath', None)
            
            # 处理温度数据
            elif 'temp' in data:
                try:
                    data['temp'] = float(data['temp'])
                except ValueError:
                    self.system_signal.emit(f"⚠️ 温度数据格式错误: {data['temp']}", False)
                    data.pop('temp', None)
            
            # 处理距离数据
            elif 'distance' in data:
                try:
                    data['distance'] = float(data['distance'])
                except ValueError:
                    self.system_signal.emit(f"⚠️ 距离数据格式错误: {data['distance']}", False)
                    data.pop('distance', None)
            
            # 确定用户ID (优先使用nfc，其次finger)
            user_id = data.get('nfc') or data.get('finger')
            
            # 检查距离安全
            distance_val = data.get('distance')
            if distance_val:
                try:
                    dist_val = float(distance_val)
                    if dist_val < 30:  # 距离小于30厘米
                        # 如果有用户信息，显示个性化警告
                        if user_id:
                            user_name = ""
                            user_info = user_db.get_user(user_id)
                            if user_info and user_info[4]:
                                user_name = user_info[4] + "，"
                                
                            self.system_signal.emit(
                                f"⚠️ 警告: {user_name}您距离屏幕过近! 请保持至少30厘米距离保护视力",
                                False
                            )
                        else:
                            # 匿名用户通用警告
                            self.system_signal.emit(
                                "⚠️ 警告: 您距离屏幕过近! 请保持至少30厘米距离保护视力",
                                False
                            )
                except ValueError:
                    pass
            
            # 如果有用户ID，更新用户信息
            if user_id:
                # 添加或更新用户到数据库
                user_db.add_or_update_user(
                    user_id, 
                    data.get('nfc'),
                    data.get('finger')
                )
                
                # 如果当前没有用户，设置为当前用户
                if not self.current_user:
                    self.current_user = user_id
                    self.user_label.setText(f"当前用户: {user_id}")
                    self.system_signal.emit(f"👤 自动选择用户: {user_id}", False)
                    
                    # 更新下拉框选择
                    for i in range(self.user_combo.count()):
                        if self.user_combo.itemData(i) == user_id:
                            self.user_combo.setCurrentIndex(i)
                            break
            
            # 保存健康数据（如果有的话）
            if any(key in data for key in ['heart', 'breath', 'temp', 'distance']):
                try:
                    self.heart_rate = float(data.get('heart', self.heart_rate))
                    self.breath_rate = float(data.get('breath', self.breath_rate))
                    self.temperature = float(data.get('temp', self.temperature))
                    self.distance = float(data.get('distance', self.distance))
                    save_user_id = user_id if user_id else (self.current_user if self.current_user else "default_user")
                    user_db.save_health_data(save_user_id, self.heart_rate, self.breath_rate, self.temperature, self.distance)
                    # self.system_signal.emit("✅ 健康数据已保存", False)
                    self.start_chart_update_thread()
                except (ValueError, TypeError) as e:
                    self.system_signal.emit(f"⚠️ 健康数据格式错误: {str(e)}", False)
                    
        except Exception as e:
            self.system_signal.emit(f"⚠️ 用户信息更新失败: {str(e)}", False)

    # def test_nfc_sector_conversion(self):
    #     """测试NFC扇区字母和数字转换"""
    #     print("=== NFC扇区转换测试 ===")
    #     for i in range(16):
    #         letter = self.sector_number_to_letter(i)
    #         number = self.sector_letter_to_number(letter)
    #         print(f"数字 {i} -> 字母 '{letter}' -> 数字 {number}")
        
    #     # 测试一些边界情况
    #     print(f"无效字母 'q' -> 数字 {self.sector_letter_to_number('q')}")
    #     print(f"无效数字 16 -> 字母 {self.sector_number_to_letter(16)}")
    #     print("=== 测试完成 ===")

    def clear_system_display(self):
        """清空系统监控区内容"""
        self.system_display.clear()

    def closeEvent(self, event):
        """窗口关闭事件"""
        # 停止语音识别线程
        if self.voice_thread.isRunning():
            self.voice_thread.stop()
            self.voice_thread.wait(3000)  # 等待最多3秒
        
        # 关闭数据库连接
        user_db.close()
        
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())