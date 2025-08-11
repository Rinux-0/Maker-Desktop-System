from pydantic import BaseModel
from tortoise.models import Model
from tortoise import fields
from typing import Optional


class Breath_DB(Model):
	id: int = fields.IntField(pk=True)  # 2
	breath: int = fields.IntField(description='呼吸')         # 2
	user: int = fields.ForeignKeyField(model_name='backend.User_DB', related_name='breath') # 1


class Heart_DB(Model):
	id: int = fields.IntField(pk=True)  # 2
	heart: int = fields.IntField(description='心率')           # 2
	user: int = fields.ForeignKeyField(model_name='backend.User_DB', related_name='heart') # 1


class Temperature_DB(Model):
	id: int = fields.IntField(pk=True)  # 2
	temperature: float = fields.FloatField(description='体温')  # 4?
	user: int = fields.ForeignKeyField(model_name='backend.User_DB', related_name='temperature')  # 1


class Distance_DB(Model):
	id: int = fields.IntField(pk=True)  # 2
	distance: int = fields.IntField(description='距离')       # 2
	user: int = fields.ForeignKeyField(model_name='backend.User_DB', related_name='distance')  # 1


class Health_IO(BaseModel):
	breath: Optional[int] = None
	heart: Optional[int] = None
	temperature: Optional[float] = None
	distance: Optional[int] = None
	user_id: int
