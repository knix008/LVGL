/*
 * wangjian<wangjian@minigui.org>
 */

#ifndef __SIZE_480_272_H
#define __SIZE_480_272_H



#define SCREEN_W        480
#define SCREEN_H        272

/*the size of soft key board window*/
#define SKB_WIN_W       480
#define SKB_WIN_H       212 

#define SKB_CLOSE_L     0
#define SKB_CLOSE_T     0
#define SKB_CLOSE_R     0
#define SKB_CLOSE_B     0 

#define SKB_SELECT_L     4
#define SKB_SELECT_T     25
#define SKB_SELECT_R     150
#define SKB_SELECT_B     48 


#define SKB_VW_L		0
#define SKB_VW_T        0
#define SKB_VW_R		432
#define SKB_VW_B        20
    
#define SKB_VW_PU_L     30
#define SKB_VW_PU_T     2
#define SKB_VW_PU_R     (SKB_VW_PU_L+20)
#define SKB_VW_PU_B     (SKB_VW_PU_T+20)
 
#define SKB_VW_PD_L	    395
#define SKB_VW_PD_T     2
#define SKB_VW_PD_R     (SKB_VW_PD_L+20)
#define SKB_VW_PD_B     (SKB_VW_PD_T+20) 


#define SKB_SW_L        180
#define SKB_SW_T        25  
#define SKB_SW_R        (SKB_SW_L+377)
#define SKB_SW_B        (SKB_SW_T+20)
   
#define SKB_KW_L        0  
#define SKB_KW_T        49 
#define SKB_KW_R        480
#define SKB_KW_B        212 

#define BUTTON_H		36
#define BUTTON_W		114
#define ROW_SPACING		5
#define COL_SPACING		5

#define  ROW1_TY		49  
#define  ROW1_BY		(ROW1_TY+BUTTON_H)
  
#define  ROW2_TY		(ROW1_BY+ROW_SPACING)
#define  ROW2_BY		(ROW2_TY+BUTTON_H) 
   
#define  ROW3_TY		(ROW2_BY+ROW_SPACING)
#define  ROW3_BY		(ROW3_TY+BUTTON_H) 
 
#define  ROW4_TY		(ROW3_BY+ROW_SPACING)
//#define  ROW4_BY		(ROW4_TY+BUTTON_H) 
#define  ROW4_BY		(ROW4_TY+BUTTON_H+5) //解决最底一行电容屏坐标跳变
    
#define  COL1_TX		5  
#define  COL1_BX		(COL1_TX+BUTTON_W)
 
#define  COL2_TX		(COL1_BX+COL_SPACING)
#define  COL2_BX		(COL2_TX+BUTTON_W)
 
#define  COL3_TX		(COL2_BX+COL_SPACING)
#define  COL3_BX		(COL3_TX+BUTTON_W)

#define  COL4_TX		(COL3_BX+COL_SPACING)
#define  COL4_BX		(COL4_TX+BUTTON_W)

 

/***** rectangle of the keys*****/
//For en keyboard....
#define RECT_EN_KEY_1           { COL1_TX,  ROW1_TY,  COL1_BX,  ROW1_BY}
#define RECT_EN_KEY_2           { COL2_TX,  ROW1_TY,  COL2_BX,  ROW1_BY}
#define RECT_EN_KEY_3           { COL3_TX,  ROW1_TY,  COL3_BX,  ROW1_BY}
#define RECT_EN_KEY_4           { COL4_TX,  ROW1_TY,  COL4_BX,  ROW1_BY}

#define RECT_EN_KEY_5           { COL1_TX,  ROW2_TY,  COL1_BX,   ROW2_BY}
#define RECT_EN_KEY_6           { COL2_TX,  ROW2_TY,  COL2_BX,  ROW2_BY}
#define RECT_EN_KEY_7           { COL3_TX,  ROW2_TY,  COL3_BX,   ROW2_BY}
#define RECT_EN_KEY_8           { COL4_TX,   ROW2_TY, COL4_BX,    ROW2_BY}

#define RECT_EN_KEY_9           { COL1_TX,  ROW3_TY,  COL1_BX,   ROW3_BY}
#define RECT_EN_KEY_10          { COL2_TX,  ROW3_TY,  COL2_BX,  ROW3_BY}
#define RECT_EN_KEY_11          { COL3_TX,  ROW3_TY,  COL3_BX,   ROW3_BY}
#define RECT_EN_KEY_12          { COL4_TX,   ROW3_TY, COL4_BX,    ROW3_BY}

#define RECT_EN_KEY_13          { COL1_TX,  ROW4_TY,  COL1_BX,   ROW4_BY}
#define RECT_EN_KEY_14          { COL2_TX,  ROW4_TY,  COL2_BX,  ROW4_BY}
#define RECT_EN_KEY_15          { COL3_TX,  ROW4_TY,  COL3_BX,   ROW4_BY}
#define RECT_EN_KEY_16          { COL4_TX,   ROW4_TY, COL4_BX,    ROW4_BY}

  
#endif

