#ifndef __COMMON_H__
#define __COMMON_H__

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "../config.h"

#define EN9_KBD_BKG               0
#define NUM9_KBD_BKG              1
#define PINYIN9_KBD_BKG           2
#define PUNCT9_KBD_BKG0           3
#define PUNCT9_KBD_BKG1           4
#define PUNCT9_KBD_BKG2           5
#define PUNCT9_KBD_BKG3           6
#define EN26_KBD_BKG              7
#define NUM26_KBD_BKG             8
#define PINYIN26_KBD_BKG          9
#define PUNCT26_KBD_BKG           10
#define CLOSE_PRESSED    		  11
#define CLOSE_NORMAL		      12
#define CHAR_KEY_MASK             13
#define FUNC_KEY_MASK             14
#define LEFT_ARROW_ENABLE         15
#define LEFT_ARROW_DISABLE        16
#define RIGHT_ARROW_ENABLE        17
#define RIGHT_ARROW_DISABLE       18
#define EN26_KBD_BKG1             19//大写字母
#define KOREAN26_KBD_BKG          20//Korean keyboard background

#define RES_BMP_SIZE  		(KOREAN26_KBD_BKG +1)


#define MGIN_SKB26IME      300
#define MGIN_SKB9IME      301

#define CN 1
#define EN 2
#define PTI_CASE_ABC                    0x01
#define PTI_CASE_Abc                    0x02
#define PTI_CASE_abc                    0x03

#define LCD_SIZE_320X240		1
#define LCD_SIZE_400X240		2
#define LCD_SIZE_240X320		3
#define LCD_SIZE_480X272		4
#define LCD_SIZE_480X800		5
#define LCD_SIZE_600X1024		6
#define LCD_SIZE_800X1280		7
#define LCD_SIZE_720X1280		8
#define LCD_SIZE_1280X720		9
#define LCD_SIZE_800X480		10


/* indicate that this key is normal character key */
#define KEY_PAD_CHAR		1

/* indicate that this key is functional key */
#define KEY_PAD_FUNC		2

/* indicate that this key is pressed */
#define KEY_PAD_PRESSED		4

/* indicate that this key shouldn't be drawed. */
#define KEY_PAD_DRAWED		8

#define KEY_PAD_HIDE		16

#define IME_OPEN			(SCANCODE_USER + 0x0153)
#define IME_CLOSE			(SCANCODE_USER + 0x0154)

#define IME_MODE_ALPHABET		(1<<0)
#define IME_MODE_PY				(1<<1)
#define IME_MODE_NUMBER    		(1<<2)
#define IME_MODE_SYMBOL			(1<<3)
#define IME_MODE_KOREAN			(1<<4)

#define SCANCODE_TOEN       (SCANCODE_USER + IME_MODE_ALPHABET)
#define SCANCODE_TOPY       (SCANCODE_USER + IME_MODE_PY)
#define SCANCODE_TONUM      (SCANCODE_USER + IME_MODE_NUMBER) 
#define SCANCODE_TOSYMBOL   (SCANCODE_USER + IME_MODE_SYMBOL) 
#define SCANCODE_TOOP       (SCANCODE_USER + IME_MODE_SYMBOL)
#define SCANCODE_TOKOREAN   (SCANCODE_USER + IME_MODE_KOREAN)

#define IME_ALLOWMODE	(IME_MODE_NUMBER | IME_MODE_PY | IME_MODE_ALPHABET | IME_MODE_SYMBOL | IME_MODE_KOREAN)
#define IME_DEFAULT		IME_MODE_ALPHABET
  
typedef struct _md_key_t {

	/* the corrosponding rect of the key pad  单个键的坐标，包括按键的具体位置和宽高*/
	RECT bound;
	
	/* the corresponging character of this key pad
	 * such as 'a','b' etc
	 */
	char key_char; 

	/* the correcponging scancode of this key pad.
	 * such as SCANCODE_F1~F3,BACKSPACE,ENTER,UP,DOWN,LEFT,RIGHT...
	 */
	int scan_code; 

	/* the style of this key pad. 
	 * KEY_CHAR indaces the keypad is a character key pad
	 * KEY_FUNC indaces the keypad is a functional key pad
	 */
	int style;

	/* private data for method update */
	void * data;

	/* this function is used to update the rect of key,
	 * which decided by key_pad. 
	 * for example:
	 * if(key->style & KEY_PAD_PRESSED) {
	 *    draw the image what key pressed.
	 * } else {
	 *    draw the image what key released.
	 * }
	 */
	void (*update)(struct _md_key_t * key, HWND hwnd);

} md_key_t;

/* indicate that page up key is visible or invisible*/
#define VW_SHOW_PU			0x10

/* indicate that page down key is visible or invisible*/
#define VW_SHOW_PD			0x20

/* indicate that page up key is pressed */
#define VW_PU_PRESSED		0x1	

/* indicate that page down key is pressed */
#define VW_PD_PRESSED		0x2	

/* indicate that elements of view window should be drawn*/
#define VW_DRAW_ELMTS		0x40

/* indicate that one element of view window was pressed */
#define VW_EL_PRESSED		0x4	

/* length of element string */
//#define VW_ELMT_LEN		    24
#define VW_ELMT_LEN		    512

/* element of view window */
typedef struct _vw_element {
	RECT bound;
	char string[VW_ELMT_LEN];
	int len;
	char* index;
}vw_element_t;

//#define VW_BUFFER_LEN	128 
//#define VW_ELMENT_NR	16

#define VW_BUFFER_LEN	512 
#define VW_ELMENT_NR	128


typedef struct _vw_add_data_t {
    PBITMAP pu_normal;
    PBITMAP pu_press;
    
    PBITMAP pd_normal;
    PBITMAP pd_press;
    
    PBITMAP sel_bk;
} vw_add_data_t;

typedef struct _view_window_t {
	/* rect of whole view window*/
	RECT bound;

	/* page up key pad */
	RECT key_pg_up;

	/* page down key pad */
	RECT key_pg_down;

	/* buffer to save the string set by IME methods*/
	char buffer[VW_BUFFER_LEN];

    PLOGFONT view_font;

	/* max string length of this view window. The max_str_len is 
	 *  used to set buffer length (also called length of one page)
	 *  in IME methods.
	 */
	int max_str_len;

	/* keeps all elements of current view window*/
	vw_element_t elements[VW_ELMENT_NR];

	/* sum of elements */
	int element_num;

	/* set elments according to str, hwnd*/
	void (*set_elements) (struct _view_window_t* view_window, HWND hwnd);

	/* clear elements */
	void (*clear_elements) (struct _view_window_t* view_window);

	/* this function is used to get element of current point*/
	vw_element_t* (*get_element) (struct _view_window_t* view_window, POINT p);

	/* the style of page up/down key. 
	 * VW_SHOW_UP indaces page up key must be shown 
	 * VW_SHOW_DOWN indaces page down key must be shown 
	 */
	int style;

	/* private data for method update */
	void * data;

	/* this function is used to update the rect of up/down key,
	 * which decided by view_window. 
	 * for example:
	 * if(view_window->style & VW_SHOW_PU) {
	 *     if(view_window->style & VW_PU_PRESSED) {
	 *        draw the image what page_up_key pressed.
	 *     } else {
	 *        draw the image what page_up_key released.
	 *     }
	 * } else if(view_window->style & VW_SHOW_PD) {
	 *     if(view_window->style & VW_PD_PRESSED) {
	 *        draw the image what page_down_key pressed.
	 *     } else {
	 *        draw the image what page_down_key released.
	 *     }
	 * }
	 */
	void (*update)(struct _view_window_t* view_window,
			HWND hwnd, vw_element_t* element);

} view_window_t;

#define SW_STR_LEN 32
typedef struct _stroke_window_t {
	/* rect of whole stroke window  */
	RECT bound;

	/*the stings will be on the stroke window*/
	char str[SW_STR_LEN];

	/* private data for method update */
	void * data;

    PLOGFONT stroke_font;

	/*this function is used to set the strings in the stroke window*/
	void (*update)(struct _stroke_window_t* stoke_window, HWND hwnd);
} stroke_window_t;


typedef struct _kw_add_data_t {
    /* for char key press */
    PBITMAP char_key_press;

    /* for func key press */
    PBITMAP func_key_press;

    /*handle of the tooltip window*/
    HWND tooltip_win;
} kw_add_data_t;

typedef struct _key_window_t {
	/* rect of whole key window  */
	RECT bound;

	/* point to the pressed postion*/
	POINT press;

	/* pointer to keys[] */
	md_key_t* key;

	/* the number of the keys[] */
	int key_num;

	/*this function is used to get the key structure .*/
	md_key_t *(*get_key) (struct _key_window_t* key_window, POINT point);

	/* private data for method update */
	void * data;

	/*this function is used to update the keyboard window*/
	void (*update) (struct _key_window_t* key_window, HWND hwnd);
} key_window_t;

#define METHOD_NAME_MAX 32
typedef int (*ime_callback)(const char *strokes, char *buffer, int buffer_len, int index, int mode);

typedef struct _md_ime_t{
	int method_id;
	char method_name[METHOD_NAME_MAX];

	ime_callback    match_keystrokes;
	ime_callback    translate_word;
	ime_callback    predict_word;
	int (*init) (void*);

} md_ime_t;

#define AC_NULL             0
#define AC_SEND_MSG         1
#define AC_SEND_EN_STRING   2
#define AC_SEND_CN_STRING   3
#define AC_CHANGE_KBD       4

typedef struct _action_t {
	int operation; //AC_SEND_MSG, AC_CHANGE_KBD...
	int message;
	int wParam;
	int lParam;
	char* str; //word to send;
    /*unused*/
	//int next_kbd_id;
}action_t;

typedef struct _key_board_t {

	int skb_win_w;
	int skb_win_h;
    /*close rect*/
    RECT close_rc;
	PBITMAP close_normal;
    PBITMAP close_press;
	/* the corrosponding rect of the view window */
	view_window_t* view_window;

	/* the corrosponding rect of the stoke window */
	stroke_window_t* stroke_window; 

	view_window_t* select_window;

	/* the corrosponding rect of the keyboard window */
	key_window_t* key_window; 

	/*the add data used to update the key window */
	void *data;

	/*this function is used to update whole keybord. Called by window process in MSG_ERASEBKGND */
	//void (*update)(struct _key_board_t* key_board, HWND hwnd, RECT* rect);
	void (*update)(struct _key_board_t* key_board, HWND hwnd, WPARAM wParam, RECT* rect);

	/*pointer of current input method operations*/
	md_ime_t* ime;

	/*actions to let hwnd do*/
	action_t action;

	
	int (*proceed_msg)(struct _key_board_t* key_board, HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

	void (*clear)(struct _key_board_t *kb);

} key_board_t;


typedef struct _SOFTKBD_DATA
{
    key_board_t* keyboard ;
    int         is_opened;
    HWND        target_hwnd;
    int         current_board_idx;
#ifdef KBD_TOOLTIP
    HWND        tooltip_win;
#endif
	int 		skb_ime;

} SOFTKBD_DATA;

typedef struct
{
	char key;
	int  case_mode;
	int  element_index;
	char szpy[10];
}St_T9key;

void softkey_reset(void);

void vw_proceed_hit(HWND hwnd, view_window_t* view_window,
		stroke_window_t* stroke_window, action_t* action,
		BOOL pressed, POINT p, int flag, md_ime_t* ime);

void vw_set_elements (struct _view_window_t* view_window, HWND hwnd);
void vw_clear_elements (struct _view_window_t* view_window);
vw_element_t* vw_get_element (struct _view_window_t* view_window, POINT p);

int symbol_proc_msg (key_board_t* key_board, HWND hwnd,
        int message, WPARAM wParam, LPARAM lParam);

void share_key_update(md_key_t *mk, HWND hWnd);
void EraseBbGround(HWND hWnd, RECT* rc);

void kp_proceed_hit(HWND hwnd, view_window_t* view_window,view_window_t* select_window,
		stroke_window_t* stroke_window, md_key_t* key, action_t* action,
		BOOL pressed, POINT p, int flag, md_ime_t* ime,WPARAM wParam, 
        LPARAM lParam);

void slw_proceed_hit(HWND hwnd, view_window_t* view_window,view_window_t* select_window,
		stroke_window_t* stroke_window, action_t* action,
		BOOL pressed, POINT p, int flag, md_ime_t* ime);

void clear_keyboard(key_board_t *kb);

void keyboard_update(key_board_t *kb, HWND hWnd, WPARAM wParam, RECT* rect);

char *getSoftkeyboardPublicPath(const char *fileName);
int get_keyboard_bitmap (HDC hdc, PBITMAP* pbmp, int id);
int load_keyboard_bitmap(int ime);
void release_keyboard_bitmap ();
int getLCDSize(void);


#endif
