# led_rgb_arduin_ethernet_http

# Contrôleur RGB pour (personnellement) gérer un bandeau led.#


## Infos:##

- gestion via http.
- utilisation de la lib simtronyx_RGB_LED
	[simtronyx_RGB_LED](http://blog.simtronyx.de/en/a-rgb-led-library-for-arduino/ "Blog")
    [simtronyx_RGB_LED](https://github.com/wmchris/ArduinoLEDStripes/tree/master/libraries/simtronyx_RGB_LED "Github")
- Utilisation de la lib Ethernet


## Matériel :##


	- 1 x Arduino Mega 2560
    - 1 x shield ethernet
    - 1 x Ruban Led
    - 1 x ULN2003A
    - 1 x L7805CV
    - 1 x Led (pour le statu visuel)
    - 2 x Résistance de 1K
    - 1 x Resistance de 220ohms (pour la led de statu visuel)
    - 2 x 2n2222
    - 1 x relais 5v
    - 1 x 1N4001
    - 1 X alim 12v 2A (suivant la taille ou le nombre de leds utilisé)
    

## Mini Doc ##

Modification du code Arduino pour coller à votre config :

### fichiers control_rgb_lan.ino ###

Modifier les infos en Gras (adresse IP pour votre réseaux, adresse mac, port, et les numéros de pin sur votre Arduino)

IPAddress ip(**192, 168, 1, 59**);
byte mac[] = {
  **0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED**
};
int portWebserver = **8282**;


#define REDPIN **46**
#define GREENPIN **45** 
#define BLUEPIN **44**
int pinRelais=**47**; 
int pinReset=**28**;
int pinLed=**52**;


****__> Optionnel : vous pouvez changer les adresses des requêtes http


### decode_request.cpp et format_html.cpp ###
	
Dans la fonction : 

	String decode_request_http(String req) {}
    
    Vous pouvez changer les adresses à ce niveau :
    if (req.indexOf("/**0xF740BF**")
    
	N’oubliez pas de changer aussi dans le fichier format_html.cpp 
    
    
    
### Utilisation ###

Access a la mini page de gestion vraiment minimaliste le but étant le contrôler via Jeedom ou autre solution domotique : 

http://ip:port

Ex : http://192.168.1.59:8282


Power Off : http://ip:port/0xF740BF
Power On :  http://ip:port/0xF7C03F


Pour le fading :

Brighter : http://ip:port/0xF700FF
Darker : http://ip:port/0xF7807F
Slower : http://ip:port/0xF7D02F
Faster : http://ip:port/0xF7F00F

2 x listes prédéfini de couleur (à vous de modifier ou ajouter des listes) :
http://ip:port/0xF7E817 (Liste pour massage :-) WAF)
http://ip:port/0xD9ER18 (Liste pour relaxation)

Pour avoir le statu (simple statu on off) : 
http://ip:port/status
(Renvoi 0 ou 1)

Pour ajouter une couleur a la liste en mémoire pour le fading : 
http://ip:port/1xrgb-r255.g122.b5-END
 
Changer le : 
	- r255
    - g122
    - b5
  Suivant la couleur voulue
  
Pour mettre une couleur unique :
http://ip:port/color-r122.g211.b45-END
  Changer le : 
	- r122
    - g211
    - b45
  Suivant la couleur voulue
  
  
Pour faire un clear de la liste des couleurs du fading: 
http://ip:port/1xclear

Pour démarrer le fading : 
http://ip:port/1xstartfade


Pour stopper le fading: 
http://ip:port/1xstopfade


Pour redémarrer :
http://ip:port/reboot

## To do ##

- Amélioré le code et le montage 
- faire un plugin/widget pour Jeedom
...



	
    
    
