#define LPT1 0

#define qF1   0x3B /*59*/
#define qF2   0x3C /*60*/
#define qF3   0x3D /*61*/
#define qF4   0x3E /*62*/
#define qF5   0x3F /*63*/
#define qF6   0x40 /*64*/
#define qF7   0x41 /*65*/
#define qF8   0x42 /*66*/
#define qF9   0x43 /*67*/
#define qF10  0x44 /*68*/

#define qCF1  0x5E
#define qCF2  0x5F
#define qCF3  0x60
#define qCF4  0x61
#define qCF5  0x62
#define qCF6  0x63
#define qCF7  0x64
#define qCF8  0x65
#define qCF9  0x66
#define qCF10 0x67

#define qAF1  0x68
#define qAF2  0x69
#define qAF3  0x6A
#define qAF4  0x6B
#define qAF5  0x6C
#define qAF6  0x6D
#define qAF7  0x6E
#define qAF8  0x6F
#define qAF9  0x70
#define qAF10 0x71

#define NOKEY -1


void press_any(void);
void term(void);
void title(void);
void panel(int colour);
int menu(void);
void bad_choice(void);
void file_transfer(void);
char *get_fnm(char *type);
void setup(void);
void new_dup(void);
void new_port(void);
void new_mode(void);
void new_baud(void);
void new_par(void);
void new_data(void);
void new_stop(void);
void new_term(void);
void new_hand(void);
int choose(int low, int high);
char *par_str(void);
void t_mon(int status);           /* example monitor function */
void my_init(void);
void draw_box(void);
void draw_little_box(void);
void display_status(void);
void dtr(int which);
void dsr(int which);
void cts(int which);
void rts(int which);
void out1(int which);
void out2(int which);
void load_parameters(void);
void save_parameters(void);
void set_parameters(void);
void set_port(void);
