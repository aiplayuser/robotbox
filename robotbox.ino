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
WiFiManager espwifi; ESP8266WebServer espweb(80); DNSServer dnsServer; Servo q,w,e,r,t,y,u,i; Ticker ticker; File file; 
int ms=100,ds=30; bool ting=0,ziyou=0;

void setup(){q.attach(D1);w.attach(D2);e.attach(D5);r.attach(D6); t.attach(D7);y.attach(D8);u.attach(D9);i.attach(D10);
  q.write(90);w.write(90);e.write(90);r.write(90);t.write(90);y.write(90);u.write(90);i.write(90);
  pinMode(D0, INPUT); pinMode(D3, INPUT_PULLUP); pinMode(D4, OUTPUT); digitalWrite(D4,0); randomSeed(analogRead(A0));
  
  SPIFFS.begin();String ssid = "ESP"+String(ESP.getChipId(),HEX);
  if(SPIFFS.exists("/pass.json")){DynamicJsonDocument doc(50); file = SPIFFS.open("/pass.json", "r");deserializeJson(doc, file); 
            String pass=doc["pass"].as<String>(); file.close();WiFi.softAP(ssid,pass);} else{espwifi.autoConnect(ssid.c_str());}
  
  espweb.begin(); ArduinoOTA.begin(); LLMNR.begin(ssid.c_str()); dnsServer.start(53,"*",IPAddress(192,168,4,1)); ticker.attach(3, chongzhi);
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
void setpwm(){String pwm = espweb.argName(0); int pwmv=espweb.arg(pwm).toInt(); switch (pwm[0]) { //舵机控制
    case '1': q.write(pwmv); break; case '2': w.write(pwmv); break; case '3': e.write(pwmv); break; case '4': r.write(pwmv); break;
    case '5': t.write(pwmv); break; case '6': y.write(pwmv); break; case '7': u.write(pwmv); break; case '8': i.write(pwmv); break;
    case '9': ms=pwmv; break; case '0': ds=pwmv; break;
      }}
void cmdsave(){ //修改json命令
    String mingzi = espweb.arg("mingzi"); int dong = espweb.arg("dongzuo").toInt(); int buint = espweb.arg("bushu").toInt();
    DynamicJsonDocument doc(8000); file = SPIFFS.open("/config.json", "r"); deserializeJson(doc, file);
    if(mingzi=="删除"){buint==0 ? doc.remove(dong) : doc[dong].remove(buint);}
    else if(mingzi=="自由活动"){delay(999); ziyou=!ziyou;cmddanbu();}
    else if(mingzi=="随机训练"){int ran[9]={0,30,45,60,90,120,135,150,180}; doc[dong][0]["name"]=mingzi;
       for(int i=1;i<=buint;i++){int qv,wv,ev,rv,tv,yv,uv,iv;
          wv=random(181);ev=random(181);yv=random(181);uv=random(181);
          wv>90?qv=random(0,91):qv=random(90,181); ev>90?rv=random(0,91):rv=random(90,181);
          yv>90?tv=random(0,91):tv=random(90,181); uv>90?iv=random(0,91):iv=random(90,181);
          doc[dong][i]["q"]=qv; doc[dong][i]["w"]=wv; doc[dong][i]["e"]=ev; doc[dong][i]["r"]=rv;
          doc[dong][i]["t"]=tv; doc[dong][i]["y"]=yv; doc[dong][i]["u"]=uv; doc[dong][i]["i"]=iv;}}
    else{ doc[dong][0]["name"]=mingzi;
          doc[dong][buint]["q"]=q.read(); doc[dong][buint]["w"]=w.read(); doc[dong][buint]["e"]=e.read(); doc[dong][buint]["r"]=r.read();
          doc[dong][buint]["t"]=t.read(); doc[dong][buint]["y"]=y.read(); doc[dong][buint]["u"]=u.read(); doc[dong][buint]["i"]=i.read();}
    file = SPIFFS.open("/config.json", "w"); serializeJson(doc, file); file.close();}
void handleA0(){ //读取json文件
    String se[8]{"q","w","e","r","t","y","u","i"},html; DynamicJsonDocument doc(8000); file = SPIFFS.open("/config.json", "r"); 
    deserializeJson(doc, file); for(int j=doc.size()-1;j>=0;j--){ 
      html+="<form>间隔<input type=number style=width:45px name=d oninput=dd.value=this.value value=100 onchange=setpwm(";
      html+=j+10; html+=")><input  type=range name=dd id="; html+=j+10; html+=" max=200 oninput=d.value=this.value onchange=setpwm(";
      html+=j+10; html+=")><br>速度<input type=number style=width:45px name=s oninput=ss.value=this.value value=30 onchange=setpds(";
      html+=j+100; html+=")><input type=range name=ss id="; html+=j+100; html+=" max=200 oninput=s.value=this.value onchange=setpds(";
      html+=j+100; html+=")><br></form>"; html+="<button style=background:#84C1FF;font-weight:bold onclick=sendbufa("; html+=j; html+=",0)>";
      html+=j; html+="："; html+=doc[j][0]["name"].as<String>();  html+="</button>速度控制<br>";
      for(int k=1;k<doc[j].size();k++){html+=k; html+="<button onclick=sendbufa("; html+=j; html+=","; html+=k; html+=")>";
        for(int i=0;i<8;i++){html+=se[i]+doc[j][k][se[i]].as<int>();}  html+="</button><br>";}}
    file.close(); espweb.send(200, "text/html", html);}
void cmddanbu(){ting=!ting; if(ting==0)ziyou=0;//执行json命令
    int sv[8]; String sn[8]{"q","w","e","r","t","y","u","i"};
    int dong = espweb.arg("dong").toInt(); int bufa = espweb.arg("bufa").toInt();
    DynamicJsonDocument doc(8000); file = SPIFFS.open("/config.json", "r"); deserializeJson(doc, file);
    while(ting){int doint=dong; if(ziyou)doint=random(2,doc.size()); 
      if(ceju()<20){jiange(5000);doint=4;}
      for(int m=0;m<2;m++){
         for(int n=1; n<doc[doint].size(); n++){if(ting==0)break;
            //if(doc[doint][0]["name"]=="随机训练"){ms=random(100);ds=random(100);}
            //if(ziyou){ms=random(100);ds=random(100);} 
            if(ceju()<20&&doint!=4)break; 
            int buint=n; if(bufa!=0)buint=bufa;
            for(int i=0;i<8;i++)sv[i]=doc[doint][buint][sn[i]]; 
            danbu(sv[0],sv[1],sv[2],sv[3],sv[4],sv[5],sv[6],sv[7]); 
            if(bufa!=0)ting=0; jiange(ms*50); }}}
    file.close();}
float ceju(){digitalWrite(D4,0); delayMicroseconds(5);digitalWrite(D4,1);delayMicroseconds(10); 
             digitalWrite(D4,0); return pulseIn(D0,1)/58;}
void jiange(int j){ for(int k=0;k<j*10;k++){espweb.handleClient();ArduinoOTA.handle();
                                         dnsServer.processNextRequest();delayMicroseconds(10);} }
void danbu(int q2,int w2,int e2,int r2,int t2,int y2,int u2,int i2){
   int q1=q.read(),w1=w.read(),e1=e.read(),r1=r.read(),t1=t.read(),y1=y.read(),u1=u.read(),i1=i.read();
   while(q1!=q2 || w1!=w2 || e1!=e2 || r1!=r2 || t1!=t2 || y1!=y2 || u1!=u2 || i1!=i2){if(ting==0)break;
        if(q1!=q2){q2>q1 ? q1++:q1--;} q.write(q1); if(w1!=w2){w2>w1 ? w1++:w1--;} w.write(w1); 
        if(e1!=e2){e2>e1 ? e1++:e1--;} e.write(e1); if(r1!=r2){r2>r1 ? r1++:r1--;} r.write(r1); 
        if(t1!=t2){t2>t1 ? t1++:t1--;} t.write(t1); if(y1!=y2){y2>y1 ? y1++:y1--;} y.write(y1); 
        if(u1!=u2){u2>u1 ? u1++:u1--;} u.write(u1); if(i1!=i2){i2>i1 ? i1++:i1--;} i.write(i1);
        jiange(ds); } }  
void loop(){espweb.handleClient();delay(10);ArduinoOTA.handle(); delay(10);  dnsServer.processNextRequest();delay(10);}
void chongzhi(){if(digitalRead(D3)==0){setwifi(); delay(999); ESP.reset();}}
void setwifi(){espwifi.resetSettings(); DynamicJsonDocument doc(50);file=SPIFFS.open("/pass.json","w");doc["pass"]=espweb.arg("setpass");
     serializeJson(doc,file);file.close();espweb.sendHeader("Location","/");espweb.send(303);}
void formatf(){SPIFFS.format();espweb.sendHeader("Location","/");espweb.send(303);}
void fuwei(){ESP.reset();}
void delf(){SPIFFS.remove(espweb.arg("delf"));delay(999); espweb.sendHeader("Location","/");espweb.send(303);} 
void handleRoot(){espweb.send(200, "text/html", sendHTML());}
String sendHTML(){ String html, ssid = "ESP"+String(ESP.getChipId(),HEX); 
  html+="<!DOCTYPE html><html><head><meta charset=UTF-8><style>a{TEXT-DECORATION:none}</style>";
  html+="<meta name=viewport content=\"width=device-width,initial-scale=1.5\"></head><body><p>"; 
  html+="esp8266ID："+ssid+"<p>"; html+="路由器SSID："; html+=WiFi.SSID(); html+="<p>";
  html+="esp8266IP："; html+=WiFi.localIP().toString(); html+="<p>";
  html+="<form action=/upload name=form1 method=POST enctype=multipart/form-data>";
  html+="<input type=file name=data onchange=document.form1.submit()></form><p>" ;
  FSInfo fsinfo; SPIFFS.info(fsinfo); 
  html+="可用空间--"; html+=fsinfo.totalBytes; html+="<p>";
  html+="已用空间--"; html+=fsinfo.usedBytes; html+="<p>";
  Dir dir=SPIFFS.openDir("/");while(dir.next()){
  html+="<form action=/delf method=post><a href=";
  html+=dir.fileName(); html+=">"; html+=dir.fileName();
  html+="</a>--"; html+=dir.fileSize(); html+="<input name=delf style=width:0px value=";
  html+=dir.fileName();html+="><input type=submit value=删除></form><p>";}
  html+="<form action=/fuwei method=POST target=nm_iframe>";
  html+="<input type=submit value=重新启动></form><p>";
  html+="<form action=/setwifi method=post>";
  html+="<input name=setpass size=12><input type=submit value=重设wifi密码></form><p>";
  html+="<form action=/formatf method=POST>";
  html+="<input type=submit value=删除所有文件></form><p>";
  html+="<iframe name=nm_iframe style=display:none></iframe></body></html>"; return html;}
void respondOK(){espweb.send(200);}
void upload(){HTTPUpload& upload = espweb.upload();
  if(upload.status==UPLOAD_FILE_START){file = SPIFFS.open("/"+upload.filename,"w");}           
  else if(upload.status==UPLOAD_FILE_WRITE){file.write(upload.buf,upload.currentSize);} 
  else if(upload.status==UPLOAD_FILE_END){file.close();espweb.sendHeader("Location","/");
                                          espweb.send(303);}}                               
void handleall(){String filename=espweb.uri();
  if(SPIFFS.exists(filename)){file = SPIFFS.open(filename,"r");
  espweb.streamFile(file, filetype(filename));file.close();}
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
