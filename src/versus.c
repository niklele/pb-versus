#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "stdlib.h"

#define MY_UUID { 0xD9, 0xF7, 0x28, 0xD3, 0x27, 0xD9, 0x4E, 0xF4, 0x8B, 0x6C, 0xA4, 0xAB, 0x26, 0xB0, 0xB0, 0xDF }
PBL_APP_INFO(MY_UUID,
			 "Versus", "Nikhil Lele",
			 0, 1, /* App version */
			 DEFAULT_MENU_ICON,
			 APP_INFO_STANDARD_APP);

Window window;

#define SCORE_LEN 16 // max number of chars for display, include null-terminator
#define MAX_SCORE 0xffff

TextLayer scoreLayer_1;
TextLayer scoreLayer_2;

TextLayer scoreLabel_1;
TextLayer scoreLabel_2;

char scoreText_1[SCORE_LEN];
char scoreText_2[SCORE_LEN];
uint16_t score_1;
uint16_t score_2;

// From Minh Nguyen at http://stackoverflow.com/questions/3440726
// Adapted to constant radix of 10
#define RADIX 10
void itoa(int value, char *sp)
{
    char tmp[SCORE_LEN]; // be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;
    int sign;

    sign = (RADIX == 10 && value < 0);
    if (sign)   v = -value;
    else    v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % RADIX;
        v /= RADIX; // v/=radix uses less CPU clocks than v=v/radix does
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    if (sign)
    *sp++ = '-';
    while (tp > tmp)
    *sp++ = *--tp;
}

void updateText_1() {
	itoa(score_1, scoreText_1);
	text_layer_set_text(&scoreLayer_1, scoreText_1);
}

void updateText_2() {
	itoa(score_2, scoreText_2);
	text_layer_set_text(&scoreLayer_2, scoreText_2);
}

void initTextLayers() {

	// Score Label 1
	text_layer_init(&scoreLabel_1, GRect(100, 40, 50 /* width */, 25 /* height */));
	text_layer_set_font(&scoreLabel_1, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_alignment(&scoreLabel_1, GTextAlignmentLeft);
	text_layer_set_text(&scoreLabel_1, "HOME");
	layer_add_child(&window.layer, &scoreLabel_1.layer);

	// Score 1
	text_layer_init(&scoreLayer_1, GRect(10, 0, 130 /* width */, 43 /* height */)); // w 144
	text_layer_set_font(&scoreLayer_1, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
	text_layer_set_text_alignment(&scoreLayer_1, GTextAlignmentRight);
	updateText_1();
	layer_add_child(&window.layer, &scoreLayer_1.layer);

	// Score 2
	text_layer_init(&scoreLayer_2, GRect(10, 100, 130 /* width */, 43 /* height */));
	text_layer_set_font(&scoreLayer_2, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
	text_layer_set_text_alignment(&scoreLayer_2, GTextAlignmentRight);
	updateText_2();
	layer_add_child(&window.layer, &scoreLayer_2.layer);

	// Score Label 2
	text_layer_init(&scoreLabel_2, GRect(100, 80, 50 /* width */, 25 /* height */));
	text_layer_set_font(&scoreLabel_2, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_alignment(&scoreLabel_2, GTextAlignmentLeft);
	text_layer_set_text(&scoreLabel_2, "AWAY");
	layer_add_child(&window.layer, &scoreLabel_2.layer);

}

/* Button Handlers */

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void)recognizer;
	(void)window;

	if (score_1 < MAX_SCORE) score_1++;
	updateText_1();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void)recognizer;
	(void)window;

	if (score_2 < MAX_SCORE) score_2++;
	updateText_2();
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void)recognizer;
	(void)window;
}

void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void)recognizer;
	(void)window;
	score_1 = 0;
	score_2 = 0;
	scoreText_1[0] = '0';
	scoreText_1[1] = '\0';
	scoreText_2[0] = '0';
	scoreText_2[1] = '\0';
	text_layer_set_text(&scoreLayer_1, scoreText_1);
	text_layer_set_text(&scoreLayer_2, scoreText_2);
}

void click_config_provider(ClickConfig **config, Window *window) {
	(void)window;

	config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

	config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

	config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
	config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

	config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
	config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

void handle_init(AppContextRef ctx) {
	(void)ctx;

	window_init(&window, "Versus");
	window_stack_push(&window, true /* Animated */);

	score_1 = 0;
	score_2 = 0;

	initTextLayers();

	// Attach our desired button functionality
	window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
	.init_handler = &handle_init
	};

	app_event_loop(params, &handlers);
}
