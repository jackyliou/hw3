#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7
I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);
DigitalOut redled(LED1);
InterruptIn btn(SW3); 
EventQueue queue1;
Thread thread1;
int m_addr = FXOS8700CQ_SLAVE_ADDR1;
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}
void blink(){
   redled = !redled;
}
float t[4];
void record(){
    if (t[2]<0.5|| abs(t[0])>0.5 || abs(t[1])>0.5)
      t[3]=1;
    else
      t[3]=0;
    pc.printf("%1.4f\r\n",t[0]);
    pc.printf("%1.4f\r\n",t[1]);
    pc.printf("%1.4f\r\n",t[2]);
    pc.printf("%1.4f\r\n",t[3]);
}

int id_r = 0;
int id_b = 0;
void stop(){
   queue1.cancel(id_b);
   redled=1;
   queue1.cancel(id_r);
}
void btn_p()  {
   id_r = queue1.call_every(100,record);
   id_b = queue1.call_every(300,blink);
   queue1.call_in(10100,stop);
}


int main() {
    pc.baud(115200);
    uint8_t who_am_i, data[2], res[6];
    int16_t acc16;
   
    int i;
   // Enable the FXOS8700Q

   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
   data[1] |= 0x01;
   data[0] = FXOS8700Q_CTRL_REG1;
   FXOS8700CQ_writeRegs(data, 2);

   // Get the slave address
   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);

   //pc.printf("Here is %x\r\n", who_am_i);
    
    redled = 1;
    thread1.start(callback(&queue1, &EventQueue::dispatch_forever));
    btn.rise(queue1.event(btn_p));   
    while(1){
      FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

      acc16 = (res[0] << 6) | (res[1] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[0] = ((float)acc16) / 4096.0f;

      acc16 = (res[2] << 6) | (res[3] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[1] = ((float)acc16) / 4096.0f;

      acc16 = (res[4] << 6) | (res[5] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[2] = ((float)acc16) / 4096.0f;
    }
    
}