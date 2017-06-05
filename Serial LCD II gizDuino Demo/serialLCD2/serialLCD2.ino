/* Demonstration Sketch for e-Gizmo Serial LCD *
*  by e-Gizmo Mechatronix Central
*
*  This sketch contains simple communications
*  interface functions you can adopt and improve
*  according to your requirements
*/

/* How to Connect */
/* Serial LCD TXD ---> gizDuino/Arduino RXD
*  Serial LCD RXD ---> gizDuino/Arduino TXD
*  Serial LCD GND ---> gizDuino/Arduino GND
*/

char  rxmsg[30];
byte stage;
byte rxctr;
byte  msg;

#define  MODEPIN  2

void setup(){
  int i;
  Serial.begin(9600);
  delay(1500);          // Allow time for Serial LCD to set up
  //configure all remote I/O as outputs
  for(i=0;i<11;i++) IO("o",i);
  digitalWrite(MODEPIN,HIGH);
  if(digitalRead(MODEPIN)==HIGH) Send_Command("F"); 
  //Send_Command("F"); 
  
}

void  loop(void)
{
  int i=0;
  int ledon=0;
  int backlight=0;
  
    // Test LCD Screen
  Send_Command("m          1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ_-+=");
  delay(1000);
  Send_Command("M          1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ_-+=");
  // no effect if Serial LCD is configured for 2x16 display
  Send_To("3","Test Program","4");
  Send_To("4","***Serial LCD Kit***","0");

  
 while(1){
   // clear all outputs
   for(i=0;i<11;i++) IO("R",i);
   IO("S",ledon);
   delay(100);
   ledon++;
   backlight++;
   if(ledon>=11) ledon=0;
   if(backlight>19) backlight=0;
   IO("B",backlight);
 }
}

// Wait for EZ HMI response

void  wait_response(void){
  while(stage !=3) read_buffer();
}

/* This routine checks the serial input *
* scans for the occurrence of STX and ETX *
* and extract the message stripped of STX *
* and ETX marker
* Rx message in rxmsg[]
* stage = 3 indicates RX complete
*/

void  read_buffer(void){
  if(Serial.available()>0){
      char  rxchar=Serial.read();  // Read one Rxed character
      if(stage==0){                // Look for STX
        if(rxchar==0x02){          // STX?
          stage=1;                // STX is found, stage 1
          return;
        }
      }
      if(stage==1){              // Store next Rx char until ETX is detected
        if(rxchar==0x03)        // ETX?
        {
          stage=3;              // ETX found, data ready=stage 3
          rxmsg[rxctr]=0;      // NULL String terminator
        }
        else
        {
            if(rxctr<30){      // collect data until ETX is found or buffer full
              rxmsg[rxctr]=rxchar;
              rxctr++;
            }
          }
      }
  
    }
}

/* This function sends the EZ HMI command
*  wrapped in STX and ETX marker
*/

void Send_Command(char *message){
  Serial.write(0x02);      //STX
  Serial.print(message);
  Serial.write(0x03);      //ETX
  stage=0;                 //Reset Rx buffer
  rxctr=0;
  wait_response();        // Wait until EZ HMI responds
}


/* Display Function with print start position
*  line= which line to print to, 0 or 1
*  message = message to print
*  pos = print start position
*/

void Send_To(char *line,char *message,char *pos){
  
 String k=message;        // Save input string
 Serial.write(0x02);      //STX
  Serial.print(">");      // Send position info
  Serial.print(pos);
  Serial.write(0x03);    //ETX
  stage=0;
  rxctr=0;
  wait_response();
  
  Serial.write(0x02); 
  Serial.print(line);    // Send message to line
  Serial.print(k);
  Serial.write(0x03);
  stage=0;
  rxctr=0;
  wait_response();

}

// I/O service
void IO(char *message,int port){
  Serial.write(0x02);      //STX
  Serial.print(message);
  Serial.print(port);
  Serial.write(0x03);      //ETX
  stage=0;                 //Reset Rx buffer
  rxctr=0;
  wait_response();        // Wait until EZ HMI responds
}
