
/* Extern display variables */
extern unsigned char displayChars[8];

/* single LED definitions */
#define LED_ALM1    6
#define LED_ALM2    7
#define LED_AT      4
#define LED_OUT1    2

#define ON 1
#define OFF 0

void DisplayScan();
void SetUpperPoint(unsigned char state);
void SetLowerPoint(unsigned char state);
void SetDisplayChars(char* buffer);
void SetLED(unsigned char led);
void ClearLED(unsigned char led);
