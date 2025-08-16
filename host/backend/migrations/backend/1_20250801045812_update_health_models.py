from tortoise import BaseDBAsyncClient


async def upgrade(db: BaseDBAsyncClient) -> str:
    return """
        CREATE TABLE IF NOT EXISTS "breath_db" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "breath" INT NOT NULL /* 呼吸 */,
    "user_id" INT NOT NULL REFERENCES "user_db" ("id") ON DELETE CASCADE
);
        CREATE TABLE IF NOT EXISTS "distance_db" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "distance" INT NOT NULL /* 距离 */,
    "user_id" INT NOT NULL REFERENCES "user_db" ("id") ON DELETE CASCADE
);
        CREATE TABLE IF NOT EXISTS "heart_db" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "heart" INT NOT NULL /* 心率 */,
    "user_id" INT NOT NULL REFERENCES "user_db" ("id") ON DELETE CASCADE
);
        CREATE TABLE IF NOT EXISTS "temperature_db" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "temperature" REAL NOT NULL /* 体温 */,
    "user_id" INT NOT NULL REFERENCES "user_db" ("id") ON DELETE CASCADE
);
        DROP TABLE IF EXISTS "health_db";"""


async def downgrade(db: BaseDBAsyncClient) -> str:
    return """
        DROP TABLE IF EXISTS "temperature_db";
        DROP TABLE IF EXISTS "heart_db";
        DROP TABLE IF EXISTS "breath_db";
        DROP TABLE IF EXISTS "distance_db";"""
