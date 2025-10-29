/*
 * wangjian<wangjian@minigui.org>
 */

#ifndef __SIZE_240_320_H
#define __SIZE_240_320_H

#define SCREEN_W        240
//#define SCREEN_H        320
#define SCREEN_H        400	//caona

/*the size of soft key board window*/
#define SKB_WIN_W       240
#define SKB_WIN_H       149

#define SKB_CLOSE_L     215
#define SKB_CLOSE_T     4
#define SKB_CLOSE_R     240
#define SKB_CLOSE_B     26

#define SKB_VW_L        7
#define SKB_VW_T        7
#define SKB_VW_R        213
#define SKB_VW_B        28

#define SKB_VW_PU_L     7
#define SKB_VW_PU_T     7
#define SKB_VW_PU_R     30
#define SKB_VW_PU_B     28

#define SKB_VW_PD_L     190
#define SKB_VW_PD_T     7
#define SKB_VW_PD_R     213
#define SKB_VW_PD_B     28 

#define SKB_SW_L        33
#define SKB_SW_T		29 
#define SKB_SW_R        190
#define SKB_SW_B		41 

#define SKB_KW_L        0  
#define SKB_KW_T        42 
#define SKB_KW_R        240
#define SKB_KW_B        149

/***** rectangle of the keys*****/

#define  ROW1_TY                46  
#define  ROW1_BY                70   
#define  ROW2_TY                72
#define  ROW2_BY                96
#define  ROW3_TY                98
#define  ROW3_BY                122
#define  ROW4_TY                124
#define  ROW4_BY                147



//For en keyboard....
#define RECT_EN_KEY_1           {  2,  ROW1_TY,  23,  ROW1_BY}
#define RECT_EN_KEY_2           { 26,  ROW1_TY,  47,  ROW1_BY}
#define RECT_EN_KEY_3           { 50,  ROW1_TY,  71,  ROW1_BY}
#define RECT_EN_KEY_4           { 74,  ROW1_TY,  95,  ROW1_BY}
#define RECT_EN_KEY_5           { 98,  ROW1_TY, 119,  ROW1_BY}
#define RECT_EN_KEY_6           {122,  ROW1_TY, 143,  ROW1_BY}
#define RECT_EN_KEY_7           {146,  ROW1_TY, 167,  ROW1_BY}
#define RECT_EN_KEY_8           {170,  ROW1_TY, 191,  ROW1_BY}
#define RECT_EN_KEY_9           {194,  ROW1_TY, 215,  ROW1_BY}
#define RECT_EN_KEY_10          {218,  ROW1_TY, 238,  ROW1_BY}

#define RECT_EN_KEY_11          { 14,  ROW2_TY,  35,  ROW2_BY}
#define RECT_EN_KEY_12          { 38,  ROW2_TY,  59,  ROW2_BY}
#define RECT_EN_KEY_13          { 62,  ROW2_TY,  83,  ROW2_BY}
#define RECT_EN_KEY_14          { 86,  ROW2_TY, 107,  ROW2_BY}
#define RECT_EN_KEY_15          {110,  ROW2_TY, 131,  ROW2_BY}
#define RECT_EN_KEY_16          {134,  ROW2_TY, 155,  ROW2_BY}
#define RECT_EN_KEY_17          {158,  ROW2_TY, 179,  ROW2_BY}
#define RECT_EN_KEY_18          {182,  ROW2_TY, 203,  ROW2_BY}
#define RECT_EN_KEY_19          {206,  ROW2_TY, 227,  ROW2_BY}

#define RECT_EN_KEY_20          {  2,  ROW3_TY,  35, ROW3_BY}
#define RECT_EN_KEY_21          { 38,  ROW3_TY,  59, ROW3_BY}
#define RECT_EN_KEY_22          { 62,  ROW3_TY,  83, ROW3_BY}
#define RECT_EN_KEY_23          { 86,  ROW3_TY, 107, ROW3_BY}
#define RECT_EN_KEY_24          {110,  ROW3_TY, 131, ROW3_BY}
#define RECT_EN_KEY_25          {134,  ROW3_TY, 155, ROW3_BY}
#define RECT_EN_KEY_26          {158,  ROW3_TY, 179, ROW3_BY}
#define RECT_EN_KEY_27          {182,  ROW3_TY, 203, ROW3_BY}
#define RECT_EN_KEY_28          {206,  ROW3_TY, 238, ROW3_BY}

#define RECT_EN_KEY_29          {  2, ROW4_TY,  30, ROW4_BY}
#define RECT_EN_KEY_30          { 33, ROW4_TY,  61, ROW4_BY}
#define RECT_EN_KEY_31          { 64, ROW4_TY, 194, ROW4_BY}
#define RECT_EN_KEY_32          {197, ROW4_TY, 238, ROW4_BY}

//For num keyboard....
#define RECT_NUM_KEY_1          {  2,  ROW1_TY,  23,  ROW1_BY}
#define RECT_NUM_KEY_2          { 26,  ROW1_TY,  47,  ROW1_BY}
#define RECT_NUM_KEY_3          { 50,  ROW1_TY,  71,  ROW1_BY}
#define RECT_NUM_KEY_4          { 74,  ROW1_TY,  95,  ROW1_BY}
#define RECT_NUM_KEY_5          { 98,  ROW1_TY, 119,  ROW1_BY}
#define RECT_NUM_KEY_6          {122,  ROW1_TY, 143,  ROW1_BY}
#define RECT_NUM_KEY_7          {146,  ROW1_TY, 167,  ROW1_BY}
#define RECT_NUM_KEY_8          {170,  ROW1_TY, 191,  ROW1_BY}
#define RECT_NUM_KEY_9          {194,  ROW1_TY, 215,  ROW1_BY}
#define RECT_NUM_KEY_10         {218,  ROW1_TY, 238,  ROW1_BY}

#define RECT_NUM_KEY_11         {  2,  ROW2_TY,  23,  ROW2_BY}
#define RECT_NUM_KEY_12         { 26,  ROW2_TY,  47,  ROW2_BY}
#define RECT_NUM_KEY_13         { 50,  ROW2_TY,  71,  ROW2_BY}
#define RECT_NUM_KEY_14         { 74,  ROW2_TY,  95,  ROW2_BY}
#define RECT_NUM_KEY_15         { 98,  ROW2_TY, 119,  ROW2_BY}
#define RECT_NUM_KEY_16         {122,  ROW2_TY, 143,  ROW2_BY}
#define RECT_NUM_KEY_17         {146,  ROW2_TY, 167,  ROW2_BY}
#define RECT_NUM_KEY_18         {170,  ROW2_TY, 191,  ROW2_BY}
#define RECT_NUM_KEY_19         {194,  ROW2_TY, 215,  ROW2_BY}
#define RECT_NUM_KEY_20         {218,  ROW2_TY, 238,  ROW2_BY}

#define RECT_NUM_KEY_21         {  2,  ROW3_TY,  33, ROW3_BY}
#define RECT_NUM_KEY_22         { 37,  ROW3_TY,  68, ROW3_BY}
#define RECT_NUM_KEY_23         { 71,  ROW3_TY, 102, ROW3_BY}
#define RECT_NUM_KEY_24         {105,  ROW3_TY, 136, ROW3_BY}
#define RECT_NUM_KEY_25         {139,  ROW3_TY, 170, ROW3_BY}
#define RECT_NUM_KEY_26         {173,  ROW3_TY, 204, ROW3_BY}
#define RECT_NUM_KEY_27         {208,  ROW3_TY, 238, ROW3_BY}

#define RECT_NUM_KEY_28         {  2, ROW4_TY,  30, ROW4_BY}
#define RECT_NUM_KEY_29         { 33, ROW4_TY,  61, ROW4_BY}
#define RECT_NUM_KEY_30         { 64, ROW4_TY, 194, ROW4_BY}
#define RECT_NUM_KEY_31         {187, ROW4_TY, 238, ROW4_BY}

#endif
