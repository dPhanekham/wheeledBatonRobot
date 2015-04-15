#include <WiFly.h>
#include <SPI.h>

#define ledPin 3 //led pin

String httpRequest;        //holds data parsed from the web browser
boolean msgAvailable=false;

Server server(80);

void setup()
{
  Serial.begin(9600);
  pinMode(ledPin,OUTPUT); //set led pin as output
  Serial.println("connecting...");
  WiFly.begin();
  Serial.println("Done!");
  server.begin();
  
  Serial.println(WiFly.ip());//Display wifly shield ip address
}

void loop()
{
  Client client = server.available(); 
  
  if(client)
  {    
    while(client.connected())
    { //while client is connected
      if (client.available())
      { //if client is available
        char c=client.read();//read data from client
        if (msgAvailable)
        {
          if(c != '\n' && c != '\r')
            httpRequest+=c;//parse data to string if condition is true
        } 
        if (c=='=')
        {
          msgAvailable=true; //if data sent is '=' intiate data parsing
        }
        if (c == '\n') 
        { //send html code to browser once the client is not available
          //header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connnection: close"));
          client.println();
          //body
          client.println(F("<!DOCTYPE html>"));
          client.println(F("<html>"));
          client.println(F("<body>"));
          client.println(F("<h1>Arduino Webserver Example</h1>"));
          client.println(F("<hr>"));
          client.println(F("<form name=\"input\" action=\"\" method=\"get\">"));
          client.println(F("	<input type=\"radio\" name=\"$\" value=\"on\">on"));
          client.println(F("	<input type=\"radio\" name=\"$\" value=\"off\">off"));
          client.println(F("	<input type=\"submit\" value=\"Submit\">"));
          client.println(F("</form> "));
          client.println(F("<hr>"));
          client.println(F("</body>"));
          client.println(F("</html>"));
          
          delay(100);
          ledControl(); //light or switch off LED
          msgAvailable=false;
          break;
        }
      }
    }
    // give the web browser time to receive the data
    delay(100);//delay is very important 
    client.flush();
    client.stop();
  }
}

void ledControl()
{
  Serial.println(httpRequest);
  if(httpRequest[0]=='o'&&httpRequest[1]=='n')
    digitalWrite(ledPin,HIGH); //set led on if client sends on
  else if(httpRequest[0]=='o'&& httpRequest[1]=='f' && httpRequest[2]=='f')
    digitalWrite(ledPin,LOW); //set led off if client sends of
  httpRequest="";
} 
