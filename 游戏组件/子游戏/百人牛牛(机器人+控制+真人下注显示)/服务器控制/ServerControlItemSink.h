#pragma once
#include "../服务器组件/ServerControl.h"
#include "../服务器组件/GameLogic.h"

#define INDEX_BANKER				1
#define CONTROL_AREA				4
#define MAX_CARD					5
#define MAX_CARDGROUP				5

class CServerControlItemSink : public IServerControl
{
	//玩家控制						
protected:
	BYTE							m_cbExcuteTimes;				//执行次数
	BYTE							m_cbControlStyle;				//控制方式
	bool							m_bWinArea[CONTROL_AREA];		//赢家区域
	BYTE							m_cbWinAreaCount;
	int								m_nCompareCard[MAX_CARDGROUP][MAX_CARDGROUP];	//对比矩阵

	//辅助信息
protected:
	BYTE							m_cbTableCardArray[MAX_CARDGROUP][MAX_CARD];	//牌组信息
	BYTE							m_cbTableCard[CARD_COUNT];		//桌面扑克
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];//全体总注
	CGameLogic						m_GameLogic;					//游戏逻辑

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//设置是否能成功
	bool IsSettingSuccess(BYTE cbControlArea[MAX_INDEX]);

public:
	//服务器控制
	virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

	//需要控制
	virtual bool  NeedControl();

	//满足控制
	virtual bool  MeetControl(tagControlInfo ControlInfo);

	//完成控制
	virtual bool  CompleteControl();

	//返回控制区域
	virtual bool  ReturnControlArea(tagControlInfo& ControlInfo);

	//开始控制
	virtual void  GetSuitResult( BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lAllJettonScore[] );

	//输赢控制
protected:

	//区域牌型
	bool AreaWinCard(BYTE cbStack[]);

	//计算分配
	bool GetSuitCardCombine(BYTE cbStack[]);

	//庄家输赢
	LONGLONG GetBankerWinScore(int nWinMultiple[]);

	//庄家赢牌
	void BankerWinCard(bool bIsWin,BYTE cbStack[]);
};
