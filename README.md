
# INF@MOUS ESP32 PONG GAME

This is a classic Pong project for the ESP32, using the incredible FabGL library to generate VGA video and sound. The game is controlled by two potentiometers (one for each player) and a button to serve.

The code is configured for a 320x240 @ 60Hz resolution.

![Add a photo or GIF of your game working here!]

## Credits and Acknowledgements

This project would not be possible without the amazing work of **Fabrizio Di Vittorio**. All credit for the VGA video generation, sound, and peripheral management goes to his [**FabGL** library](http://www.fabglib.org/).

This repository simply implements the Pong game logic on top of that powerful library.

## Compilation Requirements (Important!)

This code has been debugged to work with a very specific combination of libraries and ESP32 core versions, as detailed in [this notice](http://www.fabglib.org/):

* **Board:** ESP32 (Any model, e.g., ESP32 DEVKIT V1).
* **FabGL Library:** Version `v1.0.9` (the latest official "release").
* **ESP32 Boards Package:** Version `2.0.14` or `2.0.17`. **Do not use v3.x.x**, as it is not compatible with FabGL v1.0.9 and will cause compilation errors in the library's internal files (`ps2controller.cpp`, etc.).

## Connections (Pinout)

## Connections (Pinout)

This project uses the 8-color VGA mode (5 pins) to simplify the wiring.

| Component | ESP32 Pin | Connection |
| :--- | :--- | :--- |
| **VGA SIGNAL** | | |
| HSync | `GPIO 18` | VGA Pin 13 |
| VSync | `GPIO 5` | VGA Pin 14 |
| Red | `GPIO 22` | Resistor (330Ω-470Ω) -> VGA Pin 1 |
| Green | `GPIO 21` | Resistor (330Ω-470Ω) -> VGA Pin 2 |
| Blue | `GPIO 19` | Resistor (330Ω-470Ω) -> VGA Pin 3 |
| Ground | `GND` | VGA Pins 5, 6, 7, 8, 10 |
| | | |
| **CONTROLS** | | |
| P1 (Pot B10k) | `GPIO 34` | Center pin (Wiper). The other two to 3.3V & GND. |
| P2 (Pot B10k) | `GPIO 35` | Center pin (Wiper). The other two to 3.3V & GND. |
| Serve Button | `GPIO 12` | One side of the button. The other side to `GND`. |
| | | |
| **SOUND** | | |
| Speaker | `GPIO 25` | Positive (+) pin of the buzzer/speaker. |
| Ground | `GND` | Negative (-) pin of the buzzer/speaker. |



# INF@MOUS ESP32 PONG GAME

Este es un proyecto clásico de Pong para el ESP32, utilizando la increíble librería FabGL para generar video VGA y sonido. El juego está controlado por dos potenciómetros (uno para cada jugador) y un botón para el saque.

El código está configurado para una resolución de 320x240 @ 60Hz.

![¡Añade aquí una foto o un GIF de tu juego funcionando!]

## Créditos y Agradecimientos

Este proyecto no sería posible sin el increíble trabajo de **Fabrizio Di Vittorio**. Todo el mérito de la generación de vídeo VGA, el sonido y la gestión de periféricos es de su librería [**FabGL**](http://www.fabglib.org/).

Este repositorio simplemente implementa la lógica del juego Pong sobre esa potente librería.

## Requisitos de Compilación (¡Importante!)

Este código ha sido depurado para funcionar con una combinación muy específica de librerías y versiones del core de ESP32, como se detalla en [este aviso](http://www.fabglib.org/):

* **Placa:** ESP32 (Cualquier modelo, ej: ESP32 DEVKIT V1).
* **Librería FabGL:** Versión `v1.0.9` (la última "release" oficial).
* **Paquete de Placas ESP32:** Versión `2.0.14` o `2.0.17`. **No uses v3.x.x**, ya que no es compatible con FabGL v1.0.9 y causará errores de compilación en los archivos internos de la librería (`ps2controller.cpp`, etc.).

## Conexiones (Pinout)

Este proyecto utiliza el modo VGA de 8 colores (5 pines) para simplificar el montaje.

| Componente | Pin ESP32 | Conexión |
| :--- | :--- | :--- |
| **SEÑAL VGA** | | |
| HSync | `GPIO 18` | VGA Pin 13 |
| VSync | `GPIO 5` | VGA Pin 14 |
| Rojo | `GPIO 22` | Resistencia (330Ω-470Ω) -> VGA Pin 1 |
| Verde | `GPIO 21` | Resistencia (330Ω-470Ω) -> VGA Pin 2 |
| Azul | `GPIO 19` | Resistencia (330Ω-470Ω) -> VGA Pin 3 |
| Tierra | `GND` | VGA Pines 5, 6, 7, 8, 10 |
| | | |
| **CONTROLES** | | |
| J1 (Pot B10k) | `GPIO 34` | Pin central (Wiper). El otro a 3.3V y GND. |
| J2 (Pot B10k) | `GPIO 35` | Pin central (Wiper). El otro a 3.3V y GND. |
| Botón Saque | `GPIO 12` | Un lado del botón. El otro a `GND`. |
| | | |
| **SONIDO** | | |
| Altavoz | `GPIO 25` | Pin positivo (+) del buzzer/altavoz. |
| Tierra | `GND` | Pin negativo (-) del buzzer/altavoz. |

## Cómo Empezar

1.  **Instala el Entorno Correcto:**
    * Abre el Arduino IDE.
    * Ve a `Herramientas > Placa > Gestor de Placas...` e instala el paquete `esp32` versión **2.0.14** (o 2.0.17).
    * Ve a `Herramientas > Gestor de Librerías...` e instala `FabGL` versión **1.0.9**.
    * Reinicia el Arduino IDE.
2.  **Conecta el Hardware:** Monta el circuito siguiendo la tabla de `Pinout` de arriba.
3.  **Compila y Sube:** Abre el archivo `ESP32_Pong.ino`, selecciona tu placa ESP32 (ej: "ESP32 Dev Module") y el puerto COM correcto.
4.  **¡A Jugar!**
