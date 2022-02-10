# ECC608-TNG-AES-Test-raspi

This communicates ATECC608A-TNGTLS secure chip from raspberry pi and test AES function by using sample vector.     

# Requirements

  install "sudo apt install build-essential libssl-dev".  
  Enable I2C on raspi-config.  

# Environment reference
  
  Raspberry pi 3,4   
  this project initializes I2C 1 port.  
  pin assigned as below:  


      I2C 1 SDA pin3   
      I2C 1 SCL pin5  
          
  Microchip ATECC608B-TNGTLS(on I2C port 1)  

# Usage

"git clone --recursive <this pages URL>" on your target directory and make.     

# Run this project

execute "./tng-test" on termial.  

# License

This software is released under the MIT License, see LICENSE.
