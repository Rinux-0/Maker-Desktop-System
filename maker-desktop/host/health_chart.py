import numpy as np
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

class HealthChart(FigureCanvas):
    """
    健康数据图表控件
    用于显示心率、呼吸、体温、距离等健康数据的历史曲线。
    2行2列布局，每张图高度加大。
    横坐标为1,2,3...n，表示最近的第n个数据。
    """
    def __init__(self, parent=None, width=10, height=14, dpi=100):
        import matplotlib.pyplot as plt
        plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
        plt.rcParams['axes.unicode_minus'] = False
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        super().__init__(self.fig)
        self.setParent(parent)
        self.ax1 = self.fig.add_subplot(221)  # 心率
        self.ax2 = self.fig.add_subplot(222)  # 呼吸
        self.ax3 = self.fig.add_subplot(223)  # 体温
        self.ax4 = self.fig.add_subplot(224)  # 距离
        # 适当调大间距，去掉tight_layout
        self.fig.subplots_adjust(hspace=0.5, wspace=0.3, top=0.92, bottom=0.08, left=0.10, right=0.98)
        self.init_chart()

    def init_chart(self):
        self.ax1.clear()
        self.ax1.set_title('心率变化 (BPM)', fontsize=14, pad=16)
        self.ax1.set_ylabel('心率 (BPM)', fontsize=12)
        self.ax1.set_ylim(40, 180)
        self.ax1.grid(True)
        self.ax1.set_xlabel("")
        self.ax2.clear()
        self.ax2.set_title('呼吸变化 (次/分钟)', fontsize=14, pad=16)
        self.ax2.set_ylabel('呼吸 (次/分钟)', fontsize=12)
        self.ax2.set_ylim(0, 35)
        self.ax2.grid(True)
        self.ax2.set_xlabel("")
        self.ax3.clear()
        self.ax3.set_title('体温变化 (°C)', fontsize=14, pad=16)
        self.ax3.set_ylabel('体温 (°C)', fontsize=12)
        self.ax3.set_ylim(20, 40)
        self.ax3.grid(True)
        self.ax3.set_xlabel("")
        self.ax4.clear()
        self.ax4.set_title('屏幕距离变化 (cm)', fontsize=14, pad=16)
        self.ax4.set_ylabel('距离 (cm)', fontsize=12)
        self.ax4.set_ylim(0, 200)
        self.ax4.grid(True)
        self.ax4.set_xlabel("")
        self.draw()

    def update_chart(self, timestamps, heart_rates, breaths, temperatures, distances):
        if not timestamps:
            self.init_chart()
            return
        n = len(timestamps)
        x = list(range(1, n+1))
        heart_rates = list(reversed(heart_rates))
        breaths = list(reversed(breaths))
        temperatures = list(reversed(temperatures))
        distances = list(reversed(distances))
        self.ax1.clear()
        self.ax1.plot(x, heart_rates, 'r-', marker='o')
        self.ax1.set_title('心率变化 (BPM)', fontsize=14, pad=16)
        self.ax1.set_ylabel('心率 (BPM)', fontsize=12)
        self.ax1.set_ylim(40, 180)
        self.ax1.grid(True)
        self.ax1.set_xlabel("")
        self.ax1.set_xticks(x)
        self.ax1.tick_params(axis='x', rotation=0)
        self.ax2.clear()
        self.ax2.plot(x, breaths, 'b-', marker='s')
        self.ax2.set_title('呼吸变化 (次/分钟)', fontsize=14, pad=16)
        self.ax2.set_ylabel('呼吸 (次/分钟)', fontsize=12)
        self.ax2.set_ylim(0, 35)
        self.ax2.grid(True)
        self.ax2.set_xlabel("")
        self.ax2.set_xticks(x)
        self.ax2.tick_params(axis='x', rotation=0)
        self.ax3.clear()
        self.ax3.plot(x, temperatures, 'g-', marker='^')
        self.ax3.set_title('体温变化 (°C)', fontsize=14, pad=16)
        self.ax3.set_ylabel('体温 (°C)', fontsize=12)
        self.ax3.set_ylim(20, 40)
        self.ax3.grid(True)
        self.ax3.set_xlabel("")
        self.ax3.set_xticks(x)
        self.ax3.tick_params(axis='x', rotation=0)
        self.ax4.clear()
        self.ax4.plot(x, distances, 'm-', marker='d')
        self.ax4.set_title('屏幕距离变化 (cm)', fontsize=14, pad=16)
        self.ax4.set_ylabel('距离 (cm)', fontsize=12)
        self.ax4.set_ylim(0, 200)
        self.ax4.grid(True)
        self.ax4.set_xlabel("")
        self.ax4.set_xticks(x)
        self.ax4.tick_params(axis='x', rotation=0)
        self.draw() 