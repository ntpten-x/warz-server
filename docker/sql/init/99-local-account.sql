USE [WarZ];
GO

DECLARE @md5 varchar(32);
EXEC FN_CreateMD5Password 'test123', @md5 OUTPUT;

IF NOT EXISTS (SELECT 1 FROM Accounts WHERE email = N'local@warz.local')
BEGIN
  SET IDENTITY_INSERT Accounts ON;
  INSERT INTO Accounts (CustomerID, email, MD5Password, dateregistered, ReferralID, AccountStatus, IsDeveloper, lastlogindate, lastloginIP, SteamUserID, BadLoginTime, BadLoginCount)
  VALUES (2000001, N'local@warz.local', @md5, GETDATE(), 0, 100, 1, GETDATE(), N'127.0.0.1', N'0', '2000-01-01', 0);
  SET IDENTITY_INSERT Accounts OFF;
END
ELSE
BEGIN
  UPDATE Accounts SET MD5Password=@md5, AccountStatus=100, IsDeveloper=1, BadLoginCount=0 WHERE email=N'local@warz.local';
END
GO

USE [WarZ];
GO

IF NOT EXISTS (SELECT 1 FROM UsersData WHERE CustomerID=2000001)
BEGIN
  INSERT INTO UsersData (CustomerID, IsDeveloper, AccountType, AccountStatus, GamePoints, GameDollars, dateregistered, lastjoineddate, lastgamedate, ClanID, ClanRank, GameServerId, CharsCreated, TimePlayed, DateActiveUntil, BanCount, PremiumExpireTime, PremiumLastBonus, ResWood, ResStone, ResMetal)
  VALUES (2000001, 1, 0, 100, 500000, 500000, GETDATE(), GETDATE(), GETDATE(), 0, 99, 0, 0, 0, '2030-01-01', 0, '2013-01-01', '1973-01-01', 0, 0, 0);
END
ELSE
BEGIN
  UPDATE UsersData SET AccountStatus=100, IsDeveloper=1, GamePoints=500000, GameDollars=500000, DateActiveUntil='2030-01-01' WHERE CustomerID=2000001;
END
GO

SELECT 'local account ready' AS status;
GO
