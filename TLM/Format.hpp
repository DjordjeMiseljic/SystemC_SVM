#ifndef FORMAT_H
#define FORMAT_H
/* BOLD - bold default
 * DIM - dimmed default
 * UL - underlined default
 * BLINK - blinking default
 * D_RED - dark red font
 * BKG_RED - dark red background 
 * RED - red font
 * B_RED - bold red font
 * RST - reset font
 * BCK_RST -reset background 
 */

#define BOLD  "\e[1m"
#define DIM  "\e[2m"
#define UL  "\e[4m"
#define BLINK  "\e[5m"

#define BLACK "\e[30m"
#define GRAY "\e[37m"
#define D_GRAY "\e[90m"
#define WHITE "\e[97m"
#define B_GRAY "\e[1;30m"

#define D_RED "\e[31m"
#define BKG_RED "\e[41m"
#define RED "\e[91m"
#define B_RED "\e[1;31m"

#define D_GREEN "\e[32m"
#define BKG_GREEN "\e[42m"
#define GREEN "\e[92m"
#define B_GREEN "\e[1;32m"

#define D_YELLOW "\e[33m"
#define BKG_YELLOW "\e[43m"
#define YELLOW "\e[93m"
#define B_YELLOW "\e[1;33m"

#define D_BLUE "\e[34m"
#define BKG_BLUE "\e[44m"
#define BLUE "\e[94m"
#define B_BLUE "\e[1;34m"

#define D_MAGNETA "\e[35m"
#define BKG_MAGNETA "\e[45m"
#define MAGNETA "\e[95m"
#define B_MAGNETA "\e[1;35m"

#define D_CYAN "\e[36m"
#define BKG_CYAN "\e[46m"
#define CYAN "\e[96m"
#define B_CYAN "\e[1;36m"

#define RST  "\e[0m"
#define BKG_RST  "\e[49m"

#endif
