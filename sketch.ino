#include <LiquidCrystal.h>  // Utilisation de la librairie LiquidCrytal.h pour l'écran
#include <Keypad.h>         // Bibliothèque pour le 8 push buttons

// Constantes
#define PASSLENGTH 6
#define ROWS 4
#define COLS 4
#define MAX_TESTS 5

/* Définition de l'écran*/
// Définition des broches RS, E, et Data (DB4 à DB7)
LiquidCrystal lcd_16x2(2, 3, 7, 6, 5, 4);

/* Définition du capteur*/
// Définir où se trouve la sortie du capteur
int pirOutput = 53;

// Définir où se trouve le buzzer (ou la base du transistor en l'occurence)
int buzzer    = 29;

/* Définition du pad 4x4 */
// Définir où se trouve chaque pin des bouttons
byte buttonK0  = 31;
byte buttonK1  = 33;
byte buttonK2  = 35;
byte buttonK3  = 37;
byte buttonK4  = 39;
byte buttonK5  = 41;
byte buttonK6  = 43;
byte buttonK7  = 45;

// Créer un keypad avec la librairie Keypad.h
const char kp4x4Keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'}, 
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {buttonK7, buttonK6, buttonK5, buttonK4};  // Connecter les lignes 
byte colPins[COLS] = {buttonK3, buttonK2, buttonK1, buttonK0};  // Connecter les colonnes

// Nombre d'essais effectués
static int test = 0;

Keypad kp4x4 = Keypad(makeKeymap(kp4x4Keys), rowPins, colPins, ROWS, COLS);

void setup() {

  Serial.begin(9600);

  // Setup du capteur
  pinMode(pirOutput, INPUT);
  digitalWrite(pirOutput, LOW);

  // Setup de l'écran
  lcd_16x2.begin(16, 2);

  // Setup du buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
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
      lcd_16x2.write("*");

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
      delay(10000);
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
        delay(10000);
        lcd_16x2.clear();

        fakeLoop = false;
        test = 0;
        
      }
      
    }

  }
       
}