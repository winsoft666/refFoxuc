----------------------------------------------------------------------
-- 版权：2013
-- 时间：2013-02-20
-- 用途：测试玩家使用教程权限
----------------------------------------------------------------------
USE QPNativeWebDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].NET_PW_GetJiaoChengQuanXianByID') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].NET_PW_GetJiaoChengQuanXianByID					
GO

----------------------------------------------------------------------
CREATE PROC NET_PW_GetJiaoChengQuanXianByID
	@dwNewsID		INT,				--	新闻标识
	@dwUserID		INT,				--	用户标识
	@strErrorDescribe	NVARCHAR(127) OUTPUT		-- 输出信息		
WITH ENCRYPTION AS

BEGIN
	-- 属性设置
	SET NOCOUNT ON
	DECLARE @DanJian INT
	--DECLARE @JiaoChengQuanXianID1 INT
	SET @DanJian=0
	select @DanJian=jiaoChengRMB from news where newsID=@dwNewsID
	IF (@DanJian=0)--单价为0的为免费学习的
	BEGIN
		SET @strErrorDescribe=N'1'
		RETURN 1
	END
	
	if exists( SELECT NewsID FROM JiaoChengQuanXian where (NewsID=@dwNewsID AND UserID=@dwUserID and isShengHe=1 ))
	BEGIN
		SET @strErrorDescribe=N'1'
		RETURN 2
	END
	
SET @strErrorDescribe=N'0'
return 0  
END

