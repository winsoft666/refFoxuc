#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义
#define MAX_PELS					25									//最小象素
#define LESS_PELS					10									//最小象素
#define DEFAULT_PELS				20									//默认象素

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////

//X 排列方式
enum enXCollocateMode 
{ 
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

//////////////////////////////////////////////////////////////////////////

//扑克控件
class CCardControl : public CWnd
{
	//配置变量
protected:
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayHead;						//显示扑克 
	bool							m_bLookCard;						//看牌标志
	bool							m_bCompareCard;						//比牌标志
	bool							m_bCompareBack;						//比牌背景
	DWORD							m_dwCardHSpace;						//横向间隔
	WORD							m_wCardColor;						//扑克颜色
	WORD							m_wControlID;						//控件ID
	WORD							m_wMessageID;						//命令消息
	BYTE							m_bCardType;			//牌型

	//状态变量
protected:
	bool							m_bCaptureMouse;					//捕获标志
	CWHArray<BYTE>			m_CardDataArray;					//扑克数组

	//位置信息
protected:
	CRgn							m_CardRegion;						//扑克区域
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//静态变量
protected:
	static bool						m_bLoad;							//初始标志
	static CSize					m_CardSize;							//扑克大小
	static CBitImage				m_ImageCard;						//图片资源
	CPngImage						m_PngCardType;						//牌型

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//控制函数
public:
	//设置间距
	void SetCardSpace(DWORD dwCardSpace);
	//设置响应
	void SetPositively(bool bPositively);
	//显示扑克
	void SetDisplayHead(bool bDisplayHead);
	//基准位置
	void SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//基准位置
	void SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//获取位置
	CPoint GetCardPos();

	//功能函数
public:
	//获取数目
	DWORD GetCardCount();
	//设置扑克
	DWORD GetCardData(BYTE cbCardData[], DWORD dwMaxCount);
	//设置扑克
	DWORD SetCardData(const BYTE bCardData[], DWORD dwCardCount);
	//设置扑克
	void SetCardColor(WORD wColor);
	//设置扑克
	void SetLookCard(bool bLook);
	//获取设置
	bool GetLookCard(){return m_bLookCard;}
	//设置扑克
	void SetCompareCard(bool bCompare);
	//设置扑克
	void SetCommandID(WORD ControlID,WORD wMessageID);
	//设置扑克
	void SetCompareBack(bool bShow);
	//牌型
	void SetCardType(BYTE bCardType){m_bCardType=bCardType;};

	//内部函数
private:
	//调整位置
	void RectifyControl();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//鼠标左击
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif
