/*
 * wangjian<wangjian@minigui.org>
 */

#ifndef __SIZE_240_320_H
#define __SIZE_240_320_H

#define SCREEN_W        480
#define SCREEN_H        272

/*the size of soft key board window*/
#define SKB_WIN_W       240
#define SKB_WIN_H       260
 
#define SKB_CLOSE_L     0
#define SKB_CLOSE_T     0
#define SKB_CLOSE_R     0
#define SKB_CLOSE_B     0 

#define SKB_SW_L        10
#define SKB_SW_T        1  
#define SKB_SW_R        (SKB_SW_L+SKB_WIN_W)
#define SKB_SW_B        (SKB_SW_T+12)

#define SKB_VW_L		2
#define SKB_VW_T        (SKB_SW_B+1)
#define SKB_VW_R		SKB_WIN_W
#define SKB_VW_B        (SKB_VW_T+28)
    
#define SKB_VW_PU_L     (SKB_VW_L+2)
#define SKB_VW_PU_T      SKB_SW_B
#define SKB_VW_PU_R     (SKB_VW_PU_L+25)
#define SKB_VW_PU_B     (SKB_VW_PU_T+28)
 
#define SKB_VW_PD_L	    210
#define SKB_VW_PD_T     SKB_SW_B
#define SKB_VW_PD_R     (SKB_VW_PD_L+25)
#define SKB_VW_PD_B     (SKB_VW_PD_T+28) 

#define SKB_SELECT_L     35
#define SKB_SELECT_T     40
#define SKB_SELECT_R     240
#define SKB_SELECT_B     68 
   
#define SKB_KW_L        4  
#define SKB_KW_T        72 
#define SKB_KW_R        240
#define SKB_KW_B        260 

#define BUTTON_H		42
#define BUTTON_W		55
#define ROW_SPACING		5
#define COL_SPACING		4

#define  ROW1_TY		72  
#define  ROW1_BY		(ROW1_TY+BUTTON_H)  
    
#define  ROW2_TY		(ROW1_BY+ROW_SPACING)
#define  ROW2_BY		(ROW2_TY+BUTTON_H)
  
#define  ROW3_TY		(ROW2_BY+ROW_SPACING)
#define  ROW3_BY		(ROW3_TY+BUTTON_H)
 
#define  ROW4_TY		(ROW3_BY+ROW_SPACING)
#define  ROW4_BY		(ROW4_TY+BUTTON_H)


#define  COL1_TX		4  
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

#define RECT_EN_KEY_5           {COL1_TX,  ROW2_TY, COL1_BX,  ROW2_BY}
#define RECT_EN_KEY_6           {COL2_TX,  ROW2_TY, COL2_BX,  ROW2_BY}
#define RECT_EN_KEY_7           {COL3_TX,  ROW2_TY, COL3_BX,  ROW2_BY}
#define RECT_EN_KEY_8           {COL4_TX,  ROW2_TY, COL4_BX,  ROW2_BY}

#define RECT_EN_KEY_9           {COL1_TX,  ROW3_TY, COL1_BX,  ROW3_BY}
#define RECT_EN_KEY_10          {COL2_TX,  ROW3_TY, COL2_BX,  ROW3_BY}
#define RECT_EN_KEY_11          {COL3_TX,  ROW3_TY, COL3_BX,  ROW3_BY}
#define RECT_EN_KEY_12          {COL4_TX,  ROW3_TY, COL4_BX,  ROW3_BY}

#define RECT_EN_KEY_13          {COL1_TX,  ROW4_TY,COL1_BX,  ROW4_BY}
#define RECT_EN_KEY_14          {COL2_TX,  ROW4_TY,COL2_BX,  ROW4_BY}
#define RECT_EN_KEY_15          {COL3_TX,  ROW4_TY,COL3_BX,  ROW4_BY}
#define RECT_EN_KEY_16          {COL4_TX,  ROW4_TY,COL4_BX,  ROW4_BY}
 
#endif
