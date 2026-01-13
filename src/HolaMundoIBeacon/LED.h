/**
 * @file LED.h
 * @brief Clase para el control de un LED y funciones auxiliares de tiempo.
 * @author Rocio
 * @date 11/11/2025
 */

#ifndef LED_H_INCLUIDO
#define LED_H_INCLUIDO

/**
 * @brief Pausa la ejecución del programa durante un tiempo determinado.
 * @param tiempo Cantidad de milisegundos a esperar.
 * @details Es un envoltorio (wrapper) de la función estándar delay().
 */
void esperar (long tiempo) {
  delay (tiempo);
}

/**
 * @class LED
 * @brief Clase para gestionar un diodo LED conectado a un pin GPIO.
 * @details Permite realizar operaciones básicas como encender, apagar,
 * alternar el estado o realizar un parpadeo (brillar).
 */
class LED {
private:
  int numeroLED;    ///< Número del pin GPIO al que está conectado el LED.
  bool encendido;   ///< Estado interno del LED (true = encendido, false = apagado).

public:

  /**
   * @brief Constructor de la clase LED.
   * @param numero Número del pin donde se conecta el LED.
   * @details Configura el pin como salida (OUTPUT) y asegura que el LED 
   * comience en estado apagado.
   */
  LED (int numero)
  : numeroLED (numero), encendido(false)
  {
    pinMode(numeroLED, OUTPUT);
    apagar ();
  }

  /**
   * @brief Enciende el LED.
   * @details Pone el pin en nivel alto (HIGH) y actualiza el estado interno.
   */
  void encender () {
    digitalWrite(numeroLED, HIGH); 
    encendido = true;
  }

  /**
   * @brief Apaga el LED.
   * @details Pone el pin en nivel bajo (LOW) y actualiza el estado interno.
   */
  void apagar () {
    digitalWrite(numeroLED, LOW);
    encendido = false;
  }

  /**
   * @brief Cambia el estado actual del LED.
   * @details Si el LED está encendido lo apaga, y viceversa.
   */
  void alternar () {
    if (encendido) {
      apagar();
    } else {
      encender ();
    }
  }

  /**
   * @brief Enciende el LED durante un tiempo y luego lo apaga.
   * @param tiempo Duración del encendido en milisegundos.
   * @details Esta función es bloqueante, ya que utiliza la función esperar().
   */
  void brillar (long tiempo) {
    encender ();
    esperar(tiempo); 
    apagar ();
  }
}; // class

#endif