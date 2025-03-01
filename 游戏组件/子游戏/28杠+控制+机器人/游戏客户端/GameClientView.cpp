#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识
#define IDC_JETTON_BUTTON_10000000   199
#define IDC_JETTON_BUTTON_100		200									//按钮标识
#define IDC_JETTON_BUTTON_1000		201									//按钮标识
#define IDC_JETTON_BUTTON_10000		202									//按钮标识
#define IDC_JETTON_BUTTON_100000	203									//按钮标识
#define IDC_JETTON_BUTTON_1000000	204									//按钮标识
#define IDC_JETTON_BUTTON_5000000	205									//按钮标识
#define IDC_APPY_BANKER				206									//按钮标识
#define IDC_CANCEL_BANKER			207									//按钮标识
#define IDC_SCORE_MOVE_L			209									//按钮标识
#define IDC_SCORE_MOVE_R			210									//按钮标识
#define IDC_VIEW_CHART				211									//按钮标识
#define IDC_JETTON_BUTTON_500000	212									//按钮标识
#define IDC_AUTO_OPEN_CARD			213									//按钮标识
#define IDC_OPEN_CARD				214									//按钮标识
#define IDC_BANK					215									//按钮标识
#define IDC_CONTINUE_CARD			216									//按钮标识
#define IDC_BANK_STORAGE			217									//按钮标识
#define IDC_BANK_DRAW				218									//按钮标识
#define IDC_CONTROL_BUTTON			230									//按钮标识
#define  IDC_SOUND_CLOSE			231
#define  IDC_SOUND_OPEN				232

#define IDC_UP						219									//按钮标识
#define IDC_DOWN					220								    //按钮标识
#define IDC_BT_ADMIN				230                                 //按钮标识
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()

	ON_BN_CLICKED(IDC_BT_ADMIN,OpenAdminWnd)
	ON_BN_CLICKED(IDC_APPY_BANKER, OnApplyBanker)
	ON_BN_CLICKED(IDC_CANCEL_BANKER, OnCancelBanker)
	ON_BN_CLICKED(IDC_SCORE_MOVE_L, OnScoreMoveL)
	ON_BN_CLICKED(IDC_SCORE_MOVE_R, OnScoreMoveR)
	ON_BN_CLICKED(IDC_OPEN_CARD, OnOpenCard)
	ON_BN_CLICKED(IDC_AUTO_OPEN_CARD,OnAutoOpenCard)
	ON_BN_CLICKED(IDC_SOUND_CLOSE, OnOpenSound)
	ON_BN_CLICKED(IDC_SOUND_OPEN ,OnCloseSound)

	ON_MESSAGE(WM_VIEWLBTUP,OnViLBtUp)
	ON_BN_CLICKED(IDC_CONTROL_BUTTON, OnControl)

	ON_BN_CLICKED(IDC_BANK_STORAGE, OnBankStorage)
	ON_BN_CLICKED(IDC_BANK_DRAW, OnBankDraw)

    ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)


END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView() 
{
	//下注信息
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//全体下注
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//庄家信息
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lTmpBankerWinScore=0;

	//当局成绩
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//状态信息
	m_lCurrentJetton=0L;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bWinShunMen=false;
	m_bWinDuiMen=false;
	m_bWinDaoMen=false;
	m_bFlashResult=false;
	m_blMoveFinish = false;
	m_blAutoOpenCard = true;
	m_blCanStore=FALSE;
	m_enDispatchCardTip=enDispatchCardTip_NULL;

	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		

	m_lAreaLimitScore=0L;	

	//位置信息
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	


	//历史成绩
	m_lMeStatisticScore=0;

	//控件变量
	m_pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();

	m_btImageJetton1000	.LoadFromResource(this, hInstance, IDB_BT_JETTON_1000);				
	m_btImageJetton10000.LoadFromResource(this, hInstance,IDB_BT_JETTON_10000);				
	m_btImageJetton100000.LoadFromResource(this, hInstance,IDB_BT_JETTON_100000);				
	m_btImageJetton500000.LoadFromResource(this, hInstance,IDB_BT_JETTON_500000);				
	m_btImageJetton1000000.LoadFromResource(this, hInstance,IDB_BT_JETTON_1000000);				
	m_btImageJetton5000000.LoadFromResource(this, hInstance,IDB_BT_JETTON_5000000);				
	m_btImageJetton10000000	.LoadFromResource(this, hInstance,IDB_BT_JETTON_10000000);				

	m_ImageViewFill.LoadFromResource(this, hInstance,IDB_VIEW_FILL);
	m_ImageViewBack.LoadFromResource(this, hInstance,IDB_VIEW_BACK);
	m_ImageWinFlags.LoadFromResource(this, hInstance,IDB_WIN_FLAGS);
	m_ImageJettonView.LoadFromResource(this, hInstance,IDB_JETTOM_VIEW);
	m_ImageScoreNumber.LoadFromResource(this, hInstance,IDB_SCORE_NUMBER);
	m_ImageMeScoreNumber.LoadFromResource(this, hInstance,IDB_ME_SCORE_NUMBER);

	m_TotoalPress.n_Png_tatoalflag.LoadImage(hInstance,TEXT("IDB_IDB_TIPP_PNG"));
	m_TotoalPress.n_Png_tipC.LoadImage(hInstance,TEXT("IDB_IDB_TIPC_PNG"));
	m_TotoalPress.n_Png_tipN.LoadImage(hInstance,TEXT("IDB_IDB_TIPN_PNG"));
	m_PngScoreNumber.LoadImage(hInstance,TEXT("IDB_IDB_TIPN_PNG"));
	m_ImageTimer.LoadImage(hInstance,TEXT("IDB_IDB_CLOCK_PNG"));

	//边框资源
	m_ImageFrameShunMen.LoadFromResource(this, hInstance,IDB_FRAME_SHUN_MEN_EX);
	m_ImageFrameJiaoL.LoadFromResource(this, hInstance,IDB_FRAME_JIAO_L_EX);
	m_ImageFrameQiao.LoadFromResource(this, hInstance,IDB_FRAME_QIAO_EX);
	m_ImageFrameDuiMen.LoadFromResource(this, hInstance,IDB_FRAME_DUI_MEN_EX);
	m_ImageFrameDaoMen.LoadFromResource(this, hInstance,IDB_FRAME_DAO_EMN_EX);
	m_ImageFrameJiaoR.LoadFromResource(this, hInstance,IDB_FRAME_JIAO_R_EX);

	m_ImageGameEnd.LoadFromResource(this, hInstance, IDB_GAME_END);

	m_ImageMeBanker.LoadFromResource(this, hInstance, IDB_ME_BANKER);
	m_ImageTimerNumber.LoadFromResource(this, hInstance, IDB_TIMERNUMBER);

	m_ImageChangeBanker.LoadFromResource(this, hInstance, IDB_CHANGE_BANKER);
	m_ImageNoBanker.LoadFromResource(this, hInstance, IDB_NO_BANKER);	

	m_ImageTimeFlag.LoadFromResource(this, hInstance,IDB_TIME_FLAG);

	
	//清理数组
	for (BYTE i=0;i<CountArray(m_JettonInfoArray);i++)
	{
		m_JettonInfoArray[i].RemoveAll();
	}

	/*m_cbTableCardArray[BANKER_INDEX][0]=0x0B;
	m_cbTableCardArray[BANKER_INDEX][1]=0x14;
	m_cbTableCardArray[SHUN_MEN_INDEX][0]=0x0B;
	m_cbTableCardArray[SHUN_MEN_INDEX][1]=0x2B;
 	bool bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;*/

	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
	if(m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}
void CGameClientView::SetMoveCardTimer()
{
	KillTimer(IDI_POSTCARD);
	m_OpenCardIndex = 1;
	m_bcShowCount = 0;
	m_PostStartIndex =  m_GameLogic.GetCardValue(m_bcfirstShowCard);
	if(m_PostStartIndex>1)
	m_PostStartIndex  = (m_PostStartIndex-1)%4;
	m_PostCardIndex=m_PostStartIndex;
	for (int i = 0;i<4;i++)
	{
		m_CardControl[i].m_blAdd = true;
		m_CardControl[i].m_blAuto = m_blAutoOpenCard;
		m_CardControl[i].m_blGameEnd = true;	
	}
	SetTimer(IDI_POSTCARD,500,NULL);
}

//接收场景时结束发牌
void CGameClientView::SetFinishDiscard()
{
	//先拷贝牌
	while (1)
	{
		bool blCopy = false;
		for (int i = 0; i < 4; i++)
		{
			if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
			{
				blCopy = true;
				break;
			}
		}
		if(false == blCopy)
		{
			break;
		}
		else
		{
			m_CardControl[i].OnCopyCard();
		}
	}
	OnTimer(IDI_DISPATCH_CARD);
}

 void CGameClientView::OnOpenSound()
{
	SendEngineMessage(IDM_OPEN_SOUND,0,0);

}
 void CGameClientView::OnCloseSound()
{
	SendEngineMessage(IDM_CLOSE_SOUND,0,0);

}
//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
	m_ApplyUser.Create( IDD_DLG_GAME_RECORD	, this );
	m_GameRecord.ShowWindow(SW_HIDE);

	//下注按钮
	m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
	m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
	m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
	m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);
	m_btJetton500000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);
	m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
	m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);
	m_btJetton10000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000000);
	
	//申请按钮
	m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
	m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

	m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
	m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

	m_btAutoOpenCard.Create(NULL,WS_CHILD|WS_DISABLED ,rcCreate,this,IDC_AUTO_OPEN_CARD);
	m_btOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPEN_CARD);

	m_btBank.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK);
	m_btContinueCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_CONTINUE_CARD);

	m_btBankerStorage.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_BANK_STORAGE);
	m_btBankerDraw.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK_DRAW);

	m_btSoundOpen.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND_OPEN);
	m_btSoundClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND_CLOSE);

	m_btUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_UP);
	m_btDown.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_DOWN);

	m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_BT_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);

	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btJetton100.SetButtonImage(IDB_BT_JETTON_100,hResInstance,false, false);
	m_btJetton1000.SetButtonImage(IDB_BT_JETTON_1000,hResInstance,false, false);
	m_btJetton10000.SetButtonImage(IDB_BT_JETTON_10000,hResInstance,false, false);
	m_btJetton100000.SetButtonImage(IDB_BT_JETTON_100000,hResInstance,false, false);
	m_btJetton500000.SetButtonImage(IDB_BT_JETTON_500000,hResInstance,false, false);
	m_btJetton1000000.SetButtonImage(IDB_BT_JETTON_1000000,hResInstance,false, false);
	m_btJetton5000000.SetButtonImage(IDB_BT_JETTON_5000000,hResInstance,false, false);
	m_btJetton10000000.SetButtonImage(IDB_BT_JETTON_10000000,hResInstance,false, false);

	m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false, false);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false, false);

	m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false, false);
	m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false, false);

	//m_btAutoOpenCard.SetButtonImage(IDB_BT_AUTO_OPEN_CARD,hResInstance,false, false);
	//m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hResInstance,false, false);

	m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false, false);

	m_btContinueCard.SetButtonImage(IDB_BT_CONTINUE_CARD,hResInstance,false, false);

	m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false, false);
	m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false, false);

	m_btSoundOpen.SetButtonImage(IDB_OPENSOUND,hResInstance,false, false);
	m_btSoundClose.SetButtonImage(IDB_CLOSESOUND,hResInstance,false, false);

	m_btAutoOpenCard.SetButtonImage(IDB_BT_AUTO_OPEN_CARD,hResInstance,false, false);
	m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hResInstance,false, false);

	m_btUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false, false);
	m_btDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false, false);

	//扑克控件
	for (int i=0; i<CountArray(m_CardControl); ++i) 
	{
			m_CardControl[i].SetDisplayFlag(true);
	}
	SetTimer(IDI_CHECK_TIMER,1000,NULL);

	//控制
	m_hInst = NULL;
	m_pAdminControl = NULL;
	m_hInst = LoadLibrary(TEXT("NineXiaoBattleClientControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pAdminControl = static_cast<IClientControlDlg*>(ClientControl(this));
			m_pAdminControl->ShowWindow( SW_HIDE );
		}
	}

	
	return 0;
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//下注信息
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//全体下注
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//庄家信息
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lTmpBankerWinScore=0;

	//当局成绩
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//状态信息
	m_lCurrentJetton=0L;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bWinShunMen=false;
	m_bWinDuiMen=false;
	m_bWinDaoMen=false;
	m_bFlashResult=false;
	m_bShowGameResult=false;
	m_enDispatchCardTip=enDispatchCardTip_NULL;

	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		

	m_lAreaLimitScore=0L;	

	//位置信息
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//历史成绩
	m_lMeStatisticScore=0;

	//清空列表
	m_ApplyUser.ClearAll();
	m_btUp.ShowWindow(SW_HIDE);
	m_btUp.EnableWindow(true);
	m_btDown.ShowWindow(SW_HIDE);
	m_btDown.EnableWindow(true);

	//清除桌面
	CleanUserJetton();

	//设置按钮
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	m_btCancelBanker.ShowWindow(SW_HIDE);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false, false);

	return;
}

void CGameClientView::OnUp()
{
	m_ApplyUser.m_AppyUserList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
	double nPos = m_ApplyUser.m_AppyUserList.GetScrollPos(SB_VERT);
	double nMax = m_ApplyUser.m_AppyUserList.GetScrollLimit(SB_VERT);

}

void CGameClientView::OnDown()
{
	double nPos = m_ApplyUser.m_AppyUserList.GetScrollPos(SB_VERT);
	if(nPos>m_ApplyUser.m_AppyUserList.GetItemCount()-3)
	{
		return ;
	}
	
	m_ApplyUser.m_AppyUserList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
	double nMax = m_ApplyUser.m_AppyUserList.GetScrollLimit(SB_VERT);
};

//管理员控制
void CGameClientView::OpenAdminWnd()
{
	//如果有此权限
	CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;
	if(CUserRight::IsGameCheatUser(pIClientKernel->GetUserAttribute()->dwUserRight))
	{
		//有权限
		if(m_pAdminControl!=NULL)
		{
			if(!m_pAdminControl->IsWindowVisible()) 
				m_pAdminControl->ShowWindow(SW_SHOW);
			else 
				m_pAdminControl->ShowWindow(SW_HIDE);
		}
	}
}

//调整控件
VOID CGameClientView::RectifyControl(int nWidth, int nHeight)
{
	//位置信息
	m_nWinFlagsExcursionX = nWidth/2-260+55;	
	m_nWinFlagsExcursionY = nHeight/2 + 246;

	//区域位置
	int nCenterX=nWidth/2, nCenterY=nHeight/2, nBorderWidth=4;
	m_rcShunMen.left=nCenterX - 345-2;
	m_rcShunMen.top=nCenterY-230;
	m_rcShunMen.right=m_rcShunMen.left+200;
	m_rcShunMen.bottom=m_rcShunMen.top+306;

	m_rcJiaoL.left=m_rcShunMen.left;
	m_rcJiaoL.top=m_rcShunMen.bottom-nBorderWidth;
	m_rcJiaoL.right=m_rcShunMen.right;
	m_rcJiaoL.bottom=m_rcJiaoL.top+92;

	m_rcQiao.left=m_rcShunMen.right-nBorderWidth;
	m_rcQiao.top=m_rcShunMen.top;
	m_rcQiao.right=m_rcQiao.left+175;
	m_rcQiao.bottom=m_rcQiao.top+93;

	m_rcDuiMen.left=nCenterX-145-1;
	m_rcDuiMen.top=nCenterY-132-4;
	m_rcDuiMen.right=m_rcDuiMen.left+145*2;
	m_rcDuiMen.bottom=m_rcDuiMen.top+132*2;

	m_rcDaoMen.left=m_rcDuiMen.right-3;
	m_rcDaoMen.top=m_rcDuiMen.top-100+4;
	m_rcDaoMen.right=m_rcDaoMen.left+197-1;
	m_rcDaoMen.bottom=m_rcDaoMen.top+308;

	m_rcJiaoR.left=m_rcDaoMen.left;
	m_rcJiaoR.top=m_rcDaoMen.bottom-nBorderWidth;
	m_rcJiaoR.right=m_rcDaoMen.right;
	m_rcJiaoR.bottom=m_rcDuiMen.bottom;
	
	//筹码数字
	int ExcursionY=60;
	m_PointJettonNumber[ID_SHUN_MEN-1].SetPoint((m_rcShunMen.right+m_rcShunMen.left)/2, (m_rcShunMen.bottom+m_rcShunMen.top)/2-ExcursionY-40);
	m_PointJettonNumber[ID_JIAO_L-1].SetPoint((m_rcJiaoL.right+m_rcJiaoL.left)/2, (m_rcJiaoL.bottom+m_rcJiaoL.top)/2-ExcursionY);
	m_PointJettonNumber[ID_QIAO-1].SetPoint((m_rcQiao.right+m_rcQiao.left)/2, (m_rcQiao.bottom+m_rcQiao.top)/2-ExcursionY);
	m_PointJettonNumber[ID_DUI_MEN-1].SetPoint((m_rcDuiMen.right+m_rcDuiMen.left)/2, (m_rcDuiMen.bottom+m_rcDuiMen.top)/2-ExcursionY);
	m_PointJettonNumber[ID_DAO_MEN-1].SetPoint((m_rcDaoMen.right+m_rcDaoMen.left)/2, (m_rcDaoMen.bottom+m_rcDaoMen.top)/2-ExcursionY-40);
	m_PointJettonNumber[ID_JIAO_R-1].SetPoint((m_rcJiaoR.right+m_rcJiaoR.left)/2, (m_rcJiaoR.bottom+m_rcJiaoR.top)/2-ExcursionY);
	
	//筹码位置
	m_PointJetton[ID_SHUN_MEN-1].SetPoint(m_rcShunMen.left, m_rcShunMen.top);
	m_PointJetton[ID_JIAO_L-1].SetPoint(m_rcJiaoL.left, m_rcJiaoL.top);
	m_PointJetton[ID_QIAO-1].SetPoint(m_rcQiao.left, m_rcQiao.top);
	m_PointJetton[ID_DUI_MEN-1].SetPoint(m_rcDuiMen.left, m_rcDuiMen.top);
	m_PointJetton[ID_DAO_MEN-1].SetPoint(m_rcDaoMen.left, m_rcDaoMen.top);
	m_PointJetton[ID_JIAO_R-1].SetPoint(m_rcJiaoR.left, m_rcJiaoR.top);

	//扑克控件
	m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2,nHeight/2-355),enXCenter,enYTop);
	m_CardControl[1].SetBenchmarkPos(CPoint(nWidth/2-360,nHeight/2-152),enXLeft,enYTop);
	m_CardControl[2].SetBenchmarkPos(CPoint(nWidth/2,nHeight/2+30),enXCenter,enYTop);
	m_CardControl[3].SetBenchmarkPos(CPoint(nWidth/2+270-30,nHeight/2-152),enXLeft,enYTop);
	m_CardControl[4].SetBenchmarkPos(CPoint(nWidth/2,nHeight/2-152),enXCenter,enYTop);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(34);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	m_ApplyUser.m_viewHandle = m_hWnd;

	//列表控件
	DeferWindowPos(hDwp,m_ApplyUser,NULL,nWidth/2 + 148-10,nHeight/2-291-7,216,50,uFlags);
	DeferWindowPos(hDwp,m_btUp,NULL,nWidth/2 + 138+216-3,nHeight/2-291-9,75/5,16,uFlags);
	DeferWindowPos(hDwp,m_btDown,NULL,nWidth/2 + 138+216-3,nHeight/2-291+50-16-7,75/5,16,uFlags);
	if(m_ApplyUser.GetItemCount()>4)
	{
		m_btUp.ShowWindow(SW_SHOW);
		m_btUp.EnableWindow(true);
		m_btDown.ShowWindow(SW_SHOW);
		m_btDown.EnableWindow(true);  
	}
	else
	{
		m_btUp.ShowWindow(SW_HIDE);
		m_btUp.EnableWindow(true);
		m_btDown.ShowWindow(SW_HIDE);
		m_btDown.EnableWindow(true); 

		m_ApplyUser.m_AppyUserList.SendMessage(WM_VSCROLL, MAKELONG(SB_TOP,0),NULL);
		m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
	}
	
	//筹码按钮
	CRect rcJetton;
	m_btJetton100.GetWindowRect(&rcJetton);
	int nYPos = nHeight/2+175;
	int nXPos = nWidth/2-64-150-1+15;
	int nSpace = 3;

	//DeferWindowPos(hDwp,m_btJetton100,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton1000,NULL,nXPos ,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton10000,NULL,nXPos + nSpace * 1 + rcJetton.Width() * 1,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton100000,NULL,nXPos + nSpace * 2 + rcJetton.Width() * 2,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton500000,NULL,nXPos + nSpace * 3 + rcJetton.Width() * 3,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton1000000,NULL,nXPos + nSpace * 4 + rcJetton.Width() * 4,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton5000000,NULL,nXPos + nSpace * 5 + rcJetton.Width() * 5,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton10000000,NULL,nXPos + nSpace * 6 + rcJetton.Width() * 6,nYPos,0,0,uFlags|SWP_NOSIZE);

	m_btJetton100.ShowWindow(SW_HIDE);
	

	//上庄按钮
	DeferWindowPos(hDwp,m_btApplyBanker,NULL,nWidth/2+277-10,nHeight/2-340,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btCancelBanker,NULL,nWidth/2+277-10,nHeight/2-340,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btScoreMoveL,NULL,nWidth/2-291+54,nHeight/2+262-20,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btScoreMoveR,NULL,nWidth/2+334+4,nHeight/2+260-18,0,0,uFlags|SWP_NOSIZE);

	//开牌按钮
	DeferWindowPos(hDwp,m_btAutoOpenCard,NULL,nWidth/2+170-25+89-20+3,nHeight/2+143-65+92-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth/2+170-25+89-20+3,nHeight/2+143-65+92-5,0,0,uFlags|SWP_NOSIZE);

	//其他按钮
	DeferWindowPos(hDwp,m_btBank,NULL,nWidth/2-345,nHeight/2+143,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btContinueCard,NULL,nWidth/2-220-96,nHeight/2+143-48,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btSoundOpen,NULL,nWidth/2+170-25+89-20+75,nHeight/2+143-65+92-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btSoundClose,NULL,nWidth/2+170-25+89-20+75,nHeight/2+143-65+92-5,0,0,uFlags|SWP_NOSIZE);

	//其他按钮
	DeferWindowPos(hDwp,m_btBankerStorage,NULL,nWidth/2+170-25+89-20+75,nHeight/2+143-65+92+30,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBankerDraw,NULL,nWidth/2+170-25+89-20+3,nHeight/2+143-65+92+30,0,0,uFlags|SWP_NOSIZE);

	//结束移动
	EndDeferWindowPos(hDwp);

	return;
}

LONGLONG CGameClientView::GetBankMaxJetton()
{
	LONGLONG lBankerScore=92147483647L;
	if(m_wBankerUser!=INVALID_CHAIR) lBankerScore=m_lBankerScore;
	if(lBankerScore > 99999999999L) lBankerScore = 99999999999L;
	return lBankerScore;

}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);

	//获取状态
	BYTE cbGameStatus=m_pGameClientDlg->GetGameStatus();

	if(cbGameStatus == GS_PLACE_JETTON)
	{
		LONGLONG allJetton = 0;

		for (int i = 1;i<AREA_ARRY_COUNT;i++)
		{
			allJetton+=m_lAllJettonScore[i];
		}

		//DrawJettonPress(pDC,nWidth,nHeight,m_lBankerScore-allJetton,allJetton);
		DrawJettonPress(pDC,nWidth,nHeight,GetBankMaxJetton(),allJetton);
	}

	//状态提示
	CFont static InfoFont;
	InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	CFont * pOldFont=pDC->SelectObject(&InfoFont);
	pDC->SetTextColor(RGB(255,234,0));
	
	pDC->SelectObject(pOldFont);
	InfoFont.DeleteObject();

	//时间提示
	////CImageHandle ImageHandleTimeFlag(&m_ImageTimeFlag);
	int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
	int nFlagIndex=0;
	if (cbGameStatus==GAME_STATUS_FREE) nFlagIndex=0;
	else if (cbGameStatus==GS_PLACE_JETTON) nFlagIndex=1;
	else if (cbGameStatus==GS_GAME_END) nFlagIndex=2;

	m_ImageTimeFlag.TransDrawImage(pDC, nWidth/2-348+215, nHeight/2+225-495, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
		nFlagIndex * nTimeFlagWidth, 0,RGB(255,0,255));

	//Dlg里面的SetGameClock(GetMeChairID()）里面调用的SwitchViewChairID在桌子人数大于8时实际是ViewID转化为0了
	WORD wUserTimer=GetUserClock(m_wMeChairID);
	if (wUserTimer!=0) 
	{
		DrawUserClock(pDC,nWidth/2-323+226,nHeight/2+197-458-40,wUserTimer);
	}

	//胜利边框
	FlashJettonAreaFrame(nWidth,nHeight,pDC);

	//筹码资源n
	////CImageHandle HandleJettonView(&m_ImageJettonView);
	CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/7,m_ImageJettonView.GetHeight());

	//绘画筹码
	for (INT i=0;i<AREA_COUNT;i++)
	{
		//变量定义
		LONGLONG lScoreCount=0L;
		LONGLONG static lScoreJetton[JETTON_COUNT]={1000L,10000L,100000L,500000L,1000000L,5000000L,10000000L};
		int static nJettonViewIndex=0;

		//绘画筹码
		for (INT_PTR j=0;j<m_JettonInfoArray[i].GetCount();j++)
		{
			//获取信息
			tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

			//累计数字
			ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
			lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];

			//图片索引
		/*	if (1==pJettonInfo->cbJettonIndex || 2==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex+1;
			else if (0==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex;
			else nJettonViewIndex=pJettonInfo->cbJettonIndex+2;*/

			nJettonViewIndex=pJettonInfo->cbJettonIndex;

			//绘画界面
			m_ImageJettonView.TransDrawImage(pDC,pJettonInfo->nXPos+m_PointJetton[i].x+16,
				pJettonInfo->nYPos+m_PointJetton[i].y+16,SizeJettonItem.cx,SizeJettonItem.cy,
				nJettonViewIndex*SizeJettonItem.cx,0,RGB(255,0,255));
		}

		//绘画数字
		if (lScoreCount>0L)	DrawNumberString(pDC,lScoreCount,m_PointJettonNumber[i].x,m_PointJettonNumber[i].y);
	}

	//绘画庄家
	DrawBankerInfo(pDC,nWidth,nHeight);

	//绘画用户
	DrawMeInfo(pDC,nWidth,nHeight);

	//切换庄家
	if ( m_bShowChangeBanker )
	{
		int	nXPos = nWidth / 2 - 130+20;
		int	nYPos = nHeight / 2 - 160+20;

		//由我做庄
		if ( m_wMeChairID == m_wBankerUser )
		{
			////CImageHandle ImageHandleBanker(&m_ImageMeBanker);
			m_ImageMeBanker.TransDrawImage(pDC, nXPos, nYPos, m_ImageMeBanker.GetWidth(), m_ImageMeBanker.GetHeight(),
				0, 0,RGB(255,0,255));
		}
		else if ( m_wBankerUser != INVALID_CHAIR )
		{
			//CImageHandle ImageHandleBanker(&m_ImageChangeBanker);
			m_ImageChangeBanker.TransDrawImage(pDC, nXPos, nYPos, m_ImageChangeBanker.GetWidth(), m_ImageChangeBanker.GetHeight(),
				0, 0,RGB(255,0,255));
		}
		else
		{
			//CImageHandle ImageHandleBanker(&m_ImageNoBanker);
			m_ImageNoBanker.TransDrawImage(pDC, nXPos, nYPos, m_ImageNoBanker.GetWidth(), m_ImageNoBanker.GetHeight(),
				0, 0,RGB(255,0,255));
		}
	}

    //发牌提示
	if (enDispatchCardTip_NULL!=m_enDispatchCardTip)
	{
		if (m_ImageDispatchCardTip.IsNull()==false) m_ImageDispatchCardTip.Destroy();
		if (enDispatchCardTip_Dispatch==m_enDispatchCardTip)
			m_ImageDispatchCardTip.LoadFromResource(this, AfxGetInstanceHandle(),IDB_DISPATCH_CARD);
		else
			m_ImageDispatchCardTip.LoadFromResource(this, AfxGetInstanceHandle(),IDB_CONTINUE_CARD);
		//CImageHandle ImageHandle(&m_ImageDispatchCardTip);

		
		m_ImageDispatchCardTip.TransDrawImage(pDC,nWidth/2-m_ImageDispatchCardTip.GetWidth()/2, nHeight/2,m_ImageDispatchCardTip.GetWidth(),\
			m_ImageDispatchCardTip.GetHeight(),0,\
			0 ,RGB(255,0,255));
		//m_ImageDispatchCardTip.BitBlt(pDC->GetSafeHdc(), (nWidth-m_ImageDispatchCardTip.GetWidth())/2, nHeight/2);
	}

	if (m_ImageDispatchCardTip.IsNull()==false) m_ImageDispatchCardTip.Destroy();
	if (enDispatchCardTip_Dispatch==m_enDispatchCardTip)
		m_ImageDispatchCardTip.LoadFromResource(this, AfxGetInstanceHandle(),IDB_DISPATCH_CARD);
	else
		m_ImageDispatchCardTip.LoadFromResource(this, AfxGetInstanceHandle(),IDB_CONTINUE_CARD);
	//CImageHandle ImageHandle(&m_ImageDispatchCardTip);

	//我的下注
	DrawMeJettonNumber(pDC);

	//胜利标志
	DrawWinFlags(pDC);

	//结束状态
	if (cbGameStatus==GS_GAME_END)
	{
		CBitImageEx *ImageJettonPoint[]={&m_btImageJetton1000,&m_btImageJetton10000,&m_btImageJetton100000,\
			&m_btImageJetton500000,&m_btImageJetton1000000,&m_btImageJetton5000000,&m_btImageJetton10000000};

		static CRect rcJetton(CPoint(0,0),CPoint(0,0));
		if(rcJetton.Width()==0)
			m_btJetton100.GetWindowRect(&rcJetton);
		int nYPos = nHeight/2+175;
		int nXPos = nWidth/2-64-150-1+15;
		int nSpace = 3;

		for (int n = 0;n<7;n++)
		{
			//CImageHandle ImageHandleBanker(ImageJettonPoint[n]);
			ImageJettonPoint[n]->TransDrawImage( pDC, nXPos + nSpace * n + rcJetton.Width() * n, nYPos,ImageJettonPoint[n]->GetWidth()/5 , 
				ImageJettonPoint[n]->GetHeight(),ImageJettonPoint[n]->GetWidth()/5 * 4, 0, RGB(255, 0, 255) );
		}
		//绘画扑克
		for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].DrawCardControl(pDC);
	}

	if(m_blMoveFinish)
	{
		//显示结果
		ShowGameResult(pDC, nWidth, nHeight);
	}

	return;
}

//设置信息
void CGameClientView::SetMeMaxScore(LONGLONG lMeMaxScore)
{
	if (m_lMeMaxScore!=lMeMaxScore)
	{
		//设置变量
		m_lMeMaxScore=lMeMaxScore;
	}

	return;
}

//最大下注
void CGameClientView::SetAreaLimitScore(LONGLONG lAreaLimitScore)
{
	if ( m_lAreaLimitScore!= lAreaLimitScore )
	{
		//设置变量
		m_lAreaLimitScore=lAreaLimitScore;
	}
}

//设置筹码
void CGameClientView::SetCurrentJetton(LONGLONG lCurrentJetton)
{
	//设置变量
	ASSERT(lCurrentJetton>=0L);
	m_lCurrentJetton=lCurrentJetton;

	if(m_pGameClientDlg && m_pGameClientDlg->GetGameStatus() == GS_PLACE_JETTON)
	{
		if(m_lCurrentJetton == 0L)
		{
			m_cbAreaFlash = 0xFF;
		}
	}

	return;
}

//历史记录
void CGameClientView::SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen)
{
	//设置数据
	tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.bWinShunMen=bWinShunMen;
	GameRecord.bWinDuiMen=bWinDuiMen;
	GameRecord.bWinDaoMen=bWinDaoMen;

	//操作类型
	//操作类型
	if (0==m_lUserJettonScore[ID_SHUN_MEN]) GameRecord.enOperateShunMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_SHUN_MEN] > 0 && true==bWinShunMen) GameRecord.enOperateShunMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_SHUN_MEN] > 0 && false==bWinShunMen) GameRecord.enOperateShunMen=enOperateResult_Lost;

	if (0==m_lUserJettonScore[ID_DAO_MEN]) GameRecord.enOperateDaoMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_DAO_MEN] > 0 && true==bWinDaoMen) GameRecord.enOperateDaoMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_DAO_MEN] >0 && false==bWinDaoMen) GameRecord.enOperateDaoMen=enOperateResult_Lost;

	if (0==m_lUserJettonScore[ID_DUI_MEN]) GameRecord.enOperateDuiMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_DUI_MEN] > 0 && true==bWinDuiMen) GameRecord.enOperateDuiMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_DUI_MEN]>0 && false==bWinDuiMen) GameRecord.enOperateDuiMen=enOperateResult_Lost;

	//移动下标
	int iOrCount = m_nRecordLast;
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst )
	{
		m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
		if ( m_nScoreHead < m_nRecordFirst ) m_nScoreHead = m_nRecordFirst;
	}


	int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( MAX_FALG_COUNT < nHistoryCount ) m_btScoreMoveR.EnableWindow(TRUE);

	//移到最新记录
	if ( MAX_FALG_COUNT < nHistoryCount )
	{
		m_nScoreHead = (m_nRecordLast - MAX_FALG_COUNT + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
	}

	return;
}

//清理筹码
void CGameClientView::CleanUserJetton()
{
	//清理数组
	for (BYTE i=0;i<CountArray(m_JettonInfoArray);i++)
	{
		m_JettonInfoArray[i].RemoveAll();
	}

	//下注信息
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//全体下注
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//更新界面
	RefreshGameView();

	return;
}

//个人下注
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
	//效验参数
	ASSERT(cbViewIndex<=ID_JIAO_R);
	if (cbViewIndex>ID_JIAO_R) return;

	m_lUserJettonScore[cbViewIndex]=lJettonCount;

	//更新界面
	RefreshGameView();
}

//设置扑克
void CGameClientView::SetCardInfo(BYTE cbTableCardArray[4][2])
{
	if (cbTableCardArray!=NULL)
	{
		CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));

		//开始发牌
		DispatchCard();

		m_cbAreaFlash=0xFF;
		m_bFlashResult=false;
	}
	else
	{
		ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	}
}

//设置筹码
void CGameClientView::PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount)
{
	//效验参数
	ASSERT(cbViewIndex<=ID_JIAO_R);
	if (cbViewIndex>ID_JIAO_R) return;

	//变量定义
	bool bPlaceJetton=false;
	LONGLONG lScoreIndex[JETTON_COUNT]={1000L,10000L,100000L,500000L,1000000L,5000000L,10000000L};

	//边框宽度
	int nFrameWidth=0, nFrameHeight=0;
	int nBorderWidth=32;

	switch (cbViewIndex)
	{
	case ID_SHUN_MEN:
		{ 
			m_lAllJettonScore[ID_SHUN_MEN] += lScoreCount;
			nFrameWidth = m_rcShunMen.right-m_rcShunMen.left;
			nFrameHeight = m_rcShunMen.bottom-m_rcShunMen.top;
			break;
		}
	case ID_JIAO_L:
		{
			m_lAllJettonScore[ID_JIAO_L] += lScoreCount;
			nFrameWidth = m_rcJiaoL.right-m_rcJiaoL.left;
			nFrameHeight = m_rcJiaoL.bottom-m_rcJiaoL.top;
			break;
		}
	case ID_QIAO:
		{ 
			m_lAllJettonScore[ID_QIAO] += lScoreCount;
			nFrameWidth = m_rcQiao.right-m_rcQiao.left;
			nFrameHeight = m_rcQiao.bottom-m_rcQiao.top;
			break;
		}
	case ID_DUI_MEN:
		{ 
			m_lAllJettonScore[ID_DUI_MEN] += lScoreCount;
			nFrameWidth = m_rcDuiMen.right-m_rcDuiMen.left;
			nFrameHeight = m_rcDuiMen.bottom-m_rcDuiMen.top;
			break;
		}
	case ID_DAO_MEN:
		{ 
			m_lAllJettonScore[ID_DAO_MEN] += lScoreCount;
			nFrameWidth = m_rcDaoMen.right-m_rcDaoMen.left;
			nFrameHeight = m_rcDaoMen.bottom-m_rcDaoMen.top;
			break;
		}
	case ID_JIAO_R:
		{ 
			m_lAllJettonScore[ID_JIAO_R] += lScoreCount;
			nFrameWidth = m_rcJiaoR.right-m_rcJiaoR.left;
			nFrameHeight = m_rcJiaoR.bottom-m_rcJiaoR.top;
			break;
		}	
	default:
		{
			ASSERT(FALSE);
			return;
		}
	}

	nFrameWidth -= nBorderWidth;
	nFrameHeight -= nBorderWidth;

	//增加判断
	bool bAddJetton=lScoreCount>0?true:false;
	lScoreCount=abs((int)lScoreCount);

	//增加筹码
	for (BYTE i=0;i<CountArray(lScoreIndex);i++)
	{
		//计算数目
		BYTE cbScoreIndex=JETTON_COUNT-i-1;
		LONGLONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

		//插入过虑
		if (lCellCount==0L) continue;

		//加入筹码
		for (LONG j=0;j<lCellCount;j++)
		{
			if (true==bAddJetton)
			{
				//构造变量
				tagJettonInfo JettonInfo;
				int nJettonSize=68;
				JettonInfo.cbJettonIndex=cbScoreIndex;
				JettonInfo.nXPos=rand()%(nFrameWidth-nJettonSize);
				JettonInfo.nYPos=rand()%(nFrameHeight-nJettonSize);

				//插入数组
				bPlaceJetton=true;
				m_JettonInfoArray[cbViewIndex-1].Add(JettonInfo);
			}
			else
			{
				for (int nIndex=0; nIndex<m_JettonInfoArray[cbViewIndex-1].GetCount(); ++nIndex)
				{
					//移除判断
					tagJettonInfo &JettonInfo=m_JettonInfoArray[cbViewIndex-1][nIndex];
					if (JettonInfo.cbJettonIndex==cbScoreIndex)
					{
						m_JettonInfoArray[cbViewIndex-1].RemoveAt(nIndex);
						break;
					}
				}
			}
		}

		//减少数目
		lScoreCount-=lCellCount*lScoreIndex[cbScoreIndex];
	}

	//更新界面
	if (bPlaceJetton==true) RefreshGameView();

	return;
}

//当局成绩
void CGameClientView::SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue)
{
	m_lMeCurGameScore=lMeCurGameScore;			
	m_lMeCurGameReturnScore=lMeCurGameReturnScore;			
	m_lBankerCurGameScore=lBankerCurGameScore;			
	m_lGameRevenue=lGameRevenue;					
}

//设置胜方
void CGameClientView::SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet)
{	
	//设置时间
	if (true==bSet)
	{
		//设置定时器
		SetTimer(IDI_FLASH_WINNER,500,NULL);

		//全胜判断
		if (false==bWinShunMen && false==bWinDuiMen && false==bWinDaoMen)
		{
			//清除资源
			m_ImageFrameShunMen.Destroy();
			m_ImageFrameDuiMen.Destroy();
			m_ImageFrameDaoMen.Destroy();


			//重设资源
			HINSTANCE hInstance=AfxGetInstanceHandle();
			m_ImageFrameShunMen.LoadFromResource(this, hInstance,IDB_FRAME_SHUN_MEN);
	    	m_ImageFrameDuiMen.LoadFromResource(this, hInstance,IDB_FRAME_DUI_MEN);
			m_ImageFrameDaoMen.LoadFromResource(this, hInstance,IDB_FRAME_DAO_EMN);

		}
	}
	else 
	{
		//清楚定时器
		KillTimer(IDI_FLASH_WINNER);

		//全胜判断
		if (false==m_bWinDaoMen && false==m_bWinShunMen && false==m_bWinDuiMen)
		{
			//清除资源
			m_ImageFrameShunMen.Destroy();
			m_ImageFrameDuiMen.Destroy();
			m_ImageFrameDaoMen.Destroy();


			//重设资源
			HINSTANCE hInstance=AfxGetInstanceHandle();
			m_ImageFrameShunMen.LoadFromResource(this, hInstance,IDB_FRAME_SHUN_MEN_EX);
			m_ImageFrameDuiMen.LoadFromResource(this, hInstance,IDB_FRAME_DUI_MEN_EX);
			m_ImageFrameDaoMen.LoadFromResource(this, hInstance,IDB_FRAME_DAO_EMN_EX);
		}
	}

	//设置变量
	m_bWinShunMen=bWinShunMen;
	m_bWinDuiMen=bWinDuiMen;
	m_bWinDaoMen=bWinDaoMen;
	m_bFlashResult=bSet;
	m_bShowGameResult=bSet;
	m_cbAreaFlash=0xFF;

	//更新界面
	RefreshGameView();

	return;
}

//获取区域
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
	if (m_rcShunMen.PtInRect(MousePoint)) return ID_SHUN_MEN;
	if (m_rcDuiMen.PtInRect(MousePoint)) return ID_DUI_MEN;	
	if (m_rcDaoMen.PtInRect(MousePoint)) return ID_DAO_MEN;	
	return 0xFF;
}
//绘画数字
void CGameClientView::DrawNumberStringWithPng(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore )
{

	CSize SizeScoreNumber(m_PngScoreNumber.GetWidth()/10,m_PngScoreNumber.GetHeight());

	if ( bMeScore ) SizeScoreNumber.SetSize(m_PngScoreNumber.GetWidth()/10, m_PngScoreNumber.GetHeight());

	//计算数目
	LONGLONG lNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	INT nYDrawPos=INT(nYPos-SizeScoreNumber.cy/2);
	INT nXDrawPos=INT(nXPos+lNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx);

	//绘画桌号
	for (LONGLONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		LONGLONG lCellNumber=(LONGLONG)(lNumber%10);

		m_PngScoreNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
			INT(lCellNumber*SizeScoreNumber.cx),0);
		
		//设置变量
		lNumber/=10;
		nXDrawPos-=SizeScoreNumber.cx;
	};

	return;

}
//绘画数字
void CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore,bool blTimer)
{
	//加载资源
	//CImageHandle HandleScoreNumber(&m_ImageScoreNumber);
	//CImageHandle HandleMeScoreNumber(&m_ImageMeScoreNumber);
	//CImageHandle HandleTimerNumber(&m_ImageTimerNumber);
	
	CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());

	if ( bMeScore ) SizeScoreNumber.SetSize(m_ImageMeScoreNumber.GetWidth()/10, m_ImageMeScoreNumber.GetHeight());
	if(blTimer) SizeScoreNumber.SetSize(m_ImageTimerNumber.GetWidth()/10, m_ImageTimerNumber.GetHeight());

	//计算数目
	LONGLONG lNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	INT nYDrawPos=INT(nYPos-SizeScoreNumber.cy/2);
	INT nXDrawPos=INT(nXPos+lNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx);

	//绘画桌号
	for (LONGLONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		LONGLONG lCellNumber=(LONGLONG)(lNumber%10);

		if(blTimer) 
		{
			m_ImageTimerNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				INT(lCellNumber*SizeScoreNumber.cx),0,RGB(255,0,255));
		}
		else
		{
			if ( bMeScore )
			{
				m_ImageMeScoreNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
					INT(lCellNumber*SizeScoreNumber.cx),0,RGB(255,0,255));
			}
			else
			{
				m_ImageScoreNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
					INT(lCellNumber*SizeScoreNumber.cx),0,RGB(255,0,255));
			}
		}
		
		//设置变量
		lNumber/=10;
		nXDrawPos-=SizeScoreNumber.cx;
	};

	return;
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos)
{
	CString strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
	if (lNumber==0) strNumber=TEXT("0");
	int nNumberCount=0;
	LONGLONG lTmpNumber=lNumber;
	if (lNumber<0) lNumber=-lNumber;
	while (lNumber>0)
	{
		strTmpNumber1.Format(TEXT("%I64d"),lNumber%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if (nNumberCount==3)
		{
			if(strNumber.GetLength() > 1)
				strTmpNumber2 += (TEXT(",") +strNumber);
			strNumber=strTmpNumber2;
			nNumberCount=0;
			strTmpNumber2=TEXT("");
		}
		lNumber/=10;
	}

	if (strTmpNumber2.IsEmpty()==FALSE)
	{
		if(strNumber.GetLength() > 1)
			strTmpNumber2 += (TEXT(",") +strNumber);
		strNumber=strTmpNumber2;
	}

	if (lTmpNumber<0) strNumber=TEXT("-")+strNumber;

	//输出数字
	pDC->TextOut(nXPos,nYPos,strNumber);
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat)
{
	LONGLONG lTmpNumber = lNumber;
	CString strNumber;
	CString strTmpNumber1;
	CString strTmpNumber2;
	bool blfirst = true;
	bool bLongNum = false;
	int nNumberCount = 0;

	strTmpNumber1.Empty();
	strTmpNumber2.Empty();
	strNumber.Empty();

	if ( lNumber == 0 )
		strNumber=TEXT("0");

	if ( lNumber < 0 ) 
		lNumber =- lNumber;

	if( lNumber >= 100 )
		bLongNum = true;

	while(lNumber > 0)
	{
		strTmpNumber1.Format(TEXT("%I64d"),lNumber%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if ( nNumberCount == 3 )
		{
			if(blfirst)
			{
				strTmpNumber2 += (TEXT("") +strNumber);
				blfirst = false;
			}
			else
			{
				strTmpNumber2 += (TEXT(",") +strNumber);
			}

			strNumber = strTmpNumber2;
			nNumberCount = 0;
			strTmpNumber2 = TEXT("");
		}
		lNumber /= 10;
	}

	if ( strTmpNumber2.IsEmpty() == FALSE )
	{
		if( bLongNum )
			strTmpNumber2 += (TEXT(",") +strNumber);
		else
			strTmpNumber2 += strNumber;

		strNumber = strTmpNumber2;
	}

	if ( lTmpNumber < 0 ) 
		strNumber = TEXT("-") + strNumber;
	//输出数字
	/*if (nFormat==-1) pDC->DrawText(strNumber,rcPrint,DT_END_ELLIPSIS|DT_CENTER|DT_TOP|DT_SINGLELINE);
	else pDC->DrawText(strNumber,rcPrint,nFormat);*/

	if (nFormat==-1) 
		CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, RGB(255,234,0), DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
	else 
		CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, RGB(255,234,0), nFormat);
}

void CGameClientView::DrawJettonPress(CDC* pDC,int iWidth,int iHeight,LONGLONG lTotal,LONGLONG lHaveJetton)
{
	int iWidthPng = m_TotoalPress.n_Png_tatoalflag.GetWidth();
	
	LONGLONG haveJetton = (LONGLONG)(((DOUBLE)lHaveJetton/(DOUBLE)lTotal)*iWidthPng);
	DrawNumberStringWithPng(pDC,lTotal-lHaveJetton,iWidth/2-138+20+83+70,iHeight/2-224+18,FALSE);
	m_TotoalPress.n_Png_tipC.DrawImage(pDC,iWidth/2-138,iHeight/2-224,m_TotoalPress.n_Png_tipC.GetWidth(),m_TotoalPress.n_Png_tipC.GetHeight()/2,0,0,m_TotoalPress.n_Png_tipC.GetWidth(),m_TotoalPress.n_Png_tipC.GetHeight()/2);
	m_TotoalPress.n_Png_tatoalflag .DrawImage(pDC,iWidth/2-138+8,iHeight/2-224+20,m_TotoalPress.n_Png_tatoalflag.GetWidth()-(INT)haveJetton,m_TotoalPress.n_Png_tatoalflag.GetHeight()/2,0,m_TotoalPress.n_Png_tatoalflag.GetHeight()/2,m_TotoalPress.n_Png_tatoalflag.GetWidth()-(INT)haveJetton,m_TotoalPress.n_Png_tatoalflag.GetHeight()/2);
	m_TotoalPress.n_Png_tatoalflag .DrawImage(pDC,iWidth/2-138+8+m_TotoalPress.n_Png_tatoalflag.GetWidth()-(INT)haveJetton,iHeight/2-224+20,(INT)haveJetton,m_TotoalPress.n_Png_tatoalflag.GetHeight()/2,m_TotoalPress.n_Png_tatoalflag.GetWidth()-(INT)haveJetton,0,(INT)haveJetton,m_TotoalPress.n_Png_tatoalflag.GetHeight()/2);
	m_TotoalPress.n_Png_tipC.DrawImage(pDC,iWidth/2-138,iHeight/2-224+50,m_TotoalPress.n_Png_tipC.GetWidth(),m_TotoalPress.n_Png_tipC.GetHeight()/2,0,m_TotoalPress.n_Png_tipC.GetHeight()/2,m_TotoalPress.n_Png_tipC.GetWidth(),m_TotoalPress.n_Png_tipC.GetHeight()/2);
	DrawNumberStringWithPng(pDC,(LONGLONG)lHaveJetton,iWidth/2-138+20+83+70,iHeight/2-224+50+18,FALSE);
}

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//闪动胜方
	if (nIDEvent==IDI_FLASH_WINNER)
	{
		if(m_pGameClientDlg->GetGameStatus() != GS_GAME_END)
		{
			m_cbAreaFlash = 0xFF;
			KillTimer(IDI_FLASH_WINNER);
		}
		else//设置变量
			m_bFlashResult=!m_bFlashResult;

		//更新界面
		RefreshGameView();

		return;
	}

	if(IDI_CHECK_TIMER==nIDEvent)
	{
		if(m_pGameClientDlg->IsEnableSound())
		{
			m_btSoundClose.ShowWindow(SW_HIDE);
			m_btSoundOpen.ShowWindow(SW_SHOW );
		}
		else
		{
			m_btSoundClose.ShowWindow( SW_SHOW);
			m_btSoundOpen.ShowWindow(SW_HIDE );
		}
	}

	// 发牌
	if(IDI_POSTCARD ==nIDEvent)
	{
		if(m_bcShowCount<2)
		{
			if(m_CardControl[4].m_CardTempItemArray.GetCount()>0)
			{
				m_CardControl[4].m_blhideOneCard = false;
				m_CardControl[4].m_blGameEnd = true;
				m_CardControl[4].SetDisplayFlag(true);
				m_CardControl[4].ShowCardControl(true);
				m_CardControl[4].m_CardItemArray.SetSize(1);
				m_CardControl[4].m_CardItemArray[0]=m_CardControl[4].m_CardTempItemArray[0];
				m_CardControl[4].m_CardItemArray[0].cbCardData = m_bcfirstShowCard;
				m_CardControl[4].m_CardItemArray[0].bShoot = false;
			}
		}
		else
		{
			m_CardControl[4].m_CardItemArray.SetSize(0);

			bool blKillTimer = true;
			for (int i = 0;i<4;i++)
			{
				if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
				{
					blKillTimer = false;
					break;
				}
			}
			m_PostCardIndex = m_PostCardIndex%4;
			m_CardControl[m_PostCardIndex].OnCopyCard();
			m_PostCardIndex++;
			if(blKillTimer)
			{		
				KillTimer(IDI_POSTCARD);
				SetTimer(IDI_OPENCARD,80,NULL);
			}
			else
			{
				::AfxGetMainWnd()->PostMessage(IDM_POSTCARD_SOUND,0,0);
			}
			RefreshGameView();
		}
		m_bcShowCount++;
	}

	if(IDI_OPENCARD==nIDEvent)
	{
		//手动搓牌
		if(m_blAutoOpenCard == false)
		{
			m_blMoveFinish = false;
			//先拷贝牌
			while (1)
			{
				bool blCopy = false;
				for (int i = 0;i<4;i++)
				{
					if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
					{
						blCopy = true;
						break;
					}
				}
				if(false == blCopy)
				{
					break;
				}
				else
				{
					m_CardControl[i].OnCopyCard();
				}
			}

			for (int i = 0;i<4;i++)
			{
				m_CardControl[i].m_blGameEnd = false;
				m_CardControl[i].m_blhideOneCard = false;
			}

			//更新界面
			RefreshGameView();
			KillTimer(IDI_OPENCARD);
			KillTimer(IDI_DISPATCH_CARD);
			SetTimer(IDI_DISPATCH_CARD,6000,NULL);
			return ;
		}

		//自动搓牌
		{
			m_blMoveFinish = false;
			if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x<50&&m_CardControl[m_OpenCardIndex%4].m_blAdd)
			{
				m_CardControl[m_OpenCardIndex%4].m_blGameEnd = false;
				m_CardControl[m_OpenCardIndex%4].m_MovePoint.x+=5;
				m_CardControl[m_OpenCardIndex%4].m_MovePoint.y+=20;
				m_CardControl[m_OpenCardIndex%4].m_blhideOneCard = false;

				//更新界面
				RefreshGameView();
			}
			else
			{
				/*m_CardControl[m_OpenCardIndex%4].m_blAdd = false;
				m_CardControl[m_OpenCardIndex%4].m_blGameEnd = false;
				if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x>10)
				m_CardControl[m_OpenCardIndex%4].m_MovePoint.x-=2;
				m_CardControl[m_OpenCardIndex%4].m_MovePoint.y-=2;
				m_CardControl[m_OpenCardIndex%4].m_blhideOneCard = false;
				RefreshGameView();
				if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x<=10)*/
				{
					m_CardControl[m_OpenCardIndex%4].m_tMoveCard.m_blMoveCard = true;
					m_CardControl[m_OpenCardIndex%4].m_tMoveCard.m_blMoveFinish = true;
					m_CardControl[m_OpenCardIndex%4].m_MovePoint.x = 0;
					m_CardControl[m_OpenCardIndex%4].m_MovePoint.y = 0;
					m_CardControl[m_OpenCardIndex%4].m_blAdd = true;
					m_CardControl[m_OpenCardIndex%4].m_blGameEnd = true;
					m_OpenCardIndex++;
					if(m_OpenCardIndex==5)
					{
						for (int i = 0;i<4;i++)
						{
							m_CardControl[i].m_blGameEnd = true;
						}
						KillTimer(IDI_OPENCARD);
						RefreshGameView();
						//设置显示牌定时器
						KillTimer(IDI_DISPATCH_CARD);
						SetTimer(IDI_DISPATCH_CARD,30,NULL);
					}
				}
			}
		}
	}
	//轮换庄家
	else if ( nIDEvent == IDI_SHOW_CHANGE_BANKER )
	{
		ShowChangeBanker( false );
		return;
	}
	else if (nIDEvent==IDI_DISPATCH_CARD)
	{
		KillTimer(IDI_DISPATCH_CARD);
		//完成发牌
		FinishDispatchCard();
		{
			for (int i = 0;i<4;i++)
			{
				m_CardControl[i].m_blGameEnd = true;
				m_CardControl[i].m_blhideOneCard = false;
			}
			KillTimer(IDI_OPENCARD);
			RefreshGameView();

		}
		//先拷贝牌
		while (1)
		{
			bool blCopy = false;
			for (int i = 0;i<4;i++)
			{
				if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
				{
					blCopy = true;
					break;
				}
			}
			if(false == blCopy)
			{
				break;
			}else
			{
				m_CardControl[i].OnCopyCard();
			}
		}
		m_blMoveFinish = true;
		//更新界面
		RefreshGameView();

		return;
	}
	else if (IDI_SHOWDISPATCH_CARD_TIP==nIDEvent)
	{
		SetDispatchCardTip(enDispatchCardTip_NULL);

	}else if(nIDEvent==IDI_MOVECARD_END)
	{
		KillTimer(IDI_MOVECARD_END);
	}

	__super::OnTimer(nIDEvent);
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	for (int i = 0;i<4;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnLButtonDown(nFlags, Point);
		}
	}
		
	if (m_lCurrentJetton!=0L)
	{
		//下注区域
		BYTE cbJettonArea=GetJettonArea(Point);
	
		//最大下注
		LONGLONG lMaxJettonScore=GetUserMaxJetton();

		//合法判断
		if (lMaxJettonScore < m_lCurrentJetton) return ;

		//发送消息
		if (cbJettonArea!=0xFF) SendEngineMessage(IDM_PLACE_JETTON,cbJettonArea,0);
	}
	RefreshGameView();

	__super::OnLButtonDown(nFlags,Point);
}
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	
	for (int i = 0;i<4;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnLButtonUp(nFlags, Point);
			RefreshGameView();
		}
		
	}
	__super::OnLButtonUp(nFlags,Point);
}

//接受其他控件传来的消息
LRESULT CGameClientView::OnViLBtUp(WPARAM wParam, LPARAM lParam)
{
	CPoint *pPoint = (CPoint*)lParam;
	ScreenToClient(pPoint);
    OnLButtonUp(1,*pPoint);
	return 1;

}
//鼠标移动消息
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	for (int i=0; i<4; i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnMouseMove(nFlags, point);
			RefreshGameView();
		}
	}
	__super::OnMouseMove(nFlags,point);
}
//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置变量
	m_lCurrentJetton=0L;

	if (m_pGameClientDlg->GetGameStatus()!=GS_GAME_END && m_cbAreaFlash!=0xFF)
	{
		m_cbAreaFlash=0xFF;
		RefreshGameView();
	}

	__super::OnLButtonDown(nFlags,Point);
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if (m_lCurrentJetton != 0L)
	{
		//获取区域
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		BYTE cbJettonArea=GetJettonArea(MousePoint);

		//状态
		if(m_pGameClientDlg && m_pGameClientDlg->GetGameStatus() != GS_PLACE_JETTON)
			cbJettonArea=0xFF;

		//设置变量
		if ( m_cbAreaFlash != cbJettonArea )
		{
			m_cbAreaFlash = cbJettonArea;
			RefreshGameView();
		}

		//区域判断
		if (cbJettonArea==0xFF)
		{
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			return TRUE;
		}

		//最大下注
		LONGLONG lMaxJettonScore=GetUserMaxJetton();

		//合法判断
		if (lMaxJettonScore < m_lCurrentJetton)
		{
			SetCursor(LoadCursor(NULL,IDC_NO));
			return TRUE;
		}
	
		//设置光标
		switch (m_lCurrentJetton)
		{
		case 100:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)));
				return TRUE;
			}
		case 1000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000)));
				return TRUE;
			}
		case 10000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000)));
				return TRUE;
			}
		case 100000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)));
				return TRUE;
			}
		case 500000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_500000)));
				return TRUE;
			}
		case 1000000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000000)));
				return TRUE;
			}
		case 5000000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_5000000)));
				return TRUE;
			}
		case 10000000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000000)));
				return TRUE;
			}
		}
	}

	return __super::OnSetCursor(pWnd, nHitTest, uMessage);
}

//轮换庄家
void CGameClientView::ShowChangeBanker( bool bChangeBanker )
{
	//轮换庄家
	if ( bChangeBanker )
	{
		SetTimer( IDI_SHOW_CHANGE_BANKER, 3000, NULL );
		m_bShowChangeBanker = true;
	}
	else
	{
		KillTimer( IDI_SHOW_CHANGE_BANKER );
		m_bShowChangeBanker = false ;
	}

	//更新界面
	RefreshGameView();
}

//上庄按钮
void CGameClientView::OnApplyBanker()
{
	SendEngineMessage(IDM_APPLY_BANKER,1,0);

}

//下庄按钮
void CGameClientView::OnCancelBanker()
{
	SendEngineMessage(IDM_APPLY_BANKER,0,0);
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}
//庄家信息
void CGameClientView::SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore) 
{
	//庄家椅子号
	WORD wBankerUser=INVALID_CHAIR;

	//查找椅子号
	if (0!=dwBankerUserID)
	{
		for (WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
		{
			IClientUserItem * pUserData=GetClientUserItem(wChairID);
			if (NULL!=pUserData && dwBankerUserID==pUserData->GetUserID())
			{
				wBankerUser=wChairID;
				break;
			}
		}
	}

	//切换判断
	if (m_wBankerUser!=wBankerUser)
	{
		m_wBankerUser=wBankerUser;
		m_wBankerTime=0L;
		m_lBankerWinScore=0L;	
		m_lTmpBankerWinScore=0L;
	}
	m_lBankerScore=lBankerScore;
}

//绘画标识
void CGameClientView::DrawWinFlags(CDC * pDC)
{
	/*{
		int   bWinCount[3] = {0};
		bWinCount[0]= 50;
		int iAllrecordCount = 70;
		for (int n = 0;n<3;n++)
		{
		int	nYPos=m_nWinFlagsExcursionY+5+n*33;
		int nXPos=m_nWinFlagsExcursionX-115+10;


			float fCountRe = (float)bWinCount[n]/(float)iAllrecordCount;
			fCountRe *=100;
			CString str;
			str.Format("%0.2f",fCountRe);
			str+="%";

			pDC->TextOut(nXPos,nYPos,str);
		}

	}*/
	
	//非空判断
	if (m_nRecordLast==m_nRecordFirst) return;

	//资源变量
	//CImageHandle ImageHandleWinFlags(&m_ImageWinFlags);
	int nIndex = m_nScoreHead;
	COLORREF static clrOld ;

	clrOld = pDC->SetTextColor(RGB(255,234,0));
	int nDrawCount=0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < MAX_FALG_COUNT )
	{
		//胜利标识
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		bool static bWinMen[3];
		bWinMen[0]=ClientGameRecord.bWinShunMen;
		bWinMen[1]=ClientGameRecord.bWinDaoMen;
		bWinMen[2]=ClientGameRecord.bWinDuiMen;

		//操作结果
		enOperateResult static OperateResult[3];
		OperateResult[0]=ClientGameRecord.enOperateShunMen;
		OperateResult[1]=ClientGameRecord.enOperateDaoMen;
		OperateResult[2]=ClientGameRecord.enOperateDuiMen;

		for (int i=0; i<3; ++i)
		{
			//位置变量
			int static nYPos=0,nXPos=0;
			nYPos=m_nWinFlagsExcursionY+i*32;
			nXPos=m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 39;

			//胜利标识
			int static nFlagsIndex=0;
			if (false==bWinMen[i])
			{
				nFlagsIndex=2;
			}
			else
			{
				nFlagsIndex=0;

			}
			if (OperateResult[i] != enOperateResult_NULL)
			{			
				 nFlagsIndex+=1;

				if(nFlagsIndex>=4)
				{
					nFlagsIndex=3;
				}

				nFlagsIndex-=1;
			}else
			{
				nFlagsIndex+=1;
			}
			//绘画标识
			m_ImageWinFlags.TransDrawImage( pDC, nXPos, nYPos, m_ImageWinFlags.GetWidth()/6 , 
				m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/6 * nFlagsIndex, 0, RGB(255, 0, 255) );
		}

		//移动下标
		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}

	nIndex = 0;

	int   bWinCount[3] = {0};
	int iAllrecordCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) )
	{
		//胜利标识
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		bool static bWinMen[3];
		bWinMen[0]=ClientGameRecord.bWinShunMen;
		bWinMen[1]=ClientGameRecord.bWinDaoMen;
		bWinMen[2]=ClientGameRecord.bWinDuiMen;

		for (int n = 0;n<3;n++)
		{
			if(bWinMen[n]==true)
			{
				bWinCount[n]++;

			}
		}
		iAllrecordCount++;

		nIndex = (nIndex+1);

	}
	for (int n = 0;n<3;n++)
	{
		int	nYPos=m_nWinFlagsExcursionY+5+n*33;
		int nXPos=m_nWinFlagsExcursionX-115+10;
		float fCountRe = (float)bWinCount[n]/(float)iAllrecordCount;
		fCountRe *=100;
		CString str;
		str.Format(TEXT("%0.2f"),fCountRe);
		str+="%";

		pDC->TextOut(nXPos,nYPos,str);
	}

	pDC->SetTextColor(clrOld);
}
//手工搓牌
afx_msg void CGameClientView::OnOpenCard()
{
	SendEngineMessage(IDM_OPEN_CARD,0,0);

}
//自动搓牌
afx_msg void CGameClientView::OnAutoOpenCard()
{
	SendEngineMessage(IDM_AUTO_OPEN_CARD,0,0);

}
//移动按钮
void CGameClientView::OnScoreMoveL()
{
	if ( m_nRecordFirst == m_nScoreHead ) return;

	m_nScoreHead = (m_nScoreHead - 1 + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( m_nScoreHead == m_nRecordFirst ) m_btScoreMoveL.EnableWindow(FALSE);

	m_btScoreMoveR.EnableWindow(TRUE);

	//更新界面
	RefreshGameView();
}

//移动按钮
void CGameClientView::OnScoreMoveR()
{
	int nHistoryCount = ( m_nRecordLast - m_nScoreHead + MAX_SCORE_HISTORY ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount == MAX_FALG_COUNT ) return;

	m_nScoreHead = ( m_nScoreHead + 1 ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount-1 == MAX_FALG_COUNT ) m_btScoreMoveR.EnableWindow(FALSE);

	m_btScoreMoveL.EnableWindow(TRUE);

	//更新界面
	RefreshGameView();
}

//显示结果
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{
	//显示判断
	//if (m_pGameClientDlg->GetGameStatus()!=GS_GAME_END) return;
	if (false==m_bShowGameResult) return;

	int	nXPos = nWidth / 2 - 129;
	int	nYPos = nHeight / 2 - 208;
	//CImageHandle ImageHandleGameEnd(&m_ImageGameEnd);

	CRect rcAlpha(nXPos+2, nYPos+70, nXPos+2 + m_ImageGameEnd.GetWidth(), nYPos+70+m_ImageGameEnd.GetHeight());
	DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);
	m_ImageGameEnd.TransDrawImage(pDC, nXPos+2, nYPos+70, RGB(255,0,255));

	pDC->SetTextColor(RGB(255,234,0));
	CRect rcMeWinScore, rcMeReturnScore;
	rcMeWinScore.left = nXPos+2 + 40;
	rcMeWinScore.top = nYPos+70 + 32;
	rcMeWinScore.right = rcMeWinScore.left + 111;
	rcMeWinScore.bottom = rcMeWinScore.top + 34;

	rcMeReturnScore.left = nXPos+2 + 150;
	rcMeReturnScore.top = nYPos+70 + 32;
	rcMeReturnScore.right = rcMeReturnScore.left + 111;
	rcMeReturnScore.bottom = rcMeReturnScore.top + 34;

	CString strMeGameScore, strMeReturnScore;
	DrawNumberStringWithSpace(pDC,m_lMeCurGameScore,rcMeWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	DrawNumberStringWithSpace(pDC,m_lMeCurGameReturnScore,rcMeReturnScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	CRect rcBankerWinScore;
	rcBankerWinScore.left = nXPos+2 + 40;
	rcBankerWinScore.top = nYPos+70 + 69;
	rcBankerWinScore.right = rcBankerWinScore.left + 111;
	rcBankerWinScore.bottom = rcBankerWinScore.top + 34;

	CString strBankerCurGameScore;
	strBankerCurGameScore.Format(TEXT("%I64d"), m_lBankerCurGameScore);
	DrawNumberStringWithSpace(pDC,m_lBankerCurGameScore,rcBankerWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
}

//透明绘画
bool CGameClientView::DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha)
{
	ASSERT(pDC != 0 && lpRect != 0 && clr != CLR_NONE);
	if(pDC == 0 || lpRect == 0 || clr == CLR_NONE)
	{
		return false;
	}
	//全透明
	if( abs(fAlpha) <= 0.000001 )
	{
		return true;
	}

	for(LONG l=lpRect->top; l<lpRect->bottom; l++)
	{
		for(LONG k=lpRect->left; k<lpRect->right; k++)
		{
			COLORREF clrBk = pDC->GetPixel(k, l);
			COLORREF clrBlend = RGB(GetRValue(clrBk)*(1-fAlpha)+GetRValue(clr)*fAlpha, 
								GetGValue(clrBk)*(1-fAlpha)+GetGValue(clr)*fAlpha,
								GetBValue(clrBk)*(1-fAlpha)+GetBValue(clr)*fAlpha);
			pDC->SetPixel(k, l, clrBlend);
		}
	}

	return true;
}

//最大下注
LONGLONG CGameClientView::GetUserMaxJetton()
{
	//已下注额
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lNowJetton += m_lUserJettonScore[nAreaIndex];

	//庄家金币
	LONGLONG lBankerScore=2147483647;
	if (m_wBankerUser!=INVALID_CHAIR) lBankerScore=m_lBankerScore;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lBankerScore-=m_lAllJettonScore[nAreaIndex];

	//区域限制
	LONGLONG lMeMaxScore;
	if(m_lMeMaxScore-lNowJetton>m_lAreaLimitScore)
	{
		lMeMaxScore= m_lAreaLimitScore;

	}else
	{
		lMeMaxScore = m_lMeMaxScore-lNowJetton;
	}
	//LONGLONG lMeMaxScore=min(m_lMeMaxScore-lNowJetton,m_lAreaLimitScore);

	//庄家限制
	lMeMaxScore=min(lMeMaxScore,lBankerScore);

	//非零限制
	//ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return lMeMaxScore;
}

//成绩设置
void CGameClientView::SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore)
{
	m_lMeCurGameScore=lMeCurGameScore;
	m_lMeCurGameReturnScore=lMeCurGameReturnScore;
	m_lBankerCurGameScore=lBankerCurGameScore;	
}

//绘画数字
void CGameClientView::DrawMeJettonNumber(CDC *pDC)
{
	//绘画数字
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
	{
		if (m_lUserJettonScore[nAreaIndex] > 0 )
			DrawNumberString(pDC,m_lUserJettonScore[nAreaIndex],m_PointJettonNumber[nAreaIndex-1].x,m_PointJettonNumber[nAreaIndex-1].y+25, true);
	}
}

//开始发牌
void CGameClientView::DispatchCard()
{
	//设置界面
	for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].m_CardItemArray.SetSize(0);
	//设置标识
	m_bNeedSetGameRecord=true;
}

//结束发牌
void CGameClientView::FinishDispatchCard()
{
	//完成判断
	if (m_bNeedSetGameRecord==false) return;

	//设置标识
	m_bNeedSetGameRecord=false;

	//删除定时器
	KillTimer(IDI_DISPATCH_CARD);

	////设置扑克
	//for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].SetCardData(m_cbTableCardArray[i],2);

	//推断赢家
	bool bWinShunMen, bWinDuiMen, bWinDaoMen;
	DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);	

	//保存记录
	SetGameHistory(bWinShunMen, bWinDaoMen, bWinDuiMen);

	//累计积分
	m_lMeStatisticScore+=m_lMeCurGameScore;
	m_lBankerWinScore=m_lTmpBankerWinScore;

	//设置赢家
	SetWinnerSide(bWinShunMen, bWinDuiMen, bWinDaoMen, true);

	//播放声音
	if (m_lMeCurGameScore>0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_WIN"));
	else if (m_lMeCurGameScore<0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_LOST"));
	else m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_DRAW"));
}

//胜利边框
void CGameClientView::FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC)
{
	if(0)
	{
		pDC->Draw3dRect(m_rcDuiMen.left, m_rcDuiMen.top,m_rcDuiMen.Width(),m_rcDuiMen.Height(),RGB(255,0,255),RGB(255,0,255));
		pDC->Draw3dRect(m_rcShunMen.left, m_rcShunMen.top,m_rcShunMen.Width(),m_rcShunMen.Height(),RGB(255,0,255),RGB(255,0,255));
		pDC->Draw3dRect(m_rcDaoMen.left, m_rcDaoMen.top,m_rcDaoMen.Width(),m_rcDaoMen.Height(),RGB(255,0,255),RGB(255,0,255));

	
		//加载资源
		//CImageHandle ImageHandleShunMen(&m_ImageFrameShunMen);
		//CImageHandle ImageHandleDuiMen(&m_ImageFrameDuiMen);	
		//CImageHandle ImageHandleDaoMen(&m_ImageFrameDaoMen);	

		//绘画图片
		m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top, RGB(255,0,255));
	    m_ImageFrameDuiMen.TransDrawImage(pDC,m_rcDuiMen.left, m_rcDuiMen.top, RGB(255,0,255));
		m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top, RGB(255,0,255));

	}


	//合法判断
	if (m_cbAreaFlash==0xFF && false==m_bFlashResult) return;

		
	//下注判断
	if (false==m_bFlashResult)
	{
		switch (m_cbAreaFlash)
		{
		case ID_SHUN_MEN:	
			{
				//绘画图片
				//CImageHandle ImageHandle(&m_ImageFrameShunMen);
				m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top, RGB(255,0,255));
				break;
			}
		case ID_DUI_MEN:
			{
				//绘画图片
				//CImageHandle ImageHandle(&m_ImageFrameDuiMen);	
				m_ImageFrameDuiMen.TransDrawImage(pDC,m_rcDuiMen.left, m_rcDuiMen.top, RGB(255,0,255));
				break;
			}
		case ID_DAO_MEN:
			{
				//绘画图片
				//CImageHandle ImageHandle(&m_ImageFrameDaoMen);	
				m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top, RGB(255,0,255));
				break;
			}
		}
	}
	else
	{
		//庄全胜判断
		if (false==m_bWinDaoMen && false==m_bWinShunMen && false==m_bWinDuiMen)
		{
			//加载资源
			//CImageHandle ImageHandleShunMen(&m_ImageFrameShunMen);
	        //CImageHandle ImageHandleDuiMen(&m_ImageFrameDuiMen);	
			//CImageHandle ImageHandleDaoMen(&m_ImageFrameDaoMen);	


			//绘画图片
			m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top, RGB(255,0,255));
			m_ImageFrameDuiMen.TransDrawImage(pDC,m_rcDuiMen.left, m_rcDuiMen.top, RGB(255,0,255));
			m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top, RGB(255,0,255));

		}
		else
		{
			//门判断
			if (true==m_bWinDaoMen)
			{
				//CImageHandle ImageHandleDaoMen(&m_ImageFrameDaoMen);
				m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top, RGB(255,0,255));
			}
			if (true==m_bWinShunMen)
			{
				//CImageHandle ImageHandleShunMen(&m_ImageFrameShunMen);
				m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top, RGB(255,0,255));
			}
			if (true==m_bWinDuiMen)
			{
				//CImageHandle ImageHandleDuiMen(&m_ImageFrameDuiMen);
				m_ImageFrameDuiMen.TransDrawImage(pDC,m_rcDuiMen.left, m_rcDuiMen.top, RGB(255,0,255));
			}
		}
	}
}

void CGameClientView::DrawGameTimer(CDC* pDC,int iWidth,int iHeight,int iTimer)
{

	int ix = iWidth+25;
	int iy = iHeight+35;
	if(iTimer<100)
	{
		ix+=3;
	}
	if(iTimer<10)
	{
		ix+=1;
	}


	m_ImageTimer.DrawImage(pDC,iWidth,iHeight,m_ImageTimer.GetWidth(),m_ImageTimer.GetHeight(),0,0,m_ImageTimer.GetWidth(),m_ImageTimer.GetHeight());
		DrawNumberString(pDC,iTimer,ix,iy,false,true);
}
//推断赢家
void CGameClientView::DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen)
{
	//大小比较
	bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;
	bWinDuiMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DUI_MEN_INDEX],2)==1?true:false;
	bWinDaoMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DAO_MEN_INDEX],2)==1?true:false;
}

//控件命令
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//获取ID
	WORD wControlID=LOWORD(wParam);

	//控件判断
	switch (wControlID)
	{
	case IDC_JETTON_BUTTON_100:
		{
			//设置变量
			m_lCurrentJetton=100L;
			break;
		}
	case IDC_JETTON_BUTTON_1000:
		{
			//设置变量
			m_lCurrentJetton=1000L;
			break;
		}
	case IDC_JETTON_BUTTON_10000:
		{
			//设置变量
			m_lCurrentJetton=10000L;
			break;
		}
	case IDC_JETTON_BUTTON_100000:
		{
			//设置变量
			m_lCurrentJetton=100000L;
			break;
		}
	case IDC_JETTON_BUTTON_500000:
		{
			//设置变量
			m_lCurrentJetton=500000L;
			break;
		}
	case IDC_JETTON_BUTTON_1000000:
		{
			//设置变量
			m_lCurrentJetton=1000000L;
			break;
		}
	case IDC_JETTON_BUTTON_5000000:
		{
			//设置变量
			m_lCurrentJetton=5000000L;
			break;
		}
	case IDC_JETTON_BUTTON_10000000:
		{
			//设置变量
			m_lCurrentJetton=10000000L;
			break;
		}

	case IDC_AUTO_OPEN_CARD:
		{
			break;
		}
	case IDC_OPEN_CARD:
		{
			break;
		}
	case IDC_BANK:
		{
			break;
		}
	case IDC_CONTINUE_CARD:
		{
			SendEngineMessage(IDM_CONTINUE_CARD,0,0);
			break;
		}
	}

	return CGameFrameView::OnCommand(wParam, lParam);
}

//我的位置
void CGameClientView::SetMeChairID(DWORD dwMeUserID)
{
	//查找椅子号
	for (WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
	{
		IClientUserItem * pUserData=GetClientUserItem(wChairID);
		if (NULL!=pUserData && dwMeUserID==pUserData->GetUserID())
		{
			m_wMeChairID=wChairID;
			break;
		}
	}
}

//设置提示
void CGameClientView::SetDispatchCardTip(enDispatchCardTip DispatchCardTip)
{
	//设置变量
	m_enDispatchCardTip=DispatchCardTip;

	//设置定时器
	if (enDispatchCardTip_NULL!=DispatchCardTip) SetTimer(IDI_SHOWDISPATCH_CARD_TIP,2*1000,NULL);		
	else KillTimer(IDI_SHOWDISPATCH_CARD_TIP);

	//更新界面
	RefreshGameView();
}

//绘画庄家
void CGameClientView::DrawBankerInfo(CDC *pDC,int nWidth,int nHeight)
{
	//庄家信息																											
	pDC->SetTextColor(RGB(255,234,0));

	//获取玩家
	IClientUserItem * pUserData=m_wBankerUser==INVALID_CHAIR ? NULL : GetClientUserItem(m_wBankerUser);

	//位置信息
	CRect static StrRect;
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 328;
	StrRect.right = StrRect.left + 145;
	StrRect.bottom = StrRect.top + 15;

	//庄家名字
	pDC->DrawText(pUserData==NULL?(m_bEnableSysBanker?TEXT("系统坐庄"):TEXT("无人坐庄")):pUserData->GetNickName(), StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );
	CDFontEx::DrawText(this,pDC,  12, 400, pUserData==NULL?(m_bEnableSysBanker?TEXT("系统坐庄"):TEXT("无人坐庄")):pUserData->GetNickName(), StrRect, RGB(255,234,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

	//庄家总分
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 306-2;
	StrRect.right = StrRect.left + 190;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,pUserData==NULL?0:pUserData->GetUserScore(), StrRect);

	//庄家成绩
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 287-2;
	StrRect.right = StrRect.left + 190;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,m_lBankerWinScore,StrRect);
	//庄家局数
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 265-2;
	StrRect.right = StrRect.left + 190;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,m_wBankerTime ,StrRect);

	
}
void CGameClientView::SetFirstShowCard(BYTE bcCard)
{
	m_bcfirstShowCard =  bcCard;
	m_CardControl[4].SetCardData(NULL,0);
	m_CardControl[4].SetCardData(&bcCard,1);
	m_CardControl[4].m_blGameEnd = true;
	m_CardControl[4].SetDisplayFlag(true);
	m_CardControl[4].ShowCardControl(true);
}
//绘画玩家
void CGameClientView::DrawMeInfo(CDC *pDC,int nWidth,int nHeight)
{
	//合法判断
	if (INVALID_CHAIR==m_wMeChairID) return;

	IClientUserItem * pMeUserData=GetClientUserItem(m_wMeChairID);
	if ( pMeUserData != NULL )
	{
		//字体颜色																									
		pDC->SetTextColor(RGB(255,234,0));

		//总共下注
		LONGLONG lMeJetton=0L;
		for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lMeJetton += m_lUserJettonScore[nAreaIndex];

		//设置位置
		CRect static rcDraw;
		rcDraw.left=nWidth/2-218-95+6;
		rcDraw.top=nHeight/2+178-11;
		rcDraw.right=rcDraw.left+105;
		rcDraw.bottom=rcDraw.top+15;

		//绘画帐号
		//pDC->DrawText(pMeUserData->GetNickName(),lstrlen(pMeUserData->GetNickName()),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
		CDFontEx::DrawText(this,pDC,  12, 400, pMeUserData->GetNickName(), rcDraw, RGB(255,234,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);


		//绘画积分
		rcDraw.top=rcDraw.top+17;
		rcDraw.bottom=rcDraw.top+15;
		DrawNumberStringWithSpace(pDC,pMeUserData->GetUserScore()-lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		//绘画成绩
		rcDraw.top=rcDraw.top+17;
		rcDraw.bottom=rcDraw.top+15;
		DrawNumberStringWithSpace(pDC,m_lMeStatisticScore,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		//绘画成绩
		rcDraw.top=rcDraw.top+17;
		rcDraw.bottom=rcDraw.top+15;
		DrawNumberStringWithSpace(pDC,lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
	}
}

//////////////////////////////////////////////////////////////////////////
//银行存款
void CGameClientView::OnBankStorage()
{
	IClientKernel *pIClientKernel=(IClientKernel *)m_pGameClientDlg->m_pIClientKernel;

	if(NULL!=pIClientKernel)
	{
		CRect btRect;
		m_btBankerStorage.GetWindowRect(&btRect);
		ShowInsureSave(pIClientKernel,CPoint(btRect.left-200,btRect.top));
	}
}

//银行取款
void CGameClientView::OnBankDraw()
{
	//获取接口
	IClientKernel *pIClientKernel=(IClientKernel *)m_pGameClientDlg->m_pIClientKernel;

	if(NULL!=pIClientKernel)
	{
		CRect btRect;
		m_btBankerDraw.GetWindowRect(&btRect);
		ShowInsureGet(pIClientKernel,CPoint(btRect.left-200,btRect.top));
	}
}
//输赢控制
void CGameClientView::OnControl()
{
	SendEngineMessage(IDM_MANAGE_CONTROL,0,0);
}
void CGameClientView::ClearBank()
{
	//m_DlgBank.ShowWindow(SW_HIDE);
}

//更新视图
void CGameClientView::RefreshGameView()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

	return;
}
