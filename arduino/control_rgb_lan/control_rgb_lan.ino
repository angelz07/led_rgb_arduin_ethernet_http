#include "simtronyx_RGB_LED.h"
#include <Ethernet.h>
#include "decode_request.h"
#include "format_html.h"

IPAddress ip(192, 168, 1, 59);
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
int portWebserver = 8282;


#define REDPIN 46
#define GREENPIN 45 
#define BLUEPIN 44
int pinRelais=47; 
int pinReset=28;
int pinLed=52;

String status_on_off = "0";
String status_fading = "0";
String status_vitesse = "100";
String status_intensite = "50";
String status_cmd = "0";

int rouge_defaut = 125;
int vert_defaut = 52;
int bleu_defaut = 241;


String status_led = "0";
simtronyx_RGB_LED strip(REDPIN,GREENPIN,BLUEPIN);

// overall brightness 
int brightness = 50;
// speed for animation / color transitions
int fadingDelay = 100;

#define FADING_STEPS 200
#define COLOR_HOLD 20


EthernetServer server(portWebserver);

void setup() {
  pinMode(pinRelais,OUTPUT); //pin en mode OUTPUT
  //digitalWrite(pinRelais,HIGH);
   // active la broche en sortie (OUTPUT)  
  pinMode(pinReset, OUTPUT);
  digitalWrite(pinReset, LOW);
  pinMode(pinLed, OUTPUT); 
  digitalWrite(pinLed, LOW);
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //init webserveur et connexion lan
  Serial.println("Connexion au réseaux en cours ...");
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  server.begin();
  Serial.print("server connecté!!  adresse :  ");
  Serial.print(Ethernet.localIP());
  Serial.print(" port : ");
  Serial.println(portWebserver);
  digitalWrite(pinLed, HIGH);
  strip.animateColorTypeSet(ANIMATETYPE_RGB);

  

}

void pushReset() 
{ 
   
  Serial.println("redemarage");
   delay(1000);
  // Déactive le reset forçant la sortie au niveau bas 
  digitalWrite(pinReset, HIGH);
  Serial.println("fin redemarage");
}

void loop() {
  strip.loop();
  
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);


        String req = client.readStringUntil('\r');
        // Match the request
         if (req.indexOf("") != -10) {  //checks if you're on the main page

           String reponse_decodage = decode_request_http(req);
           Serial.println("");
           /*if(reponse_decodage == "reboot"){
            String repStatus = "HTTP/1.1 200 OK\r\n";
             repStatus += "Content-Type: text/html\r\n\r\n";
             repStatus += "<!DOCTYPE HTML>\r\n<html>\r\n";
             repStatus += "<body>\n";
             repStatus += "<span class='status_reboot'>";
             repStatus += "1";
             repStatus += "</span>\n";
             

            repStatus += "</body>\n";
            repStatus += "</html>\n";
          
            client.print(repStatus);
            
             delay(5000);
             pushReset() ;
             break;
           }*/
           if(reponse_decodage == "status"){
              reponseJson(client, "status");
              delay(2000);
              client.stop();
              break;
           }
          
           String isCommand = isCmd(reponse_decodage);
            if (isCommand == "true") {
              String retour = action(reponse_decodage);
              delay(500);
              reponseJson(client, "cmd_status");
              delay(2000);
              client.stop();
              break;
            }
            else {
              String retour = action(reponse_decodage);
            }
           
         }

         
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // Prepare the response
          String s = format_page_web();
          
          client.println(s);
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    
  }
  // strip.loop();
}


String action(String req){
  String retour = "0";
  if (req == "0xF740BF") { //checks if you clicked OFF
      status_led="0";
      status_on_off = "0";
      status_intensite = brightness;
      status_vitesse = fadingDelay;
      strip.animateStop();
      digitalWrite(pinRelais,LOW); // off commande l'extinction
      delay(1000);
      Serial.println("OFF");
      retour = "1";
    }
    if (req == "0xF7C03F") { //checks if you clicked ON
      status_led="1";
      status_on_off = "1";
      status_intensite = brightness;
      status_vitesse = fadingDelay;
      strip.animateStop();
      status_fading = "0";
      strip.setRGB(rouge_defaut,vert_defaut,bleu_defaut);
      digitalWrite(pinRelais,HIGH); // on commande l'allumage
      delay(1000);
      Serial.println("ON");
      retour = "1";
    }
   /* 
    *  
    *  int rouge_defaut = 125;
int vert_defaut = 52;
int bleu_defaut = 241;
if (req == "0xF720DF") { //red
      status_led="1";
      strip.animateStop();
      strip.setRGB(255,0,0);
      delay(1000);
      Serial.println("Rouge");
      retour = "1";
    }
*/
   
  if (req == "0xF700FF") { // brighter
      status_led="1";
      strip.useBrightnessAdjust( true );
      brightness = inc(brightness,10,100);
      strip.setBrightnessRGB(brightness);
      delay(1000);
      Serial.println(brightness);
      Serial.println("brighter");
      status_intensite = brightness;
      status_vitesse = fadingDelay;
      retour = "1";
    }

  if (req == "0xF7807F") { // darker
    status_led="1";
     strip.useBrightnessAdjust( true );
     brightness = dec(brightness,10);
      strip.setBrightnessRGB(brightness);
      delay(1000);
      Serial.println(brightness);
      Serial.println("darker");
      status_intensite = brightness;
      status_vitesse = fadingDelay;
      retour = "1";
    }


  if (req == "0xF7D02F") { // slower
    status_led="1";
      fadingDelay = inc(fadingDelay,100,5000);
      strip.animateSpeedSet(fadingDelay);
      delay(1000);
      Serial.println("Slow");
      status_intensite = brightness;
      status_vitesse = fadingDelay;
      retour = "1";
    }

  if (req == "0xF7F00F") { // faster
    status_led="1";
      fadingDelay = dec(fadingDelay, 100 );
      strip.animateSpeedSet(fadingDelay);
      delay(1000);
      Serial.println("Fast");
      status_intensite = brightness;
      status_vitesse = fadingDelay;
      retour = "1";
    }

   if (req == "0xF7E817") { // fading Massage
    status_led="1";
    status_intensite = brightness;
      status_vitesse = fadingDelay;
      status_fading = "0";
      strip.animateStop();
      delay(50);
      digitalWrite(pinRelais,HIGH); // on commande l'allumage
      status_on_off = "1";
      delay(50);
       strip.animateColorsClear();
       delay(50);
       strip.animateColorAdd(255,47,152,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(175,0,90,FADING_STEPS);  
       delay(50);
       strip.animateColorAdd(145,14,178,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(224,100,255,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(211,36,255,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(108,178,0,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(178,255,62,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(51,72,178,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(98,124,255,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(112,4,178,FADING_STEPS);
       delay(50);
       strip.animateStart();
      status_fading = "1";
      Serial.println("fading massage");
      retour = "1";
    }

    if (req == "0xD9ER18") { // fading Massage
      status_led="1";
      strip.animateStop();
      status_fading = "0";
      status_intensite = brightness;
      status_vitesse = fadingDelay;
      delay(50);
      digitalWrite(pinRelais,HIGH); // on commande l'allumage
      status_on_off = "1";
      delay(50);
       strip.animateColorsClear();
       delay(50);
       strip.animateColorAdd(13,178,61,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(62,255,178,FADING_STEPS); 
       delay(50); 
       strip.animateColorAdd(36,255,100,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(178,1,0,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(255,37,36,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(178,13,136,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(255,91,213,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(255,36,199,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(178,140,15,FADING_STEPS);
       delay(50);
       strip.animateColorAdd(36,65,255,FADING_STEPS);
       delay(50);
       strip.animateStart();
       status_fading = "1";
       Serial.println("fading relax");
       retour = "1";
    }

    if (req.indexOf("rgb:") != -1) { // add color to list RGB
      status_intensite = brightness;
      status_vitesse = fadingDelay;
        int startRgbRetour = req.indexOf("rgb:");
        String rgb = req.substring(startRgbRetour+4);
       
        if (getValue(rgb,'.',0) != -1 && getValue(rgb,'.',1) != -1 && getValue(rgb,'.',2) != -1 ) {
          String r = getValue(rgb,'.',0);
          String g = getValue(rgb,'.',1);
          String b = getValue(rgb,'.',2);

          int r_int = r.toInt();
          int g_int = g.toInt();
          int b_int = b.toInt();
          
  
          delay(100);
          strip.animateColorAdd(r_int,g_int,b_int,FADING_STEPS);
          retour = "1";
          Serial.print("Add Color : ");
          Serial.print(r_int);
          Serial.print(".");
          Serial.print(g_int);
          Serial.print(".");
          Serial.print(b_int);
          Serial.print(" To list Fading.");
          
        }
      }

      if (req.indexOf("color:") != -1) { // set Color
        status_intensite = brightness;
      status_vitesse = fadingDelay;
        int startRgbRetour = req.indexOf("color:");
        String rgb = req.substring(startRgbRetour+6);
      
        if (getValue(rgb,'.',0) != -1 && getValue(rgb,'.',1) != -1 && getValue(rgb,'.',2) != -1 ) {
          String r = getValue(rgb,'.',0);
          String g = getValue(rgb,'.',1);
          String b = getValue(rgb,'.',2);

          int r_int = r.toInt();
          int g_int = g.toInt();
          int b_int = b.toInt();
          
          digitalWrite(pinRelais,HIGH); // on commande l'allumage
          delay(100);
          status_led="1";
          strip.animateStop();
          delay(100);
          strip.setRGB(r_int,g_int,b_int);
          delay(1000);
         
          retour = "1";
          Serial.print("Set Color : ");
          Serial.print(r_int);
          Serial.print(".");
          Serial.print(g_int);
          Serial.print(".");
          Serial.println(b_int);
          
        }
      }
      if (req == "1xclear") { // Clear List Fadding
        status_intensite = brightness;
      status_vitesse = fadingDelay;
        status_fading = "0";
        strip.animateStop();
        delay(50); 
        strip.animateColorsClear();
        
        delay(1000);
        Serial.println("Clear LIst Fadding");
        retour = "1";
      }

      if (req == "1xstopfade") { // Stop Fadding
        status_led="0";
        status_intensite = brightness;
      status_vitesse = fadingDelay;
        digitalWrite(pinRelais,LOW); // off commande l'extinction
        status_on_off = "0";
        delay(50);
        strip.animateStop();
        status_fading = "0";
        delay(1000);
        Serial.println("Stop Fadding");
        retour = "1";
      }

      if (req == "1xstartfade") { // Start Fadding
        status_led="1";
        status_intensite = brightness;
      status_vitesse = fadingDelay;
        digitalWrite(pinRelais,HIGH); // on commande l'allumage
        status_on_off = "1";
        delay(50);
        strip.animateStart();
        status_fading = "1";
        delay(1000);
        Serial.println("Start Fadding");
        retour = "1";
      }

       if (req == "0xtest03F") { //Test ON
        status_led="1";
        status_on_off = "1";
        status_intensite = brightness;
        status_vitesse = fadingDelay;
        strip.animateStop();
        digitalWrite(pinRelais,HIGH); // on commande l'allumage
        status_fading = "0";
        Serial.println("Test On");
    
        retour = "1";
      }
      
      if (req == "0xtest14G") { //Test OFF
        status_on_off = "0";
        status_intensite = brightness;
        status_vitesse = fadingDelay;
        strip.animateStart();
        digitalWrite(pinRelais,LOW); // off commande l'extinction
        status_fading = "1";
        Serial.println("Test Off");
    
        retour = "1";
      }

      if (req == "reboot") { //Reboot
        
        Serial.println("Reboot");
        delay(2000);
        pushReset() ;
        retour = "1";
      }

     
      
  return retour;
}


int inc( int in, int delta, int limit ) {
  int r = in + delta;
  if( r > limit ) r = limit;
  return r;
}

int dec( int in, int delta ) {
  int r = in - delta;
  if( r < 0 ) r = 0;
  return r;
}
void reponseJson(EthernetClient client, String arg){
    // On fait notre en-tête
    // Tout d'abord le code de réponse 200 = réussite
    client.println("HTTP/1.1 200 OK");
    // Puis le type mime du contenu renvoyé, du json
    client.println("Content-Type: application/json");
    // Et c'est tout !
    // On envoie une ligne vide pour signaler la fin du header
    client.println();

    if(arg == "status"){
      // Puis on commence notre JSON par une accolade ouvrante
      client.println("{");
      
      // On envoie la première clé : "status_on_off"
      client.print("\t\"status_on_off\": ");
      // Puis la valeur de status_on_off
      client.print(status_on_off);
      //Une petite virgule pour séparer les deux clés
      client.println(",");
      // Et on envoie la seconde nommée "status_fading"
      client.print("\t\"status_fading\": ");
      client.println(status_fading);
      //Une petite virgule pour séparer les deux clés
      client.println(",");
      // Et on envoie la seconde nommée "status_vitesse"
      client.print("\t\"status_vitesse\": ");
      client.println(status_vitesse);
      //Une petite virgule pour séparer les deux clés
      client.println(",");
      // Et on envoie la seconde nommée "status_intensite"
      client.print("\t\"status_intensite\": ");
      client.println(status_intensite);
      
  
      // Et enfin on termine notre JSON par une accolade fermante
      client.println("}");
    }

    if(arg == "cmd_status"){
      // Puis on commence notre JSON par une accolade ouvrante
      client.println("{");
      
      // On envoie la première clé : "status_on_off"
      client.print("\t\"status_on_off\": ");
      // Puis la valeur de status_on_off
      client.print(status_on_off);
      //Une petite virgule pour séparer les deux clés
      client.println(",");
      // Et on envoie la seconde nommée "status_fading"
      client.print("\t\"status_fading\": ");
      client.println(status_fading);
      //Une petite virgule pour séparer les deux clés
      client.println(",");
      // Et on envoie la seconde nommée "status_vitesse"
      client.print("\t\"status_vitesse\": ");
      client.println(status_vitesse);
      //Une petite virgule pour séparer les deux clés
      client.println(",");
      // Et on envoie la seconde nommée "status_intensite"
      client.print("\t\"status_intensite\": ");
      client.println(status_intensite);
      //Une petite virgule pour séparer les deux clés
      client.println(",");
      // Et on envoie la seconde nommée "status_intensite"
      client.print("\t\"status_cmd\": ");
      client.println("1");
      
      // Et enfin on termine notre JSON par une accolade fermante
      client.println("}");
    }
    
    // Donne le temps au client de prendre les données
    delay(50);
  }

String isCmd(String arg){
  String retour = "false";
  if (arg == "0xF7C03F"){ //ON
    retour = "true";
  }
  if (arg == "0xF740BF"){ //OFF
    retour = "true";
  }
  
  if (arg == "0xtest03F"){ //Test On
    retour = "true";
  }
  if (arg == "0xtest14G"){ //Test OFF
    retour = "true";
  }
  if (arg == "0xF700FF"){ // brighter
    retour = "true";
  }
  if (arg == "0xF7807F"){ // darker
    retour = "true";
  }
  if (arg == "0xF7D02F"){ // slower
    retour = "true";
  }
  if (arg == "0xF7F00F"){ // faster
    retour = "true";
  }
  if (arg == "0xF7E817"){ // fading Massage
    retour = "true";
  }
  if (arg == "0xD9ER18"){ // fading Massage
    retour = "true";
  }
  if (arg == "1xclear"){ // Clear List Fadding
    retour = "true";
  }
  if (arg == "1xstopfade"){ // Stop Fadding
    retour = "true";
  }
  if (arg == "1xstartfade"){ // Start Fadding
    retour = "true";
  }
  if (arg == "reboot"){ // reboot
    retour = "true";
  }

  
  if (arg.indexOf("rgb:") != -1){ // add color to list RGB
    retour = "true";
  }
  if (arg.indexOf("color:") != -1){ // set Color
    retour = "true";
  }
 
  return retour;
  
}
