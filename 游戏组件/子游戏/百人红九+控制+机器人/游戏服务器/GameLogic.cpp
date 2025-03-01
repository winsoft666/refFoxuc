#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
const BYTE CGameLogic::m_cbCardListData[CARD_COUNT]=
{
	0x0C,0x2C,
	0x1B,0x3B,
	0x0A,0x1A,0x2A,0x3A,
	0x09,0x29,
	0x07,0x17,0x27,0x37,
	0x08,0x18,0x28,0x38,
	0x06,0x16,0x26,0x36,
	0x05,0x25,
	0x04,0x14,0x24,0x34,
	0x02,0x22,
	0x31,
	0x33
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//获取牌型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//合法判断
	ASSERT(2==cbCardCount);
	if (2!=cbCardCount) return CT_ERROR;

	//排序扑克
	BYTE cbCardDataSort[CARD_COUNT];
	CopyMemory(cbCardDataSort,cbCardData,sizeof(BYTE)*cbCardCount);
	SortCardList(cbCardDataSort,cbCardCount,ST_LOGIC);

	//获取点数
	BYTE cbFirstCardValue=GetCardValue(cbCardDataSort[0]);
	BYTE cbSecondCardValue=GetCardValue(cbCardDataSort[1]);

	//获取花色
	BYTE cbFistCardColor=GetCardColor(cbCardDataSort[0]);
	BYTE cbSecondCardColor=GetCardColor(cbCardDataSort[1]);

	//特殊牌型
	if (12==cbFirstCardValue && cbFirstCardValue==cbSecondCardValue) return CT_SPECIAL_1;    	

	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 2==cbFirstCardValue) return CT_SPECIAL_2; 

	if ((3+3)==(cbFistCardColor+cbSecondCardColor) && (1==cbFirstCardValue && 3==cbSecondCardValue || 3==cbFirstCardValue && 1==cbSecondCardValue)) return CT_SPECIAL_3; 

	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 8==cbFirstCardValue) return CT_SPECIAL_4; 

	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 4==cbFirstCardValue) return CT_SPECIAL_5; 

	if ((1+3)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 10==cbFirstCardValue) return CT_SPECIAL_6; 
	if ((1+3)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 6==cbFirstCardValue) return CT_SPECIAL_6; 
	if ((1+3)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 4==cbFirstCardValue) return CT_SPECIAL_6; 

	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 6==cbFirstCardValue) return CT_SPECIAL_7;
	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 7==cbFirstCardValue) return CT_SPECIAL_7;
	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 10==cbFirstCardValue) return CT_SPECIAL_7;
	if ((1+3)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 11==cbFirstCardValue) return CT_SPECIAL_7;

	if ((1+3)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 7==cbFirstCardValue) return CT_SPECIAL_8;
	if ((1+3)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 8==cbFirstCardValue) return CT_SPECIAL_8;
	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 9==cbFirstCardValue) return CT_SPECIAL_8;
	if ((0+2)==(cbFistCardColor+cbSecondCardColor) && cbFirstCardValue==cbSecondCardValue && 5==cbFirstCardValue) return CT_SPECIAL_8;

	if (12==cbFirstCardValue && 9==cbSecondCardValue) return CT_SPECIAL_9;

	if (12==cbFirstCardValue && 8==cbSecondCardValue) return CT_SPECIAL_10;

	if (2==cbFirstCardValue && 8==cbSecondCardValue) return CT_SPECIAL_11;

	//点数牌型
	return CT_POINT;
}

//大小比较
/*
cbNextCardData>cbFirstCardData  返回1
cbNextCardData<cbFirstCardData  返回-1
cbNextCardData==cbFirstCardData 返回0
*/
int CGameLogic::CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount)
{
	//合法判断
	ASSERT(2==cbFirstCardCount && 2==cbNextCardCount);
	if (!(2==cbFirstCardCount && 2==cbNextCardCount)) return 0;

	//获取牌型
	BYTE cbFirstCardType=GetCardType(cbFirstCardData, cbFirstCardCount);
	BYTE cbNextCardType=GetCardType(cbNextCardData, cbNextCardCount);

	//牌型比较
	if (cbFirstCardType != cbNextCardType) 
	{
		if (cbNextCardType > cbFirstCardType) return 1;
		else return -1;
	}

	//特殊牌型判断
	if (CT_POINT!=cbFirstCardType && cbFirstCardType==cbNextCardType)
	{
		return 0;
	}

	//获取点数
	BYTE cbFirstPip=GetCardListPip(cbFirstCardData, cbFirstCardCount);
	BYTE cbNextPip=GetCardListPip(cbNextCardData, cbNextCardCount);

	//点数比较
	if (cbFirstPip != cbNextPip)
	{
		if (cbNextPip > cbFirstPip) return 1;
		else return -1;
	}

	//零点判断
	if (0==cbFirstPip && 0==cbNextPip) return 0;

	//排序扑克
	BYTE cbFirstCardDataTmp[CARD_COUNT], cbNextCardDataTmp[CARD_COUNT];
	CopyMemory(cbFirstCardDataTmp,cbFirstCardData,sizeof(BYTE)*cbFirstCardCount);
	CopyMemory(cbNextCardDataTmp,cbNextCardData,sizeof(BYTE)*cbNextCardCount);
	SortCardList(cbFirstCardDataTmp,cbFirstCardCount,ST_LOGIC);
	SortCardList(cbNextCardDataTmp,cbNextCardCount,ST_LOGIC);

	//相等判断
	if (GetCardLogicValue(cbFirstCardDataTmp[0])==GetCardLogicValue(cbNextCardDataTmp[0])) return 0;
	else if (GetCardLogicValue(cbNextCardDataTmp[0]) > GetCardLogicValue(cbFirstCardDataTmp[0])) return 1;
	else -1;

	return 0;
}

//获取牌点
BYTE CGameLogic::GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	BYTE cbPipCount=0;

	//获取牌点
	BYTE cbCardValue=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		cbCardValue=GetCardValue(cbCardData[i]);
		cbPipCount+=(1==cbCardValue ? 6 : cbCardValue);
	}

	return (cbPipCount%10);
}

//逻辑大小
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//获取花色
	BYTE cbColor=GetCardColor(cbCardData);

	//获取数值
	BYTE cbValue=GetCardValue(cbCardData);

	//返回逻辑值
	if (12==cbValue && (0==cbColor || 2==cbColor)) return 8;
	
	if (2==cbValue && (0==cbColor || 2==cbColor)) return 7;
	
	if (8==cbValue && (0==cbColor || 2==cbColor)) return 6;
	
	if (4==cbValue && (0==cbColor || 2==cbColor)) return 5;

	if ((1==cbColor || 3==cbColor) && (10==cbValue || 6==cbValue || 4==cbValue)) return 4;

	if ((0==cbColor || 2==cbColor) && (10==cbValue || 6==cbValue || 7==cbValue)) return 3;
	if ((1==cbColor || 3==cbColor) && 11==cbValue) return 3;

	if ((1==cbColor || 3==cbColor) && (7==cbValue || 8==cbValue)) return 2;
	if ((0==cbColor || 2==cbColor) && (5==cbValue || 9==cbValue)) return 2;

	if (3==cbColor && (1==cbValue || 3==cbValue)) return 1;

	return 0;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[CARD_COUNT];
	if (ST_VALUE==cbSortType)
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardValue(cbCardData[i]);	
	}
	else 
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	
	}

	//排序操作
	bool bSorted=true;
	BYTE cbThreeCount,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbThreeCount=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbThreeCount;
				cbThreeCount=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbThreeCount;
				bSorted=false;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return;
}

//////////////////////////////////////////////////////////////////////////

