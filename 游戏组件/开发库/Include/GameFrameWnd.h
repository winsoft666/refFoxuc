#ifndef GAME_FRAME_WND_HEAD_FILE
#define GAME_FRAME_WND_HEAD_FILE

#pragma once

#include "GameFrameHead.h"
#include "GameFrameControl.h"
#include "DlgGameRule.h"

//////////////////////////////////////////////////////////////////////////////////

//框架资源
class CFrameEncircle : public CEncircleBMP
{
	//函数定义
public:
	//构造函数
	CFrameEncircle();
	//析构函数
	virtual ~CFrameEncircle();

	//重载函数
protected:
	//绘画处理
	virtual bool PreDrawEncircleImage(tagEncircleBMP & EncircleImage);
};

//////////////////////////////////////////////////////////////////////////////////

//游戏窗口
class GAME_FRAME_CLASS CGameFrameWnd : public CFrameWnd, public IGameFrameWnd,public IFlashControlSink,public IClientPropertySink
{
	//状态变量
protected:
	bool							m_bMaxShow;							//最大标志
	bool							m_bShowControl;						//显示标志
	CRect							m_rcNormalSize;						//正常位置

	//滚动变量
protected:
	INT								m_nScrollXPos;						//滚动位置
	INT								m_nScrollYPos;						//滚动位置
	INT								m_nScrollXMax;						//最大位置
	INT								m_nScrollYMax;						//最大位置

	//组件接口
protected:
	IClientKernel *					m_pIClientKernel;					//内核接口
	IGameFrameView *				m_pIGameFrameView;					//视图接口

	//控件变量
public:
	CSkinButton						m_btMin;							//最小按钮
	CSkinButton						m_btMax;							//最大按钮
	CSkinButton						m_btClose;							//关闭按钮
	CSkinButton						m_btOption;							//选项按钮
	CSkinButton						m_btMuteContrl;						//静音按钮
	CSkinButton						m_btRule;							//旁观按钮
	CSkinScrollBar					m_SkinScrollBar;					//滚动条类

	//控件指针
protected:
	CDlgGameRule *					m_pDlgGameRule;						//游戏帮助

	//功能控件
public:
	CFlashControl					m_FlashControl;						//动画控件
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CSkinSplitter					m_SkinSplitter;						//拆分控件
	CGameFrameControl				m_GameFrameControl;					//控制框架

	//资源变量
protected:
	CFrameEncircle					m_FrameEncircle;					//框架资源

	//函数定义
public:
	//构造函数
	CGameFrameWnd();
	//析构函数
	virtual ~CGameFrameWnd();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//窗口控制
public:
	//游戏规则
	virtual bool ShowGameRule();
	//最大窗口
	virtual bool MaxSizeWindow();
	//还原窗口
	virtual bool RestoreWindow();

	//控制接口
public:
	//声音控制
	virtual bool AllowGameSound(bool bAllowSound);
	//旁观控制
	virtual bool AllowGameLookon(DWORD dwUserID, bool bAllowLookon);
	//控制接口
	virtual bool OnGameOptionChange();

	//道具接口
public:
	//播放完成
	virtual VOID OnEventFlashFinish();
	//道具成功
	virtual VOID OnPropertySuccess(LPCTSTR szTargerNickName,DWORD dwSourceUserID,DWORD dwTargerUserID,WORD wPropertyIndex,WORD wPropertyCount);
	//道具失败
	virtual VOID OnPropertyFailure( LONG lErrorCode,LPCTSTR szDescribeString);

	//重载函数
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//界面函数
protected:
	//还原窗口
//	bool RestoreWindow();
	//最大窗口
//	bool MaxSizeWindow();

	//界面控制
protected:
	//调整控件
	VOID RectifyControl(INT nWidth, INT nHeight);

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//按键测试
	UINT OnNcHitTest(CPoint Point);
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	//位置消息
	VOID OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDblClk(UINT nFlags, CPoint Point);
	//改变消息
	VOID OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	//自定消息
protected:
	//标题消息
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);

	//系统消息
public:
	//窗口销毁
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif