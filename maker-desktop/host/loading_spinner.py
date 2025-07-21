from PyQt5.QtWidgets import QWidget
from PyQt5.QtCore import QTimer
from PyQt5.QtGui import QPainter, QColor, QPen

class LoadingSpinner(QWidget):
    """
    加载动画控件
    显示一个旋转的圆弧，用于表示加载中状态。
    """
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
        self.timer.start(50)  # 每50ms旋转一次
        self.show()
        
    def stop(self):
        """停止动画"""
        self.timer.stop()
        self.hide()
        
    def rotate(self):
        """旋转动画"""
        self.angle = (self.angle + 30) % 360
        self.update()
        
    def paintEvent(self, event):
        """绘制加载动画"""
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # 设置画笔
        pen = QPen(QColor("#3b82f6"), 3)
        painter.setPen(pen)
        
        # 计算中心点
        center_x = self.width() // 2
        center_y = self.height() // 2
        radius = min(center_x, center_y) - 5
        
        # 绘制旋转的圆弧
        painter.drawArc(
            center_x - radius, 
            center_y - radius, 
            radius * 2, 
            radius * 2, 
            self.angle * 16, 
            240 * 16  # 240度的圆弧
        ) 