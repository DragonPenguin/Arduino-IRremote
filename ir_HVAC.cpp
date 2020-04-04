#include "IRremote.h"
#include "IRremoteInt.h"

#if SEND_HVAC
// Mitsubishi RM 75501
// 14200 7 41 7 42 7 42 7 17 7 17 7 18 7 41 7 18 7 17 7 17 7 18 7 41 8 17 7 17 7 18 7 17 7 

// #define MITSUBISHI_HDR_MARK	250  // seen range 3500
#define MITSUBISHI_HDR_SPACE	350 //  7*50+100
#define MITSUBISHI_ONE_MARK	1950 // 41*50-100
#define MITSUBISHI_ZERO_MARK  750 // 17*50-100
// #define MITSUBISHI_DOUBLE_SPACE_USECS  800  // usually ssee 713 - not using ticks as get number wrapround
// #define MITSUBISHI_RPT_LENGTH 45000

// PANASONIC HVAC
#define HVAC_PANASONIC_HDR_MARK    3500
#define HVAC_PANASONIC_HDR_SPACE   1750
#define HVAC_PANASONIC_BIT_MARK    435
#define HVAC_PANASONIC_ONE_SPACE   1300
#define HVAC_PANASONIC_ZERO_SPACE  435
#define HVAC_PANASONIC_RPT_SPACE   10000
#define HVAC_PANASONIC_RPT_MARK   435


// HVAC MITSUBISHI_
#define HVAC_MITSUBISHI_HDR_MARK    3400
#define HVAC_MITSUBISHI_HDR_SPACE   1750
#define HVAC_MITSUBISHI_BIT_MARK    450
#define HVAC_MITSUBISHI_ONE_SPACE   1300
#define HVAC_MISTUBISHI_ZERO_SPACE  420
#define HVAC_MITSUBISHI_RPT_MARK    440
#define HVAC_MITSUBISHI_RPT_SPACE   17100 // Above original iremote limit


// HVAC TOSHIBA_
#define HVAC_TOSHIBA_HDR_MARK    4400
#define HVAC_TOSHIBA_HDR_SPACE   4300
#define HVAC_TOSHIBA_BIT_MARK    543
#define HVAC_TOSHIBA_ONE_SPACE   1623
#define HVAC_MISTUBISHI_ZERO_SPACE  472
#define HVAC_TOSHIBA_RPT_MARK    440
#define HVAC_TOSHIBA_RPT_SPACE   7048 // Above original iremote limit


/****************************************************************************
/* Send IR command to Panasonic HVAC - sendHvacPanasonic
/***************************************************************************/
void IRsend::sendHvacPanasonic(
  HvacMode        HVAC_Mode,           // Example HVAC_HOT  HvacPanasonicMode
  int             HVAC_Temp,           // Example 21  (°c)
  HvacFanMode     HVAC_FanMode,        // Example FAN_SPEED_AUTO  HvacPanasonicFanMode
  HvacVanneMode   HVAC_VanneMode,      // Example VANNE_AUTO_MOVE  HvacPanasonicVanneMode
  HvacProfileMode HVAC_ProfileMode,	// Example QUIET HvacPanasonicProfileMode
  int             HVAC_SWITCH           // Example false
)
{

#define HVAC_PANASONIC_DEBUG  // Un comment to access DEBUG information through Serial Interface

  byte mask = 1; //our bitmask
  byte data[19] = { 0x02, 0x20, 0xE0, 0x04, 0x00, 0x48, 0x3C, 0x80, 0xAF, 0x00, 0x00, 0x0E, 0xE0, 0x10, 0x00, 0x01, 0x00, 0x06, 0xBE };
  byte dataconst[8] = { 0x02, 0x20, 0xE0, 0x04, 0x00, 0x00, 0x00, 0x06};

  // data array is a valid trame, only byte to be chnaged will be updated.

  byte i;

#ifdef HVAC_PANASONIC_DEBUG
  Serial.println("Basis: ");
  for (i = 0; i < 19; i++) {
    Serial.print("_");
    Serial.print(data[i], HEX);
  }
  Serial.println(".");
#endif

  // Byte 6 - On / Off
  if (HVAC_SWITCH) {
    data[5] = (byte) 0x08; // Switch HVAC Power
  } else {
    data[5] = (byte) 0x09; // Do not switch HVAC Power
  }

  // Byte 6 - Mode
  switch (HVAC_Mode)
  {
    case HVAC_HOT:   data[5] = (byte) data[5] | B01000000; break;
    case HVAC_FAN:   data[5] = (byte) data[5] | B01100000; break;
    case HVAC_COLD:  data[5] = (byte) data[5] | B00011000; break;
    case HVAC_DRY:   data[5] = (byte) data[5] | B00100000; break;
    case HVAC_AUTO:  data[5] = (byte) data[5] | B00000000; break;
    default: break;
  }

  // Byte 7 - Temperature
  // Check Min Max For Hot Mode
  byte Temp;
  if (HVAC_Temp > 30) { Temp = 30;}
  else if (HVAC_Temp < 16) { Temp = 16; } 
  else { Temp = HVAC_Temp; };
  data[6] = (byte) (Temp - 16) <<1;
  data[6] = (byte) data[6] | B00100000;
  //bits used form the temp are [4:1]
  //data|6] = data[6] << 1;
  

  // Byte 9 - FAN / VANNE
  switch (HVAC_FanMode)
  {
    case FAN_SPEED_1:       data[8] = (byte) B00110000; break;
    case FAN_SPEED_2:       data[8] = (byte) B01000000; break;
    case FAN_SPEED_3:       data[8] = (byte) B01010000; break;
    case FAN_SPEED_4:       data[8] = (byte) B01100000; break;
    case FAN_SPEED_5:       data[8] = (byte) B01010000; break;
    case FAN_SPEED_AUTO:    data[8] = (byte) B10100000; break;
    default: break;
  }

  switch (HVAC_VanneMode)
  {
    case VANNE_AUTO:        data[8] = (byte) data[8] | B00001111; break;
    case VANNE_AUTO_MOVE:   data[8] = (byte) data[8] | B00001111; break; //same as AUTO in the PANASONIC CASE
    case VANNE_H1:          data[8] = (byte) data[8] | B00000001; break;
    case VANNE_H2:          data[8] = (byte) data[8] | B00000010; break;
    case VANNE_H3:          data[8] = (byte) data[8] | B00000011; break;
    case VANNE_H4:          data[8] = (byte) data[8] | B00000100; break;
    case VANNE_H5:          data[8] = (byte) data[8] | B00000101; break;
    default: break;
  }

   // Byte 14 - Profile
  switch (HVAC_ProfileMode)
  {
    case NORMAL:        data[13] = (byte) B00010000; break;
    case QUIET:         data[13] = (byte) B01100000; break;
    case BOOST:         data[13] = (byte) B00010001; break;
    default: break;
  }  
  
  
  // Byte 18 - CRC
  data[18] = 0;
  for (i = 0; i < 18; i++) {
    data[18] = (byte) data[i] + data[18];  // CRC is a simple bits addition
  }

#ifdef HVAC_PANASONIC_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 19; i++) {
    Serial.print("_"); Serial.print(data[i], HEX);
  }
  Serial.println(".");
  for (i = 0; i < 19; i++) {
    Serial.print(data[i], BIN); Serial.print(" ");
  }
  Serial.println(".");
#endif

  enableIROut(38);  // 38khz
  space(0);
  
  //Send constant frame #1
    mark(HVAC_PANASONIC_HDR_MARK);
    space(HVAC_PANASONIC_HDR_SPACE);
    for (i = 0; i < 8; i++) {
      // Send all Bits from Byte dataconst in Reverse Order
      for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
        if (dataconst[i] & mask) { // Bit ONE
          mark(HVAC_PANASONIC_BIT_MARK);
          space(HVAC_PANASONIC_ONE_SPACE);
        }
        else { // Bit ZERO
          mark(HVAC_PANASONIC_BIT_MARK);
          space(HVAC_PANASONIC_ZERO_SPACE);
        }
        //Next bits
      }
    }  
     mark(HVAC_PANASONIC_RPT_MARK);
     space(HVAC_PANASONIC_RPT_SPACE);
 
  //Send frame #2  
    mark(HVAC_PANASONIC_HDR_MARK);
    space(HVAC_PANASONIC_HDR_SPACE);
    for (i = 0; i < 19; i++) {
      // Send all Bits from Byte Data in Reverse Order
      for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
        if (data[i] & mask) { // Bit ONE
          mark(HVAC_PANASONIC_BIT_MARK);
          space(HVAC_PANASONIC_ONE_SPACE);
        }
        else { // Bit ZERO
          mark(HVAC_PANASONIC_BIT_MARK);
          space(HVAC_PANASONIC_ZERO_SPACE);
        }
        //Next bits
      }
    }
    // End of Packet and retransmission of the Packet
      mark(HVAC_PANASONIC_RPT_MARK);
      space(HVAC_PANASONIC_RPT_SPACE);
       space(0);


}



/****************************************************************************
/* Send IR command to Mitsubishi HVAC - sendHvacMitsubishi
/***************************************************************************/
void IRsend::sendHvacMitsubishi(
  HvacMode                HVAC_Mode,           // Example HVAC_HOT  HvacMitsubishiMode
  int                     HVAC_Temp,           // Example 21  (°c)
  HvacFanMode             HVAC_FanMode,        // Example FAN_SPEED_AUTO  HvacMitsubishiFanMode
  HvacVanneMode           HVAC_VanneMode,      // Example VANNE_AUTO_MOVE  HvacMitsubishiVanneMode
  int                     OFF                  // Example false
)
{

//#define  HVAC_MITSUBISHI_DEBUG;  // Un comment to access DEBUG information through Serial Interface

  byte mask = 1; //our bitmask
  byte data[18] = { 0x23, 0xCB, 0x26, 0x01, 0x00, 0x20, 0x08, 0x06, 0x30, 0x45, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
  // data array is a valid trame, only byte to be chnaged will be updated.

  byte i;

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 18; i++) {
    Serial.print("_");
    Serial.print(data[i], HEX);
  }
  Serial.println(".");
#endif

  // Byte 6 - On / Off
  if (OFF) {
    data[5] = (byte) 0x0; // Turn OFF HVAC
  } else {
    data[5] = (byte) 0x20; // Tuen ON HVAC
  }

  // Byte 7 - Mode
  switch (HVAC_Mode)
  {
    case HVAC_HOT:   data[6] = (byte) 0x08; break;
    case HVAC_COLD:  data[6] = (byte) 0x18; break;
    case HVAC_DRY:   data[6] = (byte) 0x10; break;
    case HVAC_AUTO:  data[6] = (byte) 0x20; break;
    default: break;
  }

  // Byte 8 - Temperature
  // Check Min Max For Hot Mode
  byte Temp;
  if (HVAC_Temp > 31) { Temp = 31;}
  else if (HVAC_Temp < 16) { Temp = 16; } 
  else { Temp = HVAC_Temp; };
  data[7] = (byte) Temp - 16;

  // Byte 10 - FAN / VANNE
  switch (HVAC_FanMode)
  {
    case FAN_SPEED_1:       data[9] = (byte) B00000001; break;
    case FAN_SPEED_2:       data[9] = (byte) B00000010; break;
    case FAN_SPEED_3:       data[9] = (byte) B00000011; break;
    case FAN_SPEED_4:       data[9] = (byte) B00000100; break;
    case FAN_SPEED_5:       data[9] = (byte) B00000100; break; //No FAN speed 5 for MITSUBISHI so it is consider as Speed 4
    case FAN_SPEED_AUTO:    data[9] = (byte) B10000000; break;
    case FAN_SPEED_SILENT:  data[9] = (byte) B00000101; break;
    default: break;
  }

  switch (HVAC_VanneMode)
  {
    case VANNE_AUTO:        data[9] = (byte) data[9] | B01000000; break;
    case VANNE_H1:          data[9] = (byte) data[9] | B01001000; break;
    case VANNE_H2:          data[9] = (byte) data[9] | B01010000; break;
    case VANNE_H3:          data[9] = (byte) data[9] | B01011000; break;
    case VANNE_H4:          data[9] = (byte) data[9] | B01100000; break;
    case VANNE_H5:          data[9] = (byte) data[9] | B01101000; break;
    case VANNE_AUTO_MOVE:   data[9] = (byte) data[9] | B01111000; break;
    default: break;
  }

  // Byte 18 - CRC
  data[17] = 0;
  for (i = 0; i < 17; i++) {
    data[17] = (byte) data[i] + data[17];  // CRC is a simple bits addition
  }

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 18; i++) {
    Serial.print("_"); Serial.print(data[i], HEX);
  }
  Serial.println(".");
  for (i = 0; i < 18; i++) {
    Serial.print(data[i], BIN); Serial.print(" ");
  }
  Serial.println(".");
#endif

  enableIROut(38);  // 38khz
  space(0);
  for (int j = 0; j < 2; j++) {  // For Mitsubishi IR protocol we have to send two time the packet data
    // Header for the Packet
    mark(HVAC_MITSUBISHI_HDR_MARK);
    space(HVAC_MITSUBISHI_HDR_SPACE);
    for (i = 0; i < 18; i++) {
      // Send all Bits from Byte Data in Reverse Order
      for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
        if (data[i] & mask) { // Bit ONE
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MITSUBISHI_ONE_SPACE);
        }
        else { // Bit ZERO
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MISTUBISHI_ZERO_SPACE);
        }
        //Next bits
      }
    }
    // End of Packet and retransmission of the Packet
    mark(HVAC_MITSUBISHI_RPT_MARK);
    space(HVAC_MITSUBISHI_RPT_SPACE);
    space(0); // Just to be sure
  }
}

/***************************************************************************/
/* Send IR command to Mitsubishi HVAC - sendHvacMitsubishi 
/* Add support for W001CP R61Y23304 Remote Controller
/***************************************************************************/
void IRsend::sendHvacMitsubishi_W001CP(
  HvacMode                  HVAC_Mode,           // Example HVAC_HOT.         HvacMitsubishiMode
                                                     // This type support HVAC_HOT,HVAC_COLD,HVAC_DRY,HVAC_FAN,HVAC_AUTO.
  int                       HVAC_Temp,           // Example 21  (°c).
                                                     // This type support 17~28 in HVAC_HOT mode, 19~30 in HVAC_COLD and HVAC_DRY mode.
  HvacFanMode               HVAC_FanMode,        // Example FAN_SPEED_AUTO.   HvacMitsubishiFanMode
                                                     // This type support FAN_SPEED_1,FAN_SPEED_2,FAN_SPEED_3,FAN_SPEED_4.
  HvacVanneMode             HVAC_VanneMode,      // Example VANNE_AUTO_MOVE.  HvacMitsubishiVanneMode
                                                     // This type support support VANNE_AUTO,VANNE_H1,VANNE_H2,VANNE_H3,VANNE_H4.
  int                       OFF                  // Example false
)
{

//#define  HVAC_MITSUBISHI_DEBUG;  // Un comment to access DEBUG information through Serial Interface

  byte mask = 1; //our bitmask
  byte data[17] = { 0x23, 0xCB, 0x26, 0x21, 0x00, 0x40, 0x52, 0x35, 0x04, 0x00, 0x00, 0xBF, 0xAD, 0xCA, 0xFB, 0xFF, 0xFF };
  // byte              0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
  // data array is a valid trame, only byte to be chnaged will be updated.

  byte i;

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 17; i++) {
    Serial.print("_");
    Serial.print(data[i], HEX);
  }
  Serial.println(".");
#endif

  // Byte 5 - On / Off
  if (OFF) {
    data[5] = (byte) 0x0; // Turn OFF HVAC
  } else {
    data[5] = (byte) 0x40; // Tuen ON HVAC
  }

  // Byte 6 - Temperature / Mode 
  byte tmpTM;
  
  switch (HVAC_Mode)  // Mode
  {
    case HVAC_HOT:   tmpTM = (byte) B00000010; break;
    case HVAC_COLD:  tmpTM = (byte) B00000001; break;
    case HVAC_DRY:   tmpTM = (byte) B00000101; break;
    case HVAC_FAN:   tmpTM = (byte) B00000000; break;
    case HVAC_AUTO:  tmpTM = (byte) B00000011; break;
    default: break;
  }

  byte Temp;  // Temperature, support 17~28 in HVAC_HOT mode, 19~30 in HVAC_COLD and HVAC_DRY mode
  if (HVAC_Temp > 30 && HVAC_Mode != HVAC_HOT) { Temp = 30;}
  else if (HVAC_Temp > 28 && HVAC_Mode == HVAC_HOT) { Temp = 28;}
  else if (HVAC_Temp < 19 && HVAC_Mode != HVAC_HOT) { Temp = 19; } 
  else if (HVAC_Temp < 17 && HVAC_Mode == HVAC_HOT) { Temp = 17; } 
  else { Temp = HVAC_Temp; };
  Temp = (byte) Temp - 16;
  
  data[6] = (byte) Temp * 16 | tmpTM; // Temperature bits are the high 4 bits, Mode bits are the low 4 bits.

  // Byte 7 - VANNE / FAN
  switch (HVAC_FanMode)
  {
    case FAN_SPEED_1:       data[7] = (byte) B00000001; break;
    case FAN_SPEED_2:       data[7] = (byte) B00000011; break;
    case FAN_SPEED_3:       data[7] = (byte) B00000101; break;
    case FAN_SPEED_4:       data[7] = (byte) B00000111; break;
    case FAN_SPEED_5:       data[7] = (byte) B00000111; break;  // this type doesn't support speed5, set to speed4
    case FAN_SPEED_AUTO:    data[7] = (byte) B00000101; break;  // this type doesn't support auto,   set to speed3 
    case FAN_SPEED_SILENT:  data[7] = (byte) B00000001; break;  // this type doesn't support slient, set to speed1 
    default: break;
  }

  switch (HVAC_VanneMode)
  {
    case VANNE_AUTO:        data[7] = (byte) data[7] | B11000000; break;
    case VANNE_H1:          data[7] = (byte) data[7] | B00000000; break;
    case VANNE_H2:          data[7] = (byte) data[7] | B00010000; break;
    case VANNE_H3:          data[7] = (byte) data[7] | B00100000; break;
    case VANNE_H4:          data[7] = (byte) data[7] | B00110000; break;
    case VANNE_H5:          data[7] = (byte) data[7] | B00110000; break;  // this type doesn't support vanne5,   set to vanne4 
    case VANNE_AUTO_MOVE:   data[7] = (byte) data[7] | B11000000; break;  // this type doesn't support automove, set to auto 
    default: break;
  }

  // Byte 11 - XOR of Byte 5
  data[11] = (byte) B11111111 ^ data[5];
  
  // Byte 12 - XOR of Byte 6
  data[12] = (byte) B11111111 ^ data[6];
  
  // Byte 13 - XOR of Byte 7
  data[13] = (byte) B11111111 ^ data[7];

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 17; i++) {
    Serial.print("_"); Serial.print(data[i], HEX);
  }
  Serial.println(".");
  for (i = 0; i < 17; i++) {
    Serial.print(data[i], BIN); Serial.print(" ");
  }
  Serial.println(".");
#endif

  enableIROut(38);  // 38khz
  space(0);
  for (int j = 0; j < 1; j++) {  // Mitsubishi W001CP IR protocol only need to send one time
    // Header for the Packet
    mark(HVAC_MITSUBISHI_HDR_MARK);
    space(HVAC_MITSUBISHI_HDR_SPACE);
    for (i = 0; i < 18; i++) {
      // Send all Bits from Byte Data in Reverse Order
      for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
        if (data[i] & mask) { // Bit ONE
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MITSUBISHI_ONE_SPACE);
        }
        else { // Bit ZERO
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MISTUBISHI_ZERO_SPACE);
        }
        //Next bits
      }
    }
    // End of Packet and retransmission of the Packet
    mark(HVAC_MITSUBISHI_RPT_MARK);
    space(HVAC_MITSUBISHI_RPT_SPACE);
    space(0); // Just to be sure
  }
}

/****************************************************************************
/* Send IR command to Mitsubishi HVAC - sendHvacMitsubishi
/***************************************************************************/
void IRsend::sendHvacMitsubishiFD(
      HvacMode                  HVAC_Mode,           // Example HVAC_HOT  HvacMitsubishiMode
      int                       HVAC_Temp,           // Example 21  (°c)
      HvacFanMode               HVAC_FanMode,        // Example FAN_SPEED_AUTO  HvacMitsubishiFanMode
      HvacVanneMode             HVAC_VanneMode,      // Example VANNE_AUTO_MOVE  HvacMitsubishiVanneMode
      HvacAreaMode              HVAC_AreaMode,       // Example AREA_AUTO
      HvacWideVanneMode         HVAC_WideMode,       // Example WIDE_MIDDLE
      int                      HVAC_PLASMA,          // Example true to Turn ON PLASMA Function
      int                      HVAC_CLEAN_MODE,      // Example false 
      int                      HVAC_ISEE,            // Example False
      int                      OFF                   // Example false to Turn ON HVAC / true to request to turn off
    )
{

//#define  HVAC_MITSUBISHI_DEBUG;  // Un comment to access DEBUG information through Serial Interface

  byte mask = 1; //our bitmask
  byte data[18] = { 0x23, 0xCB, 0x26, 0x01, 0x00, 0x20, 0x08, 0x06, 0x30, 0x45, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
  // data array is a valid trame, only byte to be changed will be updated.

  byte i;

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 18; i++) {
    Serial.print("_");
    Serial.print(data[i], HEX);
  }
  Serial.println(".");
#endif

  // Byte 5 - On / Off
  if (OFF) {
    data[5] = (byte) 0x0; // Turn OFF HVAC
  } else {
    data[5] = (byte) 0x20; // Turn ON HVAC
  }

  // Byte 6 - Mode
  switch (HVAC_Mode)
  {
    case HVAC_HOT:   data[6] = (byte) B00001000; break;
    case HVAC_COLD:  data[6] = (byte) B00011000; break;
    case HVAC_DRY:   data[6] = (byte) B00010000; break;
    case HVAC_AUTO:  data[6] = (byte) B00100000; break;
    default: break;
  }
  if (HVAC_ISEE)  { data[6] = (byte) data[6] | B01000000;  }

  // Byte 7 - Temperature
  // Check Min Max For Hot Mode
  byte Temp;
  if (HVAC_Temp > 31) { Temp = 31;}
  else if (HVAC_Temp < 16) { Temp = 16; } 
  else { Temp = HVAC_Temp; };
  data[7] = (byte) Temp - 16;

  // Byte 8 - Complement To HVAC Mode + Wide Vanne
  switch (HVAC_Mode)
  {
    case HVAC_HOT:   data[8] = (byte) B00000000; break;
    case HVAC_COLD:  data[8] = (byte) B00000110; break;
    case HVAC_DRY:   data[8] = (byte) B00000010; break;
    case HVAC_AUTO:  data[8] = (byte) B00000000; break;
    default: break;
  }
  switch (HVAC_WideMode)
  {
    case WIDE_LEFT_END:   data[8] = (byte) data[8] | B00010000; break;
    case WIDE_LEFT:       data[8] = (byte) data[8] | B00100000; break;
    case WIDE_MIDDLE:     data[8] = (byte) data[8] | B00110000; break;
    case WIDE_RIGHT:      data[8] = (byte) data[8] | B01000000; break;
    case WIDE_RIGHT_END:  data[8] = (byte) data[8] | B01010000; break;
    case WIDE_SWING:      data[8] = (byte) data[8] | B10000000; break;
    default: break;
  }

  // Byte 9 - FAN / VANNE
  switch (HVAC_FanMode)
  {
    case FAN_SPEED_1:       data[9] = (byte) B00000001; break;
    case FAN_SPEED_2:       data[9] = (byte) B00000010; break;
    case FAN_SPEED_3:       data[9] = (byte) B00000011; break;
    case FAN_SPEED_4:       data[9] = (byte) B00000100; break;
    case FAN_SPEED_5:       data[9] = (byte) B00000100; break; //No FAN speed 5 for MITSUBISHI so it is consider as Speed 4
    case FAN_SPEED_AUTO:    data[9] = (byte) B10000000; break;
    case FAN_SPEED_SILENT:  data[9] = (byte) B00000101; break;
    default: break;
  }
  // Byte 9 
  switch (HVAC_VanneMode)
  {
    case VANNE_AUTO:        data[9] = (byte) data[9] | B01000000; break;
    case VANNE_H1:          data[9] = (byte) data[9] | B01001000; break;
    case VANNE_H2:          data[9] = (byte) data[9] | B01010000; break;
    case VANNE_H3:          data[9] = (byte) data[9] | B01011000; break;
    case VANNE_H4:          data[9] = (byte) data[9] | B01100000; break;
    case VANNE_H5:          data[9] = (byte) data[9] | B01101000; break;
    case VANNE_AUTO_MOVE:   data[9] = (byte) data[9] | B01111000; break;
    default: break;
  }

  // Byte 13 - AREA MODE
  switch (HVAC_AreaMode)
  {
    case AREA_SWING:  data[13] = (byte) B00000000; break;
    case AREA_LEFT:   data[13] = (byte) B01000000; break;
    case AREA_AUTO:   data[13] = (byte) B10000000; break;
    case AREA_RIGHT:  data[13] = (byte) B11000000; break;
    default: break;
  }

  // Byte 14 - CLEAN MODE
  if (HVAC_CLEAN_MODE)  { data[14] = (byte) B00000100;  }

  // Byte 15 - PLASMA
  if (HVAC_PLASMA)  { data[15] = (byte) B00000100;  }

  // Byte 18 - CRC
  data[17] = 0;
  for (i = 0; i < 17; i++) {
    data[17] = (byte) data[i] + data[17];  // CRC is a simple bits addition
  }

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 18; i++) {
    Serial.print("_"); Serial.print(data[i], HEX);
  }
  Serial.println(".");
  for (i = 0; i < 18; i++) {
    Serial.print(data[i], BIN); Serial.print(" ");
  }
  Serial.println(".");
#endif

  enableIROut(38);  // 38khz
  space(0);
  for (int j = 0; j < 2; j++) {  // For Mitsubishi IR protocol we have to send two time the packet data
    // Header for the Packet
    mark(HVAC_MITSUBISHI_HDR_MARK);
    space(HVAC_MITSUBISHI_HDR_SPACE);
    for (i = 0; i < 18; i++) {
      // Send all Bits from Byte Data in Reverse Order
      for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
        if (data[i] & mask) { // Bit ONE
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MITSUBISHI_ONE_SPACE);
        }
        else { // Bit ZERO
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MISTUBISHI_ZERO_SPACE);
        }
        //Next bits
      }
    }
    // End of Packet and retransmission of the Packet
    mark(HVAC_MITSUBISHI_RPT_MARK);
    space(HVAC_MITSUBISHI_RPT_SPACE);
    space(0); // Just to be sure
  }
}


/***************************************************************************/
/* Send IR command to Toshiba HVAC - sendHvacToshiba
/***************************************************************************/
void IRsend::sendHvacToshiba(
  HvacMode                HVAC_Mode,           // Example HVAC_HOT  
  int                     HVAC_Temp,           // Example 21  (°c)
  HvacFanMode             HVAC_FanMode,        // Example FAN_SPEED_AUTO  
  int                     OFF                  // Example false
)
{
 
#define HVAC_TOSHIBA_DATALEN 9
#define  HVAC_TOSHIBA_DEBUG;  // Un comment to access DEBUG information through Serial Interface

  byte mask = 1; //our bitmask
  //﻿F20D03FC0150000051
  byte data[HVAC_TOSHIBA_DATALEN] = { 0xF2, 0x0D, 0x03, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00 };
  // data array is a valid trame, only byte to be chnaged will be updated.

  byte i;

#ifdef HVAC_TOSHIBA_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++) {
    Serial.print("_");
    Serial.print(data[i], HEX);
  }
  Serial.println(".");
#endif

  data[6] = 0x00;
  // Byte 7 - Mode
  switch (HVAC_Mode)
  {
    case HVAC_HOT:   data[6] = (byte) B00000011; break;
    case HVAC_COLD:  data[6] = (byte) B00000001; break;
    case HVAC_DRY:   data[6] = (byte) B00000010; break;
    case HVAC_AUTO:  data[6] = (byte) B00000000; break;
    default: break;
  }


  // Byte 7 - On / Off
  if (OFF) {
    data[6] = (byte) 0x07; // Turn OFF HVAC
  } else {
     // Turn ON HVAC (default)
  }

  // Byte 6 - Temperature
  // Check Min Max For Hot Mode
  byte Temp;
  if (HVAC_Temp > 30) { Temp = 30;}
  else if (HVAC_Temp < 17) { Temp = 17; } 
  else { Temp = HVAC_Temp; };
  data[5] = (byte) Temp - 17<<4;

  // Byte 10 - FAN / VANNE
  switch (HVAC_FanMode)
  {
    case FAN_SPEED_1:       data[6] = data[6] | (byte) B01000000; break;
    case FAN_SPEED_2:       data[6] = data[6] | (byte) B01100000; break;
    case FAN_SPEED_3:       data[6] = data[6] | (byte) B10000000; break;
    case FAN_SPEED_4:       data[6] = data[6] | (byte) B10100000; break;
    case FAN_SPEED_5:       data[6] = data[6] | (byte) B11000000; break; 
    case FAN_SPEED_AUTO:    data[6] = data[6] | (byte) B00000000; break;
    case FAN_SPEED_SILENT:  data[6] = data[6] | (byte) B00000000; break;//No FAN speed SILENT for TOSHIBA so it is consider as Speed AUTO
    default: break;
  }

  // Byte 9 - CRC
  data[8] = 0;
  for (i = 0; i < HVAC_TOSHIBA_DATALEN - 1; i++) {
    data[HVAC_TOSHIBA_DATALEN-1] = (byte) data[i] ^ data[HVAC_TOSHIBA_DATALEN -1];  // CRC is a simple bits addition
  }

#ifdef HVAC_TOSHIBA_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++) {
    Serial.print("_"); Serial.print(data[i], HEX);
  }
  Serial.println(".");
  for (i = 0; i < HVAC_TOSHIBA_DATALEN ; i++) {
    Serial.print(data[i], BIN); Serial.print(" ");
  }
  Serial.println(".");
#endif

  enableIROut(38);  // 38khz
  space(0);
  for (int j = 0; j < 2; j++) {  // For Mitsubishi IR protocol we have to send two time the packet data
    // Header for the Packet
    mark(HVAC_TOSHIBA_HDR_MARK);
    space(HVAC_TOSHIBA_HDR_SPACE);
    for (i = 0; i < HVAC_TOSHIBA_DATALEN; i++) {
      // Send all Bits from Byte Data in Reverse Order
      for (mask = 10000000; mask > 0; mask >>= 1) { //iterate through bit mask
        if (data[i] & mask) { // Bit ONE
          mark(HVAC_TOSHIBA_BIT_MARK);
          space(HVAC_TOSHIBA_ONE_SPACE);
        }
        else { // Bit ZERO
          mark(HVAC_TOSHIBA_BIT_MARK);
          space(HVAC_MISTUBISHI_ZERO_SPACE);
        }
        //Next bits
      }
    }
    // End of Packet and retransmission of the Packet
    mark(HVAC_TOSHIBA_RPT_MARK);
    space(HVAC_TOSHIBA_RPT_SPACE);
    space(0); // Just to be sure
  }
}

#endif
