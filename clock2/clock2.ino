/*--------------------------------------------------------------
  Program:      clock

  Description:  Reads the time from a PCF8563 RTC (Real Time
                Clock). Displays the time and date on a LCD
                and serial port.
                
                Allows the time to be set through the serial
                port.

  Date:         20 April 2012
 
  Author:       W.A. Smith, http://startingelectronics.com
--------------------------------------------------------------*/
#include <Wire.h>

#define RTC_ADDR  (0xA2 >> 1)


void setup() {
  Wire.begin();        // initialize the I2C/TWI interface
  Serial.begin(9600);  // initialize the serial port
}

void loop() {
  int rx_byte = 0;    // stores data byte received on serial port
  
  // check for data from the serial port
  if (Serial.available()) {
    rx_byte = Serial.read();
  }
  if ((rx_byte == 's') || (rx_byte == 'S')) {
    // set the time
    SetTime();
  }
  else {
    // print the time
    PrintTime();
  }
  rx_byte = 0;
}

// print the time to the serial port and LCD
void PrintTime(void)
{
  unsigned char time_date_raw[7];      // time/date read from RTC
  int index = 0;              // index into above array
  char time[] = "hh:mm:ss";   // time string
  char date[] = "dd/mm/20yy"; // date string
  static unsigned char raw_time = 0;   // stores old seconds value
  
  // point to the time registers in the RTC
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0x02);
  Wire.endTransmission();
  
  // get the time
  Wire.requestFrom(RTC_ADDR, 7);
  while (Wire.available()) {
    time_date_raw[index] = Wire.read();
    index++;
    if (index >= 7) {
      index = 0;
      break;
    }
  }
  
  // convert the time / date to a string only every second
  if (raw_time != time_date_raw[0]) {
    raw_time = time_date_raw[0];
    
    Serial.println("Send \"s\" to set the date and time.");
    
    // convert time to a string
    // hours
    time[0] = ((time_date_raw[2] >> 4) & 0x03) + '0';
    time[1] = (time_date_raw[2] & 0x0F) + '0';
    // minutes
    time[3] = ((time_date_raw[1] >> 4) & 0x07) + '0';
    time[4] = (time_date_raw[1] & 0x0F) + '0';
    // seconds
    time[6] = ((time_date_raw[0] >> 4) & 0x07) + '0';
    time[7] = (time_date_raw[0] & 0x0F) + '0';
    Serial.println(time);
    
    // convert date to a string
    // day
    date[0] = ((time_date_raw[3] >> 4) & 0x03) + '0';
    date[1] = (time_date_raw[3] & 0x0F) + '0';
    // month
    date[3] = ((time_date_raw[5] >> 4) & 0x01) + '0';
    date[4] = (time_date_raw[5] & 0x0F) + '0';
    //year
    date[8] = (time_date_raw[6] >> 4) + '0';
    date[9] = (time_date_raw[6] & 0x0F) + '0';
    Serial.println(date);
    Serial.print("\r\n");
  }
}

// allows the date and time to be set via the serial monitor window
void SetTime(void)
{
  char menu_option = 0;
  unsigned char new_date_time[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // 0xFF to mark unchanged field
  char ret_val = 0;
  
  PrintMenu();
  
  while (menu_option != 8) {
    if (Serial.available()) {
      menu_option = Serial.read();
      menu_option = menu_option - '0';
      // check if a valid menu option was entered
      if ((menu_option < 0) || (menu_option > 8)) {
        // invalid option selected
        Serial.println("Only 0 to 8 are valid, send 8 to quit.");
      }
      else {  // valid menu option selected
        switch (menu_option) {
          case 1:  // set year
            Serial.println("Enter 2 digit year (00 to 99 valid) or \"q\" to quit.");
            Serial.print("Year: ");
            ret_val = GetNewTime(&new_date_time[6], 0, 99);
          break;
          
          case 2: // set month
            Serial.println("Enter 2 digit month (01 to 12 valid) or \"q\" to quit.");
            Serial.print("Month: ");
            ret_val = GetNewTime(&new_date_time[5], 1, 12);
          break;
          
          case 3: // set day
            Serial.println("Enter 2 digit day (01 to 31 valid) or \"q\" to quit.");
            Serial.print("Day: ");
            ret_val = GetNewTime(&new_date_time[3], 1, 31);
          break;
          
          case 4: // set hour
            Serial.println("Enter 2 digit hour (01 to 23 valid) or \"q\" to quit.");
            Serial.print("Hour: ");
            ret_val = GetNewTime(&new_date_time[2], 1, 23);
          break;
          
          case 5: // set minute
            Serial.println("Enter 2 digit minute (00 to 59 valid) or \"q\" to quit.");
            Serial.print("Minute: ");
            ret_val = GetNewTime(&new_date_time[1], 0, 59);
          break;
          
          case 6: // set second
            Serial.println("Enter 2 digit second (00 to 59 valid) or \"q\" to quit.");
            Serial.print("Second: ");
            ret_val = GetNewTime(&new_date_time[0], 0, 59);
          break;
          
          case 7: // write changes
            WriteDateTime(new_date_time);
            menu_option = 8;
          break;
          
          default:
          break;
        }
        if (ret_val == 1) {  // user enters invalid value
          Serial.println("Invalid value.");
        }
        else if (ret_val == 2) {  // user presses 'q' key to quit
          Serial.print("\r\n");
          PrintMenu();
        }
        ret_val = 0;
      }
    }
  }
}

// print the Set Date & Time menu on the serial port
void PrintMenu(void)
{
  Serial.println("------------------------");
  Serial.println("| Set Date & Time Menu |");
  Serial.println("------------------------");
  Serial.println("(Select 1 to 8)");
  Serial.println("1. Set year.");
  Serial.println("2. Set month.");
  Serial.println("3. Set day.");
  Serial.println("4. Set hour.");
  Serial.println("5. Set minute.");
  Serial.println("6. Set second.");
  Serial.println("7. Write changes and finish.");
  Serial.println("8. Exit without changing.");
}

// get new time/date from user via serial port
// returns 0 if success, 1 if invalid data entered, 2 if user quits
// *p_data = value entered by user converted to BCD
// lower = lowest valid value user can enter on serial port
// upper = highest valid value that user can enter on serial port
char GetNewTime(unsigned char *p_data, unsigned char lower, unsigned char upper)
{
  char rx_data[3];          // data received from serial port
  int index = 0;            // index into above array
  unsigned char data_val;   // stores converted data for limit checking
  
  // get 2 characters from the user over the serial port
  while (index < 2) {
    if (Serial.available()) {
      rx_data[index] = Serial.read();
      if (rx_data[index] == 'q') {
        return 2;    // user enters 'q' to quit
      }
      index++;
    }
    if (index > 1) {  // only proceed if 2 character entered
      data_val = ((rx_data[0] - '0') * 10) + (rx_data[1] - '0'); // convert ASCII BCD to binary
      // check for valid range and valid characters
      if ((data_val >= lower) && (data_val <= upper) && (rx_data[0] >= '0') &&
                  (rx_data[0] <= '9') && (rx_data[1] >= '0') && (rx_data[1] <= '9')) {
        // send entered data over the serial port for feedback to user
        rx_data[2] = 0;  // terminate string
        Serial.println(rx_data);
        // convert 2 ASCII characters received to BCD
        rx_data[0] = rx_data[0] - '0';
        rx_data[1] = rx_data[1] - '0';
        *p_data = rx_data[1];
        *p_data |= (rx_data[0] << 4);

      }
      else {
        return 1;  // invalid data entered
      }
    }
  }
  return 0;    // success
}

// write all 7 time/date fields to the RTC registers
// new_d_t[] array contains 0xFF in date/time fields that are not to be changed
// and contains valid BCD data in fields that are to be changed 
void WriteDateTime(unsigned char new_d_t[])
{
  char time_date_raw[7];      // time/date read from RTC
  int index = 0;              // index into above array
  
  // point to the time registers in the RTC
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0x02);
  Wire.endTransmission();
  
  // get the time
  Wire.requestFrom(RTC_ADDR, 7);
  while (Wire.available()) {
    time_date_raw[index] = Wire.read();
    index++;
    if (index >= 7) {
      index = 0;
      break;
    }
  }
  
  // write the new time/date values to the array that contains values read from the RTC
  for (int i = 0; i < 7; i++) {
    if (new_d_t[i] != 0xFF) {         // skip unchanged fields
      time_date_raw[i] = new_d_t[i];  // copy value if changed
    }
  }
  
  // point to the time registers in the RTC and send the time and date
  // write new values and old unchanged values
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0x02);
  Wire.write((uint8_t*)time_date_raw, 7);
  Wire.endTransmission();
}
