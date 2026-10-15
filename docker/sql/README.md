# WarZ local SQL (Docker)

SQL Server 2022 in Docker; data lives in volume **`warz_mssql_data`**.

API / Master / Supervisor / GameServer ยังรันบน Windows โฮสต์ — ชี้มาที่ `127.0.0.1,1433`.

## ครั้งแรก / รีซีด

```bat
C:\warzlocal\docker\sql\scripts\up.bat
```

หรือ:

```powershell
cd C:\warzlocal\docker\sql
docker compose up -d
bash .\scripts\init-db.sh
```

## ใช้ประจำ

`start_local_stack.bat` จะ `docker compose up -d` ให้อัตโนมัติ

## เชื่อมต่อ

| | |
|---|---|
| Host | `127.0.0.1,1433` |
| User | `sa` |
| Password | ดูใน `.env` (`WarZ_Local_2024!`) |
| DBs | `WarZ`, `BreezeNet` |
| Login เกม | `local@warz.local` / `test123` |

## ข้อมูลถาวร

```bat
docker volume ls | findstr warz_mssql
```

ลบข้อมูลทั้งหมด (ระวัง):

```bat
cd C:\warzlocal\docker\sql
docker compose down
docker volume rm warz_mssql_data
```
