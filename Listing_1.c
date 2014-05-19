#define DCF_T0    6             // T0:    1     .. DCF_T1-1 [s÷100]
#define DCF_T1   15             // T1: DCF_T0+1 .. DCF_T2-1 [s÷100]
#define DCF_T2   17             // T2: DCF_T1+1 .. DCF_T3-1 [s÷100]
#define DCF_T3   25             // T3: DCF_T2+1 .. DCF_T4-1 [s÷100]
#define DCF_T4   95             // T4: DCF_T3+1 .. DCF_T5-1 [s÷100]
#define DCF_T5  120             // T5: DCF_T4+1 .. DCF_T6-1 [s÷100]
#define DCF_T6  220             // T6: DCF_T5+1 ..   250    [s÷100]

typedef enum                    // Ereignisse der DCF-PWM Abtastung
{   DCF_0    = '0',             // · Null erkannt
    DCF_1    = '1',             // · Eins erkannt
    DCF_MARK = 'm',             // · Minutenanfang erkannt
    DCF_FAIL = 'e',             // · Abtastfehler
    DCF_NONE = 'x'              // · Kein Ereignis
} DCFEvent;

volatile DCFEvent dcfEvent;

ISR(TIMER1_COMPA_vect)
{
    typedef enum
    {   TI = 0,
        HI = 1,
        LO = 2
    } Input;

    typedef struct
    {   uint8_t  state;
        char     output;
    } ZetaValue;

    static const ZetaValue zeta[][3] PROGMEM =
    {   //+--Ti--+ +--Hi--+ +--Lo--+ +--------+
        { { 0,'e'},{ 1,'a'},{ 0,'x'} }, // S0
        { { 2,'x'},{ 1,'x'},{ 0,'e'} }, // S1
        { { 7,'x'},{ 2,'x'},{ 3,'x'} }, // S2
        { { 4,'x'},{ 0,'e'},{ 3,'x'} }, // S3
        { { 5,'x'},{ 0,'e'},{ 4,'x'} }, // S4
        { { 6,'x'},{ 0,'e'},{ 5,'x'} }, // S5
        { {11,'0'},{ 0,'e'},{ 6,'x'} }, // S6
        { { 8,'x'},{ 7,'x'},{ 0,'e'} }, // S7
        { { 0,'e'},{ 8,'x'},{ 9,'x'} }, // S8
        { {10,'x'},{ 0,'e'},{ 9,'x'} }, // S9
        { {11,'1'},{ 0,'e'},{10,'x'} }, // S10
        { {12,'x'},{ 1,'a'},{11,'x'} }, // S11
        { { 0,'e'},{ 1,'m'},{12,'x'} }  // S12
    };
    static uint8_t  state = 0;          // Aktueller Zustand S = { 0,...,12 }
    static uint8_t  tenMs = 0;          // Hundertstelsekundenzähler [s÷100]
    Input           input;              // Eingabe: I = { TI,HI,LO }
    char            output;             // Ausgabe: O = { 'x','a','0','1','m','e' }

    if( tenMs == DCF_T0 ||
        tenMs == DCF_T1 ||
        tenMs == DCF_T2 ||
        tenMs == DCF_T3 ||
        tenMs == DCF_T4 ||
        tenMs == DCF_T5 ||
        tenMs == DCF_T6  ) input = TI;  // NB: Eingabe Ti
    else if( GetDCFPin() ) input = HI;  //     dominiert Hi
    else                   input = LO;  //     und Lo
    output = pgm_read_byte(&(zeta[state][(uint8_t)input].output));
    state  = pgm_read_byte(&(zeta[state][(uint8_t)input].state));
    if( output == 'm' ) tenMs = 0;
    if     ( output == 'a' ) tenMs = 0;
    else if( output != 'x' ) dcfEvent = (DCFEvent)output;
    tenMs++;
}

