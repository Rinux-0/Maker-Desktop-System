from PyQt5.QtWidgets import QGroupBox, QVBoxLayout, QHBoxLayout, QPushButton, QSizePolicy, QWidget, QScrollArea
from PyQt5.QtCore import QPropertyAnimation, QEasingCurve
from health_chart import HealthChart

class ExpandableChartPanel(QGroupBox):
    """
    可展开/折叠的健康数据可视化面板，内含可滚动的HealthChart图表。
    """
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
        self.main_layout = QVBoxLayout(self)
        self.main_layout.setContentsMargins(10, 20, 10, 10)
        # 顶部控制区域
        top_layout = QHBoxLayout()
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
        # 清空按钮
        self.clear_button = QPushButton("清空")
        self.clear_button.setStyleSheet("""
            QPushButton {
                font: bold 12px;
                padding: 6px 12px;
                background: #ef4444;
                color: white;
                border-radius: 6px;
            }
            QPushButton:hover { background: #dc2626; }
        """)
        self.clear_button.clicked.connect(self.clear_chart)
        top_layout.addWidget(self.toggle_button)
        top_layout.addStretch()
        top_layout.addWidget(self.clear_button)
        self.main_layout.addLayout(top_layout)
        # 图表区域（可滚动）
        self.chart_area = QWidget()
        self.chart_area.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        self.chart_area.setMaximumHeight(0)
        self.chart_area.setMinimumHeight(0)
        self.chart_layout = QVBoxLayout(self.chart_area)
        self.chart_layout.setContentsMargins(0, 0, 0, 0)
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setVerticalScrollBarPolicy(2)
        self.scroll_area.setHorizontalScrollBarPolicy(1)
        self.chart = HealthChart()
        self.chart.setMinimumHeight(500)
        self.scroll_area.setWidget(self.chart)
        self.chart_layout.addWidget(self.scroll_area)
        self.main_layout.addWidget(self.chart_area)
        self.animation = QPropertyAnimation(self.chart_area, b"maximumHeight")
        self.animation.setDuration(300)
        self.animation.setEasingCurve(QEasingCurve.OutCubic)
    def toggle_chart_area(self):
        if self.toggle_button.isChecked():
            self.toggle_button.setText("收起图表")
            self.animation.setStartValue(self.chart_area.maximumHeight())
            self.animation.setEndValue(400)
            self.animation.start()
        else:
            self.toggle_button.setText("展开图表")
            self.animation.setStartValue(self.chart_area.maximumHeight())
            self.animation.setEndValue(0)
            self.animation.start()
    def update_chart(self, timestamps, heart_rates, breaths, temperatures, distances):
        self.chart.update_chart(timestamps, heart_rates, breaths, temperatures, distances)
    def clear_chart(self):
        """清空图表数据"""
        self.chart.init_chart() 