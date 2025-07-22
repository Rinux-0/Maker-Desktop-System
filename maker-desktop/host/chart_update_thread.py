from PyQt5.QtCore import QThread, pyqtSignal
from database import user_db

class ChartUpdateThread(QThread):
    data_ready = pyqtSignal(list)  # 信号：拉取到的数据

    def __init__(self, user_id, parent=None):
        super().__init__(parent)
        self.user_id = user_id
        self.limit = 20

    def run(self):
        # 在线程中拉取数据
        health_data = user_db.get_health_data(self.user_id, self.limit)
        self.data_ready.emit(health_data) 