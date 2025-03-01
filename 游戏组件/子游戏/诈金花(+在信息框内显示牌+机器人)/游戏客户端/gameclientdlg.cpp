#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME				200									//开始定时器
#define IDI_USER_ADD_SCORE			201									//加注定时器
#define IDI_USER_COMPARE_CARD		202									//选比牌用户定时器
#define IDI_DISABLE					203									//过滤定时器

//时间标识
//#define TIME_START_GAME				99									//开始定时器
//#define TIME_USER_ADD_SCORE			99									//加注定时器
//#define	TIME_USER_COMPARE_CARD		99									//比牌定时器
#define TIME_START_GAME				30									//开始定时器
#define TIME_USER_ADD_SCORE			30									//加注定时器
#define	TIME_USER_COMPARE_CARD		30									//比牌定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
	ON_MESSAGE(IDM_MIN_SCORE,OnMinScore)
	ON_MESSAGE(IDM_MAX_SCORE,OnMaxScore)
	ON_MESSAGE(IDM_CONFIRM,OnConfirmScore)
	ON_MESSAGE(IDM_CANCEL_ADD,OnCancelAdd)
	ON_MESSAGE(IDM_LOOK_CARD,OnLookCard)
	ON_MESSAGE(IDM_COMPARE_CARD,OnCompareCard)
	ON_MESSAGE(IDM_OPEN_CARD,OnOpenCard)
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
	ON_MESSAGE(IDM_FALSH_CARD_FINISH,OnFlashCardFinish)
	ON_MESSAGE(IDM_COMPARE_USER,OnChooseCUser)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//配置变量
	m_dwCardHSpace=DEFAULT_PELS;

	//用户信息
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wWinnerUser=INVALID_CHAIR;

	//加注信息
	m_lMaxScore=0L;
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;
	m_lUserMaxScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//椅子信息
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=INVALID_CHAIR;
		m_bMingZhu[i]=false;
	}

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	//全局对象
	CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
	ASSERT(m_pGlobalUnits!=NULL);

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);
	}

	//SetIcon(hIcon,TRUE);
	//SetIcon(hIcon,FALSE);
	//AddScoreControl(SW_SHOW);

	//测试代码
#ifdef _DEBUG
	//TCHAR szBuffer[32];
	//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%s：%+I64d"),TEXT("玩家名字"),-5);
	//InsertGeneralString(szBuffer,RGB(0,0,255),true);
	//m_GameClientView.m_btStart.ShowWindow(5);
	//m_GameClientView.m_ScoreView.ResetScore();
	//LONGLONG lGameScore = 10;
	//LONGLONG lGameTax = 10;
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{	
	//	BYTE bTemp[3]={1,2,3};
	//	m_GameClientView.m_CardControl[i].SetCardData(bTemp,3);
	//	m_GameClientView.m_CardControl[i].SetDisplayHead(true);
	//	//设置信息
	//	m_GameClientView.m_ScoreView.SetGameTax(lGameTax,i);
	//	if (m_szAccounts[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),lGameScore);
	//	else m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],lGameScore);
	//}

	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMinScore.ShowWindow(5);
	//BYTE bTemp[3]={1,2,3};
	//m_GameClientView.m_CardControl[2].SetCardData(bTemp,3);
	//m_GameClientView.m_CardControl[2].SetPositively(true);
	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 A - K

	//BYTE bFirstTemp[3]={0x32,0x11,0x23};
	//BYTE bNextTemp[3]={0x2b,0x22,0x26};
	//if(m_GameLogic.CompareCard(bFirstTemp,bNextTemp,3))
	//	MessageBox("FirstTemp Win!");
	//else MessageBox("NextTemp Win!");
	/*m_GameClientView.m_ScoreView.SetGameScore(2,"abc",1122);
	m_GameClientView.m_ScoreView.SetGameScore(1,"abc",1221);
	m_GameClientView.m_ScoreView.SetGameScore(0,"ab122221c",122222221);
	m_GameClientView.m_ScoreView.SetGameScore(3,"67",11);
	for (WORD i=0;i<GAME_PLAYER;i++)m_GameClientView.m_ScoreView.SetGameTax(12222223,i);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	m_GameClientView.m_btConfirm.ShowWindow(SW_SHOW);
	m_GameClientView.m_btMaxScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btMinScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
	*/
	//BYTE bFirstTemp[3]={0x31,0x23,0x03};

	//for(int i=0;i<GAME_PLAYER;i++)
	//{
	//	//m_GameClientView.SetUserTableScore(2,50,999990);
	//	//m_GameClientView.m_CardControl[i].SetDisplayHead(true);
	//	m_GameClientView.m_CardControl[i].SetCardData(bFirstTemp,3);
	//}
	//m_GameClientView.m_CardControl[2].SetCardColor(2);

	//m_GameClientView.m_btConfirm.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btCancel.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMinScore.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMaxScore.ShowWindow(SW_SHOW);
	//m_GameClientView.m_GoldControl.SetGoldCount(10000L);
	//m_GameClientView.m_GoldControl.SetGoldCount(10000);
	//m_GameClientView.m_GoldControl.SetGold(44000L);
	//m_GameClientView.m_GoldControl.SetGoldCount(10);
	//m_GameClientView.m_GoldControl.ShowWindow(SW_SHOW);
	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	//m_GameClientView.SetScoreInfo(100,100);

	//m_GameClientView.m_btMinScore.ShowWindow(5);
#endif

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//用户信息
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wWinnerUser=INVALID_CHAIR;

	//加注信息
	m_lMaxScore=0L;
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;
	m_lUserMaxScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//椅子信息
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=INVALID_CHAIR;
		m_bMingZhu[i]=false;
	}

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=m_pGlobalUnits->m_bAllowSound;
	GameOption.m_bAllowLookon=m_pGlobalUnits->m_bAllowLookon;

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置扑克
		m_dwCardHSpace=GameOption.m_dwCardHSpace;
		for (int i=0;i<GAME_PLAYER;i++) m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);

		//设置配置
		m_pGlobalUnits->m_bAllowSound=GameOption.m_bEnableSound;
		m_pGlobalUnits->m_bAllowLookon=GameOption.m_bAllowLookon;

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	}

	return;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}
//时间消息 
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			//中止判断
			if (nElapse==0)
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return false;
			}

			//警告通知
			if ((nElapse<=5)&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_USER_ADD_SCORE:	//加注定时器
		{
			WORD wMeChairID=GetMeChairID();

			//中止判断
			if (nElapse==0)
			{
				//最少下注
				if ((IsLookonMode()==false)&&(wMeChairID==m_wCurrentUser))
				{
					//自动放弃
					OnGiveUp(0,0);		
				}
				return false;
			}

			//警告通知
			if ((nElapse<=5)&&(wMeChairID==m_wCurrentUser)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
				return true;
			}

			return true;
		}
	case IDI_DISABLE:			//过滤定时器
		{
			if (nElapse==0)
			{
				//删除定时器
				KillGameClock(IDI_DISABLE);
			}
			return true;
		}
	case IDI_USER_COMPARE_CARD:	//选择定时器
		{
			//中止判断
			if (nElapse==0)
			{
				//删除定时器
				KillGameClock(IDI_USER_COMPARE_CARD);
				WORD wMeChairID=GetMeChairID();

				//清理界面
				m_GameClientView.SetCompareCard(false,NULL);

				//构造变量
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard,sizeof(CompareCard));

				//查找上家
				for (LONGLONG i=wMeChairID-1;;i--)
				{
					if(i==-1)i=GAME_PLAYER-1;
					if(m_cbPlayStatus[i]==TRUE)
					{
						CompareCard.wCompareUser=(WORD)i;
						break;
					}
				}

				//发送消息
				SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

				return false;
			}

			//警告通知
			if ((nElapse<=5)&&(GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
				return true;
			}

			return true;
		}
	}

	return false;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//控件控制
	////if(bLookonUser)
	//{
	//	if(IsAllowLookon() 
	//		&& m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].GetLookCard())
	//		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	//	else m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(false);
	//}
	return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			//消息处理
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:		//用户下注
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_LOOK_CARD:		//看牌消息
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			return OnSubLookCard(pData,wDataSize);
		}
	case SUB_S_COMPARE_CARD:	//比牌消息
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			return OnSubCompareCard(pData,wDataSize);
		}
	case SUB_S_OPEN_CARD:		//开牌消息
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			return OnSubOpenCard(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:			//用户放弃
		{
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_PLAYER_EXIT:		//用户强退
		{
			m_GameClientView.FinishDispatchCard();
			//消息处理
			return OnSubPlayerExit(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_WAIT_COMPARE:	//等待比牌
		{
			//消息处理
			if (wDataSize!=sizeof(CMD_S_WaitCompare)) return false;
			CMD_S_WaitCompare * pWaitCompare=(CMD_S_WaitCompare *)pData;	
			ASSERT(pWaitCompare->wCompareUser==m_wCurrentUser);

			WORD wMeChairID=GetMeChairID();

			if(wMeChairID!=m_wCurrentUser)
			{
				//比牌背景
				m_GameClientView.m_CardControl[m_wViewChairID[m_wCurrentUser]].SetCompareBack(true);

				SetGameClock(m_wCurrentUser,IDI_DISABLE,TIME_USER_COMPARE_CARD);
			}

			//提示标志
			if(wMeChairID!=m_wCurrentUser || IsLookonMode()) m_GameClientView.SetWaitUserChoice(TRUE);

			return true;
		}
	case SUB_S_SEND_CARD:		//用户扑克
		{
			return true;
		}
	}

	return false;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//设置控件
			if (IsLookonMode()==false&& GetMeUserItem()->GetUserStatus()!=US_READY)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}

			return true;
		}
	case GAME_STATUS_PLAY:	//游戏状态
		{
			for (WORD i=0;i<GAME_PLAYER;i++)m_wViewChairID[i]=SwitchViewChairID(i);

			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;
			WORD wMeChairID=GetMeChairID();

			//加注信息
			m_lMaxCellScore=pStatusPlay->lMaxCellScore;
			m_lCellScore=pStatusPlay->lCellScore;
			m_lCurrentTimes=pStatusPlay->lCurrentTimes;
			m_lUserMaxScore=pStatusPlay->lUserMaxScore;
			m_GameClientView.m_GoldControl.SetGoldCount(m_lCellScore);

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			CopyMemory(m_bMingZhu,pStatusPlay->bMingZhu,sizeof(pStatusPlay->bMingZhu));
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(pStatusPlay->lTableScore));
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(pStatusPlay->cbPlayStatus));

			//设置扑克
			CopyMemory(&m_cbHandCardData[wMeChairID],&pStatusPlay->cbHandCardData,MAX_COUNT);

			//设置变量
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=m_cbPlayStatus[i];
				m_GameClientView.m_CardControl[i].SetDisplayHead(false);
				if(m_lTableScore[i]>0L)m_GameClientView.SetUserTableScore(m_wViewChairID[i],m_lTableScore[i],m_lTableScore[i]);

				//获取用户
				IClientUserItem * pClientUserItem=GetTableUserItem(i);
				if (pClientUserItem==NULL) continue;

				//效验状态
				//if(pUserData->cbUserStatus==GAME_STATUS_PLAY) ASSERT(m_cbPlayStatus[i]==TRUE);
				//不能效验,特殊:在结束时,赢家断线重新接上,状态SIT

				//用户名字
				lstrcpyn(m_szAccounts[i],pClientUserItem->GetNickName(),CountArray(m_szAccounts[i]));
			}

			//停止筹码动画
			m_GameClientView.StopMoveJettons();

			//庄家标志
			WORD wID=m_wViewChairID[m_wBankerUser];
			m_GameClientView.SetBankerUser(wID);

			//左上信息
			m_GameClientView.SetScoreInfo(m_lMaxCellScore,m_lCellScore);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//设置位置
				WORD wViewChairID=m_wViewChairID[i];

				//设置扑克
				if (m_cbPlayStatus[i]==TRUE) 
				{
					m_GameClientView.m_CardControl[wViewChairID].SetCardData(m_cbHandCardData[i],MAX_COUNT);
					if((i!=wMeChairID ||IsLookonMode()) && m_bMingZhu[i])
					{
						m_GameClientView.m_CardControl[wViewChairID].SetLookCard(true);
					}
				}
			}
			if(m_bMingZhu[wMeChairID] && (!IsLookonMode() || IsAllowLookon()))
			{
				m_GameClientView.m_CardControl[m_wViewChairID[wMeChairID]].SetDisplayHead(true);
			}

			//控件处理
			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

			//判断控件
			if(m_wCurrentUser<GAME_PLAYER)
			{
				if(!(pStatusPlay->bCompareState))
				{
					//控件信息
					if(!IsLookonMode() && wMeChairID==m_wCurrentUser)
					{
						UpdataControl();
					}

					//设置时间
					SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
				}
				else
				{	
					if(!IsLookonMode() && wMeChairID==m_wCurrentUser)
					{
						//选择玩家比牌
						BOOL bCompareUser[GAME_PLAYER];
						for(int i=0;i<GAME_PLAYER;i++)
						{
							if(m_cbPlayStatus[i]==TRUE && wMeChairID!=i)
							{
								bCompareUser[m_wViewChairID[i]]=TRUE;
							}
							else bCompareUser[m_wViewChairID[i]]=FALSE;
						}
						//设置箭头
						m_GameClientView.SetCompareCard(true,bCompareUser);

						//等待比牌
						SendSocketData(SUB_C_WAIT_COMPARE,NULL,0);

						//设置时间
						SetGameClock(wMeChairID,IDI_USER_COMPARE_CARD,TIME_USER_COMPARE_CARD);

						//提示标志
						m_GameClientView.SetWaitUserChoice(FALSE);
					}
					else 
					{
						//比牌背景
						m_GameClientView.m_CardControl[m_wViewChairID[m_wCurrentUser]].SetCompareBack(true);

						SetGameClock(m_wCurrentUser,IDI_DISABLE,TIME_USER_COMPARE_CARD);

						//提示标志
						m_GameClientView.SetWaitUserChoice(TRUE);
					}
				}
			}
			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//旁观界面
	if(IsLookonMode())OnStart(0,0);

	//数据信息
	m_lCellScore=pGameStart->lCellScore;
	m_lMaxCellScore=pGameStart->lMaxScore;
	m_lCurrentTimes=pGameStart->lCurrentTimes;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_wBankerUser=pGameStart->wBankerUser;
	m_lUserMaxScore=pGameStart->lUserMaxScore;
	m_GameClientView.m_GoldControl.SetGoldCount(m_lCellScore);

	//设置变量
	LONGLONG lTableScore=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=SwitchViewChairID(i);
		m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=FALSE;

		//获取用户
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		if (pClientUserItem==NULL) continue;

		//游戏信息		
		m_cbPlayStatus[i]=TRUE;
		m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=TRUE;
		lTableScore+=m_lCellScore;
		m_lTableScore[i]=m_lCellScore;
		m_GameClientView.SetUserTableScore(m_wViewChairID[i],m_lTableScore[i],m_lTableScore[i]);

		//用户信息
		lstrcpyn(m_szAccounts[i],pClientUserItem->GetNickName(),CountArray(m_szAccounts[i]));
	}
	//m_GameClientView.BeginMoveJettons();
	m_GameClientView.StopMoveJettons();

	//界面设置
	m_GameClientView.SetBankerUser(m_wViewChairID[m_wBankerUser]);
	m_GameClientView.SetScoreInfo(m_lMaxCellScore,m_lCellScore);

	//派发扑克
	for(WORD i=0;i<MAX_COUNT;i++)
	{
		for (WORD j=m_wBankerUser;j<m_wBankerUser+GAME_PLAYER;j++)
		{
			WORD w=j%GAME_PLAYER;
			if (m_cbPlayStatus[w]==TRUE)
			{
				m_GameClientView.DispatchUserCard(m_wViewChairID[w],0);
			}
		}
	}

	//声音效果
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//用户放弃
bool CGameClientEngine::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//删除定时器
	if(IsLookonMode() || pGiveUp->wGiveUpUser!=GetMeChairID())KillGameClock(IDI_USER_ADD_SCORE);	

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[pGiveUp->wGiveUpUser]]=FALSE;

	//变量定义
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;
	WORD wViewChairID=m_wViewChairID[wGiveUpUser];

	//扑克变灰
	m_GameClientView.m_CardControl[wViewChairID].SetCardColor(1);

	//重新设置被选比牌用户
	if(m_wCurrentUser==GetMeChairID() && m_GameClientView.GetCompareInfo())
	{
		BOOL bCompareUser[GAME_PLAYER];
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && GetMeChairID()!=i)
			{
				bCompareUser[m_wViewChairID[i]]=TRUE;
			}
			else bCompareUser[m_wViewChairID[i]]=FALSE;
		}

		//设置箭头
		m_GameClientView.SetCompareCard(true,bCompareUser);
	}

	//状态设置
	if ((IsLookonMode()==false)&&(pGiveUp->wGiveUpUser==GetMeChairID())) SetGameStatus(GAME_STATUS_FREE);

	//环境设置
	if ((IsLookonMode()==true)||(wGiveUpUser!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	return true;
}

//用户下注
bool CGameClientEngine::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;
	ASSERT(pAddScore->wCurrentUser < GAME_PLAYER);

	//删除定时器
	if(IsLookonMode() || pAddScore->wAddScoreUser!=GetMeChairID())KillGameClock(IDI_USER_ADD_SCORE);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wAddScoreUser=pAddScore->wAddScoreUser;
	WORD wViewChairID=m_wViewChairID[wAddScoreUser];
	m_lCurrentTimes=pAddScore->lCurrentTimes;
	ASSERT(m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore);

	//加注处理
	if ((IsLookonMode()==true)||(wAddScoreUser!=wMeChairID))
	{
		//下注金币
		m_lTableScore[pAddScore->wAddScoreUser]+=pAddScore->lAddScoreCount;

		//加注界面
		m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[pAddScore->wAddScoreUser],pAddScore->lAddScoreCount);
		m_GameClientView.BeginMoveJettons();

		//播放声音
		if (m_cbPlayStatus[wAddScoreUser]==TRUE)
		{
			 PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
		}
	}

	//当前用户
	m_wCurrentUser=pAddScore->wCurrentUser;

	//控件信息
	if(pAddScore->wCompareState==FALSE && (!IsLookonMode()) && wMeChairID==m_wCurrentUser)
	{
		UpdataControl();
	}

	//设置时间
	if(pAddScore->wCompareState==FALSE)
	{//if(IsLookonMode() || wMeChairID!=m_wCurrentUser)
		SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}

	return true;
}

//用户看牌
bool CGameClientEngine::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_LookCard)) return false;
	CMD_S_LookCard * pLookCard=(CMD_S_LookCard *)pBuffer;

	//重新定时
	if(IsLookonMode() || m_wCurrentUser!=GetMeChairID())
	{
		//删除定时
		KillGameClock(IDI_USER_ADD_SCORE);
		SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}
	else
	{
		//设置扑克
		CopyMemory(m_cbHandCardData[GetMeChairID()],pLookCard->cbCardData,sizeof(pLookCard->cbCardData));
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetCardData(m_cbHandCardData[GetMeChairID()],MAX_COUNT);
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	}

	//设置界面
	WORD wId=pLookCard->wLookCardUser;
	if(GetMeChairID()!=wId || IsLookonMode())m_GameClientView.m_CardControl[m_wViewChairID[wId]].SetLookCard(true);

	//允许旁观
	if(IsLookonMode() && IsAllowLookon() && m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].GetLookCard())
	{
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	}

	//环境设置
	if ((IsLookonMode()==true)||(wId!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));

	return true;
}

//用户比牌
bool CGameClientEngine::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_CompareCard)) return false;
	CMD_S_CompareCard * pCompareCard=(CMD_S_CompareCard *)pBuffer;

	//删除时间
	KillGameClock(IDI_DISABLE);

	//取消等待
	m_GameClientView. SetWaitUserChoice(INVALID_CHAIR);

	//输牌用户
	m_wLostUser=pCompareCard->wLostUser;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_wLostUserID[i]==INVALID_CHAIR)			
		{
			m_wLostUserID[i] = m_wLostUser;
			break;
		}
	}

	//设置变量
	m_cbPlayStatus[m_wLostUser]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[m_wLostUser]]=FALSE;

	//状态设置
	if ((IsLookonMode()==false)&&(m_wLostUser==GetMeChairID())) SetGameStatus(GAME_STATUS_FREE);

	//当前用户
	m_wCurrentUser=pCompareCard->wCurrentUser;

	//取消比牌背景
	WORD wViewChair1=m_wViewChairID[pCompareCard->wCompareUser[0]];
	WORD wViewChair2=m_wViewChairID[pCompareCard->wCompareUser[1]];
	m_GameClientView.m_CardControl[wViewChair1].SetCompareBack(false);
	m_GameClientView.m_CardControl[wViewChair2].SetCompareBack(false);

	//游戏结束
	if(m_wCurrentUser==INVALID_CHAIR)m_GameClientView.StopUpdataScore(true);

	//过滤动画效果
	//OnFlashCardFinish(0,0);

	//隐藏控件
	ScoreControl(SW_HIDE);
	WORD wFalshUser[]={wViewChair1,wViewChair2};

	//动画效果
	m_GameClientView.PerformCompareCard(wFalshUser,m_wViewChairID[m_wLostUser]);

	return true;
}

//用户开牌
bool CGameClientEngine::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_OpenCard)) return false;
	CMD_S_OpenCard* pOpenCard=(CMD_S_OpenCard *)pBuffer;

	//取消等待
	m_GameClientView.SetWaitUserChoice(INVALID_CHAIR);

	//胜利用户
	m_wWinnerUser=pOpenCard->wWinner;

	//隐藏控件
	ScoreControl(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//游戏结束
	m_GameClientView.StopUpdataScore(true);

	//过滤动画效果
	//OnFlashCardFinish(0,0);

	//动画效果
	WORD wFalshUser[GAME_PLAYER];
	for (LONGLONG i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)wFalshUser[i]=m_wViewChairID[i];
	m_GameClientView.bFalshCard(wFalshUser);

	return true;
}

//用户强退
bool CGameClientEngine::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PlayerExit)) return false;
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

	WORD wID=pPlayerExit->wPlayerID;

	//游戏信息
	ASSERT(m_cbPlayStatus[wID]==TRUE);
	m_cbPlayStatus[wID]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[wID]]=FALSE;

	//用户名字
	for(WORD i=0;i<32;i++)m_szAccounts[wID][i]=0;

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	WORD wMeChiar=GetMeChairID();

	//删除定时器
	KillGameClock(IDI_USER_ADD_SCORE);

	//处理控件
	AddScoreControl(SW_HIDE);
	ScoreControl(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);
	m_GameClientView.m_GoldControl.SetGoldCount(0);

	//正常结束
	//m_GameClientView.m_ScoreView.ResetScore();
	LONGLONG lTax = m_lCellScore*pGameEnd->lGameTax/1000;
	WORD wWinner=INVALID_CHAIR;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (pGameEnd->lGameScore[i]!=0L)
		{
			if(pGameEnd->lGameScore[i]>0L) 
			{
				wWinner = i;
			}
			m_GameClientView.m_ScoreView.SetGameTax(lTax,i);
			if (m_szAccounts[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameScore[i]);
			else m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],pGameEnd->lGameScore[i]);
		}
	}
	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//清理效果
	m_GameClientView.StopUpdataScore(false);

	//比牌与被比牌用户所看到的数据
	for (WORD j=0;j<4;j++)
	{
		WORD wUserID=pGameEnd->wCompareUser[wMeChiar][j];
		if(wUserID==INVALID_CHAIR)continue;

		WORD wViewChiar=m_wViewChairID[wUserID];
		m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
		m_GameClientView.m_CardControl[wViewChiar].SetLookCard(false);
		m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[wUserID],MAX_COUNT);
	}

	//自己扑克
	if(pGameEnd->wCompareUser[wMeChiar][0]!=INVALID_CHAIR || m_bMingZhu[wMeChiar])
	{
		WORD wViewChiar=m_wViewChairID[wMeChiar];
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[wMeChiar],MAX_COUNT);
		m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
	}

	////过滤动态加入用户扑克
	//IClientUserItem * pClientUserItem=GetTableUserItem(wMeChiar);
	//BYTE a= GetGameStatus();
	//BYTE B=pClientUserItem->GetUserStatus();
	//if(pClientUserItem!=NULL && GetGameStatus()!=GAME_STATUS_PLAY && pClientUserItem->GetUserStatus()!=US_OFFLINE)
	//{
	//	WORD wViewChiar=m_wViewChairID[wMeChiar];
	//	m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	//}

	//开牌结束
	if(pGameEnd->wEndState==1)
	{
		//所有比牌用户
		if(m_cbPlayStatus[wMeChiar]==TRUE)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					WORD wViewChiar=m_wViewChairID[i];
					m_GameClientView.m_CardControl[wViewChiar].SetLookCard(false);
					m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
					m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
					m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[i],MAX_COUNT);
				}
			}
		}
	}

	//数字滚动动画
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( pGameEnd->lGameScore[i] == 0 ) continue;
		WORD wViewId = m_wViewChairID[i];	
		m_GameClientView.m_NumberControl[wViewId].SetScore( pGameEnd->lGameScore[i] );
	}
	m_GameClientView.BeginMoveNumber();

	//牌型
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( pGameEnd->lGameScore[i] == 0 ) continue;
		WORD wViewId = m_wViewChairID[i];
		m_GameClientView.m_CardControl[wViewId].SetCardType(m_GameLogic.GetCardType(pGameEnd->cbCardData[i],MAX_COUNT));
	}

	//回收筹码
	if(wWinner<GAME_PLAYER)
		m_GameClientView.SetGameEndInfo(m_wViewChairID[wWinner]);

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->lGameScore[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//开始按钮
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	//成绩显示在即时聊天对话框
	TCHAR szBuffer[64];
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("\n本局结束,成绩统计:"));
	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString(szBuffer);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		if (m_szAccounts[i][0]!=0)
		{
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("\n%s:%+I64d "),m_szAccounts[i],pGameEnd->lGameScore[i]);
			if(m_pIStringMessage != NULL)
				m_pIStringMessage->InsertNormalString(szBuffer);
			//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("扑克数据：%I64d,%I64d,%I64d;颜色数据：%I64d,%I64d,%I64d "),
			//	pGameEnd->cbCardData[i][0]&0x0f,pGameEnd->cbCardData[i][1]&0x0f,pGameEnd->cbCardData[i][2]&0x0f,
			//	(pGameEnd->cbCardData[i][0]&0xf0)>>4,(pGameEnd->cbCardData[i][1]&0xf0)>>4,
			//	(pGameEnd->cbCardData[i][2]&0xf0)>>4);
			//if(m_pIStringMessage != NULL)
			//	m_pIStringMessage->InsertSystemString(szBuffer);
		}
	}

	//状态设置
	m_GameClientView.SetWaitUserChoice(INVALID_CHAIR);
	SetGameStatus(GAME_STATUS_FREE);

	return true;
}

//转换信息
void CGameClientEngine::ChangeUserInfo(BYTE bCardData[],BYTE bCardCount,CString &CardInfo)
{
	//转换信息
	for(BYTE i=0;i<bCardCount;i++)
	{
		//扑克花色
		BYTE bCardColor = bCardData[i]&LOGIC_MASK_COLOR;
		switch(bCardColor)
		{
		case 0x00:
			{
				CardInfo+=TEXT("方块");
			}
			break;
		case 0x10:
			{
				CardInfo+=TEXT("梅花");
			}
			break;
		case 0x20:
			{
				CardInfo+=TEXT("红心");
			}
			break;
		case 0x30:
			{
				CardInfo+=TEXT("黑桃");
			}
			break;
		}

		//扑克数据
		BYTE bTempCardData = bCardData[i]&LOGIC_MASK_VALUE;
		switch(bTempCardData)
		{
		case 0x01:CardInfo+=TEXT("［A］.");break;
		case 0x02:CardInfo+=TEXT("［2］.");break;
		case 0x03:CardInfo+=TEXT("［3］.");break;
		case 0x04:CardInfo+=TEXT("［4］.");break;
		case 0x05:CardInfo+=TEXT("［5］.");break;
		case 0x06:CardInfo+=TEXT("［6］.");break;
		case 0x07:CardInfo+=TEXT("［7］.");break;
		case 0x08:CardInfo+=TEXT("［8］.");break;
		case 0x09:CardInfo+=TEXT("［9］.");break;
		case 0x0a:CardInfo+=TEXT("［10］.");break;
		case 0x0b:CardInfo+=TEXT("［J］.");break;
		case 0x0c:CardInfo+=TEXT("［Q］.");break;
		case 0x0d:CardInfo+=TEXT("［K］.");break;
		}
	}

	return ;
}

//处理控制
void CGameClientEngine::ScoreControl(BOOL bShow)
{
	m_GameClientView.m_btAddScore.ShowWindow(bShow);
	m_GameClientView.m_btCompareCard.ShowWindow(bShow);
	m_GameClientView.m_btGiveUp.ShowWindow(bShow);
	m_GameClientView.m_btLookCard.ShowWindow(bShow);
	m_GameClientView.m_btFollow.ShowWindow(bShow);

	return ;
}

//加注控制
void CGameClientEngine::AddScoreControl(BOOL bShow)
{
	//if(bShow)
	m_GameClientView.m_btMinScore.ShowWindow(bShow);
	m_GameClientView.m_btMaxScore.ShowWindow(bShow);
	m_GameClientView.m_btCancel.ShowWindow(bShow);
	m_GameClientView.m_btConfirm.ShowWindow(bShow);

	//m_GameClientView.m_GoldControl.m_btMinScore.ShowWindow(bShow);
	//m_GameClientView.m_GoldControl.m_btMaxScore.ShowWindow(bShow);
	//m_GameClientView.m_GoldControl.m_btCancel.ShowWindow(bShow);
	//m_GameClientView.m_GoldControl.m_btConfirm.ShowWindow(bShow);
	m_GameClientView.m_GoldControl.ShowWindow(bShow);
	m_GameClientView.m_GoldControl.m_bMingTag=m_bMingZhu[GetMeChairID()];

	m_GameClientView.RefreshGameView();

	return;
}

//处理控制
void CGameClientEngine::UpdataControl()
{
	ScoreControl(SW_SHOW);
	ActiveGameFrame();

	//放弃按钮
	WORD wMeChairID = GetMeChairID();
	m_GameClientView.m_btGiveUp.EnableWindow(TRUE);

	//看牌按钮
	m_GameClientView.m_btLookCard.EnableWindow((m_bMingZhu[wMeChairID])?FALSE:TRUE);

	//判断开牌
	LONGLONG lTemp=(m_bMingZhu[wMeChairID])?6:5;
	if((m_lUserMaxScore-m_lTableScore[wMeChairID]) >= (m_lMaxCellScore*lTemp))
	{
		//查找上家
		for (LONGLONG j=wMeChairID-1;;j--)
		{
			if(j==-1)j=GAME_PLAYER-1;
			if(m_cbPlayStatus[j]==TRUE)break;
		}

		//跟注按钮
		m_GameClientView.m_btFollow.EnableWindow((m_lTableScore[j]==m_lCellScore)?FALSE:TRUE);

		//加注按钮
		m_GameClientView.m_btAddScore.EnableWindow(((m_lCurrentTimes*m_lCellScore) < m_lMaxCellScore)?TRUE:FALSE);

		//比牌按钮
		if(wMeChairID==m_wBankerUser || m_lTableScore[wMeChairID]>=2*m_lCellScore)
		{
			m_GameClientView.m_btCompareCard.EnableWindow(TRUE);
		}
		else m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	}
	else 
	{
		m_GameClientView.m_btFollow.EnableWindow(FALSE);
		m_GameClientView.m_btAddScore.EnableWindow(FALSE);
		m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
		m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btCompareCard.ShowWindow(SW_HIDE);
	}

	return;
}

//开始按钮
LRESULT	CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	if(!IsLookonMode())KillGameClock(IDI_START_GAME);

	//停止动画
	m_GameClientView.StopMoveJettons();
	m_GameClientView.StopMoveNumber();

	//设置界面
	m_wBankerUser=INVALID_CHAIR;
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetScoreInfo(0,0);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L,0L);

	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bMingZhu[i]=false;
		m_GameClientView.m_NumberControl[i].ResetControl();
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[i].SetDisplayHead(false);
		m_GameClientView.m_CardControl[i].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[i].SetLookCard(false);
		m_GameClientView.m_CardControl[i].SetCompareCard(false);
		m_GameClientView.m_CardControl[i].SetCardType(0);
	}
	m_GameClientView.m_JettonControl.ResetControl();

	//状态变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//加注信息
	m_lMaxCellScore=0L;
	m_lCellScore=0L;

	//发送消息
	if(!IsLookonMode())SendUserReady(NULL,0);

	return 0;
}

//加注按钮 
LRESULT	CGameClientEngine::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_lCellScore>0L);

	//处理控件
	m_GameClientView.m_GoldControl.SetMaxGold(m_lMaxCellScore);
	m_GameClientView.m_GoldControl.SetMinGold(m_lCellScore*m_lCurrentTimes);
	m_GameClientView.m_GoldControl.SetGold(m_lCellScore*m_lCurrentTimes);
	AddScoreControl(SW_SHOW);
	ScoreControl(SW_HIDE);

	//m_GameClientView.m_btMinScore.ShowWindow(SW_SHOW);
	return 0;
}

//取消消息
LRESULT CGameClientEngine::OnCancelAdd(WPARAM wParam, LPARAM lParam)
{
	//处理控件
	AddScoreControl(SW_HIDE);
	ScoreControl(SW_SHOW);

	return 0;
}

//最少加注
LRESULT	CGameClientEngine::OnMinScore(WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	////测试时间
	//BYTE bTemp[]={1,2,3};
	//for (WORD j=0;j<GAME_PLAYER;j++)
	//{
	//	m_GameClientView.m_CardControl[j].SetCardData(bTemp,3);
	//	m_GameClientView.m_CardControl[j].SetDisplayHead(true);
	//}

	//WORD wFalshUser[GAME_PLAYER];
	//for (LONGLONG i=0;i<GAME_PLAYER;i++)wFalshUser[i]=i;
	//m_GameClientView.bFalshCard(wFalshUser);

	//WORD wFalshUser1[]={1,2};

	//动画效果
	//m_GameClientView.PerformCompareCard(wFalshUser1,1);

	//////派发扑克
	////for(WORD i=0;i<MAX_COUNT;i++)
	////{
	//	for (WORD j=0;j<GAME_PLAYER;j++)
	//	{
	//		m_GameClientView.SetUserTableScore(j,1000,1000);
	//		m_GameClientView.BeginMoveJettons();
	//		//m_GameClientView.DispatchUserCard(j,0);
	//	}
	////}
	////回收筹码
	//m_GameClientView.SetGameEndInfo(2);

	////数字滚动动画
	//for( WORD i = 0; i < GAME_PLAYER; i++ )
	//{
	//	LONGLONG lscorre = ((rand()%2)?1:-1)*99999;
	//	m_GameClientView.m_NumberControl[i].SetScore( lscorre );
	//}
	//m_GameClientView.BeginMoveNumber();

	//return 0;
#endif

	ASSERT(m_lCellScore>0L);
	m_GameClientView.m_GoldControl.SetGold(m_lCellScore*m_lCurrentTimes);

	return 0;
}

//最大加注
LRESULT	CGameClientEngine::OnMaxScore(WPARAM wParam, LPARAM lParam)
{
	if(m_lMaxCellScore>0)m_GameClientView.m_GoldControl.SetGold(m_lMaxCellScore);

	return 0;
}

//加注消息 && 跟注消息
LRESULT CGameClientEngine::OnConfirmScore(WPARAM wParam, LPARAM lParam)
{
	//一为跟注
	WORD wTemp=LOWORD(wParam);

	//效验数据
	ASSERT(wTemp==1 || m_GameClientView.m_GoldControl.GetGold() >= (m_lCellScore*m_lCurrentTimes)
		&& m_GameClientView.m_GoldControl.GetGold() <= m_lMaxCellScore);

	//删除时间
	KillGameClock(IDI_USER_ADD_SCORE);

	//获取筹码
	LONGLONG lCurrentScore=(wTemp==0)?(m_GameClientView.m_GoldControl.GetGold()):(m_lCellScore*m_lCurrentTimes);
	WORD wMeChairID=GetMeChairID();
 
	//明注加倍
	if(m_bMingZhu[wMeChairID])lCurrentScore*=2;

	//预先处理
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//处理按钮
	AddScoreControl(SW_HIDE);

	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.SetButtonImage(IDB_USERCONTROL_INVEST,AfxGetInstanceHandle(),false,false);
	ScoreControl(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//界面设置
	m_GameClientView.RefreshGameView();
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.wState=0;
	AddScore.lScore=lCurrentScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//看牌消息
LRESULT	CGameClientEngine::OnLookCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	WORD wMeChairID=GetMeChairID();
	KillGameClock(IDI_USER_ADD_SCORE);

	//重新定时
	SetGameClock(wMeChairID,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	//设置数据
	m_bMingZhu[wMeChairID] = true;

	//发送消息
	SendSocketData(SUB_C_LOOK_CARD,NULL,0);
	
	//控件信息
	UpdataControl();

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));

	return 0;
}

//比牌消息
LRESULT	CGameClientEngine::OnCompareCard(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	KillGameClock(IDI_USER_ADD_SCORE);

	//处理控件
	AddScoreControl(SW_HIDE);

	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//判断明注
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.wState=TRUE;
	AddScore.lScore=lCurrentScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//当前人数
	BYTE UserCount=0;
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)UserCount++;
	}

	//庄家在第一轮没下注只能跟上家比牌 或 只剩下两人
	if(m_wBankerUser==wMeChairID && (m_lTableScore[wMeChairID]-lCurrentScore)==m_lCellScore || UserCount==2)
	{
		//构造变量
		CMD_C_CompareCard CompareCard;

		//查找上家
		for (LONGLONG i=(LONGLONG)wMeChairID-1;;i--)
		{
			if(i==-1)i=GAME_PLAYER-1;
			if(m_cbPlayStatus[i]==TRUE)
			{
				CompareCard.wCompareUser=(WORD)i;
				break;
			}
		}
		//发送消息
		SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));
	}
	else	//选择玩家比牌
	{
		BOOL bCompareUser[GAME_PLAYER];
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && wMeChairID!=i)
			{
				bCompareUser[m_wViewChairID[i]]=TRUE;
			}
			else bCompareUser[m_wViewChairID[i]]=FALSE;
		}

		//设置箭头
		m_GameClientView.SetCompareCard(true,bCompareUser);

		//发送等待"比牌"信息
		SendSocketData(SUB_C_WAIT_COMPARE,NULL,0);

		//设置时间
		SetGameClock(wMeChairID,IDI_USER_COMPARE_CARD,TIME_USER_COMPARE_CARD);

		//提示标志
		m_GameClientView.SetWaitUserChoice(FALSE);
	}

	return 0;
}

//被选比牌用户
LRESULT CGameClientEngine::OnChooseCUser(WPARAM wParam, LPARAM lParam)
{
	//效验数据
	WORD wTemp=LOWORD(wParam);
	WORD wChairID=INVALID_CHAIR;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_wViewChairID[i]==wTemp)
		{
			wChairID=i;
			break;
		}
	}
	ASSERT(wChairID!=INVALID_CHAIR && m_cbPlayStatus[wChairID]==TRUE && wChairID!=GetMeChairID());

	//删除定时器
	KillGameClock(IDI_USER_COMPARE_CARD);

	//清理界面
	m_GameClientView.SetCompareCard(false,NULL);

	//构造变量
	CMD_C_CompareCard CompareCard;
	ZeroMemory(&CompareCard,sizeof(CompareCard));
	CompareCard.wCompareUser=wChairID;

	//发送消息
	SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

	return 0;
}

//开牌消息
LRESULT	CGameClientEngine::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	KillGameClock(IDI_USER_ADD_SCORE);

	//处理控件
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	//m_GameClientView.m_btOpenCard.EnableWindow(FALSE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//查找人数
	WORD bUserCount =0;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE) bUserCount++;
	}

	//两人比牌
	if(bUserCount==2)
	{
		OnCompareCard(0,0);
		return 0;
	}

	//判断明注
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.wState=2;
	AddScore.lScore=lCurrentScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//发送消息
	SendSocketData(SUB_C_OPEN_CARD,NULL,0);

	return 0;
}

//放弃消息
LRESULT	CGameClientEngine::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameClock(IDI_USER_ADD_SCORE);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//处理按钮
	AddScoreControl(SW_HIDE);

	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//扑克变灰
	m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetCardColor(1);

	//发送消息
	SendSocketData(SUB_C_GIVE_UP,NULL,0);

	return 0;
}

//发牌完成
LRESULT CGameClientEngine::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//设置时间
	if(m_wCurrentUser==INVALID_CHAIR)return 0;
	SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	//过虑用户
	WORD wMeChairID=GetMeChairID();
	if (IsLookonMode() || wMeChairID!=m_wCurrentUser)return 0;

	//可用控件
	ActiveGameFrame();
	UpdataControl();

	return 0;
}

//闪牌完成
LRESULT CGameClientEngine::OnFlashCardFinish(WPARAM wParam, LPARAM lParam)
{
	//比牌结束
	if(m_wWinnerUser>=GAME_PLAYER)
	{
		if(m_wLostUser>GAME_PLAYER)return 0;
		ASSERT(m_wLostUser<GAME_PLAYER);

		//输牌用户
		WORD wLostUser=m_wLostUser;
		WORD wMeChairID=GetMeChairID();

		//变量定义
		WORD wViewChairID=m_wViewChairID[wLostUser];
		WORD wMeViewChairID=m_wViewChairID[wMeChairID];

		//设置扑克
		m_GameClientView.m_CardControl[wMeViewChairID].SetCardData(m_cbHandCardData[wMeChairID],MAX_COUNT);

		//环境设置
		if ((!IsLookonMode()) && wLostUser==wMeChairID) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));

		//玩家人数
		BYTE bCount=0;
		for (WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)bCount++;

		//判断结束
		if(bCount>1)
		{
			//控件信息
			if((!IsLookonMode()) && wMeChairID==m_wCurrentUser)
			{
				UpdataControl();
			}

			//设置时间
			SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
		}
		else 
		{
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)break;
			}
			WORD wMeChairID = GetMeChairID();
			if(i==wMeChairID || wMeChairID==m_wLostUser)
			{
				//发送消息
				SendSocketData(SUB_C_FINISH_FLASH,NULL,0);
			}
		}
	}
	else			//开牌结束
	{
		//ASSERT(m_wWinnerUser<GAME_PLAYER);
		if(m_wWinnerUser>GAME_PLAYER)return 0;		

		//比牌失败用户
		for (int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && i!=m_wWinnerUser)		
			{
				//扑克变裂
				WORD wViewChairID=m_wViewChairID[i];
				m_GameClientView.m_CardControl[wViewChairID].SetCardColor(2);

				//状态设置
				if ((!IsLookonMode())&&(i==GetMeChairID())) 
				{
					SetGameStatus(GAME_STATUS_FREE);
					//环境设置
					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
				}
			}
		}
		m_wWinnerUser=INVALID_CHAIR;

		//发送消息
		SendSocketData(SUB_C_FINISH_FLASH,NULL,0);
	}
	
	//过滤动态加入用户扑克
	IClientUserItem * pClientUserItem=GetTableUserItem(GetMeChairID());
	if(pClientUserItem!=NULL && pClientUserItem->GetUserStatus()!=GAME_STATUS_PLAY && pClientUserItem->GetUserStatus()!=US_OFFLINE)
	{
		WORD wViewChiar=m_wViewChairID[GetMeChairID()];
		//m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	}
	else
	{
		WORD wViewChiar=m_wViewChairID[GetMeChairID()];
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
