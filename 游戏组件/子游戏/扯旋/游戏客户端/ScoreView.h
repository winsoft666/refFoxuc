#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//结束信息
struct tagScoreViewInfo
{
	WORD							wGameScore;							//游戏得分
};

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView : public CWnd
{
	//积分信息
protected:
	LONGLONG						m_lTax[GAME_PLAYER];				//游戏税收
	LONGLONG						m_lAddScore[GAME_PLAYER];			//加注数目
	LONGLONG						m_lScore[GAME_PLAYER];				//游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][LEN_NICKNAME];//用户名字

	//资源变量
protected:
	CBitImage						m_ImageBack;						//背景图案
	CBitImage						m_ImageWinLose;						//胜负标志

	//函数定义
public:
	//构造函数
	CScoreView();
	//析构函数
	virtual ~CScoreView();
	
	//功能函数
public:
	//设置积分
	void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore,LONGLONG lAddScore);
	//设置税收
	void SetGameTax(LONGLONG lTax,WORD wChairID);
	//清理数据
	void ResetScore();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMove(int x, int y);
};

//////////////////////////////////////////////////////////////////////////

#endif
