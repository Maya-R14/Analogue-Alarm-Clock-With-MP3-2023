#use i2c(Master,fast=350000,sda=PIN_C7,scl=PIN_C6,FORCE_SW)

#Define AM 0x40 //01000000
#Define PM 0x60 //01100000

#define ON 0x1E  //00011110
#define OFF 0x1C //00011100


int DEC_2_BCD (INT to_convert)
{
   RETURN ((to_convert / 10) << 4) + (to_convert % 10);
}

int  BCD_2_DEC(INT to_convert)
{
   RETURN (to_convert >> 4) * 10 + (to_convert&0x0F);
}

void Set_Time(hr,min,AMPM)
{
   I2C_start ();
   I2C_Write (0xD0);
   I2C_Write (0);
   I2C_Write (0); //update sec
   I2C_Write (DEC_2_BCD (min)); //update min
   I2C_Write (AMPM|DEC_2_BCD (hr)); //update hour
   I2C_stop ();
}

int Read_time_BCD (INT Adr)
{
   INT data;
   I2C_start ();
   I2C_Write (0xD0);
   I2C_Write (Adr);
   I2C_start ();
   I2C_Write (0xD1);
   data = I2C_read (0);
   I2C_stop ();
   
   RETURN data;
}

int Read_time_DEC (INT Adr)
{
   INT data;
   I2C_start ();
   I2C_Write (0xD0);
   I2C_Write (Adr);
   I2C_start ();
   I2C_Write (0xD1);
   data = I2C_read (0);
   I2C_stop ();
   
   RETURN BCD_2_DEC (data) ;
}

void Alarm_status (int ONOFF) //ON and Off alarm on Control register (2nd alarm)
{
   I2C_start ();
   I2C_Write (0xD0);
   I2C_Write (0x0E);
   I2C_Write (ONOFF);
   I2C_stop ();
}
void Clear_Alarm (void) //Reset Status register and clear interrupt flag
{
   I2C_start ();
   I2C_Write (0xD0);
   I2C_Write (0x0F);
   I2C_Write (0X0C);   //00001100
   I2C_stop ();
}
 

void Set_Alarm(hr,min,AMPM) //Set alarm based on set time function
{
   I2C_start ();
   I2C_Write (0xD0);
   I2C_Write (0x0B);
   I2C_Write (DEC_2_BCD (min)); //update min
   I2C_Write (AMPM|DEC_2_BCD (hr)); //update hour
   I2C_Write (0X80); //10000000
   I2C_stop ();
}
