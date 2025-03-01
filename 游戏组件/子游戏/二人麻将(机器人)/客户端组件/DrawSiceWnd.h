#ifndef DRAW_WND_HEAD_FILE
#define DRAW_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
#define IDM_SICE_FINISH			(WM_USER+401)						//扑克操作
#define IDM_CARD_MOVE			(WM_USER+402)						//移动声音

//////////////////////////////////////////////////////////////////////////

//控制窗口
class CDrawSiceWnd : public CWnd
{
	//配置变量
protected:
	CBitImage						m_ImageBack;						//背景图案
	CBitImage						m_ImageDrawSiceFirst;				//动作解释
	CBitImage						m_ImageDrawSiceSecond;				//
	CBitImage						m_ImageDrawSiceUserFirst;				//动作解释
	CBitImage						m_ImageDrawSiceUserSecond;				//动作解释
	BYTE                            m_bSicePos;
	int m_nXPos;
	int m_nYPos;
	CDC      *ParentDC;
	CGameFrameViewGDI							*m_pSinkWnd;						//通知钩子
	//函数定义
public:
	//构造函数
	CDrawSiceWnd();
	//析构函数
	virtual ~CDrawSiceWnd();

	//公共函数
public:
	//设置色子信息
	void SetSiceInfo(CDC *dc,BYTE bMoveTime,WORD wSice1,WORD wSice2);
	//设置基准位置
	void SetBenchmarkPos(int nXPos, int nYPos);
	//设置通知钩子
	void SetWindowSink(CGameFrameViewGDI *pSinkWnd);
	//停止动画
	void StopSicing(bool bFinish=true);

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
