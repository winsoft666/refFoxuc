#include "Stdafx.h"
#include "Resource.h"
#include "ChessManual.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChessManual, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_MANUAL_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_MANUAL_PRESERVE, OnPreserve)
	ON_BN_CLICKED(IDC_MANUAL_COUNT, OnCount)
	ON_BN_CLICKED(IDC_MANUAL_RELOAD, OnReLoad)
	ON_BN_CLICKED(IDC_MANUAL_FIRST, OnFirst)
	ON_BN_CLICKED(IDC_MANUAL_BEFORE, OnBefore)
	ON_BN_CLICKED(IDC_MANUAL_NEXT, OnNext)
	ON_BN_CLICKED(IDC_MANUAL_LAST, OnLast)
	ON_LBN_SELCHANGE(IDC_MANUAL_LIST, OnLbnSelchangeManualList)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChessManual::CChessManual() : CDialog(IDD_MANUAL)
{
	//设置变量
	m_wChessStep=0L;
	m_pDlgGameLogic=NULL;

	//加载资源
	m_ImageBack.LoadFromResource(AfxGetInstanceHandle(),IDB_MANUAL_BACK);

	return;
}

//析构函数
CChessManual::~CChessManual()
{
}

//控件绑定
void CChessManual::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_MANUAL_OPEN, m_btOpen);
	DDX_Control(pDX, IDC_MANUAL_PRESERVE, m_btPreserve);
	DDX_Control(pDX, IDC_MANUAL_COUNT, m_btCount);
	DDX_Control(pDX, IDC_MANUAL_RELOAD, m_btReLoad);
	DDX_Control(pDX, IDC_MANUAL_FIRST, m_btFirst);
	DDX_Control(pDX, IDC_MANUAL_BEFORE, m_btBefore);
	DDX_Control(pDX, IDC_MANUAL_NEXT, m_btNext);
	DDX_Control(pDX, IDC_MANUAL_LAST, m_btLast);
	DDX_Control(pDX, IDC_MANUAL_LIST, m_ManualList);
}

//初始化函数
BOOL CChessManual::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("中国象棋棋谱："));

	//移动窗口
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//获取大小
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建控件
	const CSize & BoradSize=m_ChessBorad.GetChessBoradSize();
	m_ChessBorad.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,10);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btOpen.SetButtonImage(IDB_MANUAL_BT_OPEN,hInstance,false,false);
	m_btPreserve.SetButtonImage(IDB_MANUAL_BT_PRESERVE,hInstance,false,false);
	m_btCount.SetButtonImage(IDB_MANUAL_BT_COUNT,hInstance,false,false);
	m_btReLoad.SetButtonImage(IDB_MANUAL_BT_RELOAD,hInstance,false,false);
	m_btFirst.SetButtonImage(IDB_MANUAL_BT_FIRST,hInstance,false,false);
	m_btBefore.SetButtonImage(IDB_MANUAL_BT_BEFORE,hInstance,false,false);
	m_btNext.SetButtonImage(IDB_MANUAL_BT_NEXT,hInstance,false,false);
	m_btLast.SetButtonImage(IDB_MANUAL_BT_LAST,hInstance,false,false);
	m_btCancel.SetButtonImage(IDB_MANUAL_BT_CLOSE,hInstance,false,false);

	//计算位置
	CRect rcButton;
	m_btOpen.GetWindowRect(&rcButton);
	int nXButtonSpace=(rcClient.Width()-rcButton.Width()*9-34)/10;
	int nYPos=rcClient.Height()-rcButton.Height()-21;

	//移动按钮
	HDWP hDwp=BeginDeferWindowPos(9);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btOpen,NULL,20+nXButtonSpace,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPreserve,NULL,20+nXButtonSpace*2+rcButton.Width(),nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btCount,NULL,20+nXButtonSpace*3+rcButton.Width()*2,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btReLoad,NULL,20+nXButtonSpace*4+rcButton.Width()*3,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btFirst,NULL,20+nXButtonSpace*5+rcButton.Width()*4,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btBefore,NULL,20+nXButtonSpace*6+rcButton.Width()*5,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btNext,NULL,20+nXButtonSpace*7+rcButton.Width()*6,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btLast,NULL,20+nXButtonSpace*8+rcButton.Width()*7,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btCancel,NULL,20+nXButtonSpace*9+rcButton.Width()*8,nYPos,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	//移动控件
	m_ChessBorad.MoveWindow(22,22,BoradSize.cx,BoradSize.cy);
	m_ManualList.MoveWindow(BoradSize.cx+29,25,rcClient.Width()-BoradSize.cx-54,BoradSize.cy-7);

	//更新棋谱
	OnReLoad();

	return TRUE;
}

//重画函数
void CChessManual::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//绘画背景
	m_ImageBack.BitBlt(dc,0,0);

	return;
}

//绘画背景
BOOL CChessManual::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//选择改变
void CChessManual::OnLbnSelchangeManualList()
{
	int nSelItem=m_ManualList.GetCurSel();
	if ((nSelItem!=LB_ERR)&&((nSelItem+1)!=m_wChessStep))
	{
		//前进判断
		if (nSelItem>=m_wChessStep)
		{
			tagChessManual * pChessManual=NULL;
			WORD wDeadChess[361],wDeadChessCount=0;
			for (WORD i=m_wChessStep;i<(nSelItem+1);i++)
			{
				pChessManual=&m_ChessManualInfo[i];
				m_GameLogic.PlaceChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor,wDeadChess,wDeadChessCount);
			}
		}
		else m_GameLogic.RegretChess(m_wChessStep-nSelItem-1);

		//设置棋盘
		m_wChessStep=(nSelItem+1);
		m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

		//设置选择
		tagChessManual * pChessManual=&m_ChessManualInfo[m_wChessStep-1];
		m_ChessBorad.SetChessFocus(pChessManual->cbXPos,pChessManual->cbYPos);

		//更新控制
		UpdateControl();
	}

	return;
}

//更新控制
void CChessManual::UpdateControl()
{
	//设置列表
	m_ManualList.SetCurSel(m_wChessStep-1);

	//设置按钮
	m_btCount.EnableWindow(m_wChessStep>0);
	m_btFirst.EnableWindow(m_wChessStep>0);
	m_btBefore.EnableWindow(m_wChessStep>0);
	m_btNext.EnableWindow(m_wChessStep<m_ChessManualInfo.GetCount());
	m_btLast.EnableWindow(m_wChessStep<m_ChessManualInfo.GetCount());

	return;
}

//更新棋谱
void CChessManual::UpdateManualList()
{
	//还原列表
	while (m_ManualList.GetCount()>0) m_ManualList.DeleteString(0);

	//变量定义
	LPCTSTR pszColorDes[]={TEXT("黑"),TEXT("白")};

	//增加解释
	TCHAR szManualText[128];
	tagChessManual * pChessManual=NULL;
	for (INT_PTR i=0;i<m_ChessManualInfo.GetCount();i++)
	{
		pChessManual=&m_ChessManualInfo[i];
		_sntprintf(szManualText,sizeof(szManualText),TEXT("%s [ %ld，%ld ]"),pszColorDes[pChessManual->cbColor-1],
			pChessManual->cbXPos+1,pChessManual->cbYPos+1);
		m_ManualList.AddString(szManualText);
	}

	return;
}

//鼠标消息
void CChessManual::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//发送消息
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//打开按钮
void CChessManual::OnOpen()
{
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//选择文件
	LPCTSTR pszFilter=TEXT("中国象棋棋谱文件 （*.WQM）|*.WQM||");
	CFileDialog DlgOpenManual(TRUE,TEXT("WQM"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgOpenManual.m_pOFN->lpstrInitialDir=szPath;
	if (DlgOpenManual.DoModal()!=IDOK) return;

	//打开文件
	CFile FileManual;
	if (FileManual.Open(DlgOpenManual.GetPathName(),CFile::modeRead)==FALSE)
	{
		AfxMessageBox(TEXT("无法打开文件，棋谱打开失败！"),MB_ICONSTOP);
		return;
	}

	//判断文件
	DWORD dwFileLength=(DWORD)FileManual.GetLength();
	if (dwFileLength<sizeof(tagChessManualHead))
	{
		AfxMessageBox(TEXT("棋谱文件格式错误，棋谱打开失败！"),MB_ICONSTOP);
		return;
	}

	//文件头
	tagChessManualHead ChessManualHead;
	FileManual.Read(&ChessManualHead,sizeof(ChessManualHead));

	//效验版本
	if (ChessManualHead.wVersion!=VER_MANUAL)
	{
		AfxMessageBox(TEXT("棋谱文件版本错误，棋谱打开失败！"),MB_ICONSTOP);
		return;
	}

	//效验长度
	if ((ChessManualHead.dwManualCount*sizeof(tagChessManual)+sizeof(tagChessManualHead))!=dwFileLength)
	{
		AfxMessageBox(TEXT("棋谱文件长度错误，棋谱打开失败！"),MB_ICONSTOP);
		return;
	}

	//读取棋谱
	tagChessManual * pChessManual=NULL;
	m_ChessManualInfo.SetSize(ChessManualHead.dwManualCount);
	for (DWORD i=0;i<ChessManualHead.dwManualCount;i++)
	{
		pChessManual=&m_ChessManualInfo[i];
		FileManual.Read(pChessManual,sizeof(tagChessManual));
	}

	//关闭文件
	FileManual.Close();

	//设置界面
	m_wChessStep=0;
	m_GameLogic.InitGameLogic();
	m_ChessBorad.SetChessFocus(255,255);
	m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

	//更新列表
	UpdateManualList();

	//更新控制
	UpdateControl();

	return;
}

//保存按钮
void CChessManual::OnPreserve()
{
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//选择文件
	LPCTSTR pszFilter=TEXT("围棋棋谱文件 （*.WQM）|*.WQM||");
	CFileDialog DlgSaveManual(FALSE,TEXT("WQM"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgSaveManual.m_pOFN->lpstrInitialDir=szPath;
	if (DlgSaveManual.DoModal()!=IDOK) return;

	//打开文件
	CFile FileManual;
	if (FileManual.Open(DlgSaveManual.GetPathName(),CFile::modeWrite|CFile::modeCreate)==FALSE)
	{
		AfxMessageBox(TEXT("无法创建文件，棋谱保存失败！"),MB_ICONSTOP);
		return;
	}

	//文件头
	tagChessManualHead ChessManualHead;
	ChessManualHead.wVersion=VER_MANUAL;
	ChessManualHead.dwManualDate=(DWORD)time(NULL);
	ChessManualHead.dwManualCount=(DWORD)m_ChessManualInfo.GetCount();

	//写入文件头
	FileManual.SeekToBegin();
	FileManual.Write(&ChessManualHead,sizeof(ChessManualHead));

	//棋谱数据
	tagChessManual * pChessManual=NULL;
	for (DWORD i=0;i<ChessManualHead.dwManualCount;i++)
	{
		pChessManual=&m_ChessManualInfo[i];
		FileManual.Write(pChessManual,sizeof(tagChessManual));
	}

	//关闭文件
	FileManual.SetLength(sizeof(ChessManualHead)+ChessManualHead.dwManualCount*sizeof(tagChessManual));
	FileManual.Close();

	return;
}

//点目按钮
void CChessManual::OnCount()
{
	//点目判断
	if (m_ChessBorad.IsChessCount()==false)
	{
		//获取点目
		tagCountResult CountResult;
		memset(&CountResult,0,sizeof(CountResult));
		if (m_GameLogic.GetChessCountInfo(CountResult)==false)
		{
			AfxMessageBox(TEXT("棋盘上没有棋子，“点目”操作不成功"),MB_ICONQUESTION);
			return;
		}

		//设置界面
		m_ChessBorad.SetChessCountInfo(CountResult.cbCountInfo);
	}
	else m_ChessBorad.CleanCountInfo();

	return;
}

//刷新按钮
void CChessManual::OnReLoad()
{
	//更新棋谱
	INT_PTR nManualCount=m_pDlgGameLogic->m_ChessManualInfo.GetCount();
	m_ChessManualInfo.SetSize(m_pDlgGameLogic->m_ChessManualInfo.GetCount());
	CopyMemory(m_ChessManualInfo.GetData(),m_pDlgGameLogic->m_ChessManualInfo.GetData(),sizeof(tagChessManual)*nManualCount);

	//棋盘控制
	m_wChessStep=0;
	m_GameLogic.InitGameLogic();

	//更新列表
	UpdateManualList();

	//走到最后
	if (nManualCount==0)
	{
		//设置界面
		m_ChessBorad.SetChessFocus(255,255);
		m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

		//更新控制
		UpdateControl();
	}
	else OnLast();

	return;
}

//最前按钮
void CChessManual::OnFirst()
{
	//设置界面
	m_wChessStep=0;
	m_GameLogic.InitGameLogic();
	m_ChessBorad.SetChessFocus(255,255);
	m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

	//更新控制
	UpdateControl();

	return;
}

//上手按钮
void CChessManual::OnBefore()
{
	//悔棋控制
	ASSERT(m_wChessStep>0);
	m_GameLogic.RegretChess(1);

	//减少步数
	m_wChessStep--;

	//界面设置
	m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

	if (m_wChessStep>0)
	{
		tagChessManual * pChessManual=&m_ChessManualInfo[m_wChessStep-1];
		m_ChessBorad.SetChessFocus(pChessManual->cbXPos,pChessManual->cbYPos);
	}
	else m_ChessBorad.SetChessFocus(255,255);

	//更新控制
	UpdateControl();

	return;
}

//下手按钮
void CChessManual::OnNext()
{
	//获取棋谱
	ASSERT(m_ChessManualInfo.GetCount()>m_wChessStep);
	tagChessManual * pChessManual=&m_ChessManualInfo[m_wChessStep];

	//放置棋子
	WORD wDeadChess[361],wDeadChessCount=0;
	WORD wIdentifier=m_GameLogic.PlaceChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor,wDeadChess,wDeadChessCount);

	//增加步数
	m_wChessStep++;

	//设置棋盘
	m_ChessBorad.SetChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor,wIdentifier);
	if (wDeadChessCount>0) m_ChessBorad.CleanChess(wDeadChess,wDeadChessCount);

	//设置框架
	m_ChessBorad.SetChessFocus(pChessManual->cbXPos,pChessManual->cbYPos);

	//更新控制
	UpdateControl();

	return;
}

//最后按钮
void CChessManual::OnLast()
{
	//走棋设置
	tagChessManual * pChessManual=NULL;
	WORD wDeadChess[361],wDeadChessCount=0;
	for (WORD i=m_wChessStep;i<m_ChessManualInfo.GetCount();i++)
	{
		pChessManual=&m_ChessManualInfo[i];
		m_GameLogic.PlaceChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor,wDeadChess,wDeadChessCount);
	}

	//增加步数
	m_wChessStep=(WORD)m_ChessManualInfo.GetCount();

	//设置棋盘
	m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

	//设置框架
	m_ChessBorad.SetChessFocus(pChessManual->cbXPos,pChessManual->cbYPos);

	//更新控制
	UpdateControl();

	return;
}

//////////////////////////////////////////////////////////////////////////
