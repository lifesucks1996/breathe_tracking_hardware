/**
 * @file EmisoraBLE.h
 * @brief Clase para gestionar la publicidad y conexión de una emisora Bluetooth Low Energy (BLE).
 * @author Rocio
 * @date 11/11/2025
 */

#ifndef EMISORA_H_INCLUIDO
#define EMISORA_H_INCLUIDO

#include "ServicioEnEmisora.h"

/**
 * @class EmisoraBLE
 * @brief Clase que abstrae las funciones de Bluefruit para actuar como periférico BLE.
 * * Permite emitir anuncios tipo iBeacon, emitir datos de sensores personalizados 
 * y gestionar servicios y características de forma dinámica.
 */
class EmisoraBLE {
private:

  const char * nombreEmisora; ///< Nombre que se mostrará en el escaneo BLE.
  const uint16_t fabricanteID; ///< ID del fabricante (Company ID) para anuncios.
  const int8_t txPower; ///< Potencia de transmisión en dBm.

public:

  /**
   * @brief Alias para el callback de conexión establecida.
   * @param connHandle Identificador de la conexión.
   */
  using CallbackConexionEstablecida = void ( uint16_t connHandle );

  /**
   * @brief Alias para el callback de conexión terminada.
   * @param connHandle Identificador de la conexión.
   * @param reason Código de razón de la desconexión.
   */
  using CallbackConexionTerminada = void ( uint16_t connHandle, uint8_t reason);

  /**
   * @brief Constructor de la clase EmisoraBLE.
   * @param nombreEmisora_ Nombre de la emisora.
   * @param fabricanteID_ ID de fabricante (ej. 0x004c para Apple).
   * @param txPower_ Potencia de emisión.
   */
  EmisoraBLE( const char * nombreEmisora_, const uint16_t fabricanteID_,
        const int8_t txPower_ ) 
  :
  nombreEmisora( nombreEmisora_ ) ,
  fabricanteID( fabricanteID_ ) ,
  txPower( txPower_ )
  {
  }

  /**
   * @brief Inicializa el hardware Bluefruit y detiene anuncios previos.
   */
  void encenderEmisora() {
    Bluefruit.begin(); 
    (*this).detenerAnuncio();
  }

  /**
   * @brief Enciende la emisora y configura los callbacks de conexión.
   * @param cbce Callback para cuando se establece una conexión.
   * @param cbct Callback para cuando se pierde una conexión.
   */
  void encenderEmisora( CallbackConexionEstablecida cbce,
            CallbackConexionTerminada cbct ) {
    encenderEmisora();
    instalarCallbackConexionEstablecida( cbce );
    instalarCallbackConexionTerminada( cbct );
  }

  /**
   * @brief Detiene la publicidad (advertising) si está activa.
   */
  void detenerAnuncio() {
    if ( (*this).estaAnunciando() ) {
      Bluefruit.Advertising.stop(); 
    }
  } 
  
  /**
   * @brief Comprueba si la emisora está emitiendo anuncios actualmente.
   * @return true si está anunciando, false en caso contrario.
   */
  bool estaAnunciando() {
    return Bluefruit.Advertising.isRunning();
  }

  /**
   * @brief Configura y emite un anuncio estándar iBeacon.
   * @param beaconUUID Puntero al array de 16 bytes del UUID.
   * @param major Valor Major del iBeacon.
   * @param minor Valor Minor del iBeacon.
   * @param rssi Valor de RSSI calibrado a 1 metro.
   */
  void emitirAnuncioIBeacon( uint8_t * beaconUUID, int16_t major, int16_t minor, uint8_t rssi ) {
    (*this).detenerAnuncio();
    
    BLEBeacon elBeacon( beaconUUID, major, minor, rssi );
    elBeacon.setManufacturer( (*this).fabricanteID );

    Bluefruit.setTxPower( (*this).txPower );
    Bluefruit.setName( (*this).nombreEmisora );
    Bluefruit.ScanResponse.addName(); 

    Bluefruit.Advertising.setBeacon( elBeacon );
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(100, 100); 

    Bluefruit.Advertising.start( 0 ); 
  }

  /**
   * @brief Emite un anuncio iBeacon con una carga de datos personalizada.
   * @param carga Puntero a los datos (máximo 21 bytes).
   * @param tamanyoCarga Tamaño de los datos a emitir.
   */
  void emitirAnuncioIBeaconLibre( const char * carga, const uint8_t tamanyoCarga ) {
    (*this).detenerAnuncio(); 

    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData(); 

    Bluefruit.setName( (*this).nombreEmisora );
    Bluefruit.ScanResponse.addName();
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

    uint8_t restoPrefijoYCarga[4+21] = {
      0x4c, 0x00, // companyID
      0x02,       // ibeacon type
      21,         // ibeacon length (dec=21)
      '-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-'
    };

    memcpy( &restoPrefijoYCarga[4], &carga[0], ( tamanyoCarga > 21 ? 21 : tamanyoCarga ) ); 

    Bluefruit.Advertising.addData( BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
                     &restoPrefijoYCarga[0],
                     4+21 );

    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(100, 100); 
    Bluefruit.Advertising.setFastTimeout( 1 ); 
    Bluefruit.Advertising.start( 0 ); 

    Globales::elPuerto.escribir( "emitiriBeacon libre Bluefruit.Advertising.start( 0 ); \n");
  }

  /**
   * @brief Emite datos múltiples en la carga de fabricante.
   * @details Útil para enviar tramas de sensores empaquetadas.
   * @param datos Puntero a los datos.
   * @param tamanyoDatos Longitud de los datos.
   */
  void emitirDatosMultiples(const uint8_t *datos, const uint8_t tamanyoDatos) {
    (*this).detenerAnuncio(); 

    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData(); 

    Bluefruit.setName( (*this).nombreEmisora );
    Bluefruit.ScanResponse.addName();
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

    uint8_t payload[2 + tamanyoDatos];
    payload[0] = (uint8_t)((*this).fabricanteID & 0xFF);
    payload[1] = (uint8_t)((*this).fabricanteID >> 8);

    memcpy( &payload[2], datos, tamanyoDatos ); 
    
    Bluefruit.Advertising.addData( BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
                    &payload[0],
                    2 + tamanyoDatos );

    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(100, 100);
    Bluefruit.Advertising.setFastTimeout( 1 );
    Bluefruit.Advertising.start( 0 ); 
  }

  /**
   * @brief Añade un servicio a la emisora.
   * @param servicio Referencia al objeto ServicioEnEmisora.
   * @return true si se añadió correctamente.
   */
  bool anyadirServicio( ServicioEnEmisora & servicio ) {
    bool r = Bluefruit.Advertising.addService( servicio );
    if ( ! r ) {
      Serial.println( " SERVICION NO AÑADIDO \n");
    }
    return r;
  }

  /**
   * @brief Añade un servicio y sus características.
   * @param servicio Referencia al servicio.
   * @return true si tuvo éxito.
   */
  bool anyadirServicioConSusCaracteristicas( ServicioEnEmisora & servicio ) { 
    return (*this).anyadirServicio( servicio );
  }

  /**
   * @brief Template recursivo para añadir un servicio con múltiples características.
   * @tparam T Tipos de las características restantes.
   * @param servicio Referencia al servicio.
   * @param caracteristica Primera característica a añadir.
   * @param restoCaracteristicas Pack de parámetros con las demás características.
   * @return true si tuvo éxito.
   */
  template <typename ... T>
  bool anyadirServicioConSusCaracteristicas( ServicioEnEmisora & servicio,
                         ServicioEnEmisora::Caracteristica & caracteristica,
                         T& ... restoCaracteristicas) {
    servicio.anyadirCaracteristica( caracteristica );
    return anyadirServicioConSusCaracteristicas( servicio, restoCaracteristicas... );
  }

  /**
   * @brief Template para añadir servicio, características y activar el servicio.
   * @tparam T Tipos de las características.
   * @param servicio Referencia al servicio.
   * @param restoCaracteristicas Pack de características.
   * @return true si tuvo éxito.
   */
  template <typename ... T>
  bool anyadirServicioConSusCaracteristicasYActivar( ServicioEnEmisora & servicio,
                             T& ... restoCaracteristicas) {
    bool r = anyadirServicioConSusCaracteristicas( servicio, restoCaracteristicas... );
    servicio.activarServicio();
    return r;
  }

  /**
   * @brief Instala el callback para conexiones establecidas.
   * @param cb Función callback.
   */
  void instalarCallbackConexionEstablecida( CallbackConexionEstablecida cb ) {
    Bluefruit.Periph.setConnectCallback( cb );
  }

  /**
   * @brief Instala el callback para desconexiones.
   * @param cb Función callback.
   */
  void instalarCallbackConexionTerminada( CallbackConexionTerminada cb ) {
    Bluefruit.Periph.setDisconnectCallback( cb );
  }

  /**
   * @brief Obtiene un puntero al objeto de conexión a partir de su handle.
   * @param connHandle Identificador de la conexión.
   * @return Puntero a BLEConnection.
   */
  BLEConnection * getConexion( uint16_t connHandle ) {
    return Bluefruit.Connection( connHandle );
  }

};

#endif
