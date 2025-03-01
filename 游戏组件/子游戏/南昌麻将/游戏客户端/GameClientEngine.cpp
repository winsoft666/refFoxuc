#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientEngine.h"
#include "cassert"
//////////////////////////////////////////////////////////////////////////

//游戏定时器
#define IDI_START_GAME				200									//开始定时器
#define IDI_OPERATE_CARD			202									//操作定时器
#define IDI_BATCH					203									//洗澡定时器

//游戏定时器
#define TIME_START_GAME				40									//开始定时器
#define TIME_OPERATE_CARD			30									//操作定时器
#define TIME_BATCH					5									//洗澡定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
	ON_MESSAGE(IDM_GAME_START,OnGameStart)
	ON_MESSAGE(IDM_SICE_TWO,OnSiceTwo)
	ON_MESSAGE(IDM_BATCH,OnBatch)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
	ON_MESSAGE(IDM_BATCH_END,OnBatchEnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//堆立变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));
	m_wKingHeapPos =-1;
	m_wKingPos=-1;
	FillMemory(m_cbKingCardIndex,sizeof(m_cbKingCardIndex),0xFF);

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_bStart=true;
	m_wUserAction=0;

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));


	//扑克变量
	m_GameClientView.m_HandCardControl.SetKingCardIndex(m_cbKingCardIndex);
	for (BYTE j=0;j<4;j++)
	{
		m_GameClientView.m_TableCard[j].SetKingCardIndex(m_cbKingCardIndex);
	}


}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
	ASSERT(m_pGlobalUnits!=NULL);

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//堆立变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));
	m_wKingHeapPos =-1;
	m_wKingPos=-1;
	FillMemory(m_cbKingCardIndex,sizeof(m_cbKingCardIndex),0xFF);

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_wUserAction=0;
	m_bStart=true;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HeapCard[i].SetKingCardData(0xFF);
		m_GameClientView.m_HeapCard[i].SetKingPos(WORD(-1));
		m_GameClientView.m_DiscardCard[i].SetKingCardIndex(m_cbKingCardIndex);
		m_GameClientView.m_TableCard[i].SetKingCardIndex(m_cbKingCardIndex);
		for (WORD j=0;j<MAX_WEAVE;j++)
		{
			m_GameClientView.m_WeaveCard[i][j].SetKingCardIndex(m_cbKingCardIndex);
		}
	}
	m_GameClientView.m_HandCardControl.SetKingCardIndex(m_cbKingCardIndex);


	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
	
	CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();

	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=m_pGlobalUnits->IsAllowGameSound();
	
	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		

		m_pGlobalUnits->m_bAllowSound = GameOption.m_bEnableSound;
		
	}

	return;
}

//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return false;
			}
			if (nElapse<=5) 
			{

					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}
			return true;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{
	//		if ((m_wTimeOutCount>=3)&&(m_bStustee==false))	OnStusteeControl(0,0);

			//超时判断
			if (((nElapse==0)&&(IsLookonMode()==false))||(m_bStustee==true))
			{
				//获取位置
				WORD wMeChairID=GetMeChairID();

				//动作处理
				if (wChairID==wMeChairID)
				{
					if((m_bStustee==false))
					{
						m_wTimeOutCount++;
						//CString strTemp;
						//strTemp.Format(TEXT("您已经超时%d次"),m_wTimeOutCount);
						//InsertSystemString(LPCTSTR(strTemp));
						if(m_wTimeOutCount==3)
						{
							//InsertSystemString(TEXT("由于您多次超时，切换为“系统托管”模式."));

							if(m_pIStringMessage!=NULL)
								m_pIStringMessage->InsertSystemString(TEXT("由于您多次超时，切换为“系统托管”模式."));
						}
					}

					if (m_wCurrentUser==wMeChairID)
					{
						BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
						//出牌效验
						if (VerdictOutCard(cbCardData)==false)
						{
							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								//出牌效验
								if (m_cbCardIndex[i]==0) continue;
								if (VerdictOutCard(m_GameLogic.SwitchToCardData(i))==false) 
									continue;

								//设置变量
								cbCardData=m_GameLogic.SwitchToCardData(i);
							}
							//InsertSystemString(TEXT("由于您超时，系统已帮您出牌！"));

							if(m_pIStringMessage!=NULL)
								m_pIStringMessage->InsertSystemString(TEXT("由于您超时，系统已帮您出牌！"));
						}
						OnOutCard(cbCardData,cbCardData);
					}
					else
						OnCardOperate(WIK_NULL,0);
				}

				return false;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{

					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}
			return true;
		}
	case IDI_BATCH:   //洗澡定时器
		{
			if ((nElapse==0)||(m_bStustee==true))
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
				{
					OnBatch(0,0);
				}
				return true;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			return true;
		}
	}

	return false;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//扑克控制
	m_GameClientView.m_HandCardControl.SetDisplayItem(IsAllowLookon()||IsLookonMode()==false);
	m_GameClientView.RefreshGameView();

	return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			return OnSubOperateNotify(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			return OnSubOperateResult(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_BATCH:     //用户洗澡
		{
			return OnSubGameChip(pBuffer,wDataSize);
		}
	case SUB_S_TRUSTEE:			//用户托管
		{
			return OnSubTrustee(pBuffer,wDataSize);
		}
	case SUB_S_BATCHRESULT:	//洗澡结果
		{
			return OnSubBatchResult(pBuffer,wDataSize);
		}

	}

	return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStation, bool bLookonOther, VOID * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置数据
			m_wBankerUser=pStatusFree->wBankerUser;
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);
			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusFree->bTrustee[i]);
			}
			m_GameClientView.m_lLastTurnScore[SwitchViewChairID(GetMeChairID())]=pStatusFree->lLastTurnScore;
			m_GameClientView.m_lAllTurnScore[SwitchViewChairID(GetMeChairID())]=pStatusFree->lAllTurnScore;


			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?34:34);
				m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}

			//丢弃效果
			m_GameClientView.SetDiscUser(INVALID_CHAIR);
			m_GameClientView.SetTimer(IDI_DISC_EFFECT,250,NULL);
			
			//更新界面
			//m_GameClientView.m_btScore.ShowWindow(SW_SHOW);
			m_GameClientView.RefreshGameView();

			return true;
		}
	case GS_MJ_BATCH:	//卖底状态
		{
			if(wDataSize != sizeof(CMD_S_StatusBatch)) return false;
			CMD_S_StatusBatch * pStatusChip =(CMD_S_StatusBatch *)pBuffer;

			//设置变量
			m_GameClientView.SetCellScore(pStatusChip->lCellScore);
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);
			//m_GameClientView.m_btScore.ShowWindow(SW_SHOW);

			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusChip->bTrustee[i]);
			}
			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?34:34);
				m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);

				m_GameClientView.m_lAllTurnScore[wViewChairID]=pStatusChip->lAllTurnScore[i];
				m_GameClientView.m_lLastTurnScore[wViewChairID]=pStatusChip->lLastTurnScore[i];

			}

			m_wTimeOutCount=0;

			if(pStatusChip->bTrustee[GetMeChairID()])
				m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_STOP_TRUSTEE,AfxGetInstanceHandle(),false,false);
			else
				m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);


			//设置控件
			if (IsLookonMode()==false && pStatusChip->bComplete==FALSE)
			{
				//显示买底
				m_GameClientView.m_btBatch.ShowWindow(SW_SHOW);
				m_bStustee = false;
				m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);


				//显示界面
				SetGameClock(GetMeChairID(),IDI_BATCH,TIME_BATCH);
			}
			//丢弃效果
			m_GameClientView.SetDiscUser(INVALID_CHAIR);
			m_GameClientView.SetTimer(IDI_DISC_EFFECT,250,NULL);


			return true;
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount+1; //服务器预先减去精牌 所以客户端要预先增加

			//历史变量
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));

			//丢弃效果
			if(m_wOutCardUser != INVALID_CHAIR)
				m_GameClientView.SetDiscUser(SwitchViewChairID(m_wOutCardUser));
			m_GameClientView.SetTimer(IDI_DISC_EFFECT,250,NULL);


			m_GameClientView.m_cbUpDownKingCardData=pStatusPlay->cbUpDownKingCardData;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_lUpDownScore[SwitchViewChairID(i)]=pStatusPlay->lUpDownScore[i];
			}
		
			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_lAllTurnScore[wViewChairID]=pStatusPlay->lAllTurnScore[i];
				m_GameClientView.m_lLastTurnScore[wViewChairID]=pStatusPlay->lLastTurnScore[i];

			}
			//扑克变量
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));


			m_wTimeOutCount=0;
			if(pStatusPlay->bTrustee[GetMeChairID()])
				m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_STOP_TRUSTEE,AfxGetInstanceHandle(),false,false);
			else
				m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);

			
			//设置精牌
			CopyMemory(m_cbKingCardIndex,pStatusPlay->cbKingCardIndex,sizeof(m_cbKingCardIndex));
			m_GameLogic.SetKingCardIndex(pStatusPlay->cbKingCardIndex);
			m_GameClientView.m_HandCardControl.SetKingCardIndex(pStatusPlay->cbKingCardIndex);
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_HeapCard[i].SetKingCardData(m_GameLogic.SwitchToCardData(pStatusPlay->cbKingCardIndex[0]));
				m_GameClientView.m_DiscardCard[i].SetKingCardIndex(pStatusPlay->cbKingCardIndex);
				m_GameClientView.m_TableCard[i].SetKingCardIndex(pStatusPlay->cbKingCardIndex);
				for (WORD j=0;j<MAX_WEAVE;j++)
				{
					m_GameClientView.m_WeaveCard[i][j].SetKingCardIndex(pStatusPlay->cbKingCardIndex);
				}
			}	
			m_GameClientView.SetKingCardIndex(pStatusPlay->cbKingCardIndex);
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex);

			//辅助变量
			WORD wViewChairID[GAME_PLAYER]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++)
				wViewChairID[i]=SwitchViewChairID(i);

			//界面设置
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.SetBankerUser(wViewChairID[m_wBankerUser]);
			//m_GameClientView.m_btScore.ShowWindow(SW_SHOW);



			//组合扑克
			BYTE cbGangCount=0;
			BYTE cbWeaveCard[4]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				for (BYTE j=0;j<m_cbWeaveCount[i];j++)
				{
					WORD wWeaveKind=m_WeaveItemArray[i][j].wWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(wWeaveKind,cbCenterCard,cbWeaveCard);
					m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetCardData(cbWeaveCard,cbWeaveCardCount);
					if ((wWeaveKind&WIK_GANG)&&(m_WeaveItemArray[i][j].wProvideUser==i))
					{
						m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetDisplayItem(false);
					}
					if(wWeaveKind&WIK_GANG)
					{
						cbGangCount++;
					}
				}
			}

			//用户扑克
			if (m_wCurrentUser==GetMeChairID())
			{
				//调整扑克
				if (pStatusPlay->cbSendCardData!=0x00)
				{
					//变量定义
					BYTE cbCardCount=pStatusPlay->cbCardCount;
					BYTE cbRemoveCard[]={pStatusPlay->cbSendCardData};

					//调整扑克
					m_GameLogic.RemoveCard(pStatusPlay->cbCardData,cbCardCount,cbRemoveCard,1);
					pStatusPlay->cbCardData[pStatusPlay->cbCardCount-1]=pStatusPlay->cbSendCardData;
				}
				//设置扑克
				BYTE cbCardCount=pStatusPlay->cbCardCount;				//m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,cbCardCount-1,pStatusPlay->cbCardData[cbCardCount-cbCardCount]);
				SetHandCardData(pStatusPlay->cbCardData,cbCardCount-1,pStatusPlay->cbCardData[cbCardCount-1]);

			}
			else 
			{
				//m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0); 
				SetHandCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0); 
			}


			//扑克设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//用户扑克
				if (i!=GetMeChairID())
				{
					BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[i]*3;
					WORD wUserCardIndex=(wViewChairID[i]<2)?wViewChairID[i]:2;
					m_GameClientView.m_UserCard[wUserCardIndex].SetCardData(cbCardCount,(m_wCurrentUser==i));
				}

				//丢弃扑克
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(m_cbDiscardCard[i],m_cbDiscardCount[i]);
			}

			//控制设置
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayItem(true);
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);

			}

			//堆立扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?34:34);
			}

			//分发扑克
			BYTE cbTakeCount=MAX_REPERTORY-m_cbLeftCardCount+cbGangCount;
			BYTE cbSiceFirst=HIBYTE(pStatusPlay->wSiceCount);
			BYTE cbSiceSecond=LOBYTE(pStatusPlay->wSiceCount);
			BYTE cbSiceUserFirst=HIBYTE(pStatusPlay->wSiceCount2);
			BYTE cbSiceUserSecond=LOBYTE(pStatusPlay->wSiceCount2);
			
			//总和
			WORD wSiceTotal=cbSiceUserFirst+cbSiceUserSecond;
			WORD wTakeChairID=(m_wBankerUser+cbSiceFirst+cbSiceSecond-1)%GAME_PLAYER;

			//精牌变量
			m_wKingHeapPos=wTakeChairID;
			m_wKingPos=HIBYTE(pStatusPlay->wSiceCount3)+LOBYTE(pStatusPlay->wSiceCount3);

			//堆牌精牌
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(i== m_wKingHeapPos)
				{
					m_GameClientView.m_HeapCard[SwitchViewChairID(m_wKingHeapPos)].SetKingCardData(m_GameLogic.SwitchToCardData(pStatusPlay->cbKingCardIndex[0]));
					m_GameClientView.m_HeapCard[SwitchViewChairID(m_wKingHeapPos)].SetKingPos(cbSiceFirst+cbSiceSecond);
				}
				else
				{
					m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetKingCardData(0XFF);
					m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetKingPos(WORD(-1));
				}
			}

			if(wSiceTotal*2>HEAP_FULL_COUNT)
			{
				wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
				wSiceTotal=wSiceTotal%17;
			}

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//计算数目
				BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(wSiceTotal)*2:0);
				BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

				//提取扑克
				cbTakeCount-=cbRemoveCount;
				m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

				//完成判断
				if (cbTakeCount==0)
				{
					m_wHeapHand=wTakeChairID;
					m_wHeapTail=(m_wBankerUser+cbSiceUserFirst+cbSiceUserSecond-1)%GAME_PLAYER;
					break;
				}

				//切换索引
				wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
			}


			//杠牌补牌
			for (BYTE i=0;i<cbGangCount;i++) DeductionTableCard(false);

			//堆立界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_HeapCard[wViewChairID[i]].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//历史扑克
			if (m_wOutCardUser!=INVALID_CHAIR)
			{
				WORD wOutChairID=SwitchViewChairID(m_wOutCardUser);
				m_GameClientView.SetOutCardInfo(wOutChairID,m_cbOutCardData);
			}

			//出牌设置
			if (cbGameStation==GS_MJ_PLAY)
			{
				//操作界面
				if ((IsLookonMode()==false)&&(pStatusPlay->wActionMask!=WIK_NULL))
				{
					//获取变量
					WORD wActionMask=pStatusPlay->wActionMask;
					BYTE cbActionCard=pStatusPlay->cbActionCard;

					//变量定义
					tagGangCardResult GangCardResult;
					ZeroMemory(&GangCardResult,sizeof(GangCardResult));

					//杠牌判断
					if ((wActionMask&WIK_GANG)!=0)
					{
						//桌面杆牌
						if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
						{
							GangCardResult.cbCardCount=1;
							GangCardResult.cbCardData[0]=cbActionCard;
						}

						//自己杆牌
						if ((m_wCurrentUser==GetMeChairID())||(cbActionCard==0))
						{
							WORD wMeChairID=GetMeChairID();
							m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
						}
					}

					//设置界面
					m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,wActionMask,GangCardResult);
					if (m_wCurrentUser==INVALID_CHAIR) 
						SetGameClock(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
				}

				//设置时间
				if (m_wCurrentUser!=INVALID_CHAIR)
				{
					//计算时间
					WORD wTimeCount=TIME_OPERATE_CARD;
					SetGameClock(m_wCurrentUser,TIME_OPERATE_CARD,wTimeCount);
				}
			}

			//更新界面
			m_GameClientView.RefreshGameView();

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart))	return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置状态
	SetGameStatus(GS_MJ_PLAY);
	m_bStart=false;
	

	//旁观设置
	if(IsLookonMode()==true)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.m_HeapCard[i].SetKingCardData(0xFF);
			m_GameClientView.m_HeapCard[i].SetKingPos(WORD(-1));
			m_GameClientView.m_DiscardCard[i].SetKingCardIndex(m_cbKingCardIndex);
			m_GameClientView.m_TableCard[i].SetKingCardIndex(m_cbKingCardIndex);
			for (WORD j=0;j<MAX_WEAVE;j++)
			{
				m_GameClientView.m_WeaveCard[i][j].SetKingCardIndex(m_cbKingCardIndex);
			}
		}
		m_GameClientView.m_HandCardControl.SetKingCardIndex(m_cbKingCardIndex);

	}

	//设置变量
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;
	CopyMemory(m_cbKingCardIndex,pGameStart->cbKingCardIndex,sizeof(m_cbKingCardIndex));

	//设置精牌
	m_GameLogic.SetKingCardIndex(pGameStart->cbKingCardIndex);    
	
	//保留数据
	CopyMemory(m_cbCardData,pGameStart->cbCardData,sizeof(m_cbCardData));
	m_wUserAction=pGameStart->wUserAction;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置扑克
	BYTE cbCardCount=(GetMeChairID()==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//设置界面
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);

	m_GameClientView.SetDiscUser(INVALID_CHAIR);


	//旁观界面
	if (bPlayerMode==false)
	{
		m_GameClientView.SetHuangZhuang(false);
		m_GameClientView.SetStatusFlag(false,false);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
		m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	}

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?34:34);
	}

	//分发扑克
	BYTE cbTakeCount=(MAX_COUNT-1)*GAME_PLAYER+1;
	BYTE cbSiceFirst=HIBYTE(pGameStart->wSiceCount);
	BYTE cbSiceSecond=LOBYTE(pGameStart->wSiceCount);
	BYTE cbSiceUserFirst=HIBYTE(pGameStart->wSiceCount2);
	BYTE cbSiceUserSecond=LOBYTE(pGameStart->wSiceCount2);
	CString str;
	str.Format(TEXT("%d %d %d"),m_wBankerUser,cbSiceFirst,cbSiceSecond);
	//InsertSystemString(str);
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString(str);

	str.Format(TEXT("%d %d"),cbSiceUserFirst,cbSiceUserSecond);
	//InsertSystemString(str);
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString(str);
	
	//精牌
	BYTE cbsicecount31=HIBYTE(pGameStart->wSiceCount3);
	BYTE cbsicecount32=LOBYTE(pGameStart->wSiceCount3);

	//总和
	WORD wSiceTotal=cbSiceUserFirst+cbSiceUserSecond;

	WORD wTakeChairID=(m_wBankerUser+cbSiceFirst+cbSiceSecond-1)%GAME_PLAYER;

	m_GameClientView.m_DrawSiceWnd.SetSiceInfo(m_GameClientView.GetDC(),250,cbSiceFirst,cbSiceSecond,cbSiceUserFirst,cbSiceUserSecond,cbsicecount31,cbsicecount32);
	m_GameClientView.m_DrawSiceWnd.ShowWindow(SW_SHOW);

	//精牌变量
	INT wTakeSizeTotal=18-cbsicecount31-cbsicecount32-(17-wSiceTotal);
	m_wKingHeapPos=wTakeChairID;

	str.Format(TEXT("%d %d"),cbsicecount31,cbsicecount32);
	//InsertSystemString(str);
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString(str);

	if(wTakeSizeTotal<0)
	{
		m_wKingHeapPos=(m_wKingHeapPos-1+GAME_PLAYER)%GAME_PLAYER;
		wTakeSizeTotal=17-abs(wTakeSizeTotal);
	}

	m_wKingPos=wTakeSizeTotal;
	str.Format(TEXT("%d %d"),m_wKingHeapPos,m_wKingPos);
	//InsertSystemString(str);
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString(str);
	if(wSiceTotal*2>HEAP_FULL_COUNT)
	{
		wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
		wSiceTotal=wSiceTotal%17;
	}

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//计算数目
		BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(wSiceTotal)*2:0);
		BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

		//提取扑克
		cbTakeCount-=cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

		//完成判断
		if (cbTakeCount==0)
		{
			m_wHeapHand=wTakeChairID;
			m_wHeapTail=(m_wBankerUser+cbSiceUserFirst+cbSiceUserSecond-1)%GAME_PLAYER;
			break;
		}

		//切换索引
		wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
	}

	//设置积分
	CString strTemp ,strEnd=TEXT("\n上下番得分:\n");
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IClientUserItem * pUserData=GetTableUserItem(i);

		strTemp.Format(TEXT("%s: %ld\n"),pUserData->GetNickName(),pGameStart->lUpDownScore[i]);
		strEnd += strTemp;

	}
	m_GameClientView.m_cbUpDownKingCardData=pGameStart->cbUpDownKingCardData;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_lUpDownScore[SwitchViewChairID(i)]=pGameStart->lUpDownScore[i];
	}

	//消息积分
	//InsertSystemString((LPCTSTR)strEnd);
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString((LPCTSTR)strEnd);

	//更新界面
	m_GameClientView.RefreshGameView();

	//激活框架
	if (bPlayerMode==true) 	ActiveGameFrame();

	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	return true;
}

//用户出牌
bool CGameClientEngine::OnSubOutCard(VOID * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard))	return false;

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	if(!m_bStart)	OnGameStart(0,0);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	m_cbOutCardData=pOutCard->cbOutCardData;



	//其他用户
	if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=wMeChairID))
	{
		//环境设置
		KillGameClock(IDI_OPERATE_CARD); 
		PlayCardSound(pOutCard->wOutCardUser,pOutCard->cbOutCardData);

		//出牌界面
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(wOutViewChairID,pOutCard->cbOutCardData);

		//设置扑克
		if (wOutViewChairID==2)
		{
			//删除扑克
			BYTE cbCardData[MAX_COUNT];
			m_GameLogic.RemoveCard(m_cbCardIndex,pOutCard->cbOutCardData);

			//设置扑克
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,MAX_COUNT);
			//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
			SetHandCardData(cbCardData,cbCardCount,0);

		}
		else
		{
			WORD wUserIndex=(wOutViewChairID>2)?2:wOutViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
		}
	}

	return true;
}

//发牌消息
bool CGameClientEngine::OnSubSendCard(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) 		return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置变量
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃扑克
		WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);
		m_GameClientView.SetDiscUser(wOutViewChairID);

		//设置变量
		//m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//发牌处理
	if (pSendCard->cbCardData!=0)
	{
		//取牌界面
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		if (wViewChairID!=2)
		{
			WORD wUserIndex=(wViewChairID>2)?2:wViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(true);
		}
		else
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;
			//m_GameClientView.m_HandCardControl.SetCurrentCard(pSendCard->cbCardData);
			SetHandCurrentCard(pSendCard->cbCardData);
		}
		
		//扣除扑克
		DeductionTableCard(pSendCard->cbIsNotGang);
	}

	//当前用户
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		//激活框架
		ActiveGameFrame();

		//动作处理
		if (pSendCard->wActionMask!=WIK_NULL)
		{
			//获取变量
			BYTE cbActionCard=pSendCard->cbCardData;
			WORD wActionMask=pSendCard->wActionMask;

			//变量定义
			tagGangCardResult GangCardResult;
			ZeroMemory(&GangCardResult,sizeof(GangCardResult));

			//杠牌判断
			if ((wActionMask&WIK_GANG)!=0)
			{
				WORD wMeChairID=GetMeChairID();
				m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
			}

			//设置界面
			m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,wActionMask,GangCardResult);
		}
	}

	//出牌提示
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID),false);

	//更新界面
	m_GameClientView.RefreshGameView();

	//设置时间
	SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,TIME_OPERATE_CARD);

	return true;
}

//洗澡结果
bool CGameClientEngine::OnSubBatchResult(VOID *pBuffer,WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(CMD_S_BatchResult));
	if(wDataSize != sizeof(CMD_S_BatchResult)) return false;
	
	//变量定义
	CMD_S_BatchResult *pBatchResult=(CMD_S_BatchResult *)pBuffer;
	BYTE cbBatchResult[GAME_PLAYER];
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		cbBatchResult[SwitchViewChairID(i)]=pBatchResult->cbBatchResult[i];
	}

	//InsertSystemString(TEXT("洗澡开始"));
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString(TEXT("洗澡开始"));
	m_GameClientView.SetBatchResult(cbBatchResult);

	//是否有动画
	if(cbBatchResult[0]+cbBatchResult[1]+cbBatchResult[2]+cbBatchResult[3]>0)
		m_GameClientView.SetBatchEffect(true);
	return true;
}

//操作提示
bool CGameClientEngine::OnSubOperateNotify(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify))		return false;

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	//用户界面
	if ((IsLookonMode()==false)&&(pOperateNotify->wActionMask!=WIK_NULL))
	{
		//获取变量
		WORD wMeChairID=GetMeChairID();
		WORD wActionMask=pOperateNotify->wActionMask;
		BYTE cbActionCard=pOperateNotify->cbActionCard;

		//变量定义
		tagGangCardResult GangCardResult;
		ZeroMemory(&GangCardResult,sizeof(GangCardResult));

		//杠牌判断
		if ((wActionMask&WIK_GANG)!=0)
		{
			//桌面杆牌
			if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
			{
				GangCardResult.cbCardCount=1;
				GangCardResult.cbCardData[0]=cbActionCard;
			}

			//自己杆牌
			if ((m_wCurrentUser==wMeChairID)||(cbActionCard==0))
			{
				WORD wMeChairID=GetMeChairID();
				m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
			}
		}

		//设置界面
		ActiveGameFrame();
		SetGameClock(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD/3);
		m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,wActionMask,GangCardResult);
		CString str;
		str.Format(TEXT("%d"),wActionMask);
		//InsertSystemString(str);
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString(str);

	}

	return true;
}

//操作结果
bool CGameClientEngine::OnSubOperateResult(VOID * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;
	if(!m_bStart)
		OnGameStart(0,0);

	//变量定义
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard=pOperateResult->cbOperateCard;
	WORD wOperateViewID=SwitchViewChairID(wOperateUser);

	//出牌变量
	if (pOperateResult->wOperateCode!=WIK_NULL)
	{
		//m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//设置组合
	if ((pOperateResult->wOperateCode&WIK_GANG)!=0)
	{
		//设置变量
		m_wCurrentUser=INVALID_CHAIR;

		//组合扑克
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			WORD wWeaveKind=m_WeaveItemArray[wOperateUser][i].wWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(wWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wWeaveKind=pOperateResult->wOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
				break;
			}
		}

		//组合扑克
		if (cbWeaveIndex==0xFF)
		{
			//暗杠判断
			cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

			//设置扑克
			cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wWeaveKind=pOperateResult->wOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		}

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0};
		WORD wWeaveKind=pOperateResult->wOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(wWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDisplayItem((cbPublicCard==TRUE)?true:false);

		//扑克设置
		if (GetMeChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;
		}

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,MAX_COUNT);
			//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
			SetHandCardData(cbCardData,cbCardCount,0);

		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,false);
		}
	}
	else if (pOperateResult->wOperateCode!=WIK_NULL)
	{
		//设置变量
		m_wCurrentUser=pOperateResult->wOperateUser;

		//设置组合
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wWeaveKind=pOperateResult->wOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0};
		WORD wWeaveKind=pOperateResult->wOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(wWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);

		//删除扑克
		if (GetMeChairID()==wOperateUser)
		{
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
			m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		}

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,MAX_COUNT);
			//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]);
			SetHandCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]);
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,true);
		}
	}

	//设置界面
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserAction(wOperateViewID,pOperateResult->wOperateCode);
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);

	//更新界面
	m_GameClientView.RefreshGameView();

	//环境设置
	PlayActionSound(pOperateResult->wOperateUser,pOperateResult->wOperateCode);

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
	}

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//设置状态
	SetGameStatus(GS_MJ_FREE);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_DrawSiceWnd.ShowWindow(SW_HIDE);

	//设置界面
	m_GameClientView.m_btBatch.ShowWindow(SW_HIDE);
	
	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if (pGameEnd->dwChiHuKind[i]!=CHK_NULL)
			m_GameClientView.SetUserAction(wViewChairID,WIK_CHI_HU);
		m_GameClientView.m_TableCard[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);
	}
	//删除定时器
	KillGameClock(IDI_OPERATE_CARD);
	KillGameClock(IDI_BATCH);

	//设置控件
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	//变量定义
	tagScoreInfo ScoreInfo;
	tagWeaveInfo WeaveInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	ZeroMemory(&WeaveInfo,sizeof(WeaveInfo));

	//结束设置
	if (pGameEnd->cbChiHuCard!=0)
	{
			

		//成绩变量
		ScoreInfo.wBankerUser=m_wBankerUser;
		ScoreInfo.wProvideUser=pGameEnd->wProvideUser;
		ScoreInfo.cbProvideCard=pGameEnd->cbChiHuCard;

		//设置积分
		CString strTemp ,strEnd = TEXT("\n");


		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			IClientUserItem * pUserData=GetTableUserItem(i);
			//胡牌类型
			ScoreInfo.dwChiHuKind[i]=pGameEnd->dwChiHuKind[i];
			ScoreInfo.dwChiHuRight[i]=pGameEnd->dwChiHuRight[i];

			//设置成绩
			ScoreInfo.lGameScore[i]=pGameEnd->lGameScore[i];
			lstrcpyn(ScoreInfo.szUserName[i],pUserData->GetNickName(),CountArray(ScoreInfo.szUserName[i]));
			if(pGameEnd->lGameScore[i]>0)
				strTemp.Format(TEXT("%s: %+ld\n"),pUserData->GetNickName(),pGameEnd->lGameScore[i]);
			else
				strTemp.Format(TEXT("%s: %ld\n"),pUserData->GetNickName(),pGameEnd->lGameScore[i]);
			strEnd += strTemp;

			//胡牌扑克
			if ((ScoreInfo.cbCardCount==0)&&(pGameEnd->dwChiHuKind[i]!=CHK_NULL))
			{
				//组合扑克
				WeaveInfo.cbWeaveCount=m_cbWeaveCount[i];
				for (BYTE j=0;j<WeaveInfo.cbWeaveCount;j++)
				{
					WORD wWeaveKind=m_WeaveItemArray[i][j].wWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					WeaveInfo.cbPublicWeave[j]=m_WeaveItemArray[i][j].cbPublicCard;
					WeaveInfo.cbCardCount[j]=m_GameLogic.GetWeaveCard(wWeaveKind,cbCenterCard,WeaveInfo.cbCardData[j]);
				}

				//设置扑克
				ScoreInfo.cbCardCount=pGameEnd->cbCardCount[i];
				CopyMemory(ScoreInfo.cbCardData,&pGameEnd->cbCardData[i],ScoreInfo.cbCardCount*sizeof(BYTE));

				//提取胡牌
				for (BYTE j=0;j<ScoreInfo.cbCardCount;j++)
				{
					if (ScoreInfo.cbCardData[j]==pGameEnd->cbChiHuCard)
					{
						MoveMemory(&ScoreInfo.cbCardData[j],&ScoreInfo.cbCardData[j+1],(ScoreInfo.cbCardCount-j-1)*sizeof(BYTE));
						ScoreInfo.cbCardData[ScoreInfo.cbCardCount-1]=pGameEnd->cbChiHuCard;
						break;
					}
				}
			}
		}

		//消息积分
		//InsertSystemString((LPCTSTR)strEnd);
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString((LPCTSTR)strEnd);
		m_GameClientView.m_GameScoreWnd.SetScoreInfo(ScoreInfo,WeaveInfo);
	}
	else 
	{
		tagScoreInfo ScoreInfo;
		//成绩变量
		ScoreInfo.wBankerUser=m_wBankerUser;
		ScoreInfo.wProvideUser=pGameEnd->wProvideUser;
		ScoreInfo.cbProvideCard=pGameEnd->cbChiHuCard;

		//设置积分
		CString strTemp ,strEnd = TEXT("\n");


		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			IClientUserItem * pUserData=GetTableUserItem(i);
			//胡牌类型
			ScoreInfo.dwChiHuKind[i]=pGameEnd->dwChiHuKind[i];
			ScoreInfo.dwChiHuRight[i]=pGameEnd->dwChiHuRight[i];

			//设置成绩
			ScoreInfo.lGameScore[i]=pGameEnd->lGameScore[i];
			lstrcpyn(ScoreInfo.szUserName[i],pUserData->GetNickName(),CountArray(ScoreInfo.szUserName[i]));
			if(pGameEnd->lGameScore[i]>0)
				strTemp.Format(TEXT("%s: %+ld\n"),pUserData->GetNickName(),pGameEnd->lGameScore[i]);
			else
				strTemp.Format(TEXT("%s: %ld\n"),pUserData->GetNickName(),pGameEnd->lGameScore[i]);
			strEnd += strTemp;
		}

		//消息积分
		//InsertSystemString((LPCTSTR)strEnd);
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString((LPCTSTR)strEnd);

		m_GameClientView.m_GameScoreWnd.SetScoreInfo(ScoreInfo,WeaveInfo);
		//DeductionTableCard(true);
		m_GameClientView.SetHuangZhuang(true);
		//InsertSystemString("荒庄!");
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString(TEXT("荒庄!"));

	}



	//设置扑克
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	//m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);
	SetHandCardData(NULL,0,0);


	//播放声音
	LONG lScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lScore>0L)
	{

		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	}
	else if (lScore<0L)
	{

		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else 
	{

		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));
	}

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}


	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}
	//取消托管
	if(m_bStustee)
		OnStusteeControl(0,0);

	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_lAllTurnScore[wViewChairID]=pGameEnd->lAllTurnScore[i];
		m_GameClientView.m_lLastTurnScore[wViewChairID]=pGameEnd->lLastTurnScore[i];

	}

	//更新界面
	m_GameClientView.RefreshGameView();

	return true;
}

//播放声音
/*void CGameClientEngine::PlayCardSound(BYTE cbCardData,BYTE cbManderin)
{
	//变量定义
	TCHAR szSoundName[16]=TEXT("");
	TCHAR szKind[]={TEXT('W'),TEXT('S'),TEXT('T'),TEXT('F')};
	TCHAR szManderin[]={TEXT('P'),TEXT('G')};

	//构造名字
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	myprintf(szSoundName,CountArray(szSoundName),TEXT("%c%c_%d"),szManderin[m_cbManderin],szKind[cbColor],cbValue);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),szSoundName);

	return;
}*/

//播放声音
//播放声音
void CGameClientEngine::PlayActionSound(WORD wChairID,BYTE cbAction)
{
	//判断性别
	IClientUserItem* pUserData = GetTableUserItem(wChairID);
	if(pUserData == 0)
	{
		//assert(0 && "得不到玩家信息");
		return;
	}
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}
	bool bBoy = (pUserData->GetGender() == GENDER_FEMALE ? false : true);

	switch (cbAction)
	{
	case WIK_NULL:		//取消
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_OUT_CARD"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_OUT_CARD"));
			break;
		}
	case WIK_LEFT:
	case WIK_CENTER:
	case WIK_RIGHT:		//上牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_CHI"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_CHI"));		
			break;
		}
	case WIK_PENG:		//碰牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_PENG"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_PENG"));	
			break;
		}
	case WIK_GANG:		//杠牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_GANG"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_GANG"));		
			break;
		}
	case WIK_CHI_HU:	//吃胡
		{

			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_CHI_HU"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_CHI_HU"));		
			break;
		}
	}

	return;
}
//用户托管
bool CGameClientEngine::OnSubTrustee(VOID * pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	//消息处理
	CMD_S_Trustee * pTrustee=(CMD_S_Trustee *)pBuffer;
	m_GameClientView.SetTrustee(SwitchViewChairID(pTrustee->wChairID),pTrustee->bTrustee);
	if ((IsLookonMode()==true)||(pTrustee->wChairID!=GetMeChairID()))
	{
		IClientUserItem * pUserData=GetTableUserItem(pTrustee->wChairID);
		TCHAR szBuffer[256];
		if(pTrustee->bTrustee==true)
			myprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]选择了托管功能."),pUserData->GetNickName());
		else
			myprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]取消了托管功能."),pUserData->GetNickName());
		//InsertSystemString(szBuffer);

		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString(szBuffer);
	}
	return true;
}
//扣除扑克
void CGameClientEngine::DeductionTableCard(bool bHeadCard)
{
	if (bHeadCard==true)
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapHand][0]+m_cbHeapCardInfo[m_wHeapHand][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapHand=(m_wHeapHand+1)%CountArray(m_cbHeapCardInfo);

		//减少扑克
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapHand][0]++;

		//调过精牌
		if((m_wHeapHand == m_wKingHeapPos)
			&&(m_cbHeapCardInfo[m_wHeapHand][0] == (HEAP_FULL_COUNT/2-m_wKingPos+1)*2-1))
		{
			m_cbLeftCardCount--;
			m_cbHeapCardInfo[m_wHeapHand][0]++;
		}

		//堆立扑克
		WORD wHeapViewID=SwitchViewChairID(m_wHeapHand);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapHand][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapHand][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}
	else
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapTail][0]+m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapTail=(m_wHeapTail+3)%CountArray(m_cbHeapCardInfo);

		//减少扑克
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapTail][1]++;
		if((m_wHeapTail == m_wKingHeapPos) 
			&&(m_cbHeapCardInfo[m_wHeapTail][1] == (m_wKingPos*2-1)))
		{
			m_cbLeftCardCount--;
			m_cbHeapCardInfo[m_wHeapTail][1]++;
		}

		//堆立扑克
		WORD wHeapViewID=SwitchViewChairID(m_wHeapTail);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapTail][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapTail][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}

	return;
}

//显示控制
bool CGameClientEngine::ShowOperateControl(WORD wUserAction, BYTE cbActionCard)
{
	//变量定义
	tagGangCardResult GangCardResult;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//杠牌判断
	if ((wUserAction&WIK_GANG)!=0)
	{
		//桌面杆牌
		if (cbActionCard!=0)
		{
			GangCardResult.cbCardCount=1;
			GangCardResult.cbCardData[0]=cbActionCard;
		}

		//自己杆牌
		if (cbActionCard==0)
		{
			WORD wMeChairID=GetMeChairID();
			m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
		}
	}

	//显示界面
	m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,wUserAction,GangCardResult);

	return true;
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//环境设置
	KillGameClock(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);

	//设置界面
	m_GameClientView.SetDiscUser(INVALID_CHAIR);
	m_GameClientView.SetHuangZhuang(false);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);

	//扑克设置
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	//m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);
	SetHandCardData(NULL,0,0);
	m_GameClientView.SetKingCardIndex(m_cbKingCardIndex);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
		m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
	}

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?34:34);
		m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
	}

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//堆立扑克
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//发送消息
	SendUserReady(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HeapCard[i].SetKingCardData(0xFF);
		m_GameClientView.m_HeapCard[i].SetKingPos(WORD(-1));
		m_GameClientView.m_DiscardCard[i].SetKingCardIndex(m_cbKingCardIndex);
		m_GameClientView.m_TableCard[i].SetKingCardIndex(m_cbKingCardIndex);
		for (WORD j=0;j<MAX_WEAVE;j++)
		{
			m_GameClientView.m_WeaveCard[i][j].SetKingCardIndex(m_cbKingCardIndex);
		}
	}
	m_GameClientView.m_HandCardControl.SetKingCardIndex(m_cbKingCardIndex);

	return 0;
}

//出牌操作
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//出牌判断
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID()))	return 0;
	
	BYTE cbOutCardData=(BYTE)wParam;
	if(VerdictOutCard(cbOutCardData) == false)
	{
		return 0;
	}

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCardData);
	m_wUserAction =0;

	//设置扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,MAX_COUNT);
	//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
	SetHandCardData(cbCardData,cbCardCount,0);


	//设置界面
	KillGameClock(IDI_OPERATE_CARD);
	m_GameClientView.RefreshGameView();
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(2,cbOutCardData);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//播放声音
	PlayCardSound(GetMeChairID(),cbOutCardData);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCardData;
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));

	return 0;
}

//扑克操作
LRESULT CGameClientEngine::OnCardOperate(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	WORD wOperateCode=(WORD)(wParam);
	BYTE cbOperateCard=(BYTE)(LOWORD(lParam));

	//状态判断
	if ((m_wCurrentUser==GetMeChairID())&&(wOperateCode==WIK_NULL))
	{
		m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
		return 0;
	};

	//环境设置
	KillGameClock(IDI_OPERATE_CARD);
	m_GameClientView.SetStatusFlag(false,true);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.wOperateCode=wOperateCode;
	OperateCard.cbOperateCard=cbOperateCard;
	SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

	return 0;
}
//筛子动作显示完毕
LRESULT CGameClientEngine::OnSiceTwo(WPARAM wParam, LPARAM lParam)
{
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	return 0;
}
//游戏开始
LRESULT CGameClientEngine::OnGameStart(WPARAM wParam, LPARAM lParam)
{
	bool bPlayerMode=(IsLookonMode()==false);
	m_bStart=true;
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	m_GameClientView.m_DrawSiceWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
	m_GameClientView.SetKingCardIndex(m_cbKingCardIndex);
	m_GameClientView.m_HandCardControl.SetKingCardIndex(m_cbKingCardIndex);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HeapCard[i].SetKingCardData(m_GameLogic.SwitchToCardData(m_cbKingCardIndex[0]));
		m_GameClientView.m_DiscardCard[i].SetKingCardIndex(m_cbKingCardIndex);
		m_GameClientView.m_TableCard[i].SetKingCardIndex(m_cbKingCardIndex);
		for (WORD j=0;j<MAX_WEAVE;j++)
		{
			m_GameClientView.m_WeaveCard[i][j].SetKingCardIndex(m_cbKingCardIndex);
		}
	}
	//堆牌精牌
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(i== m_wKingHeapPos)
		{
			m_GameClientView.m_HeapCard[SwitchViewChairID(m_wKingHeapPos)].SetKingCardData(m_GameLogic.SwitchToCardData(m_cbKingCardIndex[0]));
			m_GameClientView.m_HeapCard[SwitchViewChairID(m_wKingHeapPos)].SetKingPos(m_wKingPos);
		}
		else
		{
			m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetKingCardData(0XFF);
			m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetKingPos(WORD(-1));

		}
	}


	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		//组合界面
		m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
		m_GameClientView.m_HeapCard[wViewChairID].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);

		//用户扑克
		if (wViewChairID!=2)
		{
			WORD wIndex=(wViewChairID>=3)?2:wViewChairID;
			m_GameClientView.m_UserCard[wIndex].SetCardData(CountArray(m_cbCardData)-1,(i==m_wBankerUser));
		}
		else
		{
			BYTE cbBankerCard=(i==m_wBankerUser)?m_cbCardData[MAX_COUNT-1]:0;
			//m_GameClientView.m_HandCardControl.SetCardData(m_cbCardData,MAX_COUNT-1,cbBankerCard);
			SetHandCardData(m_cbCardData,MAX_COUNT-1,cbBankerCard);
		}

		//旁观界面
		if (bPlayerMode==false)
		{
			m_GameClientView.m_TableCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][0].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][1].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][2].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][3].SetCardData(NULL,0);
		}
	}

	//出牌提示
	if ((bPlayerMode==true)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		WORD wMeChairID=GetMeChairID();
		if (m_wCurrentUser==wMeChairID) 
			m_GameClientView.SetStatusFlag(true,false);
	}

	//动作处理
	if ((bPlayerMode==true)&&(m_wUserAction!=WIK_NULL))
	{
		ShowOperateControl(m_wUserAction,0);
		SetGameClock(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD/3);
	}
	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR)
		SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
	return 0;
}

//洗澡动画完毕
LRESULT CGameClientEngine::OnBatchEnd(WPARAM wParam,LPARAM lParam)
{
	//正常用户
	if(IsLookonMode()==false)
	{
		SendSocketData(SUB_C_BATCH_END);
	}
	
	return 0;
}

//手牌信息
void CGameClientEngine::SetHandCardData(BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard)
{

	m_GameClientView.m_HandCardControl.SetCardData(cbCardData,wCardCount, cbCurrentCard);

}

//手牌之当前牌
void CGameClientEngine::SetHandCurrentCard(BYTE cbCurrentCard)
{

	m_GameClientView.m_HandCardControl.SetCurrentCard(cbCurrentCard);
}

//出牌校验
bool CGameClientEngine::VerdictOutCard(BYTE cbPreCardData)
{

	return true;
}
//买底分数
LRESULT CGameClientEngine::OnBatch(WPARAM wParam,LPARAM lParam)
{	
	//删除定时器
	KillGameClock(IDI_BATCH);

	//设置界面
	m_GameClientView.m_btBatch.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_BATCH Chiper;
	Chiper.cbChipTimes=(BYTE)wParam;
	Chiper.wCurrentUser=GetMeChairID();
	if(Chiper.cbChipTimes==1)
		PlayGameSound(AfxGetInstanceHandle(),TEXT("ONE_SCORE"));

	
	SendSocketData(SUB_C_BATCH,&Chiper,sizeof(Chiper));
	//InsertSystemString(TEXT("OK"));

	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString(TEXT("OK"));
	return 0;
}

//游戏洗澡
bool CGameClientEngine::OnSubGameChip(VOID *pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==0);
	if (wDataSize!=0) return false;

	//设置状态
	SetGameStatus(GS_MJ_BATCH);

	//玩家设置
	 if(IsLookonMode()==false)
	 {
		 m_GameClientView.m_btBatch.ShowWindow(SW_SHOW);

	 }
	 //设置定时期
	 SetGameClock(GetMeChairID(),IDI_BATCH,TIME_BATCH);
	 return true;
}
//拖管控制
LRESULT CGameClientEngine::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wTimeOutCount=0;

	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);
		//InsertSystemString(_T("您取消了托管功能."));
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString(TEXT("您取消了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = false;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));

	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_STOP_TRUSTEE,AfxGetInstanceHandle(),false,false);
		//InsertSystemString(_T("您选择了托管功能."));
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString(TEXT("您选择了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = true;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));

	}

	return 0;
}

//播放出牌声音
void CGameClientEngine::PlayCardSound(WORD wChairID, BYTE cbCardData)
{
	if(m_GameLogic.IsValidCard(cbCardData) == false)
	{
		return;
	}
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}

	//判断性别
	IClientUserItem* pUserData = GetTableUserItem(wChairID);
	if(pUserData == 0)
	{
		assert(0 && "得不到玩家信息");
		return;
	}
	bool bBoy = (pUserData->GetGender() == GENDER_FEMALE ? false : true);
	BYTE cbType= (cbCardData & MASK_COLOR);
	BYTE cbValue= (cbCardData & MASK_VALUE);
	CString strSoundName;
	switch(cbType)
	{
	case 0X30:	//风
		{
			switch(cbValue) 
			{
			case 1:
				{
					strSoundName = _T("F_1");
					break;
				}
			case 2:
				{
					strSoundName = _T("F_2");
					break;
				}
			case 3:
				{
					strSoundName = _T("F_3");
					break;
				}
			case 4:
				{
					strSoundName = _T("F_4");
					break;
				}
			case 5:
				{
					strSoundName = _T("F_5");
					break;
				}
			case 6:
				{
					strSoundName = _T("F_6");
					break;
				}
			case 7:
				{
					strSoundName = _T("F_7");
					break;
				}
			default:
				{
					strSoundName=_T("BU_HUA");
				}

			}
			break;
		}		
	case 0X20:	//筒
		{
			strSoundName.Format(_T("T_%d"), cbValue);
			break;
		}

	case 0X10:	//索
		{
			strSoundName.Format(_T("S_%d"), cbValue);
			break;
		}
	case 0X00:	//万
		{
			strSoundName.Format(_T("W_%d"), cbValue);
			break;
		}
	}	

	if(bBoy)
	{
		strSoundName = _T("BOY_") +strSoundName;
	}
	else
	{
		strSoundName = _T("GIRL_") + strSoundName;
	}
	PlayGameSound(AfxGetInstanceHandle(), strSoundName);
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
