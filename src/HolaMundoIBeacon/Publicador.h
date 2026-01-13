/**
 * @file Publicador.h
 * @brief Clase para la publicación de mediciones ambientales mediante beacons BLE.
 * @author Rocio
 * @date 11/11/2025
 * @details Esta clase actúa como un nivel de abstracción sobre EmisoraBLE, 
 * facilitando el envío de datos específicos (CO2, Temperatura) formateados 
 * como anuncios iBeacon.
 */

#ifndef PUBLICADOR_H_INCLUIDO
#define PUBLICADOR_H_INCLUIDO

/**
 * @class Publicador
 * @brief Gestiona la lógica de empaquetado y emisión de datos de sensores.
 * * Organiza las lecturas en el formato estándar de iBeacon, utilizando el
 * campo 'Major' para el ID de la medida y un contador, y el campo 'Minor' 
 * para el valor de la medición.
 */
class Publicador {

private:
  /** * @brief UUID del beacon (16 bytes). 
   * Representa la identidad de la red de sensores (EPSG-GTI-PROY-3A).
   */
  uint8_t beaconUUID[16] = { 
    'E', 'P', 'S', 'G', '-', 'G', 'T', 'I', 
    '-', 'P', 'R', 'O', 'Y', '-', '3', 'A'
  };

public:
  /** * @brief Instancia de la emisora BLE configurada para este publicador.
   * * Nombre: "rocio"
   * * Fabricante: Apple (0x004c)
   * * Potencia: 4 dBm
   */
  EmisoraBLE laEmisora {
    "rocio", 
    0x004c, 
    4 
  };
  
  /** @brief Valor de RSSI de referencia para el iBeacon. */
  const int RSSI = -53; 

public:

  /**
   * @enum MedicionesID
   * @brief Identificadores numéricos para los distintos tipos de sensores.
   */
  enum MedicionesID  {
    CO2 = 11,           ///< ID para sensores de Dióxido de Carbono.
    TEMPERATURA = 12,   ///< ID para sensores de Temperatura.
    RUIDO = 13          ///< ID para sensores de Ruido ambiental.
  };

  /**
   * @brief Constructor de la clase Publicador.
   * @note La emisora no se enciende aquí para evitar problemas de inicialización de hardware.
   */
  Publicador( ) {
  } 

  /**
   * @brief Activa el hardware de la emisora BLE.
   */
  void encenderEmisora() {
    (*this).laEmisora.encenderEmisora();
  } 

  /**
   * @brief Publica una medición de CO2.
   * @param valorCO2 Valor de la medición a enviar (se coloca en el campo Minor).
   * @param contador Valor incremental para diferenciar anuncios.
   * @param tiempoEspera Milisegundos que estará activo el anuncio antes de detenerse.
   * @details El campo 'Major' se forma combinando el ID del CO2 (8 bits altos) y el contador (8 bits bajos).
   */
  void publicarCO2( int16_t valorCO2, uint8_t contador,
          long tiempoEspera ) {

    // 1. Calculamos el campo Major (ID + Contador)
    uint16_t major = (MedicionesID::CO2 << 8) + contador;
    
    // 2. Emitimos el anuncio iBeacon
    (*this).laEmisora.emitirAnuncioIBeacon( (*this).beaconUUID, 
                      major,
                      valorCO2, // Minor
                      (*this).RSSI 
                  );

    // 3. Esperamos y detenemos
    esperar( tiempoEspera );
    (*this).laEmisora.detenerAnuncio();
  }

  /**
   * @brief Publica una medición de Temperatura.
   * @param valorTemperatura Valor térmico (se coloca en el campo Minor).
   * @param contador Valor incremental.
   * @param tiempoEspera Duración del anuncio en milisegundos.
   */
  void publicarTemperatura( int16_t valorTemperatura,
                uint8_t contador, long tiempoEspera ) {

    uint16_t major = (MedicionesID::TEMPERATURA << 8) + contador;
    
    (*this).laEmisora.emitirAnuncioIBeacon( (*this).beaconUUID, 
                      major,
                      valorTemperatura, // Minor
                      (*this).RSSI 
                  );
                  
    esperar( tiempoEspera );
    (*this).laEmisora.detenerAnuncio();
  } 
  
}; // class

#endif