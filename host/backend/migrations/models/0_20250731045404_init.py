from tortoise import BaseDBAsyncClient


async def upgrade(db: BaseDBAsyncClient) -> str:
    return """
        CREATE TABLE IF NOT EXISTS "user_db" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "name" VARCHAR(16) NOT NULL UNIQUE /* 姓名 */,
    "age" INT NOT NULL /* 年龄 */,
    "height" INT NOT NULL /* 身高 */,
    "weight" INT NOT NULL /* 体重 */,
    "blood" VARCHAR(2) NOT NULL /* 血型 */,
    "phone" VARCHAR(11) NOT NULL /* 电话 */,
    "intro" VARCHAR(16) NOT NULL /* 简介 */
);
CREATE TABLE IF NOT EXISTS "aerich" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "version" VARCHAR(255) NOT NULL,
    "app" VARCHAR(100) NOT NULL,
    "content" JSON NOT NULL
);"""


async def downgrade(db: BaseDBAsyncClient) -> str:
    return """
        """
