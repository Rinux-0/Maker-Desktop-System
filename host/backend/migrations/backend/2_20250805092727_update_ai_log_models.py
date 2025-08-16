from tortoise import BaseDBAsyncClient


async def upgrade(db: BaseDBAsyncClient) -> str:
    return """
        CREATE TABLE IF NOT EXISTS "ai_analyse_db" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "user_msg" VARCHAR(1024) NOT NULL /* user message */,
    "ai_msg" VARCHAR(16384) NOT NULL /* AI message */
);
        CREATE TABLE IF NOT EXISTS "ai_chat_db" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "user_msg" VARCHAR(1024) NOT NULL /* user message */,
    "ai_msg" VARCHAR(16384) NOT NULL /* AI message */
);"""


async def downgrade(db: BaseDBAsyncClient) -> str:
    return """
        DROP TABLE IF EXISTS "ai_chat_db";
        DROP TABLE IF EXISTS "ai_analyse_db";"""
