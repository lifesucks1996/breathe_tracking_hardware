/**
 * @file ServicioEnEmisora.h
 * @brief Gestión de Servicios y Características GATT para BLE.
 * @author Rocio
 * @date 11/11/2025
 * @details Este archivo define las clases necesarias para crear un perfil GATT
 * personalizado, permitiendo la lectura, escritura y notificación de datos
 * a través de conexiones Bluetooth.
 */

#ifndef SERVICIO_EMISORA_H_INCLUIDO
#define SERVICIO_EMISORA_H_INCLUIDO

#include <vector>

/**
 * @brief Invierte el orden de los elementos en un array.
 * @tparam T Tipo de los elementos del array.
 * @param p Puntero al array.
 * @param n Número de elementos.
 * @return Puntero al array modificado.
 */
template< typename T >
T * alReves( T * p, int n ) {
  T aux;
  for( int i=0; i < n/2; i++ ) {
    aux = p[i];
    p[i] = p[n-i-1];
    p[n-i-1] = aux;
  }
  return p;
}

/**
 * @brief Convierte una cadena de texto a un array de uint8_t en orden inverso.
 * @details Útil para convertir strings de UUID a formato Little Endian requerido por la pila BLE.
 * @param pString Cadena de origen (C-string).
 * @param pUint Puntero al array de destino.
 * @param tamMax Tamaño máximo a copiar (normalmente 16 para UUIDs).
 * @return Puntero al array de destino.
 */
uint8_t * stringAUint8AlReves( const char * pString, uint8_t * pUint, int tamMax ) {
  int longitudString = strlen( pString );
  int longitudCopiar = ( longitudString > tamMax ? tamMax : longitudString );
  for( int i=0; i<=longitudCopiar-1; i++ ) {
    pUint[ tamMax-i-1 ] = pString[ i ];
  }
  return pUint;
}

/**
 * @class ServicioEnEmisora
 * @brief Clase que representa un Servicio GATT de Bluetooth.
 * @details Contiene una colección de características y gestiona su activación.
 */
class ServicioEnEmisora {

public:

  /**
   * @brief Alias para el callback invocado cuando un cliente escribe en una característica.
   */
  using CallbackCaracteristicaEscrita = void ( uint16_t conn_handle,
                         BLECharacteristic * chr,
                         uint8_t * data, uint16_t len); 

  /**
   * @class Caracteristica
   * @brief Representa una Característica GATT dentro de un servicio.
   */
  class Caracteristica {
  private:
    uint8_t uuidCaracteristica[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    BLECharacteristic laCaracteristica;

  public:
    /**
     * @brief Constructor básico de característica.
     * @param nombreCaracteristica_ UUID de la característica como string.
     */
    Caracteristica( const char * nombreCaracteristica_ )
      : laCaracteristica( stringAUint8AlReves( nombreCaracteristica_, &uuidCaracteristica[0], 16 ) )
    {}

    /**
     * @brief Constructor completo de característica.
     * @param nombreCaracteristica_ UUID en formato string.
     * @param props Propiedades (Read, Write, Notify...).
     * @param permisoRead Permisos de seguridad para lectura.
     * @param permisoWrite Permisos de seguridad para escritura.
     * @param tam Tamaño máximo de los datos en bytes.
     */
    Caracteristica( const char * nombreCaracteristica_ ,
            uint8_t props,
            SecureMode_t permisoRead,
            SecureMode_t permisoWrite, 
            uint8_t tam ) 
      : Caracteristica( nombreCaracteristica_ ) 
    {
      asignarPropiedadesPermisosYTamanyoDatos( props, permisoRead, permisoWrite, tam );
    }

  private:
    void asignarPropiedades ( uint8_t props ) { laCaracteristica.setProperties( props ); }
    void asignarPermisos( SecureMode_t permisoRead, SecureMode_t permisoWrite ) {
      laCaracteristica.setPermission( permisoRead, permisoWrite );
    }
    void asignarTamanyoDatos( uint8_t tam ) { laCaracteristica.setMaxLen( tam ); }

  public:
    /**
     * @brief Configura las propiedades, permisos y tamaño de la característica.
     */
    void asignarPropiedadesPermisosYTamanyoDatos( uint8_t props,
                             SecureMode_t permisoRead,
                             SecureMode_t permisoWrite, 
                             uint8_t tam ) {
      asignarPropiedades( props );
      asignarPermisos( permisoRead, permisoWrite );
      asignarTamanyoDatos( tam );
    }

    /**
     * @brief Escribe datos de forma local en la característica.
     * @param str Cadena a escribir.
     * @return Número de bytes escritos.
     */
    uint16_t escribirDatos( const char * str ) { return laCaracteristica.write( str ); }

    /**
     * @brief Envía una notificación a los clientes suscritos.
     * @param str Datos a notificar.
     * @return Resultado de la operación de notificación.
     */
    uint16_t notificarDatos( const char * str ) { return laCaracteristica.notify( str ); }

    /**
     * @brief Configura el callback de escritura.
     * @param cb Función a ejecutar cuando un cliente escribe datos.
     */
    void instalarCallbackCaracteristicaEscrita( CallbackCaracteristicaEscrita cb ) {
      laCaracteristica.setWriteCallback( cb );
    }

    /**
     * @brief Activa la característica para que sea visible por el servicio.
     */
    void activar() {
      err_t error = laCaracteristica.begin();
      Globales::elPuerto.escribir( "Char.begin() error: " );
      Globales::elPuerto.escribir( error );
    }

  }; // class Caracteristica
  
private:
  uint8_t uuidServicio[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
  BLEService elServicio;
  std::vector< Caracteristica * > lasCaracteristicas; ///< Lista de punteros a características vinculadas.

public:
  /**
   * @brief Crea un nuevo servicio BLE.
   * @param nombreServicio_ UUID del servicio como string.
   */
  ServicioEnEmisora( const char * nombreServicio_ )
  : elServicio( stringAUint8AlReves( nombreServicio_, &uuidServicio[0], 16 ) )
  {}
  
  /**
   * @brief Muestra el UUID del servicio por el puerto serie.
   */
  void escribeUUID() {
    Serial.println ( "**********" );
    for (int i=0; i<= 15; i++) { Serial.print( (char) uuidServicio[i] ); }
    Serial.println ( "\n**********" );
  }

  /**
   * @brief Añade una característica a la lista de este servicio.
   * @param car Referencia a la característica.
   */
  void anyadirCaracteristica( Caracteristica & car ) {
    lasCaracteristicas.push_back( & car );
  }

  /**
   * @brief Activa el servicio y todas sus características asociadas.
   */
  void activarServicio( ) {
    err_t error = elServicio.begin();
    Serial.print( "Service.begin() error: " );
    Serial.println( error );

    for( auto pCar : lasCaracteristicas ) {
      pCar->activar();
    }
  }

  /**
   * @brief Operador de conversión para compatibilidad con la librería Bluefruit.
   * @return Referencia al objeto BLEService interno.
   */
  operator BLEService&() {
    return elServicio;
  }
  
}; // class

#endif