from PyQt5.QtWidgets import QTextEdit
import markdown

class ModernTextEdit(QTextEdit):
    """
    支持Markdown渲染的文本框
    用于系统消息、AI对话等区域，支持富文本显示。
    """
    def append(self, content, is_markdown=False):
        """
        添加内容到文本框
        :param content: 文本内容
        :param is_markdown: 是否以Markdown渲染
        """
        if is_markdown:
            html_text = markdown.markdown(content)
            super().append(html_text)
        else:
            super().append(content) 