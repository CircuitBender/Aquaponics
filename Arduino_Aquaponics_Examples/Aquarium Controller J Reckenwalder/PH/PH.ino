
  float adjustPHBasedOnTemp(float PH, float temp)
    {
   // http://www.omega.com/Green/pdf/pHbasics_REF.pdf
   // When the temperature is other than 25degC and the ph is other than 7
   // the temperature error is 0.03ph error/ph unit/10degC
   // which means error = 0.03*(ph away from 7)*(tempdiffC/10)
   
    float phDifference = abs(PH-7);
    float tempDifferenceC = abs(temp-25);
    float phAdjust = (0.03*phDifference)*(tempDifferenceC/10);
    
    if(PH>7 && temp<25)
      phAdjust=phAdjust;
 
    if(PH>7 && temp>25)
      phAdjust=phAdjust*-1;
 
    if(PH<7 && temp>25)
      phAdjust=phAdjust;
 
    if(PH<7 && temp<25)
      phAdjust=phAdjust*-1;
 
    float tempAdjustedPH = PH + phAdjust;
    return tempAdjustedPH;
    }
 
 
  double getPHVolts()
    {
  byte ad_high;
  byte ad_low;
  
  Wire.requestFrom(PHADDRESS, 2);        //requests 2 bytes
  while(Wire.available() < 2);         //while two bytes to receive
    
  ad_high = Wire.read();           
  ad_low = Wire.read();
  double units = (ad_high * 256) + ad_low;
  
  double volts =  (units /4096)*3; 
  return volts;  
    }  
 
 
  double getRoomTemperatureC()
    {
  Wire.requestFrom(RoomTempI2CAddress,2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
 
  int TemperatureSum = ((MSB << 8) | LSB) >> 4;
  double celsius = TemperatureSum*0.0625;
  
  return celsius;
    }
 
void SetRoomTemperataureResolutionBits(int ResolutionBits)
    {
  if (ResolutionBits < 9 || ResolutionBits > 12) exit;
  Wire.beginTransmission(RoomTempI2CAddress);
  Wire.write(B00000001); //addresses the configuration register
  Wire.write((ResolutionBits-9) << 5); //writes the resolution bits
  Wire.endTransmission();
 
  Wire.beginTransmission(RoomTempI2CAddress); //resets to reading the temperature
  Wire.write((byte)0x00);
  Wire.endTransmission();
    }
