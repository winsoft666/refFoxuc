#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "ScoreControl.h"
#include "HistoryRecord.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//视图位置
#define MYSELF_VIEW_ID				2									//视图位置

//控制消息
#define IDM_START					(WM_USER+200)						//开始消息
#define IDM_OUT_CARD				(WM_USER+201)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+202)						//PASS消息
#define IDM_OUT_PROMPT				(WM_USER+203)						//提示消息
#define	IDM_LEFT_HIT_CARD			(WM_USER+206)						//左击扑克
#define IDM_SORT_HAND_CARD			(WM_USER+207)						//排列扑克
#define IDM_LAST_TURN_CARD			(WM_USER+208)						//上轮扑克
#define IDM_TRUSTEE_CONTROL			(WM_USER+209)						//托管控制

//////////////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//游戏变量
protected:
	LONGLONG						m_lCellScore;						//单元分数
	BYTE							m_cbMainValue;						//主牌数值
	BYTE							m_cbValueOrder[GAME_PLAYER];		//等级数值
	bool							m_bAutoPlayer[GAME_PLAYER];			//托管用户
	LONGLONG						m_TurnScore;						//该轮分数
	LONGLONG						m_PlayerScore[GAME_PLAYER];			//用户分数
public:
	bool							m_bShowFirstCard;

	//用户状态
protected:
	bool							m_bUserPass[GAME_PLAYER];			//放弃标志
	bool							m_bUserContinue[GAME_PLAYER];		//继续标志

	//游戏状态
protected:
	WORD							m_wWinOrder[GAME_PLAYER];			//胜利列表
	WORD							m_wPersistInfo[GAME_PLAYER][2];		//游戏信息
	LONGLONG						m_nLButtonUpCount;					//弹起次数

	//爆炸动画
protected:
	bool							m_bBombEffect;						//爆炸效果
	BYTE							m_cbBombFrameIndex;					//帧数索引

	//移动变量
protected:
	WORD							m_wHoverCardItem;					//盘旋索引
	WORD							m_wMouseDownItem;					//点击索引
	bool							m_bMoveMouse;						//移动标志
	bool							m_bSwitchCard;						//交换标志
	bool							m_bSelectCard;						//选择标志
	CPoint							m_ptAuto[GAME_PLAYER];

	//状态变量
public:
	bool							m_bShowScore;						//积分状态
	bool							m_bLastTurnCard;					//上轮标志

	//控制按钮
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btStustee;						//拖管控制
	CSkinButton						m_btLastTurn;						//上轮扑克
	CSkinButton						m_btSortCardCount;					//排序按钮
	CSkinButton						m_btSortCardColor;					//排序按钮
	CSkinButton						m_btSortCardOrder;					//排序按钮

	//游戏按钮
public:
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//PASS按钮
	//CSkinButton						m_btOutPrompt;						//提示按钮
	CSkinButton						m_btSort;							//理牌按钮

	//游戏控件
public:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CScoreControl					m_ScoreControl;						//积分控件

	//扑克控件
public:
	CCardControl					m_HandCardControl[GAME_PLAYER];		//用户扑克
	CCardControl					m_UserCardControl[GAME_PLAYER];		//扑克视图
	CCardControl					m_5RecordCardControl;
	CCardControl					m_10RecordCardControl;
	CCardControl					m_KRecordCardControl;
	//位置变量
protected:
	CPoint							m_ptHeap[GAME_PLAYER];				//扑克位置
	CPoint							m_ptPass[GAME_PLAYER];				//放弃位置

	//资源变量
protected:
	CBitImageEx						m_ImageNumber;						//数字资源
	CBitImageEx						m_ImageViewFill;					//背景资源
	CBitImageEx						m_ImageViewBack;					//背景资源
	CBitImageEx						m_ImageUserPass;					//放弃资源
	CBitImageEx						m_ImageBombEffect;					//炸弹效果
	CBitImageEx						m_ImageValueOrder;					//数字资源
	CBitImageEx						m_ImageStatusInfo;					//状态信息
	CBitImageEx						m_ImageLastTurnTip;					//上轮提示
	CBitImageEx						m_ImageTurnScore;
	CPngImageEx						ImageScore;
	CPngImageEx						ImageWinOrder;
	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();
	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//WIN7支持
	virtual bool RealizeWIN7() { return true; }
	//重载函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//单元积分
	bool SetCellScore(LONGLONG lCellScore);
	//主牌数值
	bool SetMainValue(BYTE cbMainValue);
	//设置爆炸
	bool SetBombEffect(bool bBombEffect);
	//设置上轮
	VOID SetLastTurnCard(bool bLastTurnCard);
	//设置放弃
	bool SetUserPass(WORD wChairID, bool bUserPass);
	//设置继续
	bool SetUserContinue(WORD wChairID, bool bContinue);
	//设置名次
	bool SetUserWinOrder(WORD wChairID, WORD wWinOrder);
	//设置信息
	bool SetUserPersist(WORD wChairID, WORD wPersistInfo[2]);
	//设置等级
	bool SetUserValueOrder(BYTE cbMySelfOrder, BYTE cbEnemyOrder);
	//设置游戏分数
	bool SetScoreInfo(LONGLONG TurnScore,LONGLONG PlayerScore[4]);
	//托管用户
	void SetAutoUser(bool bAutoUser[4]);
	//牌记录
	void Set510KRecord(BYTE bRecord[3][8]);

	//内部函数
private:
	//绘画数字
	VOID DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);

	//消息函数
public:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDblClk(UINT nFlags, CPoint Point);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif