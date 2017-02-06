/** Control a domotic arduino via serial commands.
 * 
 * Main function is for heater (fil pilote)
 * @See http://u.dinask.eu/filpilote
 *
 * A Chacon command via RF433MHz can also be activated.
 */

/** activate chacon function 
 * Comment to disable.
 */
#define CHACON 

// BOF preprocessor bug prevent - insert me on top of your arduino-code
// From: http://subethasoftware.com/2013/04/09/arduino-compiler-problem-with-ifdefs-solved/
#if 1
__asm volatile ("nop");
#endif

#ifdef CHACON
/** RF send pin */
#define CHACON_SEND 12
/** RCSwitch lib for chacon command
 * @See https://github.com/sui77/rc-switch
 */
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
#endif

/** serial input line */
#define LINE_LEN 20
char line[LINE_LEN];

/** Pinouts */
#define PIN_PLUS 0
#define PIN_MOINS 1
#define NB_FILS 6
/** Pinouts for FilPilote commands
 */
uint8_t pins[NB_FILS][2] = {
  {10, 11},
  {8, 9},
  {6, 7},
  {4, 5},
  {2, 3},
  {A5, A4},
};
char* filnames[NB_FILS] = {
  "Chambre J&N",
  "Chambre E",
  "Chambre M",
  "Buanderie",
  "3eme",
  "Entree",
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
   Serial.println ("         E : Eco");
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
 * @param[in] fil the filpilote number [0..NB_FILS[
 * @param[in] mode the filpilote mode
 * @return true if success
 */
boolean setFilMode (uint8_t fil, char mode) {
  if (fil >= NB_FILS) return false;
  
  switch (mode) {
    case 'C': 
    case 'c': // Confort
       digitalWrite (pins[fil][PIN_PLUS],  LOW);
       digitalWrite (pins[fil][PIN_MOINS], LOW);
       break;
    case 'A':  // Arret
    case 'a':
       digitalWrite (pins[fil][PIN_PLUS],  HIGH);
       digitalWrite (pins[fil][PIN_MOINS], LOW);
       break;
    case 'G': // Hors Gel
    case 'g':
       digitalWrite (pins[fil][PIN_PLUS],  LOW);
       digitalWrite (pins[fil][PIN_MOINS], HIGH);
       break;
    case 'E': // Eco
    case 'e':
       digitalWrite (pins[fil][PIN_PLUS],  HIGH);
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
   // Transmitter is connected to Arduino Pin CHACON_SEND
   mySwitch.enableTransmit(CHACON_SEND);
   // Optional set number of transmission repetitions.
   mySwitch.setRepeatTransmit(15);
#endif
  pinMode(13, OUTPUT);
  for (i=0; i < 3; i++) {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
}


/** main loop */
void loop() {
   if (Serial.available()) {
      Serial.readBytesUntil('\n', line, LINE_LEN);
      switch (line[0]) {
        case 'C': case 'c':
        case 'A': case 'a':
        case 'G': case 'g':
        case 'E': case 'e':
          if (setFilMode ( line[2] - '0', line[0] ) == true) {
            Serial.print ("OK ");
          } else {
            Serial.println ("KO ");
          }
          Serial.println (filnames[line[2] - '0']);
          break;
        
#ifdef CHACON
         case 'N': // On
         case 'n':
            mySwitch.switchOn (line[2]-'0', line[4]-'0');
            Serial.println ("OK ");
            break;
         case 'F': // Off
         case 'f':
            mySwitch.switchOff (line[2]-'0', line[4]-'0');
            Serial.println ("OK ");
            break;
#endif
         case 'h':
         case 'H':
         case '?':
            help();
            break;
         default:
            Serial.print ("KO Unknown command:");
            Serial.println(line[0]);
      }
   }

   delay(500);
}

