#include <LiquidCrystal.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Botões conectados aos pinos
const int btnUp = 6;
const int btnDown = 7;
const int btnSelect = 8;
const int btnCancel = 9;

// LEDs conectados aos pinos 
const int ledPins[4] = {10, 11, A0, 13};
String ledNames[4] = {"Amarelo", "Azul", "Vermelho", "Verde"};

// Estados dos LEDs
bool blinking[4] = {false, false, false, false};        // ligado/desligado
int blinkInterval[4] = {500, 500, 500, 500};            // Intervalo de pisca (em ms)
unsigned long lastToggle[4] = {0, 0, 0, 0};             // Último tempo que o LED trocou de estado
bool ledState[4] = {false, false, false, false};        // Estado atual de cada LED (ligado/desligado)

// Controle de navegação no menu
int menuLevel = 0;      // 0 = principal, 1 = submenu LED, 2 = ajuste de velocidade
int currentItem = 0;    // Item selecionado no menu atual
int selectedLed = 0;    // LED atualmente selecionado

// Controle de debounce (evita múltiplas leituras do botão)
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  lcd.begin(16, 2);
  lcd.print("Inicializando...");
  delay(1000);
  lcd.clear();

  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  pinMode(btnSelect, INPUT_PULLUP);
  pinMode(btnCancel, INPUT_PULLUP);

  showMenu();
}

void loop() {
  // Verifica os botões pressionados
  handleButtons();
  
  // Atualiza o estado dos LEDs que estão piscando
  handleBlinking();
}

void handleButtons() {
  // Verifica se tempo de debounce passou
  if (millis() - lastButtonPress < debounceDelay) return;

  bool buttonPressed = false;

  // Botão para cima
  if (digitalRead(btnUp) == LOW) {
    currentItem--;
    if (menuLevel == 0 && currentItem < 0) currentItem = 3;
    if ((menuLevel == 1 || menuLevel == 2) && currentItem < 0) currentItem = 1;
    buttonPressed = true;
  } 
  
  // Botão para baixo
  else if (digitalRead(btnDown) == LOW) {
    currentItem++;
    if (menuLevel == 0 && currentItem > 3) currentItem = 0;
    if ((menuLevel == 1 || menuLevel == 2) && currentItem > 1) currentItem = 0;
    buttonPressed = true;
  } 
  
  // Botão de seleção (OK)
  else if (digitalRead(btnSelect) == LOW) {
    if (menuLevel == 0) {
      // Entrou no submenu de um LED
      selectedLed = currentItem;
      currentItem = 0;
      menuLevel = 1;
    } else if (menuLevel == 1) {
      // Opção 0: Liga/Desliga pisca | Opção 1: Vai para ajuste de velocidade
      if (currentItem == 0) {
        blinking[selectedLed] = !blinking[selectedLed];
        if (!blinking[selectedLed]) {
          digitalWrite(ledPins[selectedLed], LOW);
          ledState[selectedLed] = false;
        }
      } else {
        menuLevel = 2;
        currentItem = 0;
      }
    } else if (menuLevel == 2) {
      // Ajuste da velocidade de pisca
      if (currentItem == 0) {
        blinkInterval[selectedLed] += 50; // Piscar mais devagar
      } else {
        // Piscar mais rápido (mínimo 50ms)
        blinkInterval[selectedLed] = max(50, blinkInterval[selectedLed] - 50);
      }
    }
    buttonPressed = true;
  } 
  
  // Botão de cancelamento (volta ao menu anterior)
  else if (digitalRead(btnCancel) == LOW) {
    if (menuLevel > 0) {
      menuLevel--;
      currentItem = (menuLevel == 0) ? selectedLed : 0;
      lcd.clear(); // Limpa tela ao voltar
    }
    buttonPressed = true;
  }

  // Se algum botão foi pressionado, atualiza menu e registra tempo
  if (buttonPressed) {
    showMenu();
    lastButtonPress = millis();
  }
}

// Atualiza o display LCD de acordo com o nível do menu atual
void showMenu() {
  lcd.clear();

  switch (menuLevel) {
    case 0: // Menu principal - lista os LEDs
      lcd.setCursor(0, 0);
      lcd.print("> ");
      lcd.print(ledNames[currentItem]);

      lcd.setCursor(0, 1);
      lcd.print("  ");
      lcd.print(ledNames[(currentItem + 1) % 4]);
      break;

    case 1: // Submenu do LED selecionado
      lcd.setCursor(0, 0);
      lcd.print("> ");
      lcd.print(currentItem == 0 ? (blinking[selectedLed] ? "Desl. Pisca" : "Lig. Pisca") : "Velocidade");

      lcd.setCursor(0, 1);
      lcd.print("  ");
      lcd.print(currentItem == 0 ? "Velocidade" : (blinking[selectedLed] ? "Desl. Pisca" : "Lig. Pisca"));
      break;

    case 2: // Submenu de velocidade do pisca
      lcd.setCursor(0, 0);
      lcd.print("> ");
      lcd.print(currentItem == 0 ? "Piscar + devagar" : "Piscar + rapido");

      lcd.setCursor(0, 1);
      lcd.print("Atual: ");
      lcd.print(blinkInterval[selectedLed]);
      lcd.print("ms");
      break;
  }
}

// Controla o pisca-pisca dos LEDs
void handleBlinking() {
  unsigned long currentMillis = millis();

  for (int i = 0; i < 4; i++) {
    // Se o LED está em modo pisca e passou o intervalo de tempo
    if (blinking[i] && (currentMillis - lastToggle[i] >= blinkInterval[i])) {
      ledState[i] = !ledState[i]; // alterna estado (ligado/desligado)
      digitalWrite(ledPins[i], ledState[i]);
      lastToggle[i] = currentMillis;
    }
  }
}
