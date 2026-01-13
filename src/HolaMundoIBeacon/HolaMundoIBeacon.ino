// --------------------------------------------------------------
/**
 * @file HolaMundoIBeacon.ino
 * @brief Sistema de monitorización ambiental (O3, CO2, Temperatura) con tecnología BLE.
 * @author Rocio
 * @date 11/11/2025
 * @details 
 * **Historial de cambios:**
 * - 26/11/25: Implementación de lectura de sensores y gestión de batería.
 * - 09/01/26: Adaptación completa de comentarios para generación con Doxygen.
 * * Este programa gestiona la adquisición de datos de sensores de gas (Ozono), 
 * niveles de CO2, temperatura y estado de carga de batería, emitiendo dicha 
 * información mediante anuncios Bluetooth Low Energy (Beacons personalizados).
 */
// --------------------------------------------------------------

#include <bluefruit.h>

// Evitar conflictos con macros de Arduino
#undef min 
#undef max 

#include "LED.h"
#include "PuertoSerie.h"

/**
 * @namespace Globales
 * @brief Espacio de nombres para objetos con persistencia global.
 * @details Contiene las instancias de hardware y servicios compartidos por todo el sistema.
 */
namespace Globales {
  /// Instancia para el control del LED de estado (conectado al pin 7).
  LED elLED (7);

  /// Instancia para la comunicación serie (configurada a 115200 baudios).
  PuertoSerie elPuerto (115200); 
}

#include "EmisoraBLE.h"
#include "Publicador.h"
#include "Medidor.h"

namespace Globales {
  /// Objeto encargado de gestionar la emisión de anuncios BLE.
  Publicador elPublicador;

  /// Objeto encargado de realizar las mediciones de los sensores analógicos y digitales.
  Medidor elMedidor;
}

/**
 * @brief Configura los pines de entrada analógica.
 * @details Establece los modos de pin para la lectura del sensor de Ozono 
 * (VGAS y VREF) y el divisor de tensión de la batería.
 */
void inicializarPlaquita () {
  pinMode(O3_PIN_VGAS, INPUT); // A5 por defecto
  pinMode(O3_PIN_VREF, INPUT); // A4 por defecto
  pinMode(PIN_A6, INPUT);      // Pin de monitorización de Batería
}

/**
 * @brief Función de configuración inicial (Arduino Setup).
 * @details Inicializa periféricos, establece la semilla aleatoria para simulaciones,
 * arranca la emisora BLE y realiza la calibración inicial del sensor de gas.
 */
void setup() {
  // Inicialización de hardware
  inicializarPlaquita(); 

  // Semilla para valores aleatorios (usada en mediciones simuladas)
  randomSeed(analogRead(0)); 

  // Activación del servicio BLE
  Globales::elPublicador.encenderEmisora();

  // Inicialización y calibración del medidor de gas
  Globales::elMedidor.iniciarMedidor();
  esperar( 1000 );

  float vref_calibrado = Globales::elMedidor.getVrefBase();  
  Globales::elPuerto.escribir( "Vref Calibracion (V): " );
  Globales::elPuerto.escribir( vref_calibrado ); 
  Globales::elPuerto.escribir( "\n" );
  
  esperar( 1000 );
  Globales::elPuerto.escribir( "---- setup(): fin ---- \n " );
}

/**
 * @brief Ejecuta una secuencia visual de parpadeo.
 * @details Realiza una serie de encendidos y apagados del LED para indicar que 
 * el dispositivo está procesando un nuevo ciclo de medida.
 */
inline void lucecitas() {
  using namespace Globales;
  elLED.brillar( 100 ); esperar ( 400 ); 
  elLED.brillar( 100 ); esperar ( 400 ); 
  elLED.brillar( 100 ); esperar ( 400 ); 
  elLED.brillar( 1000 ); esperar ( 1000 ); 
}

/**
 * @namespace Loop
 * @brief Variables persistentes relativas al ciclo de vida del loop.
 */
namespace Loop {
  uint8_t cont = 0; ///< Contador incremental de ciclos de medición.
}

/**
 * @brief Bucle principal de ejecución (Arduino Loop).
 * @details Realiza las siguientes acciones:
 * 1. Lee los sensores (O3, CO2, Temp, Batería).
 * 2. Empaqueta los datos en un array de 9 bytes (Little Endian).
 * 3. Emite la información mediante un anuncio BLE durante 30 segundos.
 * * **Estructura del Payload (9 bytes):**
 * | Byte 0 | Bytes 1-2 | Bytes 3-4 | Bytes 5-6 | Bytes 7-8 |
 * |:------:|:---------:|:---------:|:---------:|:---------:|
 * | ID(0xAA)| O3 (ppb)  | Temp (x10)| CO2 (ppm) | Bat (%)   |
 */
void loop () {
  using namespace Loop;
  using namespace Globales;

  cont++;
  elPuerto.escribir( "\n---- loop(): empieza " );
  elPuerto.escribir( cont );
  elPuerto.escribir( "\n" );

  lucecitas();

  // --- Adquisición de Medidas ---
  float valorO3 = elMedidor.medirPPM(); 
  int valorCO2 = elMedidor.medirCO2();
  int valorTemperatura = elMedidor.medirTemperatura();
  int valorBateria = elMedidor.medirBateria();

  // Mostrar datos por puerto serie para depuración
  elPuerto.escribir( "O3 (ppm): " );
  elPuerto.escribir( valorO3); 
  elPuerto.escribir( "\n" );
  
  // --- Empaquetado de Datos ---
  // O3 se guarda en ppb (partes por billón) multiplicando ppm por 1000
  uint16_t O3_pack = (uint16_t)(valorO3 * 1000.0f); 
  uint16_t Temp_pack = (uint16_t)valorTemperatura; 
  uint16_t CO2_pack = (uint16_t)valorCO2; 
  uint16_t Bat_pack = (uint16_t)valorBateria;

  uint8_t datos_payload[9] = {0};
  datos_payload[0] = 0xAA; // Cabecera identificadora del protocolo

  // Empaquetado Little Endian (Byte bajo primero)
  datos_payload[1] = (uint8_t)(O3_pack & 0xFF);
  datos_payload[2] = (uint8_t)(O3_pack >> 8);

  datos_payload[3] = (uint8_t)(Temp_pack & 0xFF);
  datos_payload[4] = (uint8_t)(Temp_pack >> 8);

  datos_payload[5] = (uint8_t)(CO2_pack & 0xFF);
  datos_payload[6] = (uint8_t)(CO2_pack >> 8);

  datos_payload[7] = (uint8_t)(Bat_pack & 0xFF);
  datos_payload[8] = (uint8_t)(Bat_pack >> 8);

  // --- Emisión BLE ---
  elPublicador.laEmisora.emitirDatosMultiples(datos_payload, sizeof(datos_payload));
  
  esperar( 30000 ); // Mantener el anuncio activo 30 segundos

  elPublicador.laEmisora.detenerAnuncio();

  elPuerto.escribir( "---- loop(): acaba **** " );
  elPuerto.escribir( cont );
  elPuerto.escribir( "\n" );
  
} // loop ()