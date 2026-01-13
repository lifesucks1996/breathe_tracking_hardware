/**
 * @file PuertoSerie.h
 * @brief Clase para gestionar la comunicación serie del microcontrolador.
 * @author Rocio
 * @date 11/11/2025
 * @details Proporciona una interfaz simplificada para el uso del objeto Serial
 * de Arduino, permitiendo inicializar la comunicación y escribir diversos tipos de datos.
 */

#ifndef PUERTO_SERIE_H_INCLUIDO
#define PUERTO_SERIE_H_INCLUIDO

/**
 * @class PuertoSerie
 * @brief Clase de abstracción para la comunicación serie.
 * * Facilita la inicialización del puerto y permite enviar mensajes de forma genérica
 * mediante el uso de plantillas (templates).
 */
class PuertoSerie  {

public:

  /**
   * @brief Constructor de la clase PuertoSerie.
   * @param baudios Velocidad de transmisión en bits por segundo (ej. 9600, 115200).
   * @details Inicia la comunicación serie con la velocidad especificada.
   */
  PuertoSerie (long baudios) {
    Serial.begin( baudios );
  }

  /**
   * @brief Bloquea la ejecución hasta que el puerto serie esté listo.
   * @details Es útil en placas con USB nativo donde la conexión serie puede tardar
   * unos milisegundos en establecerse tras el arranque.
   */
  void esperarDisponible() {
    while ( !Serial ) {
      delay(10);   
    }
  }

  /**
   * @brief Escribe un mensaje de cualquier tipo en el puerto serie.
   * @tparam T Tipo de dato del mensaje (int, float, char*, etc.).
   * @param mensaje El valor o cadena a enviar.
   * @details Utiliza una plantilla para delegar la gestión del tipo al método Serial.print() de Arduino.
   */
  template<typename T>
  void escribir (T mensaje) {
    Serial.print( mensaje );
  }
  
}; // class PuertoSerie

#endif