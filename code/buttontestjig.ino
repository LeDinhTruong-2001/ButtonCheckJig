#define step 5 
#define dir 4

#define limit_up 6
#define limit_down 7
#define button 2
#define st 10
#define sto 3

#define up 8
#define down 9
char formattedStr[8]; // Mảng để lưu trữ dữ liệu đã định dạng
#include <SoftwareSerial.h>
int dem=0; int bt;int tocdo, ck=10, k=1; float hanhtrinh, resetpoint, reset=0, ereset, buoc;
int LU, LD, u, d; int start, dung, dstart;
bool buttonState = HIGH;  // Trạng thái nút nhấn ban đầu
bool buttonPressed = false;  // Biến cờ để kiểm tra nút đã được nhấn
SoftwareSerial mySerial(18,19); // Sử dụng SoftwareSerial cho việc gửi và nhận dữ liệu UART

// ct con chạy motor
void run()
{
  digitalWrite(step, HIGH);
  delayMicroseconds(tocdo);
  digitalWrite(step, LOW);
  delayMicroseconds(tocdo);
}

//ct con motor trở về vị trí setpoint
void setpoint()
{
  // Đoạn mã xử lý ngắt nút nhấn
  int newState = digitalRead(button);
  
  if (newState == LOW && buttonState == HIGH) {
    buttonPressed = true;  // Đặt biến cờ khi nút được nhấn
  }
  
  buttonState = newState;  // Cập nhật trạng thái nút nhấn
}
//CT con dừng động cơ
void stop()
{
  digitalWrite(step, LOW);
  delayMicroseconds(1000);
}

void UARTDataReceived() {
  // Xử lý dữ liệu từ cổng UART khi có ngắt
  while (mySerial.available() > 0) {
    char receivedChar = mySerial.read();//mySerial.readString()
    Serial.print(receivedChar); // In dữ liệu ra Serial Monitor
    // Thực hiện xử lý dữ liệu ở đây
    if(receivedChar == 't')
    {
        nutstart();
    }
    if(receivedChar == 's')
    {
        nutstop();
    }
    if(receivedChar == 'u')
    {
        nutup();
    }
    if(receivedChar == 'd')
    {
        nutdown();
    }
  }
}

void nutstart()
{
  dstart=1;
  dem=0;
  ck=10;
  buoc=0;
}
void nutstop()
{
  stop();
      delay(500);
    dstart=0;
    digitalWrite(dir, LOW);
    ereset =0;
    for (int i=0; i< buoc && LU ==0; i++)
        {
          tocdo = 150;
          run();
        }
        buoc=0;
}
void nutup()
{
  digitalWrite(dir, LOW);
      for(int i =0; i<2; i++)
      {
      tocdo=400;
      run();
      }
}
void nutdown()
{
  digitalWrite(dir, HIGH);
      for(int i =0; i<2; i++)
      {
      tocdo = 500;
      run();
      }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(button, INPUT_PULLUP); // chế độ input, điện trở kéo lên
  pinMode(limit_up, INPUT_PULLUP); // chế độ input, điện trở kéo lên
  pinMode(limit_down, INPUT_PULLUP);// chế độ input, điện trở kéo lên
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(step, OUTPUT); // chế độ output
  pinMode(dir, OUTPUT); // chế độ output
  pinMode(st, INPUT_PULLUP);
  pinMode(sto, INPUT_PULLUP);
  // NGAT CHO BUTTON
  attachInterrupt(0, setpoint, CHANGE);
  // ngắt cho giới hạn trên
 // attachInterrupt(1, nutstop, LOW);
  // ngắt cho giới hạn dưới
  //attachInterrupt(0, nonactive, LOW);
  attachInterrupt(digitalPinToInterrupt(18), UARTDataReceived, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  //bt = digitalRead(button);
  LU = digitalRead(limit_up);
  LD = digitalRead(limit_down);
  u = digitalRead(up);
  d = digitalRead(down);
  start = digitalRead(st);
  dung = digitalRead(sto);

  if(ck>0)
  {
    // nút start
    if(start == 1 && d==0 && u==0 && dung==0)
    {
      nutstart();
    }
    if (buttonPressed&& LU==0 && LD==0 && dstart==1) {
      // Xử lý sự kiện khi nút nhấn được nhấn (buttonPressed == true)
      // chạy xuống thêm 2mm khi nút active
      for(int i =0; i<3600;i++)
      {
        tocdo=150;
        run();
      }
      float(hanhtrinh) = (2*float(buoc))/7200; // Dữ liệu cần gửi
      //Serial.print("hanh trinh: ");
      sprintf(formattedStr, "%.3f", hanhtrinh);
      Serial.println(formattedStr);  // Gửi dữ liệu qua cổng UART
      //Serial.print("hanh trinh: ");
      //Serial.print("hanh trinh: ");
      delay(1000);  //Chờ 1 giây trước khi gửi lại
      digitalWrite(dir, LOW);
      buoc = buoc + 3600;
      reset=0;
      for (int i=0; i< buoc && LU ==0; i++)
      {
          tocdo = 150;
          run();
          if(!buttonPressed&& reset == 0)
          {
            float(resetpoint) =2 + hanhtrinh- (2*float(i))/7200;
            //sprintf(formattedStr, "%.3f", hanhtrinh);
            Serial.println(resetpoint);  // Gửi dữ liệu qua cổng UART
            reset=1;
          }
      }
      dem=0;
      buoc=0;

      //Serial.println("Button was pressed.");
      buttonPressed = false; // Đặt lại biến cờ
    }
    else if(!buttonPressed && LU==0 && LD==0 && dstart==1)// sua lai = 1 khi truyen uart
      {
        digitalWrite(dir, HIGH);
        dem++;
        tocdo = 500;
        run();
        buoc = dem; 
    }
    
    // nút stop
    if(dung == 1 && d==0 && u==0 && start==0)
    {
      nutstop();
    }
    // nút vật lí up
    if(u==1 && LU==0 && d==0 && start==0 && dung==0){
      nutup();
    }
    //nút vật lí down
    if(d==1 && LD==0 && u==0 && start==0 && dung==0){
      nutdown();
    }
    ck--;
  }
  
}
