#include <WiFi.h>
#include "Adafruit_ILI9341.h"



String players[20] = {"","","","","","","","","","","","","","","","","","","",""};


// ____  _____ _____   _   _ ____    _____ _____ _____
/// ___|| ____|_   _| | | | |  _ \  |_   _|  ___|_   _|
//\___ \|  _|   | |   | | | | |_) |   | | | |_    | |
// ___) | |___  | |   | |_| |  __/    | | |  _|   | |
//|____/|_____| |_|    \___/|_|       |_| |_|     |_|
//

// Defining Pins
#define TFT_DC 16
#define TFT_CS 0
#define TFT_MOSI 17
#define TFT_CLK 5
#define TFT_RST 4
#define TFT_MISO 19

#define TFT_LED 18

#define UP 23
#define DOWN 22
#define SELECT 32

// Create display interface opject
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// ____  _   _ _____ _____ ___  _   _
//| __ )| | | |_   _|_   _/ _ \| \ | |
//|  _ \| | | | | |   | || | | |  \| |
//| |_) | |_| | | |   | || |_| | |\  |
//|____/ \___/  |_|   |_| \___/|_| \_|
//

class Button {
  public:
  int pin;
  boolean state;
  boolean lastState;

  Button(int pin) {
    this->pin = pin;
    this->state = false;
    this->lastState;
  }

  boolean scan() {
    lastState = state;
    state = !digitalRead(pin);
    return (!lastState && state);
  }
};

Button butts[] = {Button(UP), Button(DOWN), Button(SELECT)};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//  ___  ____ _____ ___ ___  _   _
// / _ \|  _ \_   _|_ _/ _ \| \ | |
//| | | | |_) || |  | | | | |  \| |
//| |_| |  __/ | |  | | |_| | |\  |
// \___/|_|    |_| |___\___/|_| \_|
//

class Option {

  public: 
  int funcVal;
  String title;
  
  Option(String title = "-", int funcVal = 0) {
    this->funcVal = funcVal;
    this->title = title;
  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//  ____ ___  __  __ __  __    _    _   _ ____    _____ _   _ _   _  ____ _____
// / ___/ _ \|  \/  |  \/  |  / \  | \ | |  _ \  |  ___| | | | \ | |/ ___|_   _|
//| |  | | | | |\/| | |\/| | / _ \ |  \| | | | | | |_  | | | |  \| | |     | |
//| |__| |_| | |  | | |  | |/ ___ \| |\  | |_| | |  _| | |_| | |\  | |___  | |
// \____\___/|_|  |_|_|  |_/_/   \_\_| \_|____/  |_|    \___/|_| \_|\____| |_|
//

void commandOnPlayer(String prefix, String postfix = "") {
  getUsers();

  for(int i = 0; i < 20; i++) {
    if(players[i] == "") {break;}
    Serial.println(players[i]);
  }

  Option pages[21];
  pages[0] = Option("Back", 0);
  for(int i = 0; i < 20; i++) {
    if (players[i] != "") {
      pages[i+1] = Option(players[i], 0);
    } else {
      pages[i+1] = Option("-", 0);
    }
  }
  
  String player = menu(pages, 21, true);

  commandSend("minecraftd command " + prefix + " " + player + " " + postfix);
  
  clearText();
}


void getUsers() {
  String users = commandSend("minecraftd command list");
  users = users.substring(users.indexOf(":")+2, users.indexOf("\r"));

  int arrayIndex = 0;
  int index = 0;
  while(1 == 1) {
    index = users.indexOf(",");
    if(index > -1) {
      players[arrayIndex] = users.substring(0,index);
      users = users.substring(index+2);
      arrayIndex++;
    } else {
      players[arrayIndex] = users;
      players[arrayIndex+1] = "";
      break;
    }
  }
}


String commandSend(String command) {
  String message = pingSocket(command);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(5,200);
  tft.print("                                                 ");
  tft.setCursor(5,200);
  tft.print(message.substring(message.indexOf('\n') + 34));
  return message.substring(message.indexOf('\n') + 34);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

boolean back;

// _____ _   _ _   _  ____   _     ___ ____ _____
//|  ___| | | | \ | |/ ___| | |   |_ _/ ___|_   _|
//| |_  | | | |  \| | |     | |    | |\___ \ | |
//|  _| | |_| | |\  | |___  | |___ | | ___) || |
//|_|    \___/|_| \_|\____| |_____|___|____/ |_|
//

void runFunc(int num) {
  if(num == 0) {
    Option options[] = {Option("Back",0), Option("day", 0), Option("night", 0)};
    commandSend("minecraftd command time set " + menu(options, 3, true));
    clearText();
    }
  if(num == 1) {commandSend("minecraftd reset");}
  if(num == 2) {
    Option options[] = {Option("Back",0), Option("clear", 0), Option("rain", 0), Option("thunder", 0)};
    commandSend("minecraftd command weather " + menu( options, 4, true ));
    clearText();
    }
    
  if(num == 3) {
    Option options[] = {Option("Back",0), Option("creative",0), Option("survival",0), Option("spectator",0)};
    String gamemode = menu(options, 4, true);
    if( gamemode == "Back") {
      clearText();
      return;
    }
    commandOnPlayer("gamemode " + gamemode);
    clearText();
    
  }
  
  if(num == 4) {commandOnPlayer("kick");}
  if(num == 5) {commandOnPlayer("op");}
  if(num == 6) {commandOnPlayer("deop");}
  if(num == 7) {commandOnPlayer("kill");}
  if(num == -1) {back = true;}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// __  __ _____ _   _ _   _
//|  \/  | ____| \ | | | | |
//| |\/| |  _| |  \| | | | |
//| |  | | |___| |\  | |_| |
//|_|  |_|_____|_| \_|\___/
//

void clearText() {
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);
  for(int i = 0; i < 8; i++) {
        tft.setCursor(100,20+(16*i));
        tft.print("            ");
      }
}

String menu(Option options[], int arraySize, boolean returnName) {
  int index = 0;
  tft.setTextSize(2);
  clearText();
  back = false;
  int page = 0;
  while (back == false) {
    
    if(floor(index / 8) != page) {
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      for(int i = 0; i < 8; i++) {
        tft.setCursor(100,20+(16*i));
        tft.print("            ");
      }
    }
    
    page = int(floor(index / 8));
    
    for(int i = 0; i < 8; i++) {
      if(i+8*page >= arraySize) {
        continue;
      }
      if (i+8*floor(index / 8) == index) {
        tft.setTextColor(ILI9341_GREEN, ILI9341_WHITE);
      } else {
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      }
      
      tft.setCursor(100,20+(16*i));
      tft.print(options[int(i+8*floor(index / 8))].title);
    }
    
    boolean any = false;
    while(!any) {
      for (int i = 0; i < 3; i++) {
        any = butts[i].scan() || any;
      }
    }
    if(butts[1].state) {
      index -= 1;
      if(index < 0) {
        index = 0;
      }
    }
    
    if(butts[0].state) {
      index += 1;
      if(index >= arraySize) {
        index = arraySize - 1;
      }
    }
    if(butts[2].state) {
      if (returnName) {
        return options[index].title;
      }
      Serial.println("pushed");
      tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
      tft.setCursor(100,20+(16*(index- floor(index / 8)*8 )));
      tft.println(options[index].title);
      
      runFunc(options[index].funcVal);
      tft.setTextSize(2);
    }
    
  }
  return "";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//__        _____ _____ ___    ____ ___  _   _ _____
//\ \      / /_ _|  ___|_ _|  / ___/ _ \| \ | |  ___|
// \ \ /\ / / | || |_   | |  | |  | | | |  \| | |_
//  \ V  V /  | ||  _|  | |  | |__| |_| | |\  |  _|
//   \_/\_/  |___|_|   |___|  \____\___/|_| \_|_|
//

// Configuration for Wifi connection and TCP Socket
const char*     ssid      = "EZConnect";
const char* password =  "HackBerryPlay";
const uint16_t  port      = 9999;
const char*     host      = "10.38.8.56";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// ____  _____ _____ _   _ ____
/// ___|| ____|_   _| | | |  _ \
//\___ \|  _|   | | | | | | |_) |
// ___) | |___  | | | |_| |  __/
//|____/|_____| |_|  \___/|_|
//

// Connect to WiFi and initialize display
void setup() {
  tft.begin();
  Serial.begin(115200);
  Serial.print("Connecting to wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("/nConnected!");

// Start and prepare display
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(3);

  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(SELECT, INPUT_PULLUP);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// _     ___   ___  ____
//| |   / _ \ / _ \|  _ \
//| |  | | | | | | | |_) |
//| |__| |_| | |_| |  __/
//|_____\___/ \___/|_|
//

void loop() {
  Option pages[] = {Option("Set time",0), Option("Weather", 2), Option("Gamemode", 3), Option("Kick",4), Option("Op", 5), Option("DeOp", 6), Option("Kill", 7), Option("Reset",1)};
  menu(pages, 8, false);
  //String newData = pingSocket("minecraftd command list");
  //Serial.println(newData.substring(39));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// ____ ___ _   _  ____ ____   ___   ____ _  _______ _____
//|  _ \_ _| \ | |/ ___/ ___| / _ \ / ___| |/ / ____|_   _|
//| |_) | ||  \| | |  _\___ \| | | | |   | ' /|  _|   | |
//|  __/| || |\  | |_| |___) | |_| | |___| . \| |___  | |
//|_|  |___|_| \_|\____|____/ \___/ \____|_|\_\_____| |_|
//

// Function to connect to TCP socket and receive data
String pingSocket(String message) {
  WiFiClient client;
  if (!client.connect(host, port)) {
    delay(1000);
    return "Connection\nFailure";
  }
  client.println(message);
  delay(1000);
  String data = client.readStringUntil('\t');
  client.stop();
  return data;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
