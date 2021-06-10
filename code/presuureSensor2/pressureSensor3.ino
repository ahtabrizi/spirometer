// https://os.mbed.com/users/epremeaux/code/MS4525_pitot_tube/

#include "F303_logger.h"
#include "MS4525DO.h"
 
Serial terminal(USBTX, USBRX);
I2C i2c(I2CSDA, I2CSCL);
 
MS4525DO Pitot(i2c); 
 
DigitalOut myled(LED1);
 
 
int main() {
    terminal.baud(115200); 
    terminal.printf("MS4525DO library test\n");
 
    while(1) {
        char PitotStatus;    // A two bit field indicating the status of the I2C read
        PitotStatus = Pitot.measure();
 
        switch (PitotStatus)
        {
            case 0:
            //Serial.println("Ok ");
            terminal.printf("PSI: %f\n", Pitot.getPSI());
            terminal.printf("Temperature: %f\n", Pitot.getTemperature());
            terminal.printf("Airspeed: %f\n", Pitot.getAirSpeed());
            terminal.printf("\n");
            break;
            
            case 1:
            terminal.printf("Busy\n");
            break;
            
            case 2:
            terminal.printf("Stale\n");
            break;
            
            default:
            terminal.printf("Error\n");
            break;
        }
        myled = !myled;
        wait_ms(200);
    }
}
 
