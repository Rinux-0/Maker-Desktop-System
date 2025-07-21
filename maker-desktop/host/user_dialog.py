from PyQt5.QtWidgets import QDialog, QFormLayout, QLineEdit, QSpinBox, QComboBox, QDialogButtonBox

class UserDialog(QDialog):
    """
    用户信息编辑对话框
    用于编辑和展示单个用户的姓名、年龄、性别等信息。
    """
    def __init__(self, user_id, user_db, parent=None):
        super().__init__(parent)
        self.setWindowTitle("编辑用户信息")
        self.user_id = user_id
        self.user_db = user_db
        
        # 获取用户信息
        user_info = self.user_db.get_user(user_id)
        
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
        """
        获取用户编辑后的信息
        :return: dict 包含 name, age, gender
        """
        return {
            "name": self.name_edit.text(),
            "age": self.age_edit.value(),
            "gender": self.gender_combo.currentText()
        } 