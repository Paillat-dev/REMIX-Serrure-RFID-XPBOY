//©Experimentboy, Modifié par Paillat-dev/Paillat


#include <RFID.h>
#include <SPI.h>
#include <EEPROM.h>
bool bouton_1023 = false;
#define SS_PIN 10 //SS=SDA, 53 imposé pour Mega, puis SCK 52, MOSI 51, MISO 50
                   // Arduino Uno: RST: 9, SS (SDA): 10, MOSI: 11, MISO: 12, SCK: 13
                   // Arduino Nano: RST: D9, SS (SDA): D10, MOSI: D11, MISO: D12, SCK: D13
#define RST_PIN 9 //le pin qu'on veut pour RST
byte led_rouge = 6, led_verte = 7, bouton = A0, serrure = 8, buzz = 2, bouton_serrure = A1;
byte cur_badge[5]; //current badge, liste des 5 octets du badge que l'on lit
RFID rfid(SS_PIN, RST_PIN); 
    
void setup()
{ 
  Serial.begin(9600); //uniquement pour debug, arrête de faire pause pour lire mes conneries
  SPI.begin(); 
  rfid.init();
  pinMode(led_rouge,OUTPUT);
  pinMode(led_verte,OUTPUT);
  pinMode(bouton, INPUT);
  pinMode(serrure, OUTPUT);
  pinMode(buzz, OUTPUT);
  
  if(analogRead(bouton) == 1023)// à l'appui du bouton au moment du démarrage, effaçage de la mémoire
  {
    for (int n=0; n<1024; n++) //lancer ces deux lignes pour effacer totalement la mémoire 
        EEPROM.write(n,0);
  
  digitalWrite(led_rouge,1);
  digitalWrite(led_verte,1);
  tone(buzz,2093,1000);
  delay(1000);
  digitalWrite(led_rouge,0);
  digitalWrite(led_verte,0);
  Serial.println("MÉMOIRE FORMATÉE");
  }
  else //bip normal d'allumage
  {
    digitalWrite(led_verte,1);
    tone(buzz,2093, 150); delay(150);
    digitalWrite(led_verte,0); delay(150);
    tone(buzz, 2093, 150); 
    digitalWrite(led_verte,1); delay(150);
    digitalWrite(led_verte,0);
  }
}
 
void new_badge()
{
  // Enregistre un nouveau badge dans la mémoire EEPROM
  int start_mem = EEPROM.read(0)*5+1; //dans la première case mémoire, on stock le nombre de badge déjà sauvegardés,
                                  //et on calcule la case mémoire à partir de laquelle enregistrer le nouveau badgenb_badge
    for(byte n= 0; n<5; n++)
    {
      EEPROM.write(start_mem+n, cur_badge[n]);
    }
    EEPROM.write(0, EEPROM.read(0)+1);
}
 
bool compare_badge()
{
  //Compare le badge lu avec tous les badges enregistrés
  int nb_badge = EEPROM.read(0); //récupération du nombre de badges stockés en mémoire
  bool badgeOk = false; //0 si badge non correspondant, 1 si badge correspondant
  Serial.println("Badge actuel: ");
  Serial.println(cur_badge[0]);
  Serial.println(cur_badge[1]);
  Serial.println(cur_badge[2]);
  Serial.println(cur_badge[3]);
  Serial.println(cur_badge[4]);
  Serial.println("FIN BADGE ACTUEL");
  for(int n = 0; n<nb_badge; n++)
  {
  Serial.print("Badge memoire lu numero: "); Serial.println(n+1);
  Serial.println(EEPROM.read(n*5+1));
  Serial.println(EEPROM.read(n*5+2));
  Serial.println(EEPROM.read(n*5+3));
  Serial.println(EEPROM.read(n*5+4));
  Serial.println(EEPROM.read(n*5+5));
  Serial.println("FIN BADGE MEMOIRE LU");
    badgeOk = EEPROM.read(n*5+1)==cur_badge[0] && EEPROM.read(n*5+2)==cur_badge[1] && EEPROM.read(n*5+3)==cur_badge[2] && EEPROM.read(n*5+4)==cur_badge[3] && EEPROM.read(n*5+5)==cur_badge[4];
    if (badgeOk) return true;
  }
  return false;
}
 
 
 
void loop()
{
  if (analogRead(bouton_serrure) == 1023){
    Serial.println("Serrure ouverte de l'interieur");
    Serial.println("bouton rouge = ");
    Serial.print(bouton_serrure);
    digitalWrite(led_verte, 1);
    digitalWrite(serrure, 1);
    delay(3000);
    digitalWrite(serrure,0);
    digitalWrite(led_verte, 0);
  }
  if (analogRead(bouton) == 1023 && bouton_1023 != true) {
    bool bouton_1023 = true;
    Serial.print("Bouton appuyé = ");
    Serial.println(bouton_1023);
  }
  if (analogRead(bouton) != 1023 && bouton_1023 !=false){
    bool bouton_1023 = false;
    Serial.print("Bouton appuyé = ");
    Serial.println(bouton_1023);
  }
  
    if (rfid.isCard()) {
        if (rfid.readCardSerial())
            {
                for(byte n=0; n<5; n++)
                {
                cur_badge[n] = rfid.serNum[n]; //copie de la liste des 5 octets lus sur le badge dans la liste current badge (cur_badge))
                } 
                Serial.println(rfid.serNum[0],DEC);
                Serial.println(rfid.serNum[1],DEC);
                Serial.println(rfid.serNum[2],DEC);
                Serial.println(rfid.serNum[3],DEC);
                Serial.println(rfid.serNum[4],DEC);
             
         if(compare_badge()) //si la comparaison du badge actuel avec un des badges mémoire est ok, alors on ouvre
         {
          Serial.println("BADGE OK");
          digitalWrite(led_verte,1);
          tone(buzz,523,50);
          delay(50);
          tone(buzz, 783, 50);
          delay(50);
          tone(buzz, 1046, 50);
          delay(50);
          tone(buzz, 1568, 50);
          delay(50);
          tone(buzz, 2093, 70);
          delay(250);
          digitalWrite(serrure, 1);
          delay(3000);
          digitalWrite(led_verte,0);
          digitalWrite(serrure, 0);
         }
         else
         {
          Serial.println("BADGE INCONNU");
          digitalWrite(led_rouge, 1);
          tone(buzz,370,50);
          delay(100);
          tone(buzz, 370, 300);
          delay(1000);
          digitalWrite(led_rouge, 0);
         }
       }
    }
 
          
    if(analogRead(bouton) == 1023) //lors de l'appui du bouton pour enregistrer un nouveau badge
    {
      Serial.println("PROCESSUS NOUVEAU BADGE");
       while(!digitalRead(bouton)) {} //anti rebond sur le bouton d'enregistrement de nouveau badge
       delay(100);
       digitalWrite(led_rouge, 1); digitalWrite(led_verte,1); //allumage des deux leds simultanément pour signaler que c'est OOOKAAAAAYYYYY et prêt
  
       while (!rfid.isCard() && digitalRead(bouton)) {} //on attend la lecture d'un badge, ou le rappui du bouton qui va annuler, car le if suivant ne sera pas vérifié
       
       if (rfid.readCardSerial())
       {
        for(byte n=0; n<5; n++)
            {cur_badge[n] = rfid.serNum[n];} //lecture du badge que l'on passe
        if(!compare_badge()) //si le badge n'est pas déjà enregistré, on enregistre le nouveau badge
            {
              new_badge(); 
              digitalWrite(led_rouge, 0);
              digitalWrite(led_verte, 1);
              tone(buzz, 2093, 500);
              delay(200);
              digitalWrite(led_verte, 0);
              delay(200);
              digitalWrite(led_verte, 1);
              delay(200);
              digitalWrite(led_verte, 0);
              delay(200);
              digitalWrite(led_verte, 1);
              delay(200);
              digitalWrite(led_verte, 0);
              Serial.println("NOUVEAU BADGE ENREGISTRÉ");
            }
          else //si le badge que l'on veut ajouter est déjà enregistré, du con, ça clignote rouge
          {
              Serial.println("BADGE DÉJA EXISTANT");
              digitalWrite(led_verte, 0);
              digitalWrite(led_rouge, 1);
              tone(buzz,370,500);
              delay(200);
              digitalWrite(led_rouge, 0);
              delay(200);
              digitalWrite(led_rouge, 1);
              delay(200);
              digitalWrite(led_rouge, 0);
              delay(200);
              digitalWrite(led_rouge, 1);
              delay(200);
              digitalWrite(led_rouge, 0);
          }
        }
      else 
      {
         //si on rappuie sur le bouton, retour
        Serial.println("PROCESSUS ANNULÉ");
        digitalWrite(led_rouge,0);
        digitalWrite(led_verte,0);
        while(!digitalRead(bouton)) {} //anti redéclenchement si on reste appuyé
        delay(500);
      }
     }
    rfid.halt();
}
