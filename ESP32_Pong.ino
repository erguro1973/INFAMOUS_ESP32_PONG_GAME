/*****************************************************************************************
 * INF@MOUS ESP32 PONG "based in fabGL" (Modo 8 Colores - Compatible con v1.0.9 + Core 2.0.14)
 *
 * ¡NUEVA RESOLUCIÓN: 640x480 @ 73Hz!
 * ¡CON VELOCIDAD INCREMENTAL!
 * ¡NUEVA FUNCIÓN: Control de Volumen por Software!
 *
 * * GUÍA DE MONTAJE (Modo 8 colores, 5 pines VGA):
 * --------------------------------------------------------------------------------------
 * --- VGA (Conector DB15) ---
 * ESP32 Pin 18 (HSync) -> VGA Pin 13 (HSync)
 * ESP32 Pin 5  (VSync) -> VGA Pin 14 (VSync)
 * *
 * ESP32 Pin 22 (Rojo)  -> Resistencia (ej: 330Ω-470Ω) -> VGA Pin 1 (Rojo)
 * ESP32 Pin 21 (Verde) -> Resistencia (ej: 330Ω-470Ω) -> VGA Pin 2 (Verde)
 * ESP32 Pin 19 (Azul)  -> Resistencia (ej: 330Ω-470Ω) -> VGA Pin 3 (Azul)
 * *
 * ESP32 GND            -> VGA Pines 5, 6, 7, 8, 10 (Todas las tierras)
 * *
 * --- CONTROLES ---
 * J1 (Poti)    -> ESP32 Pin 34
 * J2 (Poti)    -> ESP32 Pin 35
 * Botón Saque  -> ESP32 Pin 12
 * Poti Volumen -> ESP32 Pin 32 (¡NUEVO!)
 * *
 * --- SONIDO (ACTIVADO - MÉTODO v1.0.9) ---
 * Altavoz / Buzzer:
 * - Pin positivo (+)    -> ESP32 Pin 25
 * - Pin negativo (-)    -> GND
 * *****************************************************************************************/

// --- Librerías de FabGL ---
#include <fabgl.h>
#include <canvas.h>
// #include <fabutils.h> // No es necesario si SoundGenerator se incluye en fabgl.h

// --- Crear instancias de los controladores de FabGL ---
fabgl::VGAController VGAController;
fabgl::Canvas        Canvas(&VGAController); // Pasar el controlador VGA al Canvas
fabgl::SoundGenerator SoundGenerator;        // <-- Objeto de sonido ACTIVADO
fabgl::SquareWaveformGenerator squareWave; // <-- Generador de onda para los "beeps"

// --- Definiciones de Pines (Modo 8 Colores) ---
const int redPin     = 22; // VGA Rojo
const int greenPin   = 21; // VGA Verde
const int bluePin    = 19; // VGA Azul
const int hsyncPin   = 18; // VGA HSync
const int vsyncPin   = 5;  // VGA VSync
const int audioPin   = 25; // Altavoz/Buzzer (Activado)

// --- Pines de Controles ---
const int buttonOnePin = 12; // Botón de inicio/saque
const int potOnePin    = 34; // Potenciómetro Jugador 1 (Izquierda)
const int potTwoPin    = 35; // Potenciómetro Jugador 2 (Derecha)
const int potVolumePin = 32; // <-- ¡NUEVO! Potenciómetro de Volumen

// --- Constantes del Juego (Ajustadas para 640x480) ---
const int vgaWidth    = 640;
const int vgaHeight   = 480;
const int Pad_Length  = 80;
const int Pad_Width   = 10;
const int radius      = 6;
const int maxDeltaX   = 20;

// --- Colores (Enum de fabgl) ---
const int COLOR_BLACK   = 0;
const int COLOR_BLUE    = 1;
const int COLOR_GREEN   = 2;
const int COLOR_CYAN    = 3;
const int COLOR_RED     = 4;
const int COLOR_MAGENTA = 5;
const int COLOR_YELLOW  = 6;
const int COLOR_WHITE   = 7;

// --- Variables Globales del Juego ---
int cx, cy;
int cx0, cy0;
int deltaX, deltaY;

int potOnePosition;
int potTwoPosition;
int potOnePositionOld;
int potTwoPositionOld;

bool buttonOneStatus;

int gameVolume = 100; // <-- ¡NUEVO! Variable de Volumen (0-100)

int scoreL = 0;
int scoreR = 0;
char scoreText[10];

int ballStatus = 1;

// --- Variables para la Marcha Imperial (Extendida) ---
#define NOTE_A4   440
#define NOTE_F4   349
#define NOTE_C5   523
#define NOTE_A5   880
#define NOTE_F5   698
#define NOTE_G4S  415  // G#4
#define NOTE_G4   392
#define NOTE_DS4  311  // Eb4
#define NOTE_C4   262
#define NOTE_AS3  233  // Bb3
#define REST      0    // Silencio

int baseDuration = 350;

const int imperialNotes[] = { 
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5,
  NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
  REST,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_F5, NOTE_C5,
  NOTE_G4S, NOTE_F4, NOTE_C5, NOTE_A4,
  REST,
  NOTE_G4, NOTE_DS4, NOTE_C4, NOTE_AS3, NOTE_F4, NOTE_AS3, NOTE_F4, NOTE_C4,
  REST,
  NOTE_G4, NOTE_DS4, NOTE_C4, NOTE_AS3, NOTE_F4, NOTE_AS3, NOTE_F4, NOTE_C4,
  REST
};

const float imperialDurations[] = {
  1, 1, 1, 0.75, 0.25,
  1, 0.75, 0.25, 1,
  1, // Silencio
  1, 1, 1, 0.75, 0.25,
  1, 0.75, 0.25, 1,
  1, // Silencio
  1, 0.75, 0.25, 1, 0.75, 0.25, 1, 1,
  1, // Silencio
  1, 0.75, 0.25, 1, 0.75, 0.25, 1, 1,
  1  // Silencio final
};


const int melodyNoteCount = sizeof(imperialNotes) / sizeof(int);
int currentNote = 0;
unsigned long lastNoteTime = 0;
int currentNoteDuration = 0;


//_______________________________________________________________________________________________________
//                                           SETUP
//_______________________________________________________________________________________________________
void setup() {
  // Inicializa el controlador VGA (con 'cast' a gpio_num_t para v1.0.9)
  VGAController.begin((gpio_num_t)redPin, (gpio_num_t)greenPin, (gpio_num_t)bluePin, (gpio_num_t)hsyncPin, (gpio_num_t)vsyncPin);
  VGAController.setResolution(VGA_640x480_73Hz); // <-- CAMBIADO a 640x480 @ 73Hz

  // Configura los pines de entrada
  pinMode(buttonOnePin, INPUT_PULLUP);
  pinMode(potOnePin, INPUT);
  pinMode(potTwoPin, INPUT);
  pinMode(potVolumePin, INPUT); // <-- ¡NUEVO!

  // Selecciona una fuente (método v1.0.9)
  Canvas.selectFont(Canvas.getFontInfo()); // Fuente 8x8
  Canvas.setPenColor((fabgl::Color)COLOR_WHITE);

  // Dibuja el campo de juego
  Canvas.drawRectangle(0, 0, vgaWidth - 1, vgaHeight - 1);

  // Dibuja la línea central
  for (int i = 0; i < vgaHeight; i += 15) {
    Canvas.drawLine(vgaWidth / 2, i, vgaWidth / 2, i + 5);
  }

  // Muestra el mensaje de inicio (CENTRADO)
  const char * titleText = "INF@MOUS ESP32 PONG";
  const char * subtitleText = "Based on FabGL";
  Canvas.drawText(vgaWidth / 2 - (strlen(titleText) * 4), vgaHeight / 2 - 20, titleText);
  Canvas.setPenColor((fabgl::Color)COLOR_YELLOW);
  Canvas.drawText(vgaWidth / 2 - (strlen(subtitleText) * 4), vgaHeight / 2 + 10, subtitleText);

  // Muestra la puntuación inicial
  printScore();

  // Lee las posiciones iniciales de las paletas
  processInputs(); // <-- Lee el volumen por primera vez
  drawPad(Pad_Width / 2, potOnePosition, COLOR_GREEN);
  drawPad(vgaWidth - Pad_Width * 1.5, potTwoPosition, COLOR_CYAN);
  potOnePositionOld = potOnePosition;
  potTwoPositionOld = potTwoPosition;

  // --- NUEVO: Inicia la Marcha Imperial ---
  lastNoteTime = millis();
  currentNote = 0;
  currentNoteDuration = baseDuration * imperialDurations[0];
  SoundGenerator.playSound(squareWave, imperialNotes[0], currentNoteDuration, gameVolume); // <-- Volumen añadido
  currentNote = 1;
}


//_______________________________________________________________________________________________________
//                             NUEVA FUNCIÓN: Melodía de Intro
//_______________________________________________________________________________________________________
void playIntroMelody() {
  if (ballStatus == 0) {
    return;
  }
  unsigned long currentTime = millis();
  if (currentTime - lastNoteTime >= (currentNoteDuration + 50)) {
    int noteToPlay = imperialNotes[currentNote];
    currentNoteDuration = baseDuration * imperialDurations[currentNote];
    if (noteToPlay > 0) {
      SoundGenerator.playSound(squareWave, noteToPlay, currentNoteDuration, gameVolume); // <-- Volumen añadido
    }
    lastNoteTime = currentTime;
    currentNote = (currentNote + 1) % melodyNoteCount;
  }
}


//_______________________________________________________________________________________________________
//                                         MAIN LOOP
//_______________________________________________________________________________________________________
void loop() {
  playIntroMelody();
  
  processInputs(); // <-- Lee el volumen en cada loop

  if (ballStatus == 0) {
    drawBall(cx, cy, COLOR_YELLOW);
  }

  if (potOnePosition != potOnePositionOld) {
    drawPad(Pad_Width / 2, potOnePositionOld, COLOR_BLACK);
    drawPad(Pad_Width / 2, potOnePosition, COLOR_GREEN);
    potOnePositionOld = potOnePosition;
  }

  if (potTwoPosition != potTwoPositionOld) {
    drawPad(vgaWidth - Pad_Width * 1.5, potTwoPositionOld, COLOR_BLACK);
    drawPad(vgaWidth - Pad_Width * 1.5, potTwoPosition, COLOR_CYAN);
    potTwoPositionOld = potTwoPosition;
  }

  if ((buttonOneStatus == LOW) && (ballStatus > 0)) {
    SoundGenerator.play(false);
    ballStatus = 0;
    Canvas.setBrushColor((fabgl::Color)COLOR_BLACK);
    Canvas.fillRectangle(vgaWidth / 2 - 120, vgaHeight / 2 - 25, vgaWidth / 2 + 120, vgaHeight / 2 + 25);
    ballIni();
  }

  if (ballStatus == 0) {
    drawBall(cx0, cy0, COLOR_BLACK);
    ballPosition();
  }

  delay(10);
}


//_______________________________________________________________________________________________________
void processInputs() {
  potOnePosition = analogRead(potOnePin);
  potTwoPosition = analogRead(potTwoPin);
  gameVolume = map(analogRead(potVolumePin), 0, 4095, 0, 100); // <-- ¡NUEVO! Lee el volumen

  potOnePosition = map(potOnePosition, 0, 4095, Pad_Length / 2, vgaHeight - Pad_Length / 2);
  potTwoPosition = map(potTwoPosition, 0, 4095, Pad_Length / 2, vgaHeight - Pad_Length / 2);
  buttonOneStatus = digitalRead(buttonOnePin);
}

//_______________________________________________________________________________________________________
void drawBall(int cx, int cy, int myColor) {
  Canvas.setBrushColor((fabgl::Color)myColor);
  Canvas.fillRectangle(cx - radius, cy - radius, cx + radius - 1, cy + radius - 1);
}

//_______________________________________________________________________________________________________
void drawPad(int xPos, int potPosition, int myColor) {
  Canvas.setBrushColor((fabgl::Color)myColor);
  Canvas.fillRectangle(xPos, potPosition - Pad_Length / 2, xPos + Pad_Width, potPosition + Pad_Length / 2);
}

//_______________________________________________________________________________________________________
void ballPosition() {
  if (ballStatus != 0) {
    if (ballStatus == 1) {
      drawBall(cx, cy, COLOR_BLACK);
      cx = Pad_Width * 1.5 + radius + 1;
      cy = potOnePosition;
      drawBall(cx, cy, COLOR_YELLOW);
    } else {
      drawBall(cx, cy, COLOR_BLACK);
      cx = vgaWidth - Pad_Width * 1.5 - radius - 1;
      cy = potTwoPosition;
      drawBall(cx, cy, COLOR_YELLOW);
    }
    return;
  }

  cx0 = cx;
  cy0 = cy;
  cx = cx + deltaX;
  cy = cy + deltaY;

  // Colisión con la Paleta 2 (Derecha)
  if (cx > (vgaWidth - Pad_Width * 1.5 - radius) && (cy > potTwoPosition - Pad_Length / 2) && (cy < potTwoPosition + Pad_Length / 2)) {
    cx = cx0;
    deltaX = -deltaX;
    if (deltaX > 0) {
      if (deltaX < maxDeltaX) deltaX += 1;
    } else {
      if (deltaX > -maxDeltaX) deltaX -= 1;
    }
    deltaY += int((cy - potTwoPosition) / (Pad_Length / 6));
    SoundGenerator.playSound(squareWave, 440, 50, gameVolume); // <-- Volumen añadido
  }

  // Colisión con la Paleta 1 (Izquierda)
  if ((cx < (Pad_Width * 1.5 + radius)) && (cy > potOnePosition - Pad_Length / 2) && (cy < potOnePosition + Pad_Length / 2)) {
    cx = cx0;
    deltaX = -deltaX;
    if (deltaX > 0) {
      if (deltaX < maxDeltaX) deltaX += 1;
    } else {
      if (deltaX > -maxDeltaX) deltaX -= 1;
    }
    deltaY += int((cy - potOnePosition) / (Pad_Length / 6));
    SoundGenerator.playSound(squareWave, 440, 50, gameVolume); // <-- Volumen añadido
  }

  // Punto para Jugador 2 (Pared izquierda)
  if (cx < radius) {
    scoreR += 1;
    printScore();
    ballStatus = 2;
    deltaX = 0;
    deltaY = 0;
    SoundGenerator.playSound(squareWave, 880, 150, gameVolume); // <-- Volumen añadido
  }

  // Punto para Jugador 1 (Pared derecha)
  if (cx > vgaWidth - radius) {
    scoreL += 1;
    printScore();
    ballStatus = 1;
    deltaX = 0;
    deltaY = 0;
    SoundGenerator.playSound(squareWave, 880, 150, gameVolume); // <-- Volumen añadido
  }

  // Ganar
  if ((scoreL > 9) || (scoreR > 9)) {
    gameOver();
  }

  // Colisión con techo/suelo
  if ((cy > vgaHeight - radius) || (cy < radius)) {
    cy = cy0;
    deltaY = -deltaY;
    SoundGenerator.playSound(squareWave, 220, 30, gameVolume); // <-- Volumen añadido
  }

  // Límite de velocidad vertical (Escalado)
  if (deltaY > 6) deltaY = 6;
  if (deltaY < -6) deltaY = -6;
}

//_______________________________________________________________________________________________________
void ballIni() {
  drawBall(cx, cy, COLOR_BLACK);
  cx = vgaWidth / 2;
  cy = vgaHeight / 2;
  deltaX = 5; // Velocidad base escalada para 640x480
  deltaY = random(-4, 5); // Ángulo escalado

  if (ballStatus == 2) {
    deltaX = -deltaX;
  }

  SoundGenerator.playSound(squareWave, 660, 70, gameVolume); // <-- Volumen añadido
}

//_______________________________________________________________________________________________________
void printScore() {
  sprintf(scoreText, "%d | %d", scoreL, scoreR);
  Canvas.setBrushColor((fabgl::Color)COLOR_BLACK);
  Canvas.fillRectangle(vgaWidth / 2 - 40, 10, vgaWidth / 2 + 40, 18);
  Canvas.setPenColor((fabgl::Color)COLOR_WHITE);
  Canvas.drawText(vgaWidth / 2 - (strlen(scoreText) * 4), 10, scoreText);
}

//_______________________________________________________________________________________________________
void gameOver() {
  Canvas.setPenColor((fabgl::Color)COLOR_YELLOW);
  const char * gano1Text = "Jugador 1 GANA!";
  const char * gano2Text = "Jugador 2 GANA!";
  
  if (scoreL > 9) {
    Canvas.drawText(vgaWidth / 2 - (strlen(gano1Text) * 4), vgaHeight / 2, gano1Text);
  } else {
    Canvas.drawText(vgaWidth / 2 - (strlen(gano2Text) * 4), vgaHeight / 2, gano2Text);
  }

  delay(3000);

  Canvas.setBrushColor((fabgl::Color)COLOR_BLACK);
  Canvas.fillRectangle(vgaWidth / 2 - (strlen(gano2Text) * 4) - 5 , vgaHeight / 2, vgaWidth / 2 + (strlen(gano2Text) * 4) + 5, vgaHeight / 2 + 10);

  scoreL = 0;
  scoreR = 0;
  printScore();

  ballStatus = 1;

  // Vuelve a mostrar el mensaje de inicio (CENTRADO)
  const char * subtitleText = "INF@MOUS ESP32 PONG";
  Canvas.setPenColor((fabgl::Color)COLOR_YELLOW);
  Canvas.drawText(vgaWidth / 2 - (strlen(subtitleText) * 4), vgaHeight / 2 + 10, subtitleText);
  
  // --- NUEVO: Reinicia la Marcha Imperial ---
  lastNoteTime = millis();
  currentNote = 0;
  currentNoteDuration = baseDuration * imperialDurations[0];
  SoundGenerator.playSound(squareWave, imperialNotes[0], currentNoteDuration, gameVolume); // <-- Volumen añadido
  currentNote = 1;
}
