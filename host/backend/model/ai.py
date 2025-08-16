from tortoise.models import Model
from tortoise import fields


class Ai_Chat_DB(Model):
	id: int = fields.IntField(pk=True)  # 1
	user_msg: str = fields.CharField(description='user message', max_length=1024)
	ai_msg: int = fields.CharField(description='AI message', max_length=16384)


class Ai_Analyse_DB(Model):
	id: int = fields.IntField(pk=True)  # 1
	user_msg: str = fields.CharField(description='user message', max_length=1024)
	ai_msg: int = fields.CharField(description='AI message', max_length=16384)
