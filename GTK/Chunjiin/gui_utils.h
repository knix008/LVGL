#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <gtk/gtk.h>
#include "chunjiin.h"

// 폰트 관련 함수
void load_korean_font(void);
void apply_button_font(GtkWidget *button, const gchar *font_name, gint font_size);

// 버튼 텍스트 함수
const wchar_t* get_button_text(InputMode mode, int button_num);

#endif // GUI_UTILS_H
