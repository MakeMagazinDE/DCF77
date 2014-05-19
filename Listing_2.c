void
ProceedDCFDecoding(void)
{
    static const uint8_t  FAIL    = 0xFF;
    static const uint8_t  BCD[]   = { 1, 2, 4, 8, 10, 20, 40, 80 };
    static uint8_t        parity  = 0;
    static uint8_t        dlsTime = 0;
    static uint8_t        bitNo   = 0;      // 0 .. 58 oder FAIL
    static uint8_t        minute  = 0;      // 0 .. 59
    static uint8_t        hour    = 0;      // 0 .. 23
    static uint8_t        day     = 1;      // 1 .. 31
    static uint8_t        month   = 1;      // 1 .. 12
    static uint8_t        year    = 0;      // (2000+) 0 .. 255
    uint8_t               dcfBit  = 0;
    DCFEvent              event;

    DisableIRQs();
    event    = dcfEvent;
    dcfEvent = DCF_NONE;
    EnableIRQs();
    if( event == DCF_MARK && bitNo == 59 )
    {   // Neue Zeit:
        // Sekunde  = 0;
        // Minute   = minute;
        // Stunde   = hour;
        // Tag      = day;
        // Monat    = month;
        // Jahr     = 2000 + year;
        // MEZ/MESZ = dlsTime == 0/1
    }
    switch( event )
    {   case DCF_NONE: return;
        case DCF_0:    dcfBit = 0; break;
        case DCF_1:    dcfBit = 1; break;
        default:       bitNo  = FAIL;
    }
    parity ^= dcfBit;
    switch( bitNo )
    {   case  0:                                              // -+
        case  1:                                              //  |
        case  2:                                              //  |
        case  3:                                              //  |
        case  4:                                              //  |
        case  5:                                              //  |
        case  6:                                              //  |
        case  7:                                              //  | Bits 0 - 16
        case  8:                                              //  | ignorieren
        case  9:                                              //  |
        case 10:                                              //  |
        case 11:                                              //  |
        case 12:                                              //  |
        case 13:                                              //  |
        case 14:                                              //  |
        case 15:                                              //  |
        case 16:                                       break; // -+
        case 17: dlsTime = dcfBit;                     break; // MESZ
        case 18: if( dcfBit == dlsTime ) bitNo = FAIL; break; // MEZ
        case 19:                                       break; // Schaltsekunde
        case 20: if( !dcfBit ) bitNo = FAIL;           break; // Startbit (1)
        case 21: parity = dcfBit; minute = 0;                 // -+
        case 22:                                              //  |
        case 23:                                              //  | Minute
        case 24:                                              //  |
        case 25:                                              //  |
        case 26:                                              //  |
        case 27: if( dcfBit ) minute += BCD[bitNo-21]; break; // -+
        case 28: if( parity ) bitNo = FAIL;            break; // Parität Minute
        case 29: parity = dcfBit; hour = 0;                   // -+
        case 30:                                              //  |
        case 31:                                              //  | Stunde
        case 32:                                              //  |
        case 33:                                              //  |
        case 34: if( dcfBit ) hour += BCD[bitNo-29];   break; // -+
        case 35: if( parity ) bitNo = FAIL;            break; // Parität Stunde
        case 36: parity = dcfBit; day  = 0;                   // -+
        case 37:                                              //  | Kalender-
        case 38:                                              //  | tag:
        case 39:                                              //  | 1 .. 31
        case 40:                                              //  |
        case 41: if( dcfBit ) day += BCD[bitNo-36];    break; // -+
        case 42:                                              // -+ Wochentag
        case 43:                                              //  |  ignorieren
        case 44:                                       break; // -+
        case 45: month = 0;                                   // -+
        case 46:                                              //  | Monat:
        case 47:                                              //  |  1 .. 12
        case 48:                                              //  |
        case 49: if( dcfBit ) month += BCD[bitNo-45];  break; // -+
        case 50: year = 0;                                    // -+
        case 51:                                              //  |
        case 52:                                              //  |
        case 53:                                              //  |  Jahr:
        case 54:                                              //  |  0 .. 99
        case 55:                                              //  |
        case 56:                                              //  |
        case 57: if( dcfBit ) year += BCD[bitNo-50];   break; // -+
        case 58: if( parity ) bitNo = FAIL;            break; // Parität Datum
        default: bitNo = FAIL;
    }
    bitNo++;
}


