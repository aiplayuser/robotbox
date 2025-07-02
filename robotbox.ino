#include <ESP8266WiFi.h> //wifi服务
#include <ArduinoOTA.h> //无线透传
#include <WiFiManager.h> //自动配网 
#include <ESP8266WebServer.h> //web服务
#include <ESP8266LLMNR.h>//dns服务
#include <DNSServer.h>//自动弹窗
#include <WiFiClient.h>//物联网
#include <FS.h>//文件系统
#include <Servo.h>//舵机库
#include <ArduinoJson.h>//json数据库
#include <Ticker.h>//多任务定时器
WiFiManager espwifi; ESP8266WebServer espweb(80); DNSServer dnsServer; 
Servo q,w,e,r,t,y,u,i; int ms=500; Ticker ticker;File file;
String head="<!DOCTYPE html><html><head><meta charset=UTF-8><style>a{TEXT-DECORATION:none}</style><meta name=viewport content=\"width=device-width,initial-scale=1.5\"></head><body><p>"; 
String body="<p><h2>重新连接espwifi</h2><p></body></html>",ssid,pass; bool ting=0;

void setup(){Serial.begin(9600);SPIFFS.begin();ssid = "ESP"+String(ESP.getChipId(),HEX);
  if(SPIFFS.exists("/pass.json")){DynamicJsonDocument doc(50);
     File file = SPIFFS.open("/pass.json", "r");deserializeJson(doc, file); 
     pass=doc["pass"].as<String>(); file.close();WiFi.softAP(ssid,pass);}
  else{espwifi.autoConnect(ssid.c_str()); }
  q.attach(D1,545,2399);w.attach(D2,545,2399);e.attach(D3,545,2399);r.attach(D4,545,2399);
  t.attach(D5,545,2399);y.attach(D6,545,2399);u.attach(D7,545,2399);i.attach(D8,545,2399);
  espweb.begin(); ArduinoOTA.begin(); LLMNR.begin(ssid.c_str()); 
  dnsServer.start(53,"*",IPAddress(192,168,4,1)); ticker.attach(0.3, chongzhi);
  espweb.on("/upload",HTTP_POST,respondOK,upload);//文件上传
  espweb.onNotFound(handleall);//处理所有请求
  espweb.on("/",HTTP_GET,handleRoot);//请求首页
  espweb.on("/formatf",formatf);//删除所有文件
  espweb.on("/delf",delf);//删除单个文件
  espweb.on("/setwifi",setwifi);//重设wifi密码
  espweb.on("/fuwei",fuwei);//重新启动
  espweb.on("/readA0",handleA0);//读取json文件
  espweb.on("/setpwm",setpwm);//舵机控制
  espweb.on("/cmdsave",cmdsave);//修改json命令
  espweb.on("/cmddanbu", cmddanbu);//执行json命令
  }
void setpwm(){ //舵机控制
    String pwmq = espweb.arg("1");if(pwmq!=0){q.write(pwmq.toInt());}
    String pwmw = espweb.arg("2");if(pwmw!=0){w.write(pwmw.toInt());}
    String pwme = espweb.arg("3");if(pwme!=0){e.write(pwme.toInt());}
    String pwmr = espweb.arg("4");if(pwmr!=0){r.write(pwmr.toInt());}
    String pwmt = espweb.arg("5");if(pwmt!=0){t.write(pwmt.toInt());}
    String pwmy = espweb.arg("6");if(pwmy!=0){y.write(pwmy.toInt());}
    String pwmu = espweb.arg("7");if(pwmu!=0){u.write(pwmu.toInt());}
    String pwmi = espweb.arg("8");if(pwmi!=0){i.write(pwmi.toInt());}
    String pwmd = espweb.arg("9");if(pwmd!=0){ms=pwmd.toInt();} }
void cmdsave(){ //修改json命令
    String mingzi = espweb.arg("mingzi");
    String dongzuo = espweb.arg("dongzuo"); int dongint = dongzuo.toInt();
    String bushu = espweb.arg("bushu"); int buint = bushu.toInt();
    DynamicJsonDocument doc(8000);//准备建立json对象
    File file = SPIFFS.open("/config.json", "r");//准备读取json文件
    deserializeJson(doc, file);//读取解析json序列
    if(mingzi=="删除"){buint==0 ? doc.remove(dongint) : doc[dongint].remove(buint);}
    else if(mingzi=="随机训练"){randomSeed(analogRead(A0));
          int ran[9]={0,30,45,60,90,120,135,150,180}; doc[dongint][0]["name"]=mingzi;
       for(int i=1;i<=buint;i++){int qv,wv,ev,rv,tv,yv,uv,iv;
          wv=random(181);ev=random(181);yv=random(181);uv=random(181);
          wv>90?qv=random(0,91):qv=random(90,181);
          ev>90?rv=random(0,91):rv=random(90,181);
          yv>90?tv=random(0,91):tv=random(90,181);
          uv>90?iv=random(0,91):iv=random(90,181);
          doc[dongint][i]["q"]=qv; doc[dongint][i]["w"]=wv;
          doc[dongint][i]["e"]=ev; doc[dongint][i]["r"]=rv;
          doc[dongint][i]["t"]=tv; doc[dongint][i]["y"]=yv;
          doc[dongint][i]["u"]=uv; doc[dongint][i]["i"]=iv;}}
    else{ doc[dongint][0]["name"]=mingzi;
          doc[dongint][buint]["q"]=q.read();
          doc[dongint][buint]["w"]=w.read();
          doc[dongint][buint]["e"]=e.read();
          doc[dongint][buint]["r"]=r.read();
          doc[dongint][buint]["t"]=t.read();
          doc[dongint][buint]["y"]=y.read();
          doc[dongint][buint]["u"]=u.read();
          doc[dongint][buint]["i"]=i.read();}
          file = SPIFFS.open("/config.json", "w");//准备写入json文件
          serializeJson(doc, file); file.close();}//写入json序列
void handleA0(){ //读取json文件
    String se[8]{"q","w","e","r","t","y","u","i"},html; 
    DynamicJsonDocument doc(8000);//建立json对象
    File file = SPIFFS.open("/config.json", "r");//读取json文件 
    deserializeJson(doc, file);//解析json文件
    for(int j=doc.size()-1;j>=0;j--){ 
      html+="<form>d<input type=number style=width:40px size=1 name=d oninput=dd.value=this.value value=500 onchange=setpwm(";
      html+=j+10; html+=")><input  type=range name=dd id=";
      html+=j+10; html+=" min=1 max=999 oninput=d.value=this.value onchange=setpwm(";
      html+=j+10; html+=")></form>";
      html+="<button onclick=sendbufa("; html+=j; html+=",0)>";
      html+=j; html+="："; html+=doc[j][0]["name"].as<String>(); 
      html+="</button>速度控制<br>";
      for(int k=1;k<doc[j].size();k++){html+=k;
        html+="<button onclick=sendbufa("; 
        html+=j; html+=","; html+=k; html+=")>";
        for(int i=0;i<8;i++){html+=se[i]+doc[j][k][se[i]].as<int>();} 
        html+="</button><br>";}}
    file.close(); espweb.send(200, "text/html", html);}
void cmddanbu(){ting=!ting; //执行json命令
        String dong = espweb.arg("dong"); int dongint = dong.toInt();
        String bufa = espweb.arg("bufa"); int bufaint = bufa.toInt();
        DynamicJsonDocument doc(8000);//建立json对象
        File file = SPIFFS.open("/config.json", "r");//读取json文件 
        deserializeJson(doc, file);//解析json文件
    if(bufaint==0){while(ting){if(!ting){break;}
      for(int n=1; n<doc[dongint].size(); n++){if(!ting){break;}
        for(int k=0;k<ms*2;k++){espweb.handleClient();delay(1);}
        q.write(doc[dongint][n]["q"].as<int>()); 
        w.write(doc[dongint][n]["w"].as<int>());
        e.write(doc[dongint][n]["e"].as<int>()); 
        r.write(doc[dongint][n]["r"].as<int>());
        t.write(doc[dongint][n]["t"].as<int>()); 
        y.write(doc[dongint][n]["y"].as<int>());
        u.write(doc[dongint][n]["u"].as<int>()); 
        i.write(doc[dongint][n]["i"].as<int>());}}}
   else{q.write(doc[dongint][bufaint]["q"].as<int>()); 
        w.write(doc[dongint][bufaint]["w"].as<int>());
        e.write(doc[dongint][bufaint]["e"].as<int>()); 
        r.write(doc[dongint][bufaint]["r"].as<int>());
        t.write(doc[dongint][bufaint]["t"].as<int>()); 
        y.write(doc[dongint][bufaint]["y"].as<int>());
        u.write(doc[dongint][bufaint]["u"].as<int>()); 
        i.write(doc[dongint][bufaint]["i"].as<int>());}
        file.close();}
void loop(){espweb.handleClient();ArduinoOTA.handle();
    dnsServer.processNextRequest();}
void chongzhi(){if(analogRead(A0)>11){setwifi();
     espwifi.resetSettings(); ESP.reset();}}
void setwifi(){String setpass = espweb.arg("setpass"); 
  DynamicJsonDocument doc(50);File file=SPIFFS.open("/pass.json", "w");
  doc["pass"]=setpass;  serializeJson(doc,file);file.close();
  espweb.sendHeader("Location","/"); espweb.send(303);}
void formatf(){SPIFFS.format(); espweb.sendHeader("Location","/");espweb.send(303);}
void fuwei(){espweb.send(200,"text/html",head+body); delay(1000); ESP.reset();}
void delf(){SPIFFS.remove(espweb.arg("delf"));delay(1000);
            espweb.sendHeader("Location","/");espweb.send(303);} 
void handleRoot(){espweb.send(200, "text/html", sendHTML());}
String sendHTML(){ String html; html+=head;
  html+="esp8266ID："+ssid+"<p>";
  html+="路由器SSID："; html+=WiFi.SSID(); html+="<p>";
  html+="esp8266IP："; html+=WiFi.localIP().toString(); html+="<p>";
  html+="<form action=/upload name=form1 method=POST enctype=multipart/form-data>";
  html+="<input type=file name=data onchange=document.form1.submit()></form><p>" ;
  FSInfo fsinfo; SPIFFS.info(fsinfo); 
  html+="可用空间--"; html+=fsinfo.totalBytes; html+="<p>";
  html+="已用空间--"; html+=fsinfo.usedBytes; html+="<p>";
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
  html+="</body></html>"; return html;}
void respondOK(){espweb.send(200);}
void upload(){HTTPUpload& upload = espweb.upload();
  if(upload.status == UPLOAD_FILE_START){                     // 如果上传状态为UPLOAD_FILE_START
  String filename = upload.filename;                        // 建立字符串变量用于存放上传文件名
  if(!filename.startsWith("/")) filename = "/" + filename;  // 为上传文件名前加上"/"
  file = SPIFFS.open(filename, "w"); }           // 在SPIFFS中建立文件用于写入用户上传的文件数据
  else if(upload.status == UPLOAD_FILE_WRITE){          // 如果上传状态为UPLOAD_FILE_WRITE      
  file.write(upload.buf, upload.currentSize);} // 向SPIFFS文件写入浏览器发来的文件数据
  else if(upload.status == UPLOAD_FILE_END){file.close();
  espweb.sendHeader("Location","/");espweb.send(303);}}// 发送相应代码303（重定向到新页面）                               
void handleall(){String path=espweb.uri();
  if(path.endsWith("/")){path="/index.html";} 
  if(SPIFFS.exists(path)){File file = SPIFFS.open(path,"r");
  espweb.streamFile(file, filetype(path));file.close();}
  else{handleRoot();}}
String filetype(String filename){
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
