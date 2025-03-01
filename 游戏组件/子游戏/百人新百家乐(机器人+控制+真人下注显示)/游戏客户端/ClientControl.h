#pragma once

#define IDM_ADMIN_COMMDN WM_USER+1000

//游戏控制基类
class IClientControlDlg : public CDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

	virtual bool __cdecl ReqResult(const void * pBuffer)=NULL;
	//更新库存
	virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
	//更新控件
	virtual void __cdecl UpdateControl() = NULL;
};
