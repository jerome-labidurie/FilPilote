/** Control a domotic arduino via serial commands.
 * 
 * Main function is for heater (fil pilote)
 * @See http://u.dinask.eu/filpilote
 *
 * A Chacon command via RF433MHz can also be activated.
 */

/** activate chacon function */
// TODO: WTF #ifdef ?
#define CHACON 


#ifdef CHACON
#define CHACON_SEND 12
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
#endif

/** serial input line */
#define LINE_LEN 20
char line[LINE_LEN];

/** Pinouts */
#define PIN_PLUS 0
#define PIN_MOINS 1
#define NB_FILS 5
// TODO: change pinout pour meilleur cablage :)
uint8_t pins[NB_FILS][2] = {
  {10, 11},
  {8, 9},
  {6, 7},
  {4, 5},
  {2, 3},
};
char* filnames[NB_FILS] = {
  "Chambre J&N",
  "Chambre E",
  "Chambre M",
  "Buanderie",
  "3eme"
};

/** Display cmdline help
 */
void help (void) {
   uint8_t i;
   Serial.println ("H,h,? : affiche l'aide");
   Serial.println ("** Commandes fil pilote");
   Serial.println ("<cmd> <fil>");
   Serial.println (" <cmd> : C : Confort");
   Serial.println ("         A : Arret");
   Serial.println ("         G : Hors Gel");
   Serial.println (" eg : A 0 : arrete le chauffage 0");
   for (i=0; i<NB_FILS; i++) {
      Serial.print (" Chauffage ");
      Serial.print (i);
      Serial.print (" : ");
      Serial.println (filnames[i]);
   }
#ifdef CHACON
   Serial.println ("** Commandes prises");
   Serial.println ("<cmd> <system> <code>");
   Serial.println (" <cmd> : N : on");
   Serial.println ("         F : off");
   Serial.println (" eg : N 1 1");
#endif

} // help

/** Set mode for one fil
 * @param[in] fil the filpilote number [0:NB_FILS[
 * @param[in] mode the filpilote mode
 * @return true if success
 */
boolean setFilMode (uint8_t fil, char mode) {
  if (fil >= NB_FILS) return false;
  
  switch (mode) {
    case 'C':
       digitalWrite (pins[fil][PIN_PLUS], LOW);
       digitalWrite (pins[fil][PIN_MOINS], LOW);
       break;
    case 'A':
       digitalWrite (pins[fil][PIN_PLUS], HIGH);
       digitalWrite (pins[fil][PIN_MOINS], LOW);
       break;
    case 'G':
       digitalWrite (pins[fil][PIN_PLUS], LOW);
       digitalWrite (pins[fil][PIN_MOINS], HIGH);
       break;
    default:
       return false;
  }
  return true;
} // setFilMode()

/** Hardware initialisations
 */
void setup() {
   byte i;
   Serial.begin(9600);
   Serial.println("DomoticSerial v1");
   
   // pins init
   for (i=0; i < NB_FILS; i++) {
      pinMode (pins[i][PIN_PLUS],  OUTPUT);
      pinMode (pins[i][PIN_MOINS], OUTPUT);
      setFilMode (i, 'A'); // default value is Arret
   }

#ifdef CHACON
   // Transmitter is connected to Arduino Pin #12
   mySwitch.enableTransmit(CHACON_SEND);
   // Optional set number of transmission repetitions.
   mySwitch.setRepeatTransmit(15);
#endif

}


/** main loop */
void loop() {
   if (Serial.available()) {
      Serial.readBytesUntil('\n', line, LINE_LEN);
      switch (line[0]) {
        case 'C':
        case 'A':
        case 'G':
          if (setFilMode ( line[2] - '0', line[0] ) == true) {
            Serial.println (filnames[line[2] - '0']);
          } else {
            Serial.println ("Echec");
          }
          break;
        
#ifdef CHACON
         case 'N': // On
            mySwitch.switchOn (line[2]-'0', line[4]-'0');
            break;
         case 'F': // Off
            mySwitch.switchOff (line[2]-'0', line[4]-'0');
            break;
#endif
         case 'h':
         case 'H':
         case '?':
            help();
            break;
         default:
            Serial.print ("Unknown command:");
            Serial.println(line[0]);
      }
   }

   delay(500);
}

