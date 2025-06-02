
float k1,k,k2=0;
float target =0;
int w;
void motorForward(int distance){
  if(distance >0){
    PORTH = 64;
  }
  else{
    PORTH =32;
  }
  k=0;
  target = distance;  
}

void setup() {
  DDRB |= 0b00100000;
  DDRD &=0;
  DDRH = 0b01100000;
  Serial.begin(115200);
  EICRB = 0x03;
  EIMSK = 0x10;
  PORTH |=64; 

  TCCR1A = (1 << COM1A1) | (1 << WGM11); // Fast PWM mode
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler = 8
  ICR1 = 10000; // Set TOP value for 50Hz PWM
  OCR1A = 9999; // 50% duty cycle

  motorForward(1100);
  sei();
}

void loop() {
  w= 9999*2*atan((target - k)/200)/3.141;
  OCR1A = (int)abs(w);
  Serial.print(w);
  Serial.print("  ");
  Serial.print((k-k1));
  Serial.print("  ");
  Serial.println(k);
}

ISR(INT4_vect){
  if(PINE/32){
    k1 =k;
    k +=1;
  }
  else{
    k1=k;
    k -=1;
  }

}
