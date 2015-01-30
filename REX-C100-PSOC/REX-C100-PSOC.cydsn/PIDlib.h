

extern unsigned char PID_Power_percent;
extern unsigned short targetTempX10;

void PID_Calc(short tempX10);

#define PID_CYCLE_TIME_MS 10000
