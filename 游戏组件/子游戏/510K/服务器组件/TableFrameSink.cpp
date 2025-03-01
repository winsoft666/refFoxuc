#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////
//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//辅助变量
	m_dwPlayTimeCount=0L;

	//请求状态
	ZeroMemory(m_cbResponses,sizeof(m_cbResponses));
	ZeroMemory(m_wRequestTimes,sizeof(m_wRequestTimes));

	//游戏变量
	m_wHeadUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbContinue,sizeof(m_cbContinue));
	ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));
	ZeroMemory(m_510KCardRecord,sizeof(m_510KCardRecord));
	m_bFirstUserLastTurn=false;

	//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//游戏等级
	m_wOrderIndex=0;
	m_cbMainValue=0x02;
	m_cbValueOrder[0]=0x02;
	m_cbValueOrder[1]=0x02;

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//出牌信息
	m_cbTurnCardCount=0;
	m_cbTurnCardType=CT_ERROR;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//组件变量
	m_pITableFrame=NULL;
	//m_pITableFrameControl=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}

//接口查询
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//配置桌子
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//错误判断
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return false;
	}

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	return true;
}

//消费能力
LONGLONG CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//效验状态
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetTableID()==m_pITableFrame->GetTableID()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetTableID()!=m_pITableFrame->GetTableID())) return 0;


	//用户判断
	switch (pIServerUserItem->GetUserStatus())
	{
	case US_SIT:		//坐下状态
	case US_READY:		//准备状态
		{
			return pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore;
		}
	case US_PLAYING:	//游戏状态
	case US_OFFLINE:	//断线状态
		{
			//WORD wMaxTimes=m_pGameCustomRule->wMaxScoreTimes;
			//return pIServerUserItem->GetUserScore()-m_pGameServiceOption->lCellScore*wMaxTimes;
			return pIServerUserItem->GetUserScore()-m_pGameServiceOption->lCellScore;
		}
	case US_LOOKON:		//旁观状态
		{
			return pIServerUserItem->GetUserScore();
		}
	}

	return 0L;
}

//最少积分
LONGLONG CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0L;
}


//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//辅助变量
	m_dwPlayTimeCount=0L;

	//请求状态
	ZeroMemory(m_cbResponses,sizeof(m_cbResponses));
	ZeroMemory(m_wRequestTimes,sizeof(m_wRequestTimes));
	m_bFirstUserLastTurn=false;

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbContinue,sizeof(m_cbContinue));
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));
	ZeroMemory(m_510KCardRecord,sizeof(m_510KCardRecord));

	//得分变量
	m_TurnScore=0;

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));


	//出牌信息
	m_cbTurnCardCount=0;
	m_cbTurnCardType=CT_ERROR;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//游戏时间
	m_dwPlayTimeCount=(DWORD)time(NULL);

	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	do{
		//混乱扑克
		BYTE cbRandCard[FULL_COUNT];
		m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));

		//用户扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_cbHandCardCount[i]=(MAX_COUNT-1);
			CopyMemory(&m_cbHandCardData[i],&cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
		}
	}while(IsAllUserHaveScore(m_cbHandCardData)==false);

	//设置用户

	m_wCurrentUser=rand()%GAME_PLAYER;
	m_wTurnWiner=m_wCurrentUser;

	//设置逻辑
	m_GameLogic.SetMainValue(m_cbMainValue);

	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//构造变量
	GameStart.cbMainValue=m_cbMainValue;
	GameStart.wCurrentUser=m_wCurrentUser;
	CopyMemory(GameStart.cbValueOrder,m_cbValueOrder,sizeof(m_cbValueOrder));
	CopyMemory(GameStart.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//构造扑克
		ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);		
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
	}

	//排列扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			m_TurnScore=0;
			GameEnd.TurnScore=m_TurnScore;

			//胜利信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_cbHandCardCount[i]!=0)
				{
					m_wWinOrder[m_wWinCount++]=i;
				}
			}

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//升级等级
			BYTE cbValueOrder=0;
			if ((m_wWinOrder[0]%2)==(m_wWinOrder[1]%2)) cbValueOrder=3;
			else if ((m_wWinOrder[0]%2)==((m_wWinOrder[3]+1)%2)) cbValueOrder=2;
			else cbValueOrder=1;

			//变量定义
			bool bStatScore=false;
			bool bFinishDraw=false;
			WORD wWinnerUser=m_wWinOrder[0];
			CString szMessage;
			if( (m_wWinOrder[0]==0)&&(m_wWinOrder[1]==2) || (m_wWinOrder[0]==2)&&(m_wWinOrder[1]==0) )
			{
				m_PlayerScore[0]=50;
				m_PlayerScore[2]=50;
				//双扣胜利信息

				szMessage.Format(TEXT("[ %s ]与[ %s ]双扣胜利，游戏结束。\n"),m_pITableFrame->GetTableUserItem(0)->GetNickName(),m_pITableFrame->GetTableUserItem(2)->GetNickName());
				//发送胜利信息
				m_pITableFrame->SendGameMessage(szMessage,SMT_PROMPT);

			}
			else if( (m_wWinOrder[0]==1)&&(m_wWinOrder[1]==3) || (m_wWinOrder[0]==3)&&(m_wWinOrder[1]==1))
			{
				m_PlayerScore[1]=50;
				m_PlayerScore[3]=50;	
				//双扣胜利信息
				szMessage.Format(TEXT("[ %s ]与[ %s ]双扣胜利，游戏结束。\n"),m_pITableFrame->GetTableUserItem(1)->GetNickName(),m_pITableFrame->GetTableUserItem(3)->GetNickName());
				//发送胜利信息
				m_pITableFrame->SendGameMessage(szMessage,SMT_PROMPT);

			}
			else
			{
				int LeaveScore=0;
				LeaveScore=m_GameLogic.GetCardScore(m_cbHandCardData[m_wWinOrder[3]],m_cbHandCardCount[m_wWinOrder[3]]);
				//贡献信息
				szMessage.Format(TEXT("玩家[ %s ]的得分 %I64d 贡献给第一名[ %s ]。手中牌分 %d 贡献给对方。\n"),
					m_pITableFrame->GetTableUserItem(m_wWinOrder[3])->GetNickName(),m_PlayerScore[m_wWinOrder[3]],m_pITableFrame->GetTableUserItem(wWinnerUser)->GetNickName(),LeaveScore);
				//发送贡献信息
				m_pITableFrame->SendGameMessage(szMessage,SMT_PROMPT);

				//下游分数给上游
				m_PlayerScore[wWinnerUser]+=m_PlayerScore[m_wWinOrder[3]];
				m_PlayerScore[m_wWinOrder[3]]=0;

				//对方得到剩余手中牌分
				m_PlayerScore[(m_wWinOrder[3]+1)%GAME_PLAYER]+=LeaveScore;
			}

			//放入结束分数
			CopyMemory(GameEnd.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));

			//过关判断
			//if ( (m_PlayerScore[0]+m_PlayerScore[2])!=0 || (m_PlayerScore[1]+m_PlayerScore[3])!=0 
			//	|| (m_wWinOrder[0]==0)&&(m_wWinOrder[1]==2) || (m_wWinOrder[0]==2)&&(m_wWinOrder[1]==0) 
			//	|| (m_wWinOrder[0]==1)&&(m_wWinOrder[1]==3) || (m_wWinOrder[0]==3)&&(m_wWinOrder[1]==1)
			//	)
			{
				//设置变量
				bStatScore=true;
				bFinishDraw=true;

				//设置变量
				m_wOrderIndex=0;
				m_cbMainValue=0x02;
				m_cbValueOrder[0]=0x02;
				m_cbValueOrder[1]=0x02;
			}

			//取得最低分确定倍数
			LONGLONG minScore;
			IServerUserItem * pIServerUserItem=NULL;			
			pIServerUserItem=m_pITableFrame->GetTableUserItem(0);			
			minScore=pIServerUserItem->GetUserScore();
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem==NULL) continue;
				
				if(pIServerUserItem->GetUserScore()<minScore)
					minScore=pIServerUserItem->GetUserScore();					
			}
			LONGLONG lScoreTimes=1L;
			if(minScore<=9L)				lScoreTimes=1L;
			else if(minScore<39L)		lScoreTimes=2L;
			else if(minScore<79L)		lScoreTimes=4L;
			else if(minScore<159L)		lScoreTimes=8L;
			else if(minScore<319L)		lScoreTimes=16L;
			else if(minScore<639L)		lScoreTimes=32L;
			else if(minScore<1279L)		lScoreTimes=64L;
			else if(minScore<2559L)		lScoreTimes=128L;
			else if(minScore<5119L)		lScoreTimes=256L;
			else if(minScore<10239L)		lScoreTimes=512L;
			else if(minScore<20479L)		lScoreTimes=1024L;
			else if(minScore<40959L)		lScoreTimes=2048L;
			else if(minScore<81919L)		lScoreTimes=4096L;
			else if(minScore<163839L)	lScoreTimes=8192L;
			else if(minScore<327679L)	lScoreTimes=16384L;
			else if(minScore<655359L)	lScoreTimes=32768L;
			else if(minScore<1310719L)	lScoreTimes=65536L;
			else if(minScore<2621439L)	lScoreTimes=131072L;
			else if(minScore<5242879L)	lScoreTimes=262144L;
			else						lScoreTimes=524288L;


			//积分统计
			if (bStatScore==true)
			{
				//变量定义
				LONGLONG lRevenue=0L;
				LONGLONG lUserScore[4];
				ZeroMemory(lUserScore,sizeof(lUserScore));
				LONGLONG lCellScore=m_pGameServiceOption->lCellScore*lScoreTimes;

				//普通情况
				if( (m_PlayerScore[1]+m_PlayerScore[3])>(m_PlayerScore[0]+m_PlayerScore[2]) )
				{
					lUserScore[0]=-1L*lCellScore;
					lUserScore[2]=-1L*lCellScore;
					lUserScore[1]=1L*lCellScore;
					lUserScore[3]=1L*lCellScore;
				}
				else if( (m_PlayerScore[0]+m_PlayerScore[2])>(m_PlayerScore[1]+m_PlayerScore[3]) )
				{
					lUserScore[0]=1L*lCellScore;
					lUserScore[2]=1L*lCellScore;
					lUserScore[1]=-1L*lCellScore;
					lUserScore[3]=-1L*lCellScore;
				}

				//分数相同的情况
				if( (m_PlayerScore[0]+m_PlayerScore[2]) == (m_PlayerScore[1]+m_PlayerScore[3]) )
				{
					lUserScore[m_wWinOrder[0]]=1L*lCellScore;
					lUserScore[(m_wWinOrder[0]+2)%GAME_PLAYER]=1L*lCellScore;
					lUserScore[(m_wWinOrder[0]+1)%GAME_PLAYER]=-1L*lCellScore;
					lUserScore[(m_wWinOrder[0]+3)%GAME_PLAYER]=-1L*lCellScore;
				}

				//特殊情况
				if(m_PlayerScore[m_wWinOrder[0]]<=0)//头家零分
				{
					if(	((m_wWinOrder[0]+2)%GAME_PLAYER)==m_wWinOrder[3]  && 
						(m_PlayerScore[m_wWinOrder[0]]+m_PlayerScore[m_wWinOrder[3]])<=0 
						)//头家的对家第4名且两人总分为0
					{
						lUserScore[m_wWinOrder[0]]=-4L*lCellScore;
						lUserScore[m_wWinOrder[3]]=-4L*lCellScore;
						lUserScore[m_wWinOrder[1]]=4L*lCellScore;
						lUserScore[m_wWinOrder[2]]=4L*lCellScore;
					}
				}
				else if(m_PlayerScore[m_wWinOrder[1]]<=0)//二家零分
				{
					if( ((m_wWinOrder[1]+2)%GAME_PLAYER)==m_wWinOrder[3]  && 
						(m_PlayerScore[m_wWinOrder[1]]+m_PlayerScore[m_wWinOrder[3]])<=0 
						)//头家的对家第4名且两人总分为0
					{
						lUserScore[m_wWinOrder[1]]=-3L*lCellScore;
						lUserScore[m_wWinOrder[3]]=-3L*lCellScore;
						lUserScore[m_wWinOrder[0]]=3L*lCellScore;
						lUserScore[m_wWinOrder[2]]=3L*lCellScore;
					}
				}

				//双扣情况
				if( (m_wWinOrder[0]==0)&&(m_wWinOrder[1]==2) || (m_wWinOrder[0]==2)&&(m_wWinOrder[1]==0) )
				{
					lUserScore[0]=2L*lCellScore;
					lUserScore[2]=2L*lCellScore;
					lUserScore[1]=-2L*lCellScore;
					lUserScore[3]=-2L*lCellScore;
				}
				else if( (m_wWinOrder[0]==1)&&(m_wWinOrder[1]==3) || (m_wWinOrder[0]==3)&&(m_wWinOrder[1]==1) )
				{
					lUserScore[0]=-2L*lCellScore;
					lUserScore[2]=-2L*lCellScore;
					lUserScore[1]=2L*lCellScore;
					lUserScore[3]=2L*lCellScore;	
				}

				CopyMemory(GameEnd.lGameScore,lUserScore,sizeof(lUserScore));

				tagScoreInfo ScoreInfo[GAME_PLAYER];
				ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
				//统计积分
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					//设置积分
					if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0&&lUserScore[i]>0)
					{
						lRevenue=lUserScore[i]*m_pGameServiceOption->wRevenueRatio/1000;
						GameEnd.lGameScore[i]=lUserScore[i]-lRevenue;
					}
					else
					{
						lRevenue=0;
						GameEnd.lGameScore[i]=lUserScore[i];
					}
					
					//写入积分
					BYTE ScoreKind=(GameEnd.lGameScore[i]>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
					ScoreInfo[i].cbType=ScoreKind;
					ScoreInfo[i].lScore = GameEnd.lGameScore[i];
					ScoreInfo[i].lRevenue = lRevenue;
					
				}

				m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

				//清除游戏得分
				ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));
				CopyMemory(GameEnd.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));
			}

			//累计名次
			CopyMemory(GameEnd.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));

			if(bFinishDraw==true)
			{
				//结束信息
				CString szEndMessage;
				szEndMessage.Format(TEXT("本局结束，成绩统计：\n %s ： %I64d 分\n %s ： %I64d 分\n %s ： %I64d 分\n %s ： %I64d 分\n"),
					m_pITableFrame->GetTableUserItem(0)->GetNickName(),GameEnd.lGameScore[0],m_pITableFrame->GetTableUserItem(1)->GetNickName(),GameEnd.lGameScore[1],
					m_pITableFrame->GetTableUserItem(2)->GetNickName(),GameEnd.lGameScore[2],m_pITableFrame->GetTableUserItem(3)->GetNickName(),GameEnd.lGameScore[3]);

				//发送结束信息
				m_pITableFrame->SendGameMessage(szEndMessage,SMT_PROMPT);

			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//设置变量
			m_wHeadUser=m_wWinOrder[0];

			//名次清理
			if (bFinishDraw==true) ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

			//结束游戏
			if (bFinishDraw==true) m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			//设置状态
			if (bFinishDraw==false)
			{
				RepositionSink();
				m_pITableFrame->SetGameStatus(GAME_SCENE_WAIT);
			}

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//游戏等级
			m_wOrderIndex=0;
			m_cbMainValue=0x02;
			m_cbValueOrder[0]=0x02;
			m_cbValueOrder[1]=0x02;

			//名次信息
			ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//游戏等级
			m_wOrderIndex=0;
			m_cbMainValue=0x02;
			m_cbValueOrder[0]=0x02;
			m_cbValueOrder[1]=0x02;

			//名次信息
			ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//玩家得分
			GameEnd.lGameScore[wChairID]=-5*m_pGameServiceOption->lCellScore;

			//发送信息			
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (i==wChairID)
				{
					ScoreInfo[i].lScore = GameEnd.lGameScore[wChairID];
					ScoreInfo[i].cbType = SCORE_TYPE_FLEE;
				}
				else
				{
					ScoreInfo[i].lScore = GameEnd.lGameScore[i];
					ScoreInfo[i].cbType = SCORE_TYPE_DRAW;
				}
			}
			//修改积分
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
			//通知消息
			CString szMessage;
			szMessage.Format(TEXT("由于 [ %s ] 离开游戏，游戏结束"),pIServerUserItem->GetNickName());
			m_pITableFrame->SendGameMessage(szMessage,SMT_PROMPT);
		

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//游戏变量
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusFree.bAutoStatus,m_bAutoPlayer,sizeof(StatusFree.bAutoStatus));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));
			CopyMemory(StatusPlay.bAutoStatus,m_bAutoPlayer,sizeof(StatusPlay.bAutoStatus));

			//等级变量
			StatusPlay.cbMainValue=m_cbMainValue;
			CopyMemory(StatusPlay.cbValueOrder,m_cbValueOrder,sizeof(StatusPlay.cbValueOrder));

			//胜利信息
			StatusPlay.wWinCount=m_wWinCount;
			CopyMemory(StatusPlay.wWinOrder,m_wWinOrder,sizeof(StatusPlay.wWinOrder));

			//出牌信息
			StatusPlay.wTurnWiner=m_wTurnWiner;
			StatusPlay.cbTurnCardType=m_cbTurnCardType;
			StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
			CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
			CopyMemory(StatusPlay.cbMagicCardData,m_cbMagicCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克信息
			WORD wUserIndex=(m_cbHandCardCount[wChiarID]==0)?(wChiarID+2)%GAME_PLAYER:wChiarID;
			CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wUserIndex],sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);

			CopyMemory(StatusPlay.b510KCardRecord,m_510KCardRecord,sizeof(m_510KCardRecord));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GAME_SCENE_WAIT:	//等待状态
		{
			//构造数据
			CMD_S_StatusWait StatusWait;
			ZeroMemory(&StatusWait,sizeof(StatusWait));

			//设置变量
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				m_cbContinue[wChiarID]=FALSE;
			}

			//游戏变量
			StatusWait.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusWait.cbContinue,m_cbContinue,sizeof(m_cbContinue));
			CopyMemory(StatusWait.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));
			CopyMemory(StatusWait.bAutoStatus,m_bAutoPlayer,sizeof(StatusWait.bAutoStatus));

			//等级变量
			StatusWait.cbMainValue=m_cbMainValue;
			CopyMemory(StatusWait.cbValueOrder,m_cbValueOrder,sizeof(StatusWait.cbValueOrder));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusWait,sizeof(StatusWait));
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParam)
{
	return false;
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return true;
}


//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_AUTOMATISM:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Automatism));
			if (wDataSize!=sizeof(CMD_C_Automatism)) return false;

			CMD_C_Automatism * pAutomatism =(CMD_C_Automatism *)pData;

			m_bAutoPlayer[pIServerUserItem->GetChairID()]=pAutomatism->bAutomatism;

			CMD_S_UserAutomatism UserAutomatismData;
			UserAutomatismData.bTrusee=pAutomatism->bAutomatism;
			UserAutomatismData.wChairID=pIServerUserItem->GetChairID();
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_AUTOMATISM,&UserAutomatismData,sizeof(UserAutomatismData));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_AUTOMATISM,&UserAutomatismData,sizeof(UserAutomatismData));

			return true;
		}
	case SUB_C_OUT_CARD:			//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserOutCard(wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:			//用户放弃
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_CONTINUE_GAME:		//继续游戏
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_WAIT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_WAIT) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserContinueGame(pIServerUserItem->GetChairID());
		}
	}


	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置庄家
	if (m_wHeadUser==INVALID_CHAIR) m_wHeadUser=pIServerUserItem->GetChairID();

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//游戏信息
	if (bLookonUser==false)
	{
		//主牌数值
		m_wOrderIndex=0;
		m_cbMainValue=0x02;
		m_cbValueOrder[0]=0x02;
		m_cbValueOrder[1]=0x02;

		//游戏变量
		ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));
	}

	//庄家设置
	if ((bLookonUser==false)&&(m_wHeadUser==pIServerUserItem->GetChairID())) m_wHeadUser=INVALID_CHAIR;

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return false;

	//变幻扑克
	BYTE cbMagicCardData[MAX_COUNT];
	m_GameLogic.MagicCardData(cbCardData,cbCardCount,cbMagicCardData,m_cbTurnCardType);

	//获取类型
	BYTE cbCardType=m_GameLogic.GetCardType(cbMagicCardData,cbCardCount);

	//类型判断
	if (cbCardType==CT_ERROR) 
	{
		ASSERT(FALSE);
		return false;
	}

	//出牌判断
	if (m_cbTurnCardCount!=0)
	{
		//对比扑克
		if (m_GameLogic.CompareCard(m_cbMagicCardData,cbMagicCardData,m_cbTurnCardCount,cbCardCount)==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_cbTurnCardType=cbCardType;
	m_cbTurnCardCount=cbCardCount;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);
	CopyMemory(m_cbMagicCardData,cbMagicCardData,sizeof(BYTE)*cbCardCount);

	//本轮分数
	m_TurnScore+=m_GameLogic.GetCardScore(cbCardData,cbCardCount);

	//胜利判断
	if (m_cbHandCardCount[wChairID]==0) 
	{
		//设置用户
		m_wWinOrder[m_wWinCount++]=wChairID;
		if(m_wWinCount==1)	m_bFirstUserLastTurn=true;

		//取得对家分数
		if(wChairID==m_wWinOrder[0])
		{
			m_PlayerScore[wChairID]+=m_PlayerScore[(wChairID+2)%GAME_PLAYER];
			m_PlayerScore[(wChairID+2)%GAME_PLAYER]=0;
		}

		//用户统计
		if (m_cbHandCardCount[(wChairID+2)%GAME_PLAYER]==0) 
		{
			//最后一手牌的分
			if(m_TurnScore>0) 
			{
				m_PlayerScore[wChairID]+=m_TurnScore;
				m_TurnScore=0;
			}
			m_wCurrentUser=INVALID_CHAIR;
		}
	}

	//切换用户
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//设置用户
		m_wTurnWiner=wChairID;
		m_wCurrentUser=(wChairID+1)%GAME_PLAYER;

		//用户调整
		while (m_cbHandCardCount[m_wCurrentUser]==0) m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	}

	//510K记录
	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(m_GameLogic.GetCardValue(cbCardData[i])==5)
		{
			for(BYTE j=0;j<8;j++)
			{
				if(m_510KCardRecord[0][j]==0)
				{
					m_510KCardRecord[0][j]=cbCardData[i];
					break;
				}
			}
		}
		if(m_GameLogic.GetCardValue(cbCardData[i])==10)
		{
			for(BYTE j=0;j<8;j++)
			{
				if(m_510KCardRecord[1][j]==0)
				{
					m_510KCardRecord[1][j]=cbCardData[i];
					break;
				}
			}
		}
		if(m_GameLogic.GetCardValue(cbCardData[i])==13)
		{
			for(BYTE j=0;j<8;j++)
			{
				if(m_510KCardRecord[2][j]==0)
				{
					m_510KCardRecord[2][j]=cbCardData[i];
					break;
				}
			}
		}
	}

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.TurnScore=m_TurnScore;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.b510KCardRecord,m_510KCardRecord,sizeof(m_510KCardRecord));
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.cbCardData);
	WORD wSendSize=wHeadSize+OutCard.cbCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//搭档信息
	if ((m_cbHandCardCount[wChairID]==0)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//构造变量
		CMD_S_CardInfo CardInfo;
		ZeroMemory(&CardInfo,sizeof(CardInfo));

		//设置扑克
		CardInfo.cbCardCount=m_cbHandCardCount[(wChairID+2)%GAME_PLAYER];
		CopyMemory(CardInfo.cbCardData,m_cbHandCardData[(wChairID+2)%GAME_PLAYER],sizeof(BYTE)*CardInfo.cbCardCount);

		//发送数据
		WORD wHeadSize=sizeof(CardInfo)-sizeof(CardInfo.cbCardData);
		m_pITableFrame->SendTableData(wChairID,SUB_S_CARD_INFO,&CardInfo,wHeadSize+CardInfo.cbCardCount*sizeof(BYTE));
		m_pITableFrame->SendLookonData(wChairID,SUB_S_CARD_INFO,&CardInfo,wHeadSize+CardInfo.cbCardCount*sizeof(BYTE));
	}

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameConclude(wChairID,NULL,GER_NORMAL);

	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbTurnCardCount!=0));
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return false;

	//用户切换
	do
	{
		//设置用户
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;

		//完成判断
		if (m_wCurrentUser==m_wTurnWiner)
		{
			m_PlayerScore[m_wCurrentUser]+=m_TurnScore;
			m_TurnScore=0;

			//过手分判断
			if( m_bFirstUserLastTurn==true && (m_wTurnWiner==(m_wWinOrder[0]+2)%GAME_PLAYER) )
			{
				m_PlayerScore[(m_wTurnWiner+2)%GAME_PLAYER]+=m_PlayerScore[m_wCurrentUser];
				m_PlayerScore[m_wCurrentUser]=0;
				m_bFirstUserLastTurn=false;
			}

			//设置变量
			m_cbTurnCardCount=0;
			m_cbTurnCardType=CT_ERROR;
			m_bFirstUserLastTurn=false;

			//接风处理
			if (m_cbHandCardCount[m_wTurnWiner]==0)
			{
				m_wCurrentUser=(m_wTurnWiner+1)%GAME_PLAYER;
			}
		}

		//继续处理
		if (m_cbHandCardCount[m_wCurrentUser]!=0) break;

	} while(true);

	//构造消息
	CMD_S_PassCard PassCard;
	PassCard.TurnScore=m_TurnScore;
	CopyMemory(PassCard.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

//继续游戏
bool CTableFrameSink::OnUserContinueGame(WORD wChairID)
{
	//效验状态
	ASSERT(m_cbContinue[wChairID]==FALSE);
	if (m_cbContinue[wChairID]==TRUE) return true;

	//设置变量
	m_cbContinue[wChairID]=TRUE;

	//用户统计
	WORD i=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbContinue[i]==FALSE) break;
	}

	//开始判断
	if (i<GAME_PLAYER)
	{
		//变量定义
		CMD_S_ContinueGame ContinueGame;
		ZeroMemory(&ContinueGame,sizeof(ContinueGame));

		//设置变量
		ContinueGame.wChairID=wChairID;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CONTINUE_GAME,&ContinueGame,sizeof(ContinueGame));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CONTINUE_GAME,&ContinueGame,sizeof(ContinueGame));
	}
	else
	{
		//开始游戏
		OnEventGameStart();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//是否都有分
bool CTableFrameSink::IsAllUserHaveScore(BYTE bCardData[GAME_PLAYER][MAX_COUNT])
{
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_GameLogic.GetCardScore(bCardData[i],MAX_COUNT-1)==0)	return false;
	}
	return true;
}

//设置对家
void CTableFrameSink::SearchFriend(BYTE bFirstCard,const BYTE bHandCardData[4][28])
{
	//变量定义
	WORD wFirstCardUser[2];
	ZeroMemory(wFirstCardUser,sizeof(wFirstCardUser));

	//寻找用户
	BYTE bUserCount=0;
	for(WORD i=0;i<GAME_PLAYER;i++) for(BYTE j=0;j<MAX_COUNT-1;j++)
	{
		if(m_cbHandCardData[i][j]==bFirstCard)
		{
			wFirstCardUser[bUserCount]=i;
			bUserCount++;
		}
	}
	ASSERT(bUserCount==2);

	//首出用户
	m_wHeadUser=wFirstCardUser[0];

	//同一个人拿两张牌 不调整
	if(wFirstCardUser[1]==m_wHeadUser) return;

	//检测对家
	WORD bFriendUser=wFirstCardUser[1];						//友方目前位置
	WORD bFriendPosUser=(wFirstCardUser[0]+2)%GAME_PLAYER;	//友方应该在的位置

	//友方未在对门
	if(bFriendPosUser!=bFriendUser)
	{
		WORD OriginStation[2]={bFriendPosUser,bFriendUser};
		WORD TargetStation[2]={bFriendUser,bFriendPosUser};
		//m_pITableFrameControl->SwitchUserChair(OriginStation,TargetStation,2);	

		//牌数据交换
		BYTE tempCard[28];
		ZeroMemory(tempCard,sizeof(tempCard));
		CopyMemory(tempCard,m_cbHandCardData[bFriendPosUser],sizeof(BYTE)*m_cbHandCardCount[bFriendPosUser]);
		CopyMemory(m_cbHandCardData[bFriendPosUser],m_cbHandCardData[bFriendUser],sizeof(BYTE)*m_cbHandCardCount[bFriendPosUser]);
		CopyMemory(m_cbHandCardData[bFriendUser],tempCard,sizeof(BYTE)*m_cbHandCardCount[bFriendUser]);

		//发送信息
		TCHAR szMessage[512]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("翻出【 %s 】, [ %s ] 与 [ %s ] 交换座位。\n"),TranslateCardText(bFirstCard),m_pITableFrame->GetTableUserItem(bFriendPosUser)->GetNickName(),m_pITableFrame->GetTableUserItem(bFriendUser)->GetNickName());
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			IServerUserItem* pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_PROMPT);
		}
	}

	return;
}

//转换文字
CString CTableFrameSink::TranslateCardText(BYTE bCardData)
{
	//变量定义
	TCHAR szColor[16]=TEXT("");
	TCHAR szValue[16]=TEXT("");
	TCHAR szBuffer[32]=TEXT("");

	//大小王过滤
	if(bCardData==0x4F) _sntprintf(szBuffer,CountArray(szBuffer),TEXT("大王"));
	if(bCardData==0x4E) _sntprintf(szBuffer,CountArray(szBuffer),TEXT("小王"));
	if(bCardData==0x4F||bCardData==0x4E) return szBuffer;	

	//取得数据
	BYTE bColor=m_GameLogic.GetCardColor(bCardData);
	BYTE bValue=m_GameLogic.GetCardValue(bCardData);

	//花色信息
	switch(bColor)
	{
	case 0x00: _sntprintf(szColor,CountArray(szColor),TEXT("方块"));break;
	case 0x10: _sntprintf(szColor,CountArray(szColor),TEXT("草花"));break;
	case 0x20: _sntprintf(szColor,CountArray(szColor),TEXT("红桃"));break;
	case 0x30: _sntprintf(szColor,CountArray(szColor),TEXT("黑桃"));break;
	}

	//点数信息
	switch(bValue)
	{
	case 1:  _sntprintf(szValue,CountArray(szValue),TEXT(" A"));break;
	case 2:  _sntprintf(szValue,CountArray(szValue),TEXT(" 2"));break;
	case 3:  _sntprintf(szValue,CountArray(szValue),TEXT(" 3"));break;
	case 4:  _sntprintf(szValue,CountArray(szValue),TEXT(" 4"));break;
	case 5:  _sntprintf(szValue,CountArray(szValue),TEXT(" 5"));break;
	case 6:  _sntprintf(szValue,CountArray(szValue),TEXT(" 6"));break;
	case 7:  _sntprintf(szValue,CountArray(szValue),TEXT(" 7"));break;
	case 8:  _sntprintf(szValue,CountArray(szValue),TEXT(" 8"));break;
	case 9:  _sntprintf(szValue,CountArray(szValue),TEXT(" 9"));break;
	case 10: _sntprintf(szValue,CountArray(szValue),TEXT(" 10"));break;
	case 11: _sntprintf(szValue,CountArray(szValue),TEXT(" J"));break;
	case 12: _sntprintf(szValue,CountArray(szValue),TEXT(" Q"));break;
	case 13: _sntprintf(szValue,CountArray(szValue),TEXT(" K"));break;
	}

	//设置信息
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%s%s"),szColor,szValue);
	return szBuffer;	
}


//////////////////////////////////////////////////////////////////////////////////
