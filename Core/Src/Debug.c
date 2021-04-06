#include <Debug.h>
#include <lcd_i2cModule.h>
#include <string.h>

char _last_line[16] = "";
int _last_row = 0;

void Debug_WriteLine(char str[16]) {
	LCD_Clear();
	_last_row++;
	if (_last_row == 3) {
		_last_row = 0;
		_last_row++;
		LCD_SetCursor(_last_row, 1);
		LCD_Send_String(_last_line, STR_NOSLIDE);
		_last_row++;
	}
	else if(_last_row==2){
		LCD_SetCursor(1, 1);
		LCD_Send_String(_last_line, STR_NOSLIDE);
	}
	LCD_SetCursor(_last_row, 1);
	LCD_Send_String(str, STR_NOSLIDE);
	strncpy(_last_line, str, 16);
}
void Debug_Clear() {
	LCD_Clear();
	_last_row = 0;

}
