#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"


//////////////////////////////////////////////////////////////////////////
//历史记录
#define MAX_SCORE_HISTORY			16						//历史个数
//////////////////////////////////////////////////////////////////////////

struct TAndroidUser 
{
	bool blAndroidUser;
	int  PlayCount;
	int  BankCount;
	TAndroidUser()
	{
		memset(this,0,sizeof(*this));
	};
};
struct UserLeft
{
	WORD wChair;
	BOOL blWriteScore;
	LONGLONG WriteScore;

	UserLeft()
	{
		memset(this,0,sizeof(*this));
	}
};
//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//总下注数
protected:
	LONGLONG						m_lAllJettonScore[AREA_ARRY_COUNT];	//全体总注
	
	//个人下注
protected:
	LONGLONG						m_lUserJettonScore[AREA_ARRY_COUNT][GAME_PLAYER];//个人总注

	//控制变量
protected:
	LONGLONG						m_lAreaLimitScore;						//区域限制
	LONGLONG						m_lUserLimitScore;						//区域限制
	LONGLONG						m_lApplyBankerCondition;				//申请条件
	TCHAR							m_szConfigFileName[MAX_PATH];			//配置文件


	//玩家成绩
protected:
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//玩家成绩
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//返回下注
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//玩家税收
	BYTE							m_cbLeftCardCount;						//扑克数目
	bool							m_bContiueCard;							//继续发牌
	BYTE							m_bcFirstPostCard;						//最新發牌點數

	//扑克信息
protected:
	BYTE							m_cbCardCount[4];						//扑克数目
    BYTE							m_cbTableCardArray[4][2];				//桌面扑克
	BYTE							m_cbTableCard[CARD_COUNT];				//桌面扑克
	BYTE							m_cbWinSideControl;					    //控制输赢
	TAndroidUser                    m_AndroidUser[GAME_PLAYER];

	//状态变量
protected:
	DWORD							m_dwJettonTime;							//开始时间

	//庄家信息
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//申请玩家
	WORD							m_wCurrentBanker;						//当前庄家
	WORD							m_wBankerTime;							//做庄次数
	LONGLONG						m_lBankerWinScore;						//累计成绩
	LONGLONG						m_lBankerCurGameScore;					//当前成绩
	bool							m_bEnableSysBanker;						//系统做庄

	//控制变量
protected:
	int								m_nSendCardCount;					    //发送次数
	bool							m_bBankerLeft;							//庄家逃跑

	//记录变量
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
	int								m_nRecordFirst;							//开始记录
	int								m_nRecordLast;							//最后记录
	CFile							m_FileRecord;							//记录结果
	DWORD							m_dwRecordCount;						//记录数目

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	//ITableFrameControl				* m_pITableFrameControl;				//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

protected:
	//服务控制
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;
    

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	
	//控制变量
protected:
	TCHAR							m_szGameRoomName[SERVER_LEN];			//房间名称
	LONGLONG						m_StorageStart;							//房间启动每桌子的库存数值，读取失败按 0 设置
	LONGLONG						m_StorageDeduct;						//每局游戏结束后扣除的库存比例，读取失败按 1.00 设置

	//庄家设置
protected:
	//加庄局数设置：当庄家坐满设定的局数之后(m_lBankerMAX)，
	//所带金币值还超过下面申请庄家列表里面所有玩家金币时，
	//可以再加坐庄m_lBankerAdd局，加庄局数可设置。
	LONGLONG						m_lBankerMAX;							//最大庄家数
	LONGLONG						m_lBankerAdd;							//庄家增加数

	//金币超过m_lBankerScoreMAX之后，就算是下面玩家的金币值大于他的金币值，他也可以再加庄m_lBankerScoreAdd局。
	LONGLONG						m_lBankerScoreMAX;						//庄家钱
	LONGLONG						m_lBankerScoreAdd;						//庄家钱大时,坐庄增加数

	//最大庄家数
	LONGLONG						m_lPlayerBankerMAX;						//玩家最大庄家数

	//换庄
	bool							m_bExchangeBanker;						//交换庄家

	//时间设置
protected:
	BYTE							m_cbFreeTime;							//空闲时间
	BYTE							m_cbBetTime;							//下注时间
	BYTE							m_cbEndTime;							//结束时间

	//机器人控制
	int								m_nMaxChipRobot;						//最大数目 (下注机器人)
	int								m_nChipRobotCount;						//人数统计 (下注机器人)

	//时间设置
protected:
	CMD_C_AdminControl              m_AdminCommand;                         //管理员控制

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { }
	
	//接口查询
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);


	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);

	//信息接口
public:
	
	//游戏状态
	virtual bool  IsUserPlaying(WORD wChairID);


	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//数据事件
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize);

	//动作事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//游戏事件
protected:
	//加注事件
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONG lJettonScore);
	//申请庄家
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//取消申请
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);
	//管理员命令
	bool OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const VOID*pDataBuffer);

	//辅助函数
private:
	//发送扑克
	bool DispatchTableCard();
	//发送庄家
	VOID SendApplyUser( IServerUserItem *pServerUserItem );
	//更换庄家
	bool ChangeBanker(bool bCancelCurrentBanker);
	//轮换判断
	VOID TakeTurns();
	//发送记录
	VOID SendGameRecord(IServerUserItem *pIServerUserItem);
	//发送消息
	VOID SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//发送消息
	VOID SendPlaceJettonFail(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore);

	//下注计算
private:
	//最大下注
	LONGLONG GetUserMaxJetton(WORD wChairID);
	VOID RandList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	bool ChangeMaxUser(bool blMin = false,int iIndex = 1);

	//处理机器人上庄
	VOID MakeAndroidUserBanker();

	//游戏统计
private:
	//计算得分
    LONGLONG CalculateScore(bool bBankerLeft);
	//推断赢家
	VOID DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen);
	//试探性判断
	bool ProbeJudge();

	//操作判断
	bool  IsAllowBankAction(WORD wChairID, IServerUserItem * pIServerUserItem);

};

//////////////////////////////////////////////////////////////////////////

#endif
