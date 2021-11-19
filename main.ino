#include <LiquidCrystal.h>  // Utilisation de la librairie LiquidCrytal.h pour l'écran
#include <Keypad.h>         // Bibliothèque pour le 8 push buttons

// Constantes
#define PASSLENGTH 6
#define ROWS 4
#define COLS 4
#define MAX_TESTS 5

/* Définition des LEDs */
int RED_LED     = 13;
int ORANGE_LED  = 12;
int GREEN_LED   = 11;

/* Définition de l'écran*/
// Définition des broches RS, E, et Data (DB4 à DB7)
LiquidCrystal lcd_16x2(2, 3, 4, 5, 6, 7);

/* Définition du capteur*/
// Définir où se trouve la sortie du capteur
int pirOutput = 53;

// Définir où se trouve le buzzer (ou la base du transistor en l'occurence)
int buzzer    = 29;

/* Définition du pad 4x4 */
// Définir où se trouve chaque pin des bouttons
byte K0  = 31;
byte K1  = 33;
byte K2  = 35;
byte K3  = 37;
byte K4  = 39;
byte K5  = 41;
byte K6  = 43;
byte K7  = 45;

// Créer un keypad avec la librairie Keypad.h
const char kp4x4Keys[ROWS][COLS] = {
  {'1', '2', '3', 'S'}, 
  {'4', '5', '6', 'G'}, 
  {'7', '8', '9', 'L'}, 
  {'E', '0', 'X', 'P'}
};
byte rowPins[ROWS] = {K0, K1, K2, K3};  // Connecter les lignes 
byte colPins[COLS] = {K7, K6, K5, K4};  // Connecter les colonnes

// Nombre d'essais effectués
static int test = 0;

Keypad kp4x4 = Keypad(makeKeymap(kp4x4Keys), rowPins, colPins, ROWS, COLS);

void setup() {

  Serial.begin(9600);

  // Setup des LEDs
  pinMode(RED_LED, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);      delay(1000);

  // Setup du capteur
  pinMode(pirOutput, INPUT);
  digitalWrite(pirOutput, LOW);

  // Setup de l'écran
  lcd_16x2.begin(16, 2);
  digitalWrite(RED_LED, LOW);       digitalWrite(ORANGE_LED, HIGH);   delay(1000); 

  // Setup du buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
  digitalWrite(ORANGE_LED, LOW);    digitalWrite(GREEN_LED, HIGH);
  
}

bool pass_verify() {
  // Déclaration du mot de passe qu'il faudra rentrer
  static const char password[PASSLENGTH] = "123456";

  // Déclaration du mot de passe piégé
  static const char fakePassword[PASSLENGTH] = "260900";

  // Déclaration d'un tableau qui récupèrera toutes les valeurs entrées
  static char passToVerify[PASSLENGTH+1];

  // Déclaration d'une variable pour compter le nombre de touches appuyées
  int n = 0;

  // Déclaration d'une variable pour avoir un index
  int index = 0;

  while (n < sizeof(password)) {
    // Lire les touches du pad appuyées
    char pressedKey = kp4x4.getKey();

    // Vérifier quand une touches est appuyée
    if (pressedKey) {

      // Afficher caractère "étoile" à chaque appuie {TODO WORKSHOP}
      lcd_16x2.setCursor(index++, 2);
      lcd_16x2.print("*");

      // Insérer les touches appuyées dans le tableau
      passToVerify[n] = pressedKey;
      n++;

      if (n == PASSLENGTH) { 

        // Vérifier si le mot de passe entré correspond au mot de passe voulu
        if (strcmp(passToVerify, password) == 0) {
          
          n = 0;
          return true;
          
        } else if (strcmp(passToVerify, fakePassword) == 0) {

          // TODO : Feature alarme différente

          n = 0;
          return false;

        } else {
          
          n = 0; 
          return false;
          
        }
      
      }

    }

  }
  
}

void lcd_16x2_home() {

  lcd_16x2.clear();
  lcd_16x2.setCursor(0, 0);

}

void loop() {
  // Variable pour créer une fausse boucle tant que le mot de passe n'a pas été ou a été trouvé
  bool fakeLoop = false;

  // Vérifier si le capteur a détecté un mouvement
  if(digitalRead(pirOutput) == HIGH) { 
    lcd_16x2_home();            lcd_16x2.print("  Detection de");
    lcd_16x2.setCursor(0, 1);   lcd_16x2.print("  Mouvement");
    delay(2000);

    fakeLoop = true;

  } else {

      // Attendre un mouvement
      lcd_16x2.setCursor(0, 0);
      lcd_16x2.print("   En attente");
    
  }

  while (fakeLoop) {

    lcd_16x2_home();
    lcd_16x2.print(" Entrez MDP : ");

    bool test_pass = pass_verify();

    if (test_pass) {

      // Mot de passe correcte
      lcd_16x2_home();
      lcd_16x2.print(" Bienvenue ");
      
      // Bip toutes les 500ms pendant 500ms 3 fois
      int count = 0;
      while (count < 3) {

        sound_good_pass();
        count++;

      }
      
      lcd_16x2.clear();
      fakeLoop = false;
      
    } else {

      // Mot de passe incorrecte
      test++;

      if (test < MAX_TESTS) {

        lcd_16x2_home();
        lcd_16x2.print("MDP incorrecte !");
        lcd_16x2.setCursor(0, 1);
        lcd_16x2.print("Reste: "); lcd_16x2.print(MAX_TESTS-test); lcd_16x2.print(" essais");
        delay(3000); 
        lcd_16x2.clear();
        
      } else {

        lcd_16x2.clear();
        lcd_16x2.setCursor(0, 1);
        lcd_16x2.print("   MDP bloque");
        
        // Alarm
        int count = 0;
        while (count < 5) {
  
          alarm();
          count++;
  
        }
        
        lcd_16x2.clear();

        fakeLoop = false;
        test = 0;
        
      }
      
    }

  }
       
}

void sound_good_pass(void) {

  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
        
}

void alarm(void) {

  digitalWrite(buzzer, HIGH);
  delay(1500);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(100);
        
}
