// DlgBank.cpp : implementation file
//

#include "stdafx.h"
#include "DlgBank.h"
#include "resource.h"
#include ".\dlgbank.h"

//定时器I D
#define IDI_CHARMVALUE_UPDATE_VIEW		1								//更新界面
#define TIME_CHARMVALUE_UPDATE_VIEW		200								//更新界面


void MakeString(CString &strNum,LONGLONG lNumber)
{
	CString static strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
	strTmpNumber1.Empty();
	strTmpNumber2.Empty();
	strNumber.Empty();
	if (lNumber==0) strNumber=TEXT("0");
	int nNumberCount=0;
	LONGLONG lTmpNumber=lNumber;
	if (lNumber<0) lNumber=-lNumber;
	bool blfirst = true;
	bool bLongNum = false;
	if(lNumber>=100)
	{
		bLongNum = true;
	}
	while (lNumber>0)
	{

		strTmpNumber1.Format(TEXT("%I64d"),lNumber%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if (nNumberCount==3)
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

			strNumber=strTmpNumber2;
			nNumberCount=0;
			strTmpNumber2=TEXT("");
		}
		lNumber/=10;
	}

	if (strTmpNumber2.IsEmpty()==FALSE)
	{
		if(bLongNum)
			strTmpNumber2 += (TEXT(",") +strNumber);
		else
			strTmpNumber2 += strNumber;

		strNumber=strTmpNumber2;
	}

	if (lTmpNumber<0) strNumber=TEXT("-")+strNumber;

	strNum = strNumber ;


}

void MakeStringToNum(CString str,LONGLONG &Num)
{

	CString str1;
	int Pos = 0;
	int len = 0;
	int Start = 0;
	while (1)
	{
		len = str.GetLength();
		Pos = str.Find(_T(","),Pos);
		if(Pos==-1)
		{
			str1+=str;
			break;
		}else
		{
			char Tmp[126];
			memset(Tmp,0,126);
			memcpy(Tmp,&str.GetBuffer()[Start],Pos);
			if(Pos+1<=str.GetLength())
			{
				str = &str.GetBuffer()[Pos+1];
				str1+=Tmp;
				Start =  0;
				Pos = 0;
			}
			else break;
		}
	}

	_sntscanf(str1.GetBuffer(),str1.GetLength(),_TEXT("%I64d"),&Num);
}

IMPLEMENT_DYNAMIC(CDlgBank, CSkinDialog)
CDlgBank::CDlgBank(CWnd* pParent /*=NULL*/)
	: CSkinDialog(IDD_BANK_STORAGE, pParent)
{
	m_lInCount=0;
	m_lGameGold=0;
	m_bCanDraw=true;
//	m_bDrawAllGold=false;
//	m_bStorageAllGold=false;
	m_lStorageGold=0;
	m_pMeUserData=NULL;
	m_pIClientKernel=NULL;
	m_bBankStorage=true;
}

CDlgBank::~CDlgBank()
{
}

void CDlgBank::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_USER_PASSWORD, m_Edit);
	DDX_Control(pDX, IDC_STATIC_PASS, m_Static);
}


BEGIN_MESSAGE_MAP(CDlgBank, CSkinDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()	
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK,OnBnClickedOk)
	ON_EN_CHANGE(IDC_IN_COUNT, OnEnChangeInCount)
END_MESSAGE_MAP()


void CDlgBank::OnEnRadioStore()
{
	SetBankActionType(true);
}

void CDlgBank::OnEnRadioGet()
{

	SetBankActionType(false);
}

void CDlgBank::AllowStorage(bool bAllow)
{
	m_blCanStore=bAllow;
	ShowItem();
}

void CDlgBank::AllowDraw(bool bAllow)
{
    m_bCanDraw=bAllow;
	ShowItem();
}

//提取按钮
void CDlgBank::OnBnClickedOk()
{

	CString strinCount;
	GetDlgItemText(IDC_IN_COUNT,strinCount);
	MakeStringToNum(strinCount,m_lInCount);

	if(((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck()==1)
	{
		if(!m_bBankStorage)
			m_lInCount=m_pMeUserData->GetUserInsure();
		else
			m_lInCount=m_pMeUserData->GetUserScore();
	}
	
	//参数验证
	if (false==m_bBankStorage && (m_lInCount <= 0 || m_lInCount > m_lStorageGold ))
	{
		CString strMessage;
		if(m_lStorageGold>0)strMessage.Format(TEXT("你输入的游戏币值必须在1和%I64d之间"), m_lStorageGold);
		else strMessage.Format(TEXT("你的存储游戏币数目为0,不能进行提取操作!"));
		MessageBox(strMessage,TEXT("温馨提示"));

		GetDlgItem(IDC_IN_COUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(0,-1);
		SetDlgItemText(IDC_USER_PASSWORD,TEXT(""));
		return;
	}


	//参数验证
	if (true==m_bBankStorage && (m_lInCount <= 0 || m_lInCount > m_lGameGold ))
	{
		CString strMessage;
		if(m_lGameGold>0)strMessage.Format(TEXT("你输入的游戏币值必须在1和%I64d之间"),m_lGameGold );
		else strMessage.Format(TEXT("你的当前游戏币数目为0,不能进行存储操作!"));
		MessageBox(strMessage,TEXT("温馨提示"));

		GetDlgItem(IDC_IN_COUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(0,-1);
		SetDlgItemText(IDC_USER_PASSWORD,TEXT(""));
		return;
	}
	//密码验证
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_USER_PASSWORD,szPassword,CountArray(szPassword));
	
	if (szPassword[0]==0&&m_blUsingPassWord)
	{
		MessageBox(TEXT("密码不能为空，请重新输入密码！"),TEXT("温馨提示"));
		GetDlgItem(IDC_USER_PASSWORD)->SetWindowText(TEXT(""));
		GetDlgItem(IDC_USER_PASSWORD)->SetFocus();
		return;
	}
	
//	if(m_blUsingPassWord)
//	{
//		TCHAR szTempPassword[PASS_LEN]=TEXT("");
//		CopyMemory(szTempPassword,szPassword,sizeof(szTempPassword));
//		CMD5Encrypt::EncryptData(szTempPassword,szPassword);
//	}
//
//	if (false==m_bBankStorage)
//	{
//		//发送信息
//		CMD_GF_BankGet BankGetGold;
//		ZeroMemory(&BankGetGold, sizeof(BankGetGold));
//		BankGetGold.lGetValue = (LONGLONG)m_lInCount;
//		BankGetGold.cbGameAction=TRUE;
//		CopyMemory(BankGetGold.szPassword,szPassword,sizeof(BankGetGold.szPassword));
//		SendSocketData(MDM_GF_BANK, SUB_GF_BANK_GET, &BankGetGold, sizeof(BankGetGold));
//	}
//	else
//	{
//		//状态判断
//		if (US_PLAY==m_pMeUserData->cbUserStatus)
//		{
//			MessageBox(TEXT("请结束游戏后再存款！"),TEXT("温馨提示"));
//			return;	
//		}
//		//发送信息
//		CMD_GF_BankStorage BankStorage;
//		ZeroMemory(&BankStorage, sizeof(BankStorage));
//		BankStorage.cbGameAction=TRUE;
//		BankStorage.lStorageValue = (LONGLONG)m_lInCount;
////		CopyMemory(BankStorage.szPassword,szPassword,sizeof(BankStorage.szPassword));
//		SendSocketData(MDM_GF_BANK, SUB_GF_BANK_STORAGE, &BankStorage, sizeof(BankStorage));
//	}

	SetDlgItemText(IDC_USER_PASSWORD,TEXT(""));
	return;
}

//初始化函数
BOOL CDlgBank::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	UpdateData(FALSE);
	SetWindowText(TEXT("银行"));
	SetBankActionType(true);
	//定时更新
	SetTimer(IDI_CHARMVALUE_UPDATE_VIEW,TIME_CHARMVALUE_UPDATE_VIEW,NULL);
	return TRUE;
}

//更新界面
void CDlgBank::UpdateView()
{
	CString strinCount;
	GetDlgItemText(IDC_IN_COUNT,strinCount);
	LONGLONG OutNum = 0;
	MakeStringToNum(strinCount,OutNum);

	if(m_OrInCount!= OutNum)
	{
		m_OrInCount = OutNum;
		if(OutNum!=0)
		{
			MakeString(strinCount,OutNum);
			SetDlgItemText(IDC_IN_COUNT,strinCount);
			((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(strinCount.GetLength(),strinCount.GetLength(),TRUE); 
		}
		//参数验证
		if ((OutNum < 0 || OutNum > m_lStorageGold ))
			return;	
	}
	UpdateData(TRUE);

	if(m_pMeUserData==NULL)return;

	//设置信息
	m_lGameGold=((m_pMeUserData->GetUserScore()<0)?0:m_pMeUserData->GetUserScore());
	m_lStorageGold=m_pMeUserData->GetUserInsure();

	CString strlGameGold;

	MakeString(strlGameGold,m_lGameGold);
	SetDlgItemText(IDC_GAME_GOLD,strlGameGold);

	CString strlStorageGold;
	MakeString(strlStorageGold,m_lStorageGold);
	SetDlgItemText(IDC_STORAGE_GOLD,strlStorageGold);

	if(((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck()==1)
	{
		if(!m_bBankStorage)
			SetDlgItemText(IDC_IN_COUNT,strlStorageGold);
		else
			SetDlgItemText(IDC_IN_COUNT,strlGameGold);
	}

	UpdateData(FALSE);
}

//鼠标信息
void CDlgBank::OnLButtonDown(UINT nFlags, CPoint point)
{
	CSkinDialog::OnLButtonDown(nFlags, point);
}

//鼠标信息
void CDlgBank::OnMouseMove(UINT nFlags, CPoint point)
{
	CSkinDialog::OnMouseMove(nFlags, point);
}

//命令信息
BOOL CDlgBank::PreTranslateMessage(MSG* pMsg)
{
	//过滤字母
	if(GetFocus()==GetDlgItem(IDC_IN_COUNT)&& pMsg->message==WM_CHAR)
	{
		BYTE bMesValue = (BYTE)(pMsg->wParam);
		BYTE bTemp = bMesValue-'0';
		if((bTemp<0 || bTemp>9) && bMesValue!=VK_BACK) return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

//时间消息
void CDlgBank::OnTimer(UINT nIDEvent)
{
	//时间消息
	switch(nIDEvent)
	{
	case IDI_CHARMVALUE_UPDATE_VIEW:		//更新界面
		{
			//更新界面
			UpdateView();
			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//输入信息
void CDlgBank::OnEnChangeInCount()
{
	CString strCount;
	GetDlgItem(IDC_IN_COUNT)->GetWindowText(strCount);
	m_lInCount=_tstoi64(strCount);

	return ;
}

//是否选中
bool CDlgBank::IsButtonSelected(UINT uButtonID)
{
	return ((CButton *)GetDlgItem(uButtonID))->GetCheck()==BST_CHECKED;
}

int CDlgBank::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	return 0;
}

//选中按钮
void CDlgBank::SetButtonSelected(UINT uButtonID, bool bSelected)
{
	CButton * pButton=(CButton *)GetDlgItem(uButtonID);
	if (bSelected) pButton->SetCheck(BST_CHECKED);
	else pButton->SetCheck(BST_UNCHECKED);
	return;
}

//设置信息
void CDlgBank::SetSendInfo(IClientKernel *pClientKernel,IClientUserItem *pMeUserData)
{
	ASSERT(pClientKernel!=NULL);

	//设置信息
	m_pIClientKernel = pClientKernel;
	m_pMeUserData=(pMeUserData);

	return;
}
void CDlgBank::SetPostPoint(CPoint Point)
{
	CRect Rect;
	GetWindowRect(&Rect);
	SetWindowPos(NULL,Point.x,Point.y,Rect.right-Rect.left,Rect.bottom-Rect.top,SWP_NOZORDER);
}
//发送信息
void CDlgBank::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	ASSERT(m_pIClientKernel!=NULL);

	//发送信息
	m_pIClientKernel->SendSocketData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);

	return ;
}

void CDlgBank::ShowItem()
{
	
	if(m_bBankStorage)
	{
		GetDlgItem(IDC_IN_COUNT)->EnableWindow(TRUE);
		GetDlgItem(IDC_USER_PASSWORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PASS)->ShowWindow(SW_HIDE);
	    GetDlgItem(IDC_USER_PASSWORD)->EnableWindow(false);
		m_btOK.EnableWindow(m_blCanStore);
	}

	else
	{
		GetDlgItem(IDC_IN_COUNT)->EnableWindow(TRUE);
		GetDlgItem(IDC_USER_PASSWORD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_PASS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_USER_PASSWORD)->EnableWindow(TRUE);
		m_btOK.EnableWindow(m_bCanDraw);
	}

}
//设置类型
void CDlgBank::SetBankActionType(bool bStorage) 
{
	//设置变量
	m_bBankStorage=bStorage;
	//设置标题
	if (m_bBankStorage)
	{
//		m_bDrawAllGold=((CButton*)(GetDlgItem(IDC_CHECK_ALL)))->GetCheck()==1?true:false;
	//	((CButton*)(GetDlgItem(IDC_CHECK_ALL)))->SetCheck(m_bStorageAllGold?1:0);
		SetDlgItemText(IDC_IN_COUNT,_T(""));
		SetDlgItemText(IDC_USER_PASSWORD,_T(""));
		GetDlgItem(IDOK)->SetWindowText(TEXT("存款"));
		GetDlgItem(IDC_CHECK_ALL)->SetWindowText(_T("存入所有金币"));
		m_blUsingPassWord = false;
	}
	else
	{
	//	m_bStorageAllGold=((CButton*)(GetDlgItem(IDC_CHECK_ALL)))->GetCheck()==1?true:false;
	//	((CButton*)(GetDlgItem(IDC_CHECK_ALL)))->SetCheck(m_bDrawAllGold?1:0);
		SetDlgItemText(IDC_IN_COUNT,_T(""));
		SetDlgItemText(IDC_USER_PASSWORD,_T(""));
		GetDlgItem(IDOK)->SetWindowText(TEXT("取款"));
		GetDlgItem(IDC_CHECK_ALL)->SetWindowText(_T("取出所有金币"));
		m_blUsingPassWord = true;
	}
	ShowItem();
}

void CDlgBank::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(0);
	CSkinDialog::OnCancel();
}
