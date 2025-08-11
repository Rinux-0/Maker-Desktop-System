from pydantic import BaseModel
from tortoise.models import Model
from tortoise import fields


class User_DB(Model):
	id: int = fields.IntField(pk=True)                                                # 1
	name: str = fields.CharField(description='姓名', unique=True, max_length=16)      # 15
	age: int = fields.IntField(description='年龄')                                    # 1
	height: int = fields.IntField(description='身高')                                 # 2
	weight: int = fields.IntField(description='体重')                                 # 2
	blood: str = fields.CharField(description='血型', max_length=2)                   # 2
	phone: str = fields.CharField(description='电话', max_length=11)                  # 11
	intro: str = fields.CharField(description='简介', max_length=16)                  # 48


# class User_IO(BaseModel):
# 	name: str
# 	age: int
# 	height: int
# 	weight: int
# 	blood: str
# 	phone: str
# 	intro: str
