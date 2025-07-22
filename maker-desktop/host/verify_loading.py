#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
验证加载动画功能是否正常工作
"""

import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QPushButton, QLabel
from PyQt5.QtCore import QTimer, QThread, pyqtSignal
from PyQt5.QtGui import QPainter, QColor, QPen

def test_imports():
    """测试所有必要的导入"""
    try:
        from PyQt5.QtCore import QTimer
        print("✅ QTimer 导入成功")
        
        from PyQt5.QtGui import QPainter, QColor, QPen
        print("✅ QPainter, QColor, QPen 导入成功")
        
        from PyQt5.QtWidgets import QWidget
        print("✅ QWidget 导入成功")
        
        return True
    except ImportError as e:
        print(f"❌ 导入失败: {e}")
        return False

class SimpleLoadingSpinner(QWidget):
    """简化的加载动画组件用于测试"""
    def __init__(self, parent=None, size=40):
        super().__init__(parent)
        self.size = size
        self.angle = 0
        self.timer = QTimer()
        self.timer.timeout.connect(self.rotate)
        self.setFixedSize(size, size)
        self.setStyleSheet("background: transparent;")
        
    def start(self):
        """开始动画"""
        self.angle = 0
        self.timer.start(50)
        self.show()
        print("🔄 加载动画已启动")
        
    def stop(self):
        """停止动画"""
        self.timer.stop()
        self.hide()
        print("⏹️ 加载动画已停止")
        
    def rotate(self):
        """旋转动画"""
        self.angle = (self.angle + 30) % 360
        self.update()
        
    def paintEvent(self, event):
        """绘制加载动画"""
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        pen = QPen(QColor("#3b82f6"), 3)
        painter.setPen(pen)
        
        center_x = self.width() // 2
        center_y = self.height() // 2
        radius = min(center_x, center_y) - 5
        
        painter.drawArc(
            center_x - radius, 
            center_y - radius, 
            radius * 2, 
            radius * 2, 
            self.angle * 16, 
            240 * 16
        )

class TestThread(QThread):
    """模拟API调用的测试线程"""
    finished = pyqtSignal()
    
    def run(self):
        """模拟耗时操作"""
        import time
        time.sleep(2)  # 模拟2秒的API调用
        self.finished.emit()

class VerificationWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("加载动画功能验证")
        self.setGeometry(300, 300, 400, 300)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        layout = QVBoxLayout(central_widget)
        
        # 状态标签
        self.status_label = QLabel("点击按钮验证加载动画功能")
        layout.addWidget(self.status_label)
        
        # 加载动画
        self.loading_spinner = SimpleLoadingSpinner(self, size=50)
        self.loading_spinner.hide()
        layout.addWidget(self.loading_spinner)
        
        # 测试按钮
        self.test_button = QPushButton("开始验证")
        self.test_button.clicked.connect(self.start_verification)
        layout.addWidget(self.test_button)
        
        # 测试线程
        self.test_thread = TestThread()
        self.test_thread.finished.connect(self.on_verification_finished)
        
    def start_verification(self):
        """开始验证"""
        self.status_label.setText("正在验证加载动画...")
        self.loading_spinner.start()
        self.test_button.setEnabled(False)
        self.test_thread.start()
        
    def on_verification_finished(self):
        """验证完成"""
        self.status_label.setText("✅ 加载动画功能验证成功！")
        self.loading_spinner.stop()
        self.test_button.setEnabled(True)

def main():
    print("🔍 开始验证加载动画功能...")
    
    # 测试导入
    if not test_imports():
        print("❌ 导入测试失败，请检查依赖")
        return
    
    print("✅ 所有导入测试通过")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = VerificationWindow()
    window.show()
    
    print("🚀 验证窗口已启动，请点击按钮测试加载动画")
    
    sys.exit(app.exec_())

if __name__ == "__main__":
    main() 