/*
接线说明:该程序应该烧录进合宙的板子中去

程序说明:该板子是客户端,通过向服务器发送请求并接受信息得到服务器中的按键状态来改变自身的LED的亮灭
        – 定时向服务器发送请求，从而获取服务器开发板上按钮状态
        – 将服务器端发来的按钮引脚电平状态解析为数字量，并且用该数字量控制板上LED点亮和熄灭

注意事项:注意你的HTTP请求的书写格式,该程序曾经少打了一个空格导致服务器无法运行,客户端的串口输出正常,额,好难找到的错误

函数示例:无

作者:灵首

时间:2023_3_26

*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>

WiFiMulti wifi_multi;  //建立WiFiMulti 的对象,对象名称是 wifi_multi

#define LED_A 10
#define LED_B 11

static int ledState;


//定义的函数
void wifi_multi_con(void);
void wifi_multi_init(void);
void wifiClientRequest(const char* host,const int httpPort);



/*
# brief 连接WiFi的函数
# param 无
# retval 无
*/
void wifi_multi_con(void){
  int i=0;
  while(wifi_multi.run() != WL_CONNECTED){
    delay(1000);
    i++;
    Serial.print(i);
  }

}


/*
# brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
# param 无
# retval  无
*/
void wifi_multi_init(void){
  wifi_multi.addAP("LINGSOU123","12345678");
  wifi_multi.addAP("LINGSOU12","12345678");
  wifi_multi.addAP("LINGSOU1","12345678");
  wifi_multi.addAP("LINGSOU234","12345678");   //通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}


/*
# brief 通过WiFiClient库向指定网址建立连接并发出信息
# param  const char* host:需要建立连接的网站的网址
# param   const int httpPort:对应的端口号
# retval  无,但是会通过串口打印一些内容
*/
void wifiClientRequest(const char* host,const int httpPort){
  WiFiClient client;


  //格式很重要 String("GET ") 这个中有一个空格,应该是不能省的,省略会导致HTTP请求发送不出去,很关键的
  String httpRequest =  String("GET /update") + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";



  //输出连接的网址
  Serial.print("connecting to :");
  Serial.print(host);
  Serial.print("\n");


  //连接网络服务器
  if(client.connect(host,httpPort)){
    //成功后输出success
    Serial.print("success\n");

    //向服务器发送HTTP请求
    client.print(httpRequest);    

    //串口输出HTTP请求信息
    Serial.print("sending request:");   
    Serial.print(httpRequest);
    Serial.print("\n");
  }
  else{
    Serial.print("connect failed!!!\n");
  }


  //检查是否连接成功并在成功连接后进行相关操作
  while(client.connected() || client.available()){
    //这是将从服务器端接收到的信息中的关于LED亮灭的字符串信息找出来,并转化为数字(int)类型
    if (client.find("buttonState: ")){
      ledState = client.parseInt();
      Serial.print("ledState: " ); 
      Serial.println(ledState); 
    }
    else{
      Serial.print("failed!!!");
    }
  }
  client.stop();    //断开与服务器的连接

  //根据上面找到的信息控制客户端的LED灯的亮灭
  if (ledState == 0){
    digitalWrite(LED_A,1);
    digitalWrite(LED_B,1);
  }
  else{
    digitalWrite(LED_A,0);
    digitalWrite(LED_B,0);
  }

    
}

void setup() {
  //连接串口
  Serial.begin(9600);
  Serial.print("serial is OK\n");

  //设置按键引脚,这是输出模式
  pinMode(LED_A,OUTPUT);
  pinMode(LED_B,OUTPUT);
  digitalWrite(LED_A,1);
  digitalWrite(LED_B,0);

  //wifi 连接设置
  wifi_multi_init();
  wifi_multi_con();
  Serial.print("wifi connected!!!\n");

  //输出连接信息(连接的WIFI名称及开发板的IP地址)
  Serial.print("\nconnect wifi:");
  Serial.print(WiFi.SSID());
  Serial.print("\n");
  Serial.print("\nIP address:");
  Serial.print(WiFi.localIP());
  Serial.print("\n");

}

void loop() {
  //同指定网址建立连接,同时进行相关的操作
  wifiClientRequest("192.168.0.123",80);
  delay(1000);
}