import sys
from datetime import datetime
from threading import Thread
import numpy as np
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PyQt5.QtCore import Qt, pyqtSlot, pyqtSignal, QSize, QPropertyAnimation, QEasingCurve, QTimer
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                             QTextEdit, QPushButton, QLineEdit, QLabel, QComboBox, QDialog,
                             QFormLayout, QDialogButtonBox, QSpinBox, QGroupBox, QSizePolicy,
                             QScrollArea, QFrame, QSplitter, QSlider)
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

vosk_model = None
vosk_recognizer = None

class UserDialog(QDialog):
    """用户信息编辑对话框"""
    def __init__(self, user_id, parent=None):
        super().__init__(parent)
        self.setWindowTitle("编辑用户信息")
        self.user_id = user_id
        
        # 获取用户信息
        user_info = user_db.get_user(user_id)
        
        layout = QFormLayout()
        
        self.name_edit = QLineEdit()
        self.age_edit = QSpinBox()
        self.age_edit.setRange(0, 120)
        self.gender_combo = QComboBox()
        self.gender_combo.addItems(["男", "女", "其他"])
        
        if user_info:
            # 索引: 0:id, 1:user_id, 2:nfc_id, 3:fingerprint_id, 4:name, 5:age, 6:gender
            self.name_edit.setText(user_info[4] if user_info[4] else "")
            self.age_edit.setValue(user_info[5] if user_info[5] else 0)
            if user_info[6]:
                index = self.gender_combo.findText(user_info[6])
                if index >= 0:
                    self.gender_combo.setCurrentIndex(index)
        
        layout.addRow("姓名:", self.name_edit)
        layout.addRow("年龄:", self.age_edit)
        layout.addRow("性别:", self.gender_combo)
        
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        
        layout.addRow(buttons)
        self.setLayout(layout)
    
    def get_user_info(self):
        return {
            "name": self.name_edit.text(),
            "age": self.age_edit.value(),
            "gender": self.gender_combo.currentText()
        }

class HealthChart(FigureCanvas):
    """健康数据图表"""
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        super().__init__(self.fig)
        self.setParent(parent)
        
        # 创建三个子图
        self.ax1 = self.fig.add_subplot(311)
        self.ax2 = self.fig.add_subplot(312)
        self.ax3 = self.fig.add_subplot(313)
        
        self.fig.subplots_adjust(hspace=0.5)
        
        # 初始化图表
        self.init_chart()
    
    def init_chart(self):
        """初始化空图表"""
        self.ax1.clear()
        self.ax1.set_title('心率变化 (BPM)')
        self.ax1.set_ylabel('心率')
        self.ax1.grid(True)
        
        self.ax2.clear()
        self.ax2.set_title('体温变化 (°C)')
        self.ax2.set_ylabel('体温')
        self.ax2.grid(True)
        
        self.ax3.clear()
        self.ax3.set_title('屏幕距离变化 (cm)')
        self.ax3.set_ylabel('距离')
        self.ax3.set_xlabel('时间')
        self.ax3.grid(True)
        
        self.draw()
    
    def update_chart(self, timestamps, heart_rates, temperatures, distances):
        """更新图表数据"""
        if not timestamps:
            self.init_chart()
            return
            
        # 转换时间戳为可读格式
        times = [datetime.strptime(ts, "%Y-%m-%d %H:%M:%S") for ts in timestamps]
        
        self.ax1.clear()
        self.ax1.plot(times, heart_rates, 'r-', marker='o')
        self.ax1.set_title('HRERT(BPM)')
        self.ax1.set_ylabel('RATE')
        self.ax1.grid(True)
        self.ax1.tick_params(axis='x', rotation=15)
        
        self.ax2.clear()
        self.ax2.plot(times, temperatures, 'b-', marker='s')
        self.ax2.set_title('TEMP(°C)')
        self.ax2.set_ylabel('TEMP')
        self.ax2.grid(True)
        self.ax2.tick_params(axis='x', rotation=15)
        
        self.ax3.clear()
        self.ax3.plot(times, distances, 'g-', marker='^')
        self.ax3.set_title('DIST (cm)')
        self.ax3.set_ylabel('DIST')
        self.ax3.set_xlabel('TIME')
        self.ax3.grid(True)
        self.ax3.tick_params(axis='x', rotation=15)
        
        # 添加安全线
        self.ax3.axhline(y=30, color='r', linestyle='--', label='安全距离')
        self.ax3.legend()
        
        # 调整布局
        self.fig.tight_layout()
        self.draw()

class ModernTextEdit(QTextEdit):
    """支持 Markdown 样式的文本框"""
    def append(self, content, is_markdown=False):
        if is_markdown:
            html_text = markdown.markdown(content)
            super().append(html_text)
        else:
            super().append(content)

class ExpandableChartPanel(QGroupBox):
    """可展开的数据可视化面板"""
    def __init__(self, parent=None):
        super().__init__("数据可视化", parent)
        self.setStyleSheet("""
            QGroupBox {
                font: bold 14px '微软雅黑';
                border: 1px solid #d1d5db;
                border-radius: 8px;
                margin-top: 20px;
                padding-top: 20px;
                background: #f9fafb;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
            }
        """)
        
        # 主布局
        self.main_layout = QVBoxLayout(self)
        self.main_layout.setContentsMargins(10, 20, 10, 10)
        
        # 顶部控制区域
        top_layout = QHBoxLayout()
        
        # 展开/折叠按钮
        self.toggle_button = QPushButton("展开图表")
        self.toggle_button.setCheckable(True)
        self.toggle_button.setChecked(False)
        self.toggle_button.setStyleSheet("""
            QPushButton {
                font: bold 12px;
                padding: 6px 12px;
                background: #3b82f6;
                color: white;
                border-radius: 6px;
            }
            QPushButton:hover { background: #2563eb; }
        """)
        self.toggle_button.clicked.connect(self.toggle_chart_area)
        
        # 刷新按钮
        self.refresh_button = QPushButton("刷新数据")
        self.refresh_button.setStyleSheet("""
            QPushButton {
                font: bold 12px;
                padding: 6px 12px;
                background: #10b981;
                color: white;
                border-radius: 6px;
            }
            QPushButton:hover { background: #059669; }
        """)
        
        top_layout.addWidget(self.toggle_button)
        top_layout.addStretch()
        top_layout.addWidget(self.refresh_button)
        
        self.main_layout.addLayout(top_layout)
        
        # 图表区域 - 初始为折叠状态
        self.chart_area = QWidget()
        self.chart_area.setVisible(False)
        chart_layout = QVBoxLayout(self.chart_area)
        chart_layout.setContentsMargins(0, 10, 0, 0)
        
        # 添加图表
        self.chart = HealthChart()
        self.chart.setMinimumHeight(400)
        chart_layout.addWidget(self.chart)
        
        # 添加图表说明
        chart_info = QLabel("图表显示最近20条健康数据记录。红线表示安全距离阈值(30厘米)。")
        chart_info.setStyleSheet("font: 12px; color: #6b7280; margin-top: 10px;")
        chart_info.setAlignment(Qt.AlignCenter)
        chart_layout.addWidget(chart_info)
        
        self.main_layout.addWidget(self.chart_area)
        
        # 设置尺寸策略
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        self.setMinimumHeight(80)
        self.setMaximumHeight(80)
    
    def toggle_chart_area(self):
        """切换图表区域的显示状态"""
        is_visible = not self.chart_area.isVisible()
        self.chart_area.setVisible(is_visible)
        self.toggle_button.setText("折叠图表" if is_visible else "展开图表")
        
        # 调整高度
        if is_visible:
            self.setMaximumHeight(700)
            self.setMinimumHeight(500)
        else:
            self.setMaximumHeight(80)
            self.setMinimumHeight(80)
    
    def update_chart(self, timestamps, heart_rates, temperatures, distances):
        """更新图表数据"""
        self.chart.update_chart(timestamps, heart_rates, temperatures, distances)

class MainWindow(QMainWindow):
    system_signal = pyqtSignal(str, bool)  # 系统消息信号
    chat_signal = pyqtSignal(str, bool)    # 聊天消息信号

    def __init__(self):
        super().__init__()
        self.system_signal.connect(self._append_system_message)
        self.chat_signal.connect(self._append_chat_message)
        self.setWindowTitle('智能医疗监测系统')
        self.setGeometry(200, 200, 1200, 800)
        
        # 初始化当前用户
        self.current_user = None
        
        self.setup_ui()
        self.init_udp_client()
    
    @pyqtSlot(str, bool)
    def _append_system_message(self, content, is_markdown=False):
        """在左侧系统区域显示消息"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        formatted_content = f"<div style='color: #6b7280;'>{timestamp} 📡 系统</div>{content}"
        self.system_display.append(formatted_content, is_markdown)
        # 自动滚动到底部
        self.system_display.verticalScrollBar().setValue(
            self.system_display.verticalScrollBar().maximum()
        )
    
    @pyqtSlot(str, bool)
    def _append_chat_message(self, content, is_markdown=False):
        """在右侧聊天区域显示消息"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        formatted_content = f"<div style='color: #6b7280;'>{timestamp} 👤 用户</div>{content}"
        self.chat_display.append(formatted_content, is_markdown)
        # 自动滚动到底部
        self.chat_display.verticalScrollBar().setValue(
            self.chat_display.verticalScrollBar().maximum()
        )
      
    def setup_ui(self):
        # 创建主分割布局
        main_splitter = QSplitter(Qt.Horizontal)
        
        # 左侧区域（健康监控）
        left_widget = QWidget()
        left_layout = QVBoxLayout(left_widget)
        left_layout.setContentsMargins(10, 10, 10, 10)
        
        # 右侧区域（AI对话）
        right_widget = QWidget()
        right_layout = QVBoxLayout(right_widget)
        right_layout.setContentsMargins(10, 10, 10, 10)
        
        # === 左侧内容 ===
        # 用户管理区域
        user_group = QGroupBox("用户管理")
        user_group.setStyleSheet("""
            QGroupBox {
                font: bold 14px '微软雅黑';
                border: 1px solid #d1d5db;
                border-radius: 8px;
                margin-top: 5px;
                padding-top: 20px;
                background: #f9fafb;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
            }
        """)
        user_layout = QVBoxLayout(user_group)
        
        # 用户选择
        user_select_layout = QHBoxLayout()
        self.user_label = QLabel("当前用户: 未选择")
        self.user_combo = QComboBox()
        self.user_combo.setMinimumWidth(200)
        self.user_combo.setStyleSheet("""
            QComboBox {
                font: 14px '微软雅黑';
                padding: 8px;
                border: 1px solid #d1d5db;
                border-radius: 4px;
            }
        """)
        self.user_combo.currentIndexChanged.connect(self.on_user_selected)
        
        self.btn_manage = QPushButton("管理用户")
        self.btn_manage.setStyleSheet("""
            QPushButton {
                font: bold 12px;
                padding: 6px 12px;
                background: #8b5cf6;
                color: white;
                border-radius: 6px;
            }
            QPushButton:hover { background: #7c3aed; }
        """)
        self.btn_manage.clicked.connect(self.manage_user)
        
        self.btn_refresh_users = QPushButton("刷新用户")
        self.btn_refresh_users.setStyleSheet("""
            QPushButton {
                font: bold 12px;
                padding: 6px 12px;
                background: #3b82f6;
                color: white;
                border-radius: 6px;
            }
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
        system_group.setStyleSheet("""
            QGroupBox {
                font: bold 14px '微软雅黑';
                border: 1px solid #d1d5db;
                border-radius: 8px;
                margin-top: 10px;
                padding-top: 20px;
                background: #f9fafb;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
            }
        """)
        system_layout = QVBoxLayout(system_group)
        
        self.system_display = ModernTextEdit()
        self.system_display.setReadOnly(True)
        self.system_display.setStyleSheet("""
            QTextEdit {
                font: 13px '微软雅黑';
                border: 2px solid #d1d5db;
                border-radius: 12px;
                padding: 12px;
                background: #ffffff;
                color: #2c3e50;
            }
        """)
        self.system_display.setMinimumHeight(200)
        system_layout.addWidget(self.system_display)
        
        # 设备控制按钮
        btn_layout = QHBoxLayout()
        self.btn_request = QPushButton("请求设备数据")
        self.btn_request.setStyleSheet("""
            QPushButton {
                font: bold 12px;
                padding: 8px 16px;
                background: #10b981;
                color: white;
                border-radius: 6px;
            }
            QPushButton:hover { background: #059669; }
        """)
        self.btn_request.clicked.connect(self.send_request)
        
        btn_layout.addWidget(self.btn_request)
        btn_layout.addStretch()
        
        system_layout.addLayout(btn_layout)
        left_layout.addWidget(system_group)
        
        # === 右侧内容 ===
        # AI对话区域
        chat_group = QGroupBox("AI健康助手")
        chat_group.setStyleSheet("""
            QGroupBox {
                font: bold 14px '微软雅黑';
                border: 1px solid #d1d5db;
                border-radius: 8px;
                margin-top: 5px;
                padding-top: 20px;
                background: #f9fafb;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
            }
        """)
        chat_layout = QVBoxLayout(chat_group)
        
        # 对话显示区域
        self.chat_display = ModernTextEdit()
        self.chat_display.setReadOnly(True)
        self.chat_display.setStyleSheet("""
            QTextEdit {
                font: 14px '微软雅黑';
                border: 2px solid #e5e7eb;
                border-radius: 12px;
                padding: 15px;
                background: #f9fafb;
                color: #1f2937;
                min-height: 300px;
            }
        """)
        chat_layout.addWidget(self.chat_display)
        
        # 输入区域
        input_layout = QHBoxLayout()
        
        self.input_field = QLineEdit()
        self.input_field.setPlaceholderText("输入健康问题...")
        self.input_field.setStyleSheet("""
            QLineEdit {
                font: 14px '微软雅黑';
                border: 2px solid #d1d5db;
                border-radius: 8px;
                padding: 12px;
            }
        """)
        self.input_field.returnPressed.connect(self.on_query)
        
        self.btn_voice = QPushButton("🎤")
        self.btn_voice.setStyleSheet("""
            QPushButton {
                font: bold 14px '微软雅黑';
                padding: 12px 15px;
                background: #8b5cf6;
                color: white;
                border-radius: 8px;
            }
            QPushButton:hover { background: #7c3aed; }
        """)
        self.btn_voice.setToolTip("语音提问")
        self.btn_voice.setMaximumWidth(60)
        self.btn_voice.clicked.connect(self.start_voice_input)
        
        self.btn_query = QPushButton("发送")
        self.btn_query.setStyleSheet("""
            QPushButton {
                font: bold 14px '微软雅黑';
                padding: 12px 20px;
                background: #3b82f6;
                color: white;
                border-radius: 8px;
            }
            QPushButton:hover { background: #2563eb; }
        """)
        self.btn_query.setMaximumWidth(100)
        self.btn_query.clicked.connect(self.on_query)
        
        input_layout.addWidget(self.input_field)
        input_layout.addWidget(self.btn_voice)
        input_layout.addWidget(self.btn_query)
        
        chat_layout.addLayout(input_layout)
        
        # 音频控制区域
        audio_control_layout = QVBoxLayout()
        
        # 进度条
        self.progress_slider = QSlider(Qt.Horizontal)
        self.progress_slider.setRange(0, 100)
        self.progress_slider.setValue(0)
        self.progress_slider.setStyleSheet("""
            QSlider {
                height: 20px;
                background: #f5f5f5;
                margin: 10px 0;
            }
            QSlider::groove:horizontal {
                height: 5px;
                background: #d1d5db;
                border-radius: 2px;
            }
            QSlider::handle:horizontal {
                width: 15px;
                height: 15px;
                background: #3b82f6;
                border-radius: 7px;
                margin: -5px 0;
            }
        """)
        self.progress_slider.valueChanged.connect(self.on_progress_changed)
        
        # 音量控制
        volume_layout = QHBoxLayout()
        self.volume_label = QLabel("音量:")
        self.volume_slider = QSlider(Qt.Horizontal)
        self.volume_slider.setRange(0, 100)
        self.volume_slider.setValue(50)
        self.volume_slider.setStyleSheet("""
            QSlider {
                height: 20px;
                background: #f5f5f5;
                margin: 10px 0;
            }
            QSlider::groove:horizontal {
                height: 5px;
                background: #d1d5db;
                border-radius: 2px;
            }
            QSlider::handle:horizontal {
                width: 15px;
                height: 15px;
                background: #3b82f6;
                border-radius: 7px;
                margin: -5px 0;
            }
        """)
        self.volume_slider.valueChanged.connect(self.on_volume_changed)
        
        volume_layout.addWidget(self.volume_label)
        volume_layout.addWidget(self.volume_slider)
        
        # 播放控制按钮
        control_layout = QHBoxLayout()
        self.btn_play_pause = QPushButton("暂停")
        self.btn_play_pause.setStyleSheet("""
            QPushButton {
                font: bold 12px;
                padding: 8px 16px;
                background: #3b82f6;
                color: white;
                border-radius: 6px;
            }
            QPushButton:hover { background: #2563eb; }
        """)
        self.btn_play_pause.clicked.connect(self.toggle_play_pause)
        control_layout.addWidget(self.btn_play_pause)
        
        audio_control_layout.addWidget(self.progress_slider)
        audio_control_layout.addLayout(volume_layout)
        audio_control_layout.addLayout(control_layout)
        
        chat_layout.addLayout(audio_control_layout)
        
        # 添加小贴士
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
        
        # 设置分割比例
        main_splitter.addWidget(left_widget)
        main_splitter.addWidget(right_widget)
        main_splitter.setSizes([800, 400])  # 左侧占2/3，右侧占1/3
        
        # 设置中央部件
        self.setCentralWidget(main_splitter)
        
        # 加载用户列表
        self.load_users()
        
        # 初始化pygame
        pygame.mixer.init()
        
        # 定时器用于更新进度条
        self.timer = QTimer(self)
        self.timer.setInterval(1000)  # 每秒更新一次
        self.timer.timeout.connect(self.update_progress)
        
        # 显示欢迎信息
        self.system_signal.emit("系统已启动，等待设备连接...", False)

    def load_users(self):
        """从数据库加载用户列表"""
        self.user_combo.clear()
        users = user_db.get_all_users()
        
        # 添加默认选项
        self.user_combo.addItem("-- 选择用户 --", None)
        
        for user_id, name in users:
            display_name = f"{name} ({user_id})" if name else f"用户 {user_id}"
            self.user_combo.addItem(display_name, user_id)
        
        self.system_signal.emit("✅ 用户列表已刷新", False)
    
    def on_user_selected(self, index):
        """用户选择变更"""
        user_id = self.user_combo.itemData(index)
        if user_id:
            self.current_user = user_id
            user_info = user_db.get_user(user_id)
            if user_info:
                name = user_info[4] if user_info[4] else f"用户 {user_id}"
                self.user_label.setText(f"当前用户: {name}")
                user_db.set_current_user(user_id)
                self.system_signal.emit(f"👤 切换用户: {name}", False)
                # 自动更新图表
                self.update_charts()
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
        
        # 确保 receive_thread 已初始化
        if self.udp_client.receive_thread:
            self.udp_client.receive_thread.update_db_signal.connect(self.update_database)
    
    @pyqtSlot(str)
    def handle_udp_status(self, msg):
        """处理UDP状态消息"""
        self.system_signal.emit(f"⚙️ {msg}", False)

    @pyqtSlot(str)
    def handle_udp_data(self, data):
        try:
            # 解析接收到的数据
            parsed_data = self.parse_data(data)
            if parsed_data:  # 确保解析成功
                self.system_signal.emit(
                    f"📥 接收数据: {parsed_data}", 
                    False
                )
                # 更新用户信息
                self.update_user_info(parsed_data)
            else:
                self.system_signal.emit(f"⚠️ 数据格式无效: {data}", False)
        except Exception as e:
            self.system_signal.emit(f"⚠️ 数据处理错误: {str(e)}", False)

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
                    timestamp, heart, temp, distance = data
                    health_data_context += (
                        f"记录 {i+1} ({timestamp}):\n"
                        f"- 心率: {heart} BPM\n"
                        f"- 体温: {temp}°C\n"
                        f"- 屏幕距离: {distance} cm\n\n"
                    )
        
        # 组合查询内容
        full_query = (
            f"用户问题{user_context}: {query}"
            f"{health_data_context if health_data_context else '（无健康数据记录）'}"
        )
        
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

    def parse_data(self, data):
        """解析收到的字符串数据"""
        parsed = {}
        try:
            if isinstance(data, str):
                # 新的数据格式: "nfc:0; finger:1; heart:2; temp:3; distance:4"
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
            # 更新数据库
            user_db.save_health_data(
                user_id="default_user",  # 假设有一个默认用户或根据实际情况设置
                heart_rate=float(parsed_data.get('heart_rate', 0)) if parsed_data.get('heart_rate') else None,
                temperature=float(parsed_data.get('temp', 0)) if parsed_data.get('temp') else None,
                distance=float(parsed_data.get('distance', 0)) if parsed_data.get('distance') else None
            )
            self.system_signal.emit("✅ 健康数据已保存", False)

            # 更新图表
            self.update_charts()

        except Exception as e:
            self.system_signal.emit(f"⚠️ 数据库更新失败: {str(e)}", False)

    def update_charts(self):
        """更新图表数据"""
        # 从数据库获取健康数据
        health_data = user_db.get_health_data("default_user", 20)  # 获取最近20条记录
        if not health_data:
            self.system_signal.emit("⚠️ 没有可用的健康数据", False)
            return

        # 准备数据
        timestamps = []
        heart_rates = []
        temperatures = []
        distances = []

        for data in health_data:
            # 数据格式: (timestamp, heart_rate, temperature, distance)
            timestamps.append(data[0])
            heart_rates.append(data[1])
            temperatures.append(data[2])
            distances.append(data[3])

        # 更新图表
        self.chart_panel.update_chart(timestamps, heart_rates, temperatures, distances)
        self.system_signal.emit("✅ 图表数据已更新", False)

    def start_voice_input(self):
        """启动语音输入"""
        self.system_signal.emit("🎤 正在录音...", False)
        
        global vosk_model, vosk_recognizer
        
        # 初始化 Vosk 模型（如果尚未初始化）
        if vosk_model is None:
            try:
                vosk_model = Model("vosk-model-small-cn-0.22")
                vosk_recognizer = KaldiRecognizer(vosk_model, 16000)
            except Exception as e:
                self.system_signal.emit(f"⚠️ 初始化 Vosk 模型失败: {str(e)}", False)
                return
        
        # 使用 pyaudio 进行音频录制
        p = pyaudio.PyAudio()
        stream = p.open(format=pyaudio.paInt16, channels=1, rate=16000, input=True, frames_per_buffer=8192)
        
        try:
            self.system_signal.emit("🎤 正在识别语音...", False)
            while True:
                data = stream.read(4096)
                if vosk_recognizer.AcceptWaveform(data):
                    result = vosk_recognizer.Result()
                    # 解析 JSON 结果
                    result_dict = json.loads(result)
                    if 'text' in result_dict:
                        query = result_dict['text'].strip()
                        if query:
                            self.input_field.setText(query)  # 将识别结果填入输入框
                            self.on_query()  # 触发查询
                            break
        except Exception as e:
            self.system_signal.emit(f"⚠️ 语音识别失败: {str(e)}", False)
        finally:
            stream.stop_stream()
            stream.close()
            p.terminate()
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
                
                # 重置进度条
                self.progress_slider.setValue(0)
                
                # 启动定时器
                self.timer.start()
                
                pygame.mixer.music.play()
            else:
                error_info = response.json()
                self.system_signal.emit(f"⚠️ 语音合成失败: {error_info.get('message', '未知错误')}", False)
        except Exception as e:
            self.system_signal.emit(f"⚠️ 文字转语音失败: {str(e)}", False)

    def update_progress(self):
        """更新进度条"""
        if pygame.mixer.music.get_busy():
            current_pos = pygame.mixer.music.get_pos() / 1000  # 获取当前播放位置（秒）
            if self.audio_length > 0:
                progress = int((current_pos / self.audio_length) * 100)
                self.progress_slider.setValue(progress)
        else:
            self.timer.stop()
            self.progress_slider.setValue(0)

    def on_progress_changed(self, value):
        """进度条值改变时的处理"""
        if pygame.mixer.music.get_busy() and hasattr(self, 'audio_length') and self.audio_length > 0:
            target_pos = (value / 100) * self.audio_length
            pygame.mixer.music.set_pos(target_pos)

    def on_volume_changed(self, value):
        """音量滑块值改变时的处理"""
        volume = value / 100.0
        pygame.mixer.music.set_volume(volume)

    def toggle_play_pause(self):
        """切换播放/暂停状态"""
        if pygame.mixer.music.get_busy():
            pygame.mixer.music.pause()
            self.btn_play_pause.setText("播放")
        else:
            pygame.mixer.music.unpause()
            self.btn_play_pause.setText("暂停")
    def update_user_info(self, data):
        """更新用户信息并检查距离安全（支持分次 NFC 数据）"""
        try:
            # 处理 NFC 分片数据
            if 'nfc_chunk' in data:
                # 初始化 NFC 缓冲区（如果尚未存在）
                if not hasattr(self, 'nfc_buffer'):
                    self.nfc_buffer = []
                
                # 添加分片到缓冲区
                self.nfc_buffer.append(data['nfc_chunk'])
                
                # 检查是否收集到完整的 NFC ID（16个分片）
                if len(self.nfc_buffer) >= 16:
                    # 组合完整 NFC ID
                    full_nfc = ''.join(self.nfc_buffer[:16])
                    # 移除已处理的分片
                    self.nfc_buffer = self.nfc_buffer[16:]
                    
                    # 将完整 NFC ID 添加到数据中
                    data['nfc'] = full_nfc
                    
                    # 发出 NFC 收集完成信号
                    self.system_signal.emit(f"🔑 NFC ID 收集完成: {full_nfc}", False)
            
            # 处理温度数据
            if 'temp' in data:
                try:
                    # 转换温度格式 t+26.3 -> 26.3
                    temp_str = data['temp'].lstrip('t').lstrip('+')
                    data['temp'] = float(temp_str)
                except ValueError:
                    self.system_signal.emit(f"⚠️ 温度数据格式错误: {data['temp']}", False)
                    data.pop('temp', None)  # 移除无效数据
            
            # 处理距离数据
            if 'distance' in data:
                try:
                    # 转换距离格式 d138 -> 138.0
                    dist_str = data['distance'].lstrip('d')
                    data['distance'] = float(dist_str)
                except ValueError:
                    self.system_signal.emit(f"⚠️ 距离数据格式错误: {data['distance']}", False)
                    data.pop('distance', None)  # 移除无效数据
            
            # 确定用户ID (优先使用nfc，其次finger)
            user_id = data.get('nfc') or data.get('finger')
            
            # 即使没有用户ID也检查距离安全
            distance_val = data.get('distance')
            if distance_val:
                try:
                    dist_val = float(distance_val)
                    if dist_val < 30:  # 距离小于30厘米
                        # 如果有用户信息，显示个性化警告
                        if user_id:
                            user_name = ""
                            user_info = user_db.get_user(user_id)
                            if user_info and user_img[4]:
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
            
            # 如果没有用户ID，提前返回
            if not user_id:
                return
                
            # 添加或更新用户到数据库
            user_db.add_or_update_user(
                user_id, 
                data.get('nfc'),
                data.get('finger')
            )
            
            # 保存健康数据
            try:
                heart_rate = float(data.get('heart', 0))
                temperature = float(data.get('temp', 0))
                distance = float(data.get('distance', 0))
                
                user_db.save_health_data(user_id, heart_rate, temperature, distance)
                self.system_signal.emit("✅ 健康数据已保存", False)
            except (ValueError, TypeError) as e:
                self.system_signal.emit(f"⚠️ 健康数据格式错误: {str(e)}", False)
            
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
        except Exception as e:
            self.system_signal.emit(f"⚠️ 用户信息更新失败: {str(e)}", False)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())