#pragma once

//////////////////////////////////////////////////////////////////////////

void MakeString(CString &strNum,SCORE lNumber);
void MakeStringToNum(CString str,SCORE &Num);

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// CDlgBank dialog
#define  WM_CREADED						WM_USER+1
class CDlgBank : public CSkinDialog
{
	//变量定义
public:
	bool							m_bBankStorage;					//存储标识
	IClientUserItem					*m_pMeUserData;					//用户信息
	SCORE							m_lGameGold;					//银行数目
	SCORE							m_lStorageGold;					//存储数目
	SCORE							m_lInCount;						//输入游戏币
	SCORE							m_OrInCount;                    //
	bool							m_blCanStore;                   //能存
	bool                            m_blCanGetOnlyFree;             //仅空闲能取

	//控件变量
protected:
	CSkinButton						m_btOK;							//确定按钮
	CSkinButton						m_btCancel;						//取消按钮
	CSkinButton						m_btFresh;						//刷新按钮
	CEdit							m_Edit;
	CStatic							m_Static;
	bool							m_blUsingPassWord;
	CPngImage						m_ImageFrame;					//	
	CPngImage						m_ImageNumber;

	//接口变量
protected:	
	IClientKernel					* m_pIClientKernel;				//内核接口

public:
	CDlgBank(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBank();

	//辅助函数
public:
	//更新界面
	void UpdateView();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定按钮
	afx_msg void OnBnClickedOk();
	//鼠标信息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标信息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//命令信息
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//输入信息
	afx_msg void OnEnChangeInCount();

	afx_msg void OnEnRadioStore();

	afx_msg void OnEnRadioGet();
	//勾选
	afx_msg void OnBnClickedCheck();
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

	

    int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg LRESULT  OnCreateEd(WPARAM wparam,LPARAM lparam);

	//接口函数
public:
	//设置类型
	void SetBankerActionType(bool bStorage);

	void SetPostPoint(CPoint Point);

	void ShowItem();

	void HideStorage();	

	//功能函数
public:
	//是否选中
	bool IsButtonSelected(UINT uButtonID);
	//选中按钮
	void SetButtonSelected(UINT uButtonID, bool bSelected);
	//设置组件
	void SetSendInfo(IClientKernel *pClientKernel,IClientUserItem const*pMeUserDatas);
	//发送信息
	void SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);
	void OnCancelEvent();
	//绘画数字
	VOID DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgBank)
protected:
	virtual void OnCancel();
	virtual void OnOK();
};
