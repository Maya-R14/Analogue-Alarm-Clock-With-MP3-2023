#use i2c(Master,fast=350000,sda=PIN_C4,scl=PIN_C5,FORCE_SW)

void MCP23008_Write(int Device, int Address, int Data)
{
   //Device = Device | 0x00;           // 1 at lowest bit for indicating writing, 0x40 is device address
   i2c_start();                                    // Start of communication
   i2c_write(Device);                       // To indicate to I2C which device we're using
   i2c_write(Address);                     // Indicates which register we're writing to
   i2c_write(Data);                               // Byte to write for config or for setting the pins
   i2c_stop();                                     // End of communication
}

Void Enable_MCP23008 (void)
{
  MCP23008_Write(0x40, 0x00, 0x00); //IODIR
  MCP23008_Write(0x40, 0x05, 0x20); //IOCON
}

Void MCP23008_OUT (int output)
{
  MCP23008_Write(0x40, 0X09, output);
}
