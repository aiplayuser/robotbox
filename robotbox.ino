#include <ESP8266WiFi.h> //建立网络服务
#include <ArduinoOTA.h> //无线下载
#include <WiFiManager.h> //自动配网 
#include <ESP8266WebServer.h> //建立网页服务
#include <ESP8266LLMNR.h>//dns服务
#include <WiFiClient.h>
#include <FS.h>
#include <Servo.h>
#include <EEPROM.h>
#include <Ticker.h>
WiFiManager espwifi;  ESP8266WebServer espweb(80);Ticker tick0,tick1;
File spfile; FSInfo fsinfo; Servo q,w,e,r,t,y,u,i; byte ms=44;int a0v;
String sn[8]{"-q","-w","-e","-r","-t","-y","-u","-i"};int j,k,m,n,b,v;
String head="<!DOCTYPE html><html><head><meta charset=UTF-8><style>a{TEXT-DECORATION:none}</style><meta name=viewport content=\"width=device-width,initial-scale=1.5\"></head><body><p>"; 
String body="<p><h2>重新连接espwifi</h2><p></body></html>";

void setup(){Serial.begin(9600);
  q.attach(D1,545,2399);w.attach(D2,545,2399);e.attach(D3,545,2399);r.attach(D4,545,2399);
  t.attach(D5,545,2399);y.attach(D6,545,2399);u.attach(D7,545,2399);i.attach(D8,545,2399);
  SPIFFS.begin();SPIFFS.info(fsinfo);EEPROM.begin(4096);
  pinMode(D0, OUTPUT);d01();
  
  if(analogRead(A0)>99){WiFi.softAP("esp11492104","");
     Serial.println("ap模式--esp11492104");Serial.println(WiFi.softAPIP());}
  else{espwifi.autoConnect("esp11492104");Serial.print("sta模式--");
     Serial.println(WiFi.SSID());Serial.println(WiFi.localIP());}    

  espweb.begin();ArduinoOTA.begin();LLMNR.begin("esp11492104");
  espweb.on("/upload",HTTP_POST,respondOK,upload);//文件上传
  espweb.onNotFound(handleall);//处理所有请求
  espweb.on("/",handleRoot);//请求首页
  espweb.on("/clrwifi",clrwifi);//重新配置网络
  espweb.on("/formatf",formatf);//删除所有文件
  espweb.on("/readA0", handleA0);//读取A0
  espweb.on("/setpwm",setpwm);//舵机控制
  espweb.on("/saveee",saveee);//保存舵机状态
  espweb.on("/cmdsave",cmdsave);//执行舵机命令
  espweb.on("/respwm", respwm);//舵机复位
  espweb.on("/delf",delf);//删除单个文件
  espweb.on("/fuwei",fuwei);//重新启动
  }
void setpwm(){
  String pwmq = espweb.arg("1");if(pwmq!=0){q.write(pwmq.toInt());}
  String pwmw = espweb.arg("2");if(pwmw!=0){w.write(pwmw.toInt());}
  String pwme = espweb.arg("3");if(pwme!=0){e.write(pwme.toInt());}
  String pwmr = espweb.arg("4");if(pwmr!=0){r.write(pwmr.toInt());}
  String pwmt = espweb.arg("5");if(pwmt!=0){t.write(pwmt.toInt());}
  String pwmy = espweb.arg("6");if(pwmy!=0){y.write(pwmy.toInt());}
  String pwmu = espweb.arg("7");if(pwmu!=0){u.write(pwmu.toInt());}
  String pwmi = espweb.arg("8");if(pwmi!=0){i.write(pwmi.toInt());}
  String pwmd = espweb.arg("9");if(pwmd!=0){ms=pwmd.toInt();}
  }
void saveee(){String pwmvol=espweb.arg("pwmvol");String html;
  byte pwms[8]{q.read(),w.read(),e.read(),r.read(),t.read(),y.read(),u.read(),i.read()};
  if(pwmvol=="0"){for(j=0;j<8;j++){EEPROM.write(j+100,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="1"){for(j=0;j<8;j++){EEPROM.write(j+110,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="2"){for(j=0;j<8;j++){EEPROM.write(j+120,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="3"){for(j=0;j<8;j++){EEPROM.write(j+130,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="4"){for(j=0;j<8;j++){EEPROM.write(j+140,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="5"){for(j=0;j<8;j++){EEPROM.write(j+150,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="6"){for(j=0;j<8;j++){EEPROM.write(j+160,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="7"){for(j=0;j<8;j++){EEPROM.write(j+170,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="8"){for(j=0;j<8;j++){EEPROM.write(j+180,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="9"){for(j=0;j<8;j++){EEPROM.write(j+190,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="10"){for(j=0;j<8;j++){EEPROM.write(j+200,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="11"){for(j=0;j<8;j++){EEPROM.write(j+210,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="12"){for(j=0;j<8;j++){EEPROM.write(j+220,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="13"){for(j=0;j<8;j++){EEPROM.write(j+230,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="14"){for(j=0;j<8;j++){EEPROM.write(j+240,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="15"){for(j=0;j<8;j++){EEPROM.write(j+250,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="16"){for(j=0;j<8;j++){EEPROM.write(j+260,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="17"){for(j=0;j<8;j++){EEPROM.write(j+270,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="18"){for(j=0;j<8;j++){EEPROM.write(j+280,pwms[j]);EEPROM.commit();delay(50);}}
  if(pwmvol=="19"){for(j=0;j<8;j++){EEPROM.write(j+290,pwms[j]);EEPROM.commit();delay(50);}}
  k=100;m=0;for(b=0;b<=190;b+=10){html+=m++;
              for(j=0;j<8;j++){html+=sn[j];html+=EEPROM.read(j+k+b);delay(10);}
              html+="<br>";}espweb.send(200, "text/html", html);
              }
void cmdsave(){String from=espweb.arg("from");v=from.toInt();
               String to=espweb.arg("to");b=to.toInt();
               String cishu=espweb.arg("cishu");n=cishu.toInt();
  for(m=0;m<n;m++){j=100+v*10;for(k=0;k<=b-v;k++){
  q.write(EEPROM.read(j));delay(10);j++;w.write(EEPROM.read(j));delay(10);j++;
  e.write(EEPROM.read(j));delay(10);j++;r.write(EEPROM.read(j));delay(10);j++;
  t.write(EEPROM.read(j));delay(10);j++;y.write(EEPROM.read(j));delay(10);j++;
  u.write(EEPROM.read(j));delay(10);j++;i.write(EEPROM.read(j));delay(10);j++;
  delay(ms);j++;delay(ms);j++;delay(ms);delay(ms);delay(ms);delay(ms);delay(ms);}}
              }
void respwm(){ String qval=espweb.arg(sn[0]);q.write(qval.toInt());
               String wval=espweb.arg(sn[1]);w.write(wval.toInt());
               String eval=espweb.arg(sn[2]);e.write(eval.toInt());
               String rval=espweb.arg(sn[3]);r.write(rval.toInt());
               String tval=espweb.arg(sn[4]);t.write(tval.toInt());
               String yval=espweb.arg(sn[5]);y.write(yval.toInt());
               String uval=espweb.arg(sn[6]);u.write(uval.toInt());
               String ival=espweb.arg(sn[7]);i.write(ival.toInt());}
void delf(){SPIFFS.remove(espweb.arg("delf"));delay(500);
            espweb.sendHeader("Location","/");espweb.send(303);} 
void fuwei(){espweb.send(200,"text/html",head+body); delay(500); ESP.reset();}
void handleA0(){espweb.send(200,"text/plain",String(analogRead(A0)));}
void formatf(){SPIFFS.format();espweb.sendHeader("Location","/");espweb.send(303);}
void clrwifi(){espweb.send(200,"text/html",head+body); delay(500);
               espwifi.resetSettings();  ESP.reset();}
void loop(){espweb.handleClient();delay(99);ArduinoOTA.handle();delay(99);}
void d01(){digitalWrite(D0,1);tick0.once_ms(ms,d00);}
void d00(){digitalWrite(D0,0);a0v=analogRead(A0);tick1.once_ms(1024-a0v,d01);}

void handleRoot(){espweb.send(200, "text/html", sendHTML());}
String sendHTML(){ String html; html+=head;
  html+="esp8266ID："; html+=ESP.getChipId(); html+="<p>";
  html+="esp8266SSID："; html+=WiFi.SSID(); html+="<p>";
  html+="esp8266IP："; html+=WiFi.localIP().toString(); html+="<p>";
  
  html+="<form action=/upload name=form1 method=POST enctype=multipart/form-data>";
  html+="<input type=file name=data onchange=document.form1.submit()></form><p>" ;
  
  FSInfo fsinfo; SPIFFS.info(fsinfo); 
    html+="闪存信息(共2M字节)<p>可用空间--"; html+=fsinfo.totalBytes; html+="<p>";
    html+="已用空间--"; html+=fsinfo.usedBytes; html+="<p><h4>文件列表：</h4>";
  Dir dir=SPIFFS.openDir("/");while(dir.next()){html+="<a href=";
    html+=dir.fileName(); html+=">"; html+=dir.fileName();
    html+="</a>"; html+="--"; html+=dir.fileSize(); html+="<p>";
    html+="<form action=/delf method=post><input name=delf value=";
    html+=dir.fileName();html+="><input type=submit value=删除></form><p>";}

  html+="<form action=/fuwei method=POST>";
  html+="<input type=submit value=重新启动></form><p>";
  
  html+="<form action=/setwifi method=post>";
  html+="<input name=setpass size=12><input type=submit value=重设wifi密码></form><p>";
  
  html+="<form action=/formatf method=POST>";
  html+="<input type=submit value=删除所有文件></form><p>";

  html+="<form action=/clrwifi method=POST>";
  html+="<input type=submit value=重新配置网络></form><p>";

  html+="<h4>EEPROM信息</h4>";
  for(int i=0;i<512;i++){char di[33]; int eep=EEPROM.read(i); 
        sprintf(di, "--%.4d--%.2x", i,eep);
        html+=di; html+="<br>";}

  html+="</body></html>"; return html;
}
void respondOK(){espweb.send(200);}
void upload(){HTTPUpload& upload = espweb.upload();
  if(upload.status == UPLOAD_FILE_START){                     // 如果上传状态为UPLOAD_FILE_START
  String filename = upload.filename;                        // 建立字符串变量用于存放上传文件名
  if(!filename.startsWith("/")) filename = "/" + filename;  // 为上传文件名前加上"/"
  spfile = SPIFFS.open(filename, "w"); }           // 在SPIFFS中建立文件用于写入用户上传的文件数据
  else if(upload.status == UPLOAD_FILE_WRITE){          // 如果上传状态为UPLOAD_FILE_WRITE      
  spfile.write(upload.buf, upload.currentSize);} // 向SPIFFS文件写入浏览器发来的文件数据
  else if(upload.status == UPLOAD_FILE_END){spfile.close();
  espweb.sendHeader("Location","/");espweb.send(303);}// 发送相应代码303（重定向到新页面）                               
}
void handleall(){String webAddress = espweb.uri();
                 bool fileReadOK = handleFileRead(webAddress);}
bool handleFileRead(String path) {if (path.endsWith("/")) {path = "/index.html";} 
   String contentType = getContentType(path);
   if (SPIFFS.exists(path)){File file = SPIFFS.open(path,"r");
   espweb.streamFile(file, contentType);file.close();return true;}return false;  
}
String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
