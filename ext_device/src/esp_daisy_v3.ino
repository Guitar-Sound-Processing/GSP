/*
  esp_daisy

  This code runs in ESP32 module, usually ESP32 Wrover Tigo. You need to install
  the ESP32 board support for Arduino.
  Select the ESP32 Wrover Module in Arduino for uploading this code.

  The esp_daisy (this code) performs the following tasks
    1 - Receives GSP commands from Serial Terminal and send then to Daisy Seed through
        UART1 Serial_1 line
    2 - Receives GSP commands from Serial Bluetooth and send then to Daisy Seed through
        UART1 Serial_1 line
    3 - Receives replies from Daisy Seed UART1 serial line and delivers it to console
        (Serial Terminal) or Bluetooth
    4 - Receives Expression Pedal configuration commands from Daisy Seed and interpret
        them.
    5 - Reads analogue signals on specific ports from Expression Pedals and send the
        data to Daisy Seed.
*/

#include "BluetoothSerial.h"
#include "HardwareSerial.h"

// ESP32 WROOM SD pins 
//#define   SCLK  24
//#define   CS    23
//#define   MISO  25
//#define   MOSI  30

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Hardware Serial - Daisy Seed
HardwareSerial Serial_1(1);   // UART1  Command
// >>> HardwareSerial Serial_2(2);   // UART2  Pot data

// Blue tooth - required to GSP terminal
BluetoothSerial SerialBT;

// Potentiometers
uint32_t  timer, dtime;
bool      send_pot_data;
uint8_t   number_pot, ipot;
int       serial_delay  = 40;
char      pot_cs;
int       i, j, k;
bool      bi, bj;

//                      VP  VN                      
uint8_t   pot_pin[8] = {33, 34, 35, 36, 39, 12, 25, 26}; // for pot_id = 0, 1, ... 7
uint8_t   pot_seq[8];  // pin sequence to send pot data
int       max_number_pot  = 8;
uint16_t  pot;
uint8_t   pot_2bytes[2];

// UART - Daisy Seed
uint8_t   uart_ready = 0;
uint8_t   uart_ds;
uint8_t   uart_comm = 0;
char      ds_cs;

uint8_t   com_flag  = 1;
//uint32_t  com_time; 

// input device: 0=BT, 1=Serial Monitor
uint8_t   input_device = 0; 

// Serial line traffic control (time delay)
unsigned long com_delay, com_time;
uint32_t  wait_to_send;
bool      free_to_receive;

// Serial (console):
int32_t   serial_ptr = 0, serial_length;
bool      serial_ready;
char      serial_str[256];

// Serial 1 (Daisy Seed commands):
int32_t   serial1_ptr = 0, serial1_length;
bool      serial1_ready;
char      serial1_str[256];

// Serial 2 (Daisy Seed pot):
int32_t   serial2_ptr = 0, serial2_length;
bool      serial2_ready;
char      serial2_str[256];

// Serial BT (Blue Tooth):
int32_t   serialbt_ptr = 0, serialbt_length;
bool      serialbt_ready;
char      serialbt_str[256];

// Uart console
char      cl_cs;
char      cl_st[512];
uint8_t   cl_pt;

// Bluetooth Serial
char      bt_cs;
char      bt_st[512];
uint8_t   bt_pt;

// Prototypes:
void SerialCB ();
void Serial1CB ();
void Serial2CB ();
void SerialBTCB ();
void convert_to_2bytes(uint16_t data, uint8_t lsb_msb[]);

// Setup and loop functions
void setup() 
{

  uint8_t   iret;

  Serial.begin(115200);   // Console terminal
  
  SerialBT.begin("GSP"); //Bluetooth device name - To Android
  //Serial.println("The device started, now you can pair it with bluetooth!");

  //int tx1, rx1;
  //rx1   = 22;
  //tx1   = 23;
  //Serial_1.begin(115200, SERIAL_8N1, rx1, tx1); //Serial to Daisy Seed - Command
  
  Serial_1.begin(115200, SERIAL_8N1, 22, 23); //Serial to Daisy Seed - Command
  //Serial_1.begin(19200, SERIAL_8N1, 22, 23); //Serial to Daisy Seed - Command

// RX2   = 18;
// TX2   = 19;
// Serial_2.begin(115200, SERIAL_8N1, RX2, TX2); //Serial to Daisy Seed - Pots

  timer   = millis();
  
  // ADC for potentiometers
  dtime       = 100;
  number_pot  = 1;

  com_delay   = 1000;
  com_time    = 0;

  //  pinMode(pot_pin[0], INPUT_PULLUP);
  //  pinMode(pot_pin[1], INPUT_PULLUP);
  //  pinMode(pot_pin[2], INPUT_PULLUP);
  //  pinMode(pot_pin[3], INPUT_PULLUP);
  //  pinMode(pot_pin[4], INPUT_PULLUP);
  //  pinMode(pot_pin[5], INPUT_PULLUP);
  //  pinMode(pot_pin[6], INPUT_PULLUP);
  //  pinMode(pot_pin[7], INPUT_PULLUP);

  free_to_receive   = true;
  wait_to_send    = 100;

  serial_ptr    = 0;
  serial1_ptr   = 0;
  serial2_ptr   = 0;
  serialbt_ptr  = 0;

}

void loop() 
{

//  ****************************************************************
//  ===== Writing Serial_2: Sending POT data (nP1P2...Pn) to DS 

  if (millis() > timer + dtime)
  {
    timer   = millis();
    if (send_pot_data)
    {
      if (number_pot > 0)
      {
        //Serial_2.write(number_pot);
        Serial_1.print("}P");
        Serial_1.write(number_pot);
        //Serial.print("Pot data: ");
        //Serial.print(number_pot);
        for (ipot = 0; ipot < number_pot; ipot++)
        {
          pot     = analogRead(pot_seq[ipot])*4;   // convert 12 bits to 14 bits
          convert_to_2bytes(pot, pot_2bytes);
          Serial_1.write(pot_2bytes[0]);
          Serial_1.write(pot_2bytes[1]);
          //Serial.print(pot.full);
          //Serial.print(" ");
        }
        Serial_1.write(13); // Sending carriage return
        //Serial.println();
        delay(wait_to_send);
      }
    }
  }

//  ****************************************************************
//  Receiving commands from console (Serial), sending to DS (Serial1)

  //if (Serial.available() && free_to_receive == true)   // Input command from serial monitor
  while (Serial.available() && !serial_ready && free_to_receive)   // Input command from serial monitor
  {
    SerialCB();
  }
  if (serial_ready)
  {
    //Serial.println("Sending to DS");
    Serial_1.print("{");
    Serial_1.println(serial_str);
    //Serial_1.write('\n');
    //Serial_1.write('\r');
    serial_ready   = false;

    //Serial.print("ESP-DS:");        // remove
    //Serial.print(serial_str);       // remove

    serial1_ptr   = 0;        // needed to clear the input buffer of Serial1
    serial2_ptr   = 0;

    input_device = 1;
  }

//  ****************************************************************
//   Receiving commands from blue tooth (SerialBT), sending to DS (Serial1)

  //if (SerialBT.available() && free_to_receive == true)   // Input command from blue tooth

  while (SerialBT.available() && !serialbt_ready && free_to_receive)   // Input command from blue tooth
  {
    SerialBTCB();
  }
  if (serialbt_ready)
  {
    free_to_receive   = false;

    Serial_1.write('{');
    Serial_1.println(serialbt_str);
    //Serial_1.write('\n');

    serialbt_ready  = false;
    input_device = 0;
    //delay(wait_to_send);
  }

//  ****************************************************************
//  Receiving reply from Daisy Seed (Serial1), sending to console or blue tooth

  //if (Serial_1.available() && free_to_receive == false)   // reply from DS
  while (Serial_1.available())   // reply from DS
  {
    Serial1CB();
  }
  if (serial1_ready)
  {
    ds_cs   = serial1_str[0];
    serial1_length--;

    for (i = 0; i < serial1_length; i++)
    {
      serial1_str[i]  = serial1_str[i+1];
    }
    serial1_str[serial1_length] = 0;
    
    // Effect command
    if (ds_cs == '{')
    {
      if (input_device == 0)
      {
        SerialBT.println(serial1_str); // Sending to BT
        //delay(wait_to_send);
      }
      if (input_device == 1)
      {
        Serial.println(serial1_str);   // Sending to console
      }
    }
    // Expression pedal  command
    if (ds_cs == '}')
    {
      // Expression pedal
      uart_ds   = serial1_str[1];
      // S command (Start sending Expression Pedal data)
      if (uart_ds == 'S')
      {
        // Start to send potentiometer data
        send_pot_data   = true;
      }
      // A command (Decode Expression pedal command)
      if (uart_ds == 'A')
      {
        // Decode command to assign pot id with data sequence: "A<pot_id><pot_id>...\n"
        // pot_id: potentiometer id number (0, 1, 2, ...)
        number_pot  = 0;
        pot_cs      = serial1_str[2];
        while (pot_cs != 0)
        {
          // cs - 48 is the pot identifier
          if(number_pot >= max_number_pot)
          {
            if (input_device == 0) 
            {
              SerialBT.print("Can't assign more than ");
              SerialBT.print(max_number_pot);
              SerialBT.println(" Expression Pedals");
            }
            if (input_device == 1)
            {
              Serial.print("Can't assign more than ");
              Serial.print(max_number_pot);
              Serial.println(" Expression Pedals");
            }   
          }
          else
          {
            pot_seq[number_pot]   = pot_pin[pot_cs - 48];
            number_pot++;
            pot_cs    = serial1_str[number_pot+2];
          }
        }
        send_pot_data = false;
      }
      // C command (Stop sending Expression Pedal data and clear register)
      if (uart_ds == 'C')
      {
        // Stop sending potentiometer data and clear pot sequence
        send_pot_data = false;
        number_pot    = 0;
      }
    }
    delay(wait_to_send);
    serial1_ready   = false;
    free_to_receive   = true;
  }

//  ****************************************************************
  // Detecting that Daisy Seed is not replying to ESP32
  if (com_flag == 1)
  {
    if (com_time + 2000 < millis())
    {
      if (input_device == 0)
      {
        SerialBT.println("Daisy isn't responding..."); // Sending to BT
      }
      if (input_device == 1)
      {
        Serial.println("Daisy isn't responding...");  // Sending to console
      }
      com_flag = 0;
    }
  }
  
  return;
}

void SerialCB ()
{
  ds_cs   = Serial.read();
  if (ds_cs == 10 || ds_cs == 13 || ds_cs == 0)
  {
    if (serial_ptr > 0)
    {
      serial_str[serial_ptr] = 0;
      serial_length   = serial_ptr;
      serial_ready    = true;
      serial_ptr      = 0;
    }
  }
  else
  {
    serial_str[serial_ptr] = ds_cs;
    serial_ptr++;
  }
  return;
}

void Serial1CB ()
{
  ds_cs   = Serial_1.read();

  if (ds_cs == 10 || ds_cs == 13 || ds_cs == 0)
  {
    if (serial1_ptr > 0)
    {
      serial1_str[serial1_ptr] = 0;
      serial1_length  = serial1_ptr;
      serial1_ready   = true;
      serial1_ptr     = 0;
    }
  }
  else
  {
    serial1_str[serial1_ptr] = ds_cs;
    serial1_ptr++;
  }
  return;
}

/*
void Serial2CB ()
{
  ds_cs   = Serial_2.read();
  if (ds_cs == 10 || ds_cs == 13 || ds_cs == 0)
  {
    if (serial2_ptr > 0)
    {
      serial2_str[serial2_ptr] = 0;
      serial2_length  = serial2_ptr;
      serial2_ready   = true;
      serial2_ptr     = 0;
    }
  }
  else
  {
    serial2_str[serial2_ptr] = ds_cs;
    serial2_ptr++;
  }
  return;
}
*/

void SerialBTCB ()
{
  ds_cs   = SerialBT.read();
  if (ds_cs == 10 || ds_cs == 13 || ds_cs == 0)
  {
    if (serialbt_ptr > 0)
    {
      serialbt_str[serialbt_ptr] = 0;
      serialbt_length  = serialbt_ptr;
      serialbt_ready   = true;
      serialbt_ptr     = 0;
    }
}
  else
  {
    serialbt_str[serialbt_ptr] = ds_cs;
    serialbt_ptr++;
  }
  return;
}

void convert_to_2bytes(uint16_t data, uint8_t lsb_msb[])
{
  /*
    Function to convert a 14 bit unsigned integer number 
    stored in data to two bytes with 7 significant bits each 
    in lsb_msb[2] (less significant byte and most significant 
    byte). The most signficant bit of lsb_msb will allways be
    set to one. Therefore both bytes never will be in the non
    printable range of ASCII.
  */
  lsb_msb[0]  = 128 | (127 & data);
  lsb_msb[1]  = 128 | (127 & (data >> 7));
  return;
}

