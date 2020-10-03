#include<Wire.h>
const int piezo_pin = A0; // Piezo output A0
const int moisture_pin = A1; // moisture sensor A1
int fire_sensor=7; //smoke sensor- fire 7
int result [15];
int ir_pin = 13; // Connecting D7 to IR sensor Vout 
int val = 0; // variable for reading the pin status 
int hall=0;

#include <SFE_BMP180.h> //Including BMP180 library
#define ALTITUDE 35.6 //Altitude where I live (change this to your altitude)
SFE_BMP180 pressure; //Creating an object
//temp and humidity
int tempsense = 0;
int rhsense = 0;
int temp = 0;
int rh = 0;
char dataString[100] = {0};
const int MPU_addr=0x69;
int16_t axis_X,axis_Y,axis_Z;

int minVal=265;
int maxVal=402;

double x;
double y;
double z;
 
void setup() 
{  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
 
  Serial.begin(9600);


pinMode(fire_sensor,INPUT);  //fire sensor input

pinMode(ir_pin, INPUT); // declare Infrared sensor as input

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  pinMode(3, INPUT); // Read the input on pin 2
pinMode(12, OUTPUT); //led
}


void loop() 
{
  // Read Piezo ADC value in, and convert it to a voltage
  int piezoADC = analogRead(piezo_pin);
  float piezoV = piezoADC / 1023.0 * 5.0;
  //Serial.print("piezo:");
  //Serial.println(piezoV); // Print the voltage.
  //delay(200);
  result[0]=2;
  result[1]=(int)piezoV;

  //MOISTURE
  float moisture_percentage;
  int sensor_analog;
  sensor_analog = analogRead(moisture_pin);
  moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) );
  //Serial.print("Moisture Percentage = ");
  //Serial.print(moisture_percentage);
  //Serial.print("%\n\n");
  //delay(200);
  result[2]=(int)moisture_percentage;

  //FIRE
  int gas_value;
  gas_value=digitalRead(fire_sensor);
//Serial.print("fire");
//Serial.println(gas_value);
//delay(200);
result[3]=gas_value;
//HUMIDITY AND TEMP
tempsense = (analogRead(3));
rhsense = (analogRead(4));
temp = ((tempsense/50)+10);
rh = ((30.855*(rhsense/204.6))-11.504);

//Serial.print("Temperature: ");
//Serial.print(temp);
//Serial.println(" C ");
result[4]=(int)temp;
//delay(200);
//Serial.print("Humidity: ");
//Serial.print(rh);
//Serial.println("% "); 
  result[5]=(int)rh;                                         
//delay(200);


//OBSTACLE
 val = digitalRead(ir_pin); // read input value 
   if (val == HIGH)
   { // check if the input is HIGH
      //Serial.println("NO OBSTACLE");  
      result[6]=0;
   } 
   else 
   { 
      //Serial.println("THERE'S AN OBSTACLE");
      result[6]=1;
   } 

//   delay(200);

//INCLINATION
Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  axis_X=Wire.read()<<8|Wire.read();
  axis_Y=Wire.read()<<8|Wire.read();
  axis_Z=Wire.read()<<8|Wire.read();
    int xAng = map(axis_X,minVal,maxVal,-90,90);
    int yAng = map(axis_Y,minVal,maxVal,-90,90);
    int zAng = map(axis_Z,minVal,maxVal,-90,90);

       x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
       y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
       z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

  /*   Serial.print("Angle of inclination in X axis = ");
     Serial.print(x);
     Serial.println();

     Serial.print("Angle of inclination in Y axis= ");
     Serial.print(y);
     Serial.println();

     Serial.print("Angle of inclination in Z axis= ");
     Serial.print(z);
     Serial.println();
     Serial.println("-------------------------------------------");
     delay(100);*/
     result[7]=(int)x;
     result[8]=(int)y;
     result[9]=(int)z;



//ATMOSPHERIC PRESSURE
char status;
  double T, P, p0; //Creating variables for temp, pressure and relative pressure
  
  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0) {
     // Serial.print("Temp: ");
      //Serial.print(T, 1);
      //Serial.println(" deg C");

      status = pressure.startPressure(3);

      if (status != 0) {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0) {
        //  Serial.print("Pressure measurement: ");
        //  Serial.print(P);
        //  Serial.println(" hPa (Pressure measured using temperature)");
result[10]=P;
          p0 = pressure.sealevel(P, ALTITUDE);
         // Serial.print("Relative (sea-level) pressure: ");
        //  Serial.print(p0);
         // Serial.println("hPa");
         result[11]=p0;
        }
      }
    }
  }

//HALL
hall = digitalRead( 3 ); // Go read the pin.
//Serial.println(val); // just to see on the serial monitor what I read.

if ( hall == HIGH )
{digitalWrite ( 12, HIGH ); // Turn on the LED when the value is high.
result[12]=1;
}
else
{digitalWrite ( 12, LOW ); // Turn off the LED when the value is low.
result[12]=0;
}
  
  result[13]=4; 
  sprintf(dataString,"%02X;%02X;%02X;%02X;%02X;%02X;%02X;%02X;%02X;%02X;%02X;%02X;%02X;%02X",result[0],result[1],result[2],result[3],result[4],result[5],result[6],result[7],result[8],result[9],result[10],result[11],result[12],result[13],0);
Serial.println(dataString);
}

