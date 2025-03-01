#include "Stdafx.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameOption, CSkinDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameOption::CGameOption() : CSkinDialog(IDD_OPTION)
{
	m_bEnableSound=true;
	m_bDoubleMouse=false;
	m_bShowChessID=false;

	return;
}

//析构函数
CGameOption::~CGameOption()
{
}

//控件绑定
void CGameOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CGameOption::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏配置"));

	//设置控件
	if (m_bShowChessID==true) ((CButton *)GetDlgItem(IDC_SHOW_ID))->SetCheck(BST_CHECKED);
	if (m_bEnableSound==true) ((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->SetCheck(BST_CHECKED);
	if (m_bDoubleMouse==true) ((CButton *)GetDlgItem(IDC_DOUBLE_MOUSE))->SetCheck(BST_CHECKED);

	return TRUE;
}

//确定消息
void CGameOption::OnOK()
{
	//获取变量
	m_bShowChessID=(((CButton *)GetDlgItem(IDC_SHOW_ID))->GetCheck()==BST_CHECKED);
	m_bEnableSound=(((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->GetCheck()==BST_CHECKED);
	m_bDoubleMouse=(((CButton *)GetDlgItem(IDC_DOUBLE_MOUSE))->GetCheck()==BST_CHECKED);

	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////
