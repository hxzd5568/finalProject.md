volatile int rate[10];
volatile unsigned long sampleCounter = 0;
volatile unsigned long lastBeatTime = 0;
volatile int P = 512;
volatile int T = 512;
volatile int thresh = 512;
volatile int amp = 100;
volatile boolean firstBeat = true;
volatile boolean secondBeat = false;
int pulsePin = A0;
int blinkPin = 13;
volatile int BPM;
volatile int Signal;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;

char val; // Data received from the serial port
//int ledPin = 4; // Set the pin to digital I/O 4



int DI = 12;
int RW = 11;
int DB[] = {3, 4, 5, 6, 7, 8, 9, 10};//使用数组来定义总线需要的管脚
int Enable = 2;


void setup() {
  // pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  pinMode(blinkPin, OUTPUT);
  Serial.begin(115200);
  interruptSetup();




  int i = 0;
  for (i = Enable; i <= DI; i++) {
    pinMode(i, OUTPUT);
  }
  delay(100);
  // 短暂的停顿后初始化LCD
  // 用于LCD控制需要
  LcdCommandWrite(0x38);  // 设置为8-bit接口，2行显示，5x7文字大小
  delay(64);
  LcdCommandWrite(0x38);  // 设置为8-bit接口，2行显示，5x7文字大小
  delay(50);
  LcdCommandWrite(0x38);  // 设置为8-bit接口，2行显示，5x7文字大小
  delay(20);
  LcdCommandWrite(0x06);  // 输入方式设定
  // 自动增量，没有显示移位
  delay(20);
  LcdCommandWrite(0x0E);  // 显示设置
  // 开启显示屏，光标显示，无闪烁
  delay(20);
  LcdCommandWrite(0x01);  // 屏幕清空，光标位置归零
  delay(100);
  LcdCommandWrite(0x80);  // 显示设置
  // 开启显示屏，光标显示，无闪烁
  delay(20);
}






void loop() {
  while (Serial.available()) { // If data is available to read,
    val = Serial.read(); // read it and store it in val
  }
  if (val == 'H') writeThingOne();
  if (val == 'M') writeThingTwo();
   if (val == 'N') writeThingThree();


  sendDataToProcessing('S', Signal);
  if (QS == true) {
    sendDataToProcessing('B', BPM);
    sendDataToProcessing('Q', IBI);
    QS = false;

  }
  delay(20);
}
void sendDataToProcessing(char symbol, int data ) {
  Serial.print(symbol);
  Serial.println(data);
}








void interruptSetup() {
  TCCR2A = 0x02;
  TCCR2B = 0x06;
  OCR2A = 0X7C;
  TIMSK2 = 0x02;
  sei();
}




ISR(TIMER2_COMPA_vect) {
  cli();
  Signal = analogRead(pulsePin);
  sampleCounter += 2;
  int N = sampleCounter - lastBeatTime;
  if (Signal < thresh && N > (IBI / 5) * 3) {
    if (Signal < T) {
      T = Signal;
    }
  }
  if (Signal > thresh && Signal > P) {
    P = Signal;
  }
  if (N > 250) {
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3) ) {
      Pulse = true;
      digitalWrite(blinkPin, HIGH);
      IBI = sampleCounter - lastBeatTime;
      lastBeatTime = sampleCounter;
      if (secondBeat) {
        secondBeat = false;
        for (int i = 0; i <= 9; i++) {
          rate[i] = IBI;
        }
      }
      if (firstBeat) {
        firstBeat = false;
        secondBeat = true;
        sei();
        return;
      }
      word runningTotal = 0;
      for (int i = 0; i <= 8; i++) {
        rate[i] = rate[i + 1];
        runningTotal += rate[i];
      }
      rate[9] = IBI;
      runningTotal += rate[9];
      runningTotal /= 10;
      BPM = 60000 / runningTotal;
      QS = true;
    }
  }
  if (Signal < thresh && Pulse == true) {
    digitalWrite(blinkPin, LOW);
    Pulse = false;
    amp = P - T;
    thresh = amp / 2 + T;
    P = thresh;
    T = thresh;
  }

  if (N > 2500) {
    thresh = 512;
    P = 512;
    T = 512;
    lastBeatTime = sampleCounter;
    firstBeat = true;
    secondBeat = false;
  }
  sei();
}




void writeThingOne() {
  LcdCommandWrite(0x01);  // 屏幕清空，光标位置归零
  delay(10);
  LcdCommandWrite(0x80 + 1);
  delay(10);
  // 写入欢迎信息
  LcdDataWrite('I');
  LcdDataWrite(' ');
  LcdDataWrite('w');
  LcdDataWrite('i');
  LcdDataWrite('l');
  LcdDataWrite('l');
  LcdDataWrite(' '); LcdDataWrite('b'); LcdDataWrite('e'); LcdDataWrite(' ');
  LcdDataWrite('b');
  LcdDataWrite('a');
  LcdDataWrite('c'); LcdDataWrite('k');
  delay(10);
  LcdCommandWrite(0xc0 + 1); // 定义光标位置为第二行第二个位置
  delay(10);
  LcdDataWrite('a'); LcdDataWrite('t'); LcdDataWrite(' '); LcdDataWrite('9'); LcdDataWrite('p'); LcdDataWrite('m');
  delay(5000);
  LcdCommandWrite(0x01);  // 屏幕清空，光标位置归零
  delay(10);
  LcdCommandWrite(0x80 + 1); //定义光标位置为第一行第六个位置
  delay(10);
  LcdDataWrite('Y');
  LcdDataWrite('o');
  LcdDataWrite('u');
  LcdDataWrite(' ');
  LcdDataWrite('e');
  LcdDataWrite('a');
  LcdDataWrite('t');
  LcdDataWrite(' ');
  LcdDataWrite('o');
  LcdDataWrite('u');
  LcdDataWrite('t');
  LcdDataWrite(' ');
  LcdDataWrite('b');
  LcdDataWrite('y');
  LcdDataWrite(' ');

  LcdCommandWrite(0xc0 + 1); // 定义光标位置为第二行第二个位置
  delay(10);

  LcdDataWrite('y'); LcdDataWrite('o'); LcdDataWrite('u'); LcdDataWrite('r'); LcdDataWrite('s'); LcdDataWrite('e'); LcdDataWrite('l'); LcdDataWrite('f'); delay(3000);

}
void writeThingTwo() {
  LcdCommandWrite(0x01);  // 屏幕清空，光标位置归零
  delay(10);
  LcdCommandWrite(0x80);
  delay(10);
  // 写入欢迎信息
  LcdDataWrite('I');
  LcdDataWrite(' ');
  LcdDataWrite('m');
  LcdDataWrite(' ');
  LcdDataWrite('p');
  LcdDataWrite('l');
  LcdDataWrite('a');
  LcdDataWrite('y');
  LcdDataWrite('i');
  LcdDataWrite('n');
  LcdDataWrite('g');
  LcdDataWrite(' ');
  LcdDataWrite('w');
  LcdDataWrite('i');
  LcdDataWrite('t');
  LcdDataWrite('h');
  delay(10);
  LcdCommandWrite(0xc0 + 1); // 定义光标位置为第二行第二个位置
  delay(10);

  LcdDataWrite('T');
  LcdDataWrite('o');
  LcdDataWrite('m');
  
  LcdDataWrite(' ');
  LcdDataWrite('a');
  LcdDataWrite('t');
  LcdDataWrite(' ');
  LcdDataWrite('g');
  LcdDataWrite('a');
  LcdDataWrite('r');
  LcdDataWrite('d');
  LcdDataWrite('e');
  LcdDataWrite('n');
  delay(5000);
   LcdCommandWrite(0x01);  // 屏幕清空，光标位置归零
  delay(10);
  LcdCommandWrite(0x80+3);
  delay(10);
    LcdDataWrite('D');
  LcdDataWrite('o');
  LcdDataWrite('n');
  LcdDataWrite('n');
  LcdDataWrite('o');
  LcdDataWrite('t');
  LcdDataWrite(' ');
  LcdDataWrite('w');
  LcdDataWrite('o');
  LcdDataWrite('r');
  LcdDataWrite('r');
  LcdDataWrite('y');
   delay(10);
  LcdCommandWrite(0xc0 + 5); // 定义光标位置为第二行第二个位置
  delay(10);

  LcdDataWrite('a');
  LcdDataWrite('b');
  LcdDataWrite('o');
  LcdDataWrite('u');
  LcdDataWrite('t');
  LcdDataWrite(' ');
  LcdDataWrite('m');
  LcdDataWrite('e');
  LcdDataWrite('.');
  delay(5000);
}



void writeThingThree() {
  LcdCommandWrite(0x01);  // 屏幕清空，光标位置归零
  delay(10);
  LcdCommandWrite(0x80 );
  delay(10);
  // 写入欢迎信息
  LcdDataWrite('I');
  LcdDataWrite(' ');LcdDataWrite('a');
  LcdDataWrite('m');
  LcdDataWrite(' ');
  LcdDataWrite('t');
  LcdDataWrite('o');
  LcdDataWrite('o');
  LcdDataWrite(' ');
  LcdDataWrite('s');
  LcdDataWrite('l');
  LcdDataWrite('e');
  LcdDataWrite('e');
  LcdDataWrite('p');
  LcdDataWrite('y');
  LcdDataWrite(' ');

  delay(10);
  LcdCommandWrite(0xc0 ); // 定义光标位置为第二行第二个位置
  delay(10);
  LcdDataWrite('t');
  LcdDataWrite('o');
  LcdDataWrite(' ');
  LcdDataWrite('w');
  LcdDataWrite('a');
  LcdDataWrite('i');
  LcdDataWrite('t');
  LcdDataWrite(' ');
  LcdDataWrite('f');
  LcdDataWrite('o');
  LcdDataWrite('r');
  LcdDataWrite(' ');
  LcdDataWrite('y');
  LcdDataWrite('o');
  LcdDataWrite('u');
  LcdDataWrite('r');

  delay(5000);
  LcdCommandWrite(0x01);  // 屏幕清空，光标位置归零
  delay(10);
  LcdCommandWrite(0x80 + 1); //定义光标位置为第一行第六个位置
  delay(10);
  LcdDataWrite('r');
  LcdDataWrite('e');
  LcdDataWrite('t');
  LcdDataWrite('u');
  LcdDataWrite('r');
  LcdDataWrite('n');
  delay(10);
  LcdCommandWrite(0xc0 ); // 定义光标位置为第二行第二个位置
  delay(10);
  LcdDataWrite('S');
  LcdDataWrite('e');
   LcdDataWrite('e');
  LcdDataWrite(' ');
  LcdDataWrite('y');
   LcdDataWrite('o');
  LcdDataWrite('u');
  LcdDataWrite(' ');
   LcdDataWrite('t');
  LcdDataWrite('o');
  LcdDataWrite('m');
   LcdDataWrite('o');
  LcdDataWrite('r');
  LcdDataWrite('r');
   LcdDataWrite('o');
    LcdDataWrite('w');
 

}

void LcdCommandWrite(int value) {
  // 定义所有引脚
  int i = 0;
  for (i = DB[0]; i <= DI; i++) //总线赋值
  {
    digitalWrite(i, value & 01); //因为1602液晶信号识别是D7-D0(不是D0-D7)，这里是用来反转信号。
    value >>= 1;
  }
  digitalWrite(Enable, LOW);
  delayMicroseconds(1);
  digitalWrite(Enable, HIGH);
  delayMicroseconds(1);  // 延时1ms
  digitalWrite(Enable, LOW);
  delayMicroseconds(1);  // 延时1ms
}

void LcdDataWrite(int value) {
  // 定义所有引脚
  int i = 0;
  digitalWrite(DI, HIGH);
  digitalWrite(RW, LOW);
  for (i = DB[0]; i <= DB[7]; i++) {
    digitalWrite(i, value & 01);
    value >>= 1;
  }
  digitalWrite(Enable, LOW);
  delayMicroseconds(1);
  digitalWrite(Enable, HIGH);
  delayMicroseconds(1);
  digitalWrite(Enable, LOW);
  delayMicroseconds(1);  // 延时1ms
}

