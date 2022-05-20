#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "bitmaps.h"

// Pin Belegung fuer das Display.
#define TFT_DC 9
#define TFT_CS 10

// Hardware SPI nutzen (on Uno, #13, #12, #11)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


#define Joystick_X A4
#define Joystick_Y A3
#define Joystick_Button 2
#define maximum_snake_lenght 300 // So lang darf die Schlange werden
#define spielfeld_breite 31 // Spielfeld breite
#define spielfeld_hoehe 20 // Spielfedl höhe

#define p2s_x(p_x) (10*p_x+5) // Umrechnung in Spiel Koordinaten
#define p2s_y(p_y) (10*p_y+35) // Umrechnung in Spiel Koordinaten 
// Ein Define kann auch einen kleine Funktion sein.

//------------------------------------------------------------------------------------------------------------------------
//----------------------------------Variablen initialisieren--------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
int JoystickX;
int JoystickY;

int score = 0;
int highscore=0;
int game_status = 0; // Wird benötigt für die Switch Case Anweisung

int CursorX;
int CursorY;  

int dx=1,dy=0;
int schlangen_laenge; // Aktuelle länge der Schlange
byte blocked[spielfeld_breite][spielfeld_hoehe]; //Maxiamel Länge der Schlange
byte position_x[maximum_snake_lenght]; //Maximale Länge der Einheiten die gespeichert werden können für die Positionen bei denen die Schlange war.
byte position_y[maximum_snake_lenght]; //Maximale Länge der Einheiten die gespeichert werden können für die Positionen bei denen die Schlange war.
int head = 0; // wo habe ich im array gespeichert.
int tail = 0; // Angeafnegn bei -5 weil somit ein Puffer von 5 Einheiten erstellt wird. das ist die aktuelle Länge der Schlange.
int counter = 5;

int random_food_x; // Variable X für den ersten Spawnpunkt des Essens
int random_food_y; // Variable Y für den zweiten Spawnpunkt des Essens
int random_generator_for_food_x; // Random generierte Zahlen für den Apfel
int random_generator_for_food_y; // Random generierte Zahlen für den Apfel

//------------------------------------------------------------------------------------------------------------------------
//-----------------------------------Variablen initialisieren Ende--------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------
//-----------------------------------Funktionen Anfang--------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void wellcome_screen()
{
  tft.fillScreen(ILI9341_BLACK); // Hintergrund schwarz machen. Sonst wäre er weis
  tft.setTextColor(ILI9341_WHITE); // Farbe des Textes setzen
  tft.setTextSize(3); // Größe des Textes
  tft.setCursor(40, 30); // Koordinaten setzen wo die Schrift anfängt
  tft.print("Willkommen zu"); // Text auf das Display schreiben
  tft.drawBitmap(110, 75, wellcome_snake, 92, 56, ILI9341_GREEN);
  tft.setCursor(35, 160); // Koordinaten setzen wo die Schrift anfängt
  tft.setTextSize(2);
  tft.print("Push Joystick to start"); // Text auf das Display schreiben
}

void game_screen()
{
  //Display Rahmen, Hintergrund und Text:Score
  tft.fillScreen(ILI9341_BLACK); // Hintergrund schwar machen. Sonst wäre er weis
  tft.drawRect(0, 30, 320, 210, ILI9341_PURPLE); // Rechteck malen
  tft.drawRect(1, 31, 318, 208, ILI9341_PURPLE);
  tft.drawRect(2, 32, 316, 206, ILI9341_PURPLE);
  tft.drawRect(3, 33, 314, 204, ILI9341_PURPLE);
  tft.drawRect(4, 34, 312, 202, ILI9341_PURPLE);
  
  tft.setTextColor(ILI9341_RED); // Farbe des Textes setzen
  tft.setTextSize(3); // Größe des Textes
  tft.setCursor(65, 4); // Koordinaten setzen wo die Schrift anfängt
  tft.print("Score:"); // Text auf das Display schreiben
  tft.print(score);
}

void loser_screen()
{
  tft.fillScreen(ILI9341_BLACK); // Hintergrund schwarz machen. Sonst wäre er weis
  tft.setTextColor(ILI9341_WHITE); // Farbe des Textes setzen
  tft.setTextSize(3); // Größe des Textes
  tft.setCursor(50, 10); // Koordinaten setzen wo die Schrift anfängt
  tft.print("! YOU LOST !"); // Text auf das Display schreiben
  tft.drawBitmap(100, 55, dead_snake, 124, 38, ILI9341_RED);
  tft.setCursor(45, 110);
  tft.setTextSize(3);
  tft.print("Learn to play"); // Text auf das Display schreiben
  tft.setCursor(65, 140);
  tft.setTextColor(ILI9341_RED); // Farbe des Textes setzen
  tft.print("Score:"); // Text auf das Display schreiben
  tft.print(score);
  tft.setCursor(40, 170);
  tft.print("Highscore:"); // Text auf das Display schreiben
  tft.print(highscore);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE); // Farbe des Textes setzen
  tft.setCursor(12, 210); // Koordinaten setzen wo die Schrift anfängt
  tft.print("Play again? Push Joystick"); // Text auf das Display schreiben*/
}

void food_generator()
{
      
    random_generator_for_food_x = random(0,31); // Random Zahl wischen 0 und 31 generieren
    random_generator_for_food_y = random(0,20); // Random Zahl wischen 0 und 20 generieren 
    random_food_x = p2s_x(random_generator_for_food_x); // X Koordiante für den Apfel ausrechnen
    random_food_y = p2s_y(random_generator_for_food_y); // Y Koordiante für den Apfel ausrechnen
}

void move_tail()
{    
    if(counter > 0)
    {
      counter-=1;
    }
    else
    {
      tft.drawRect(p2s_x(position_x[tail]), p2s_y(position_y[tail]), 10, 10, ILI9341_BLACK); // Rechteck das eigentlich grün geworden wäre wieder schwarz machen
      blocked[position_x[tail]][position_y[tail]] = 0; // An der Stelle an der die Schlange eigentlich gewesen wäre muss das array blocked an dieser Stelle wieder auf 0 gesetzt werden. 
                                                       //Sonst würde bei einem erneuten vorbei kommen an dieser Stelle eine Kollision stattfinden obwohl dort überhaupt keine Schlange ist
      tail++; // Hochzählen der Tail Variable damit die Schlange immer in der gleichen Länge bleibt 
      if(tail >= maximum_snake_lenght)// Wenn tail größer oder gleich maximum_snake_lenght-300 erreicht hat, wird head auf 0 gesetzt um neue Werte speichern zu können.
      {
       tail = 0;
      }
    }
}

void print_score()
{
    tft.fillRect(173, 4, 250, 24, ILI9341_BLACK); // Schwarztes Rechteck malen sonst bleibt der letzte gezeichnete Score stehen.
    tft.setTextColor(ILI9341_RED); // Farbe des Textes setzen
    tft.setTextSize(3); // Größe des Textes
    tft.setCursor(65, 4); // Koordinaten setzen wo die Schrift anfängt
    tft.print("Score:"); // Text auf das Display schreiben
    tft.print(score);
}
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Funktionen Ende-----------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Setup Anfang---------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void setup() // COde der nur einmal ausgeführt werden soll
{
  Serial.begin(9600);
  pinMode(Joystick_Button, INPUT_PULLUP);
  tft.begin(); // Display initalisieren
  tft.setRotation(3); // Display umdrehen. -- Oder 1 wenn das Display gedreht ist.
  Serial.print(blocked[spielfeld_breite][spielfeld_hoehe]);
}

//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Setup Ende-----------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Loop Anfang----------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void loop(void) // Code der wiederholt ausgeführt wird
{
      
  //Auslesen der X & Y Achse des Joysticks.
  int JoystickX = analogRead(Joystick_X);
  int JoystickY = analogRead(Joystick_Y);
  

  switch(game_status) 
  {
    case 0: // The game has not been started yet.
          wellcome_screen();
          game_status=1;
          break;

    case 1:
      if(digitalRead(Joystick_Button)==LOW) //Überprüfen ob gedrückt und Variable gleich 0. Wenn Variable 0 dann ist das Game noch nicht gestartet
      {
        game_screen(); // Game Screen starten
        game_status = 2;
        //Serial.println("Das Game starten ..."); // Überprüfen ob das Game gestartet wird

        //Variablen zurücksetzen --------------
        CursorX = 0; dx=1;
        CursorY = 0; dy=0;
        head=0;
        tail=0;
        counter=4;
        score=0;
        print_score();
        // ------------------------------------

        //Serial.println("Erasing board..."); //Löschen des Arrays. Sonst sind die Werte beim nächsten Spiel falsch
        for (int i=0;i<31;i++) {
          for (int j=0;j<20;j++) {
            blocked[i][j]=0;
          }
        }
        //Serial.println(blocked[0][0]);
        //Serial.println("Board erased...");
        food_generator();
      }

      break;
      
    case 2: // The game has been started!
    
      if(blocked[CursorX][CursorY]==1)//Überprüfen pb die Schlange schon an der Stelle war an der sie grade ist. -> Werte wo sie war werden im Array blocked gespeichert
      { 
        game_status=3; //I'm dying.... 
        if(score > highscore)
        {
          highscore = score;
        }
      }
      // ------------------ Schlange laufen lassen ------------------------------------------------------------------------------------------------------------------------------------------------------

      tft.drawRect(random_food_x,random_food_y, 10, 10, ILI9341_RED); // Zeichen des Apfels
      tft.drawRect(p2s_x(CursorX), p2s_y(CursorY), 10, 10, ILI9341_GREEN); // Draw Snake 
      tft.drawRect(random_food_x,random_food_y, 10, 10, ILI9341_RED); // Zeichen des Apfels
      blocked[CursorX][CursorY]=1; // Da war die Schlnage schon, wird in Array blocked gespeichert
        
      delay(125);

      position_x[head] = CursorX; //Wo die Schlange grade war (CursorX) wird in array position_x gespeichert. Diese Stelle wird immer eine Stelle hochgezählt um den alten wert nicht zu überschreiben.
      position_y[head] = CursorY; //Wo die Schlange grade war (CursorX) wird in array position_y gespeichert. Diese Stelle wird immer eine Stelle hochgezählt um den alten wert nicht zu überschreiben.
      head++; // Hochzählen des Wertes für die zwei oberen Befehle

      if(head >= maximum_snake_lenght) // Wenn head größer oder gleich maximum_snake_lenght-300 erreicht hat, wird head auf 0 gesetzt um neue Werte speichern zu können.
      {
        head=0;
      }

      move_tail();

      // ------------------ Schlange laufen lassen Ende ------------------------------------------------------------------------------------------------------------------------------------------------------
  
      //------Kolision bemerken und Spiel beenden Anfang ----------------


      CursorX+=dx;
      CursorY+=dy;
      
      if(CursorX == 31) // Überprüfen ob Schlange den rechten Rand berührt hat 
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }
      if(CursorX < 0) // Überprüfen ob Schlange den linken Rand berüht hat
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }
      if(CursorY == -1) // Überprüfen ob Schlange den oberen Rand berüht hat
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }
      if(CursorY == 20) // Überprüfen ob Schlange den unteren Rand berührt hat
      {
        game_status = 3;
        if(score > highscore)
        {
          highscore = score;
        }
      }

      //------Kolision bemerken und Spiel beenden Ende----------------
      
      //--------------------Essen gegessen? Anfang -----------------------------------

      if(random_generator_for_food_x == CursorX && random_generator_for_food_y == CursorY) // Überprüfen ob Schlange gegessen hat. Wenn ja dann:
      {
        food_generator(); // Aufrufen um einen Apfel an anderer Stelle spawnen zu lassen
        counter=1; // Schlange um 1 Block erhöhen
        score+=10; // Score um 10 erhöhen
        print_score(); // Score ausgeben
      }
      //--------------------Essen gegessen? Ende-- -----------------------------------

      //--------------Joystick Position bestimmen und schreiben ---------------------
   
      if(JoystickX>900)
      {
        //Serial.println("laufe nach rechts ");  
        dx=1;dy=0;     
       }   
      if(JoystickX<300)
      {
        //Serial.println("laufe nach links");
        dx=-1;dy=0;
      }
      if(JoystickY>900)
      {
        //Serial.println("laufe nach unten ");
        dx=0;dy=1;
      }
      if(JoystickY<300)
      {
        //Serial.println("laufe nach oben");
        dx=0;dy=-1;
      } 
      //--------------Joystick Position bestimmen und schreiben Ende----------------
      
      break;   

    case 3:
    {
      loser_screen(); // Lose Screen anzeigen 
      game_status=1; // Game Status auf 1 stellen. Der Lose Screen wird so lange angezeigt bis case 1 der Status des Buttons überprüft wird. Ob er gedrückt wurde.
      break;
    }
    
  }

}
 
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------Void Loop Ende------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
