// --------------------------------------------------------------
//
// Rocio
// 11/11/2025
// Modificado 26/11/25
//
// --------------------------------------------------------------

#include <bluefruit.h>

#undef min 
#undef max 

// --------------------------------------------------------------
// --------------------------------------------------------------

#include "LED.h"
#include "PuertoSerie.h"


// --------------------------------------------------------------
// --------------------------------------------------------------
namespace Globales {
  
  LED elLED (7);

  PuertoSerie elPuerto ( /* velocidad = */ 115200 ); // 115200 o 9600 o ...

};

// --------------------------------------------------------------
// --------------------------------------------------------------

#include "EmisoraBLE.h"
#include "Publicador.h"
#include "Medidor.h"

// --------------------------------------------------------------
// --------------------------------------------------------------
namespace Globales {

  Publicador elPublicador;

  Medidor elMedidor;

}; // namespace

// --------------------------------------------------------------
// --------------------------------------------------------------
void inicializarPlaquita () {

  pinMode(O3_PIN_VGAS, INPUT); // A5 default
  pinMode(O3_PIN_VREF, INPUT); // A4 default
  pinMode(PIN_A6, INPUT); // Batería

} // ()

// --------------------------------------------------------------
// setup()
// --------------------------------------------------------------
void setup() {

  //----------------------------------------------------------------- 

  inicializarPlaquita(); // Encienda placa

  //-----------------------------------------------------------------
  //-----------------------------------------------------------------
  
  randomSeed(analogRead(0)); //Asegura que se use orden random en las medidas fake

  // Suspend Loop() to save power
  // suspendLoop();

  Globales::elPublicador.encenderEmisora();

  //-----------------------------------------------------------------

  Globales::elMedidor.iniciarMedidor();

  esperar( 1000 );

  float vref_calibrado = Globales::elMedidor.getVrefBase();  
  Globales::elPuerto.escribir( "Vref Calibracion (V): " );
  Globales::elPuerto.escribir( vref_calibrado ); 
  Globales::elPuerto.escribir( "\n" );
  esperar( 1000 );

  Globales::elPuerto.escribir( "---- setup(): fin ---- \n " );

} // setup ()

// --------------------------------------------------------------
// --------------------------------------------------------------
inline void lucecitas() {
  using namespace Globales;

  elLED.brillar( 100 ); // 100 encendido
  esperar ( 400 ); //  100 apagado
  elLED.brillar( 100 ); // 100 encendido
  esperar ( 400 ); //  100 apagado
  Globales::elLED.brillar( 100 ); // 100 encendido
  esperar ( 400 ); //  100 apagado
  Globales::elLED.brillar( 1000 ); // 1000 encendido
  esperar ( 1000 ); //  100 apagado
} // ()

// --------------------------------------------------------------
// loop ()
// --------------------------------------------------------------
namespace Loop {
  uint8_t cont = 0;
};

// ..............................................................
// ..............................................................
void loop () {

  using namespace Loop;
  using namespace Globales;

  cont++;

  elPuerto.escribir( "\n---- loop(): empieza " );
  elPuerto.escribir( cont );
  elPuerto.escribir( "\n" );


  lucecitas();

  //-------------------------------------------
  // Leer V para ajustar las mediciones de O3
  //-------------------------------------------
    float Vg_lectura = elMedidor.leerVgas(10);
    float Vref_base = elMedidor.getVrefBase();
    float DeltaV = Vg_lectura - Vref_base;
  //-------------------------------------------



  //-------------------------------------------
  // Llamada a funciones de mediciones
  //-------------------------------------------
    float valorO3 = elMedidor.medirPPM(); // MEDICION REAAAL
    int valorCO2 = elMedidor.medirCO2();
    int valorTemperatura = elMedidor.medirTemperatura();
    int valorBateria = elMedidor.medirBateria();
    //float valorO3_float = elMedidor.medirPPMSimulado(); // MEDICION FALSAAA
  //-------------------------------------------



  //-------------------------------------------
  // Muestra ozono en puerto serie
  //-------------------------------------------  
    elPuerto.escribir( "O3 (ppm): " );
    elPuerto.escribir( valorO3); // MEDICION REAL
    //elPuerto.escribir( valorO3_float); // MEDICION FALSA
    elPuerto.escribir( "\n" );
  //-------------------------------------------
  

  //-------------------------------------------
  // Convertir los valores a enteros para empaquetar
  //-------------------------------------------
    uint16_t O3_pack = (uint16_t)(valorO3 * 1000.0f); // Valor realll
    //uint16_t O3_pack = (uint16_t)(valorO3_float * 1000.0f); // Valor fakees
    uint16_t Temp_pack = (uint16_t)valorTemperatura; 
    uint16_t CO2_pack = (uint16_t)valorCO2; 
    uint16_t Bat_pack = (uint16_t)valorBateria;
  //-------------------------------------------

  //-------------------------------------------
  // Crear el array de 9 bytes: [ID] [O3_L] [O3_H] [TEMP_L] [TEMP_H] [CO2_L] [CO2_H] [BAT_L] [BAT_H]
  //-------------------------------------------
    uint8_t datos_payload[9] = {0};

    datos_payload[0] = 0xAA; 

    // O3 (Bytes 1 y 2)
    datos_payload[1] = (uint8_t)(O3_pack & 0xFF);
    datos_payload[2] = (uint8_t)(O3_pack >> 8);

    // Temperatura (Bytes 3 y 4)
    datos_payload[3] = (uint8_t)(Temp_pack & 0xFF);
    datos_payload[4] = (uint8_t)(Temp_pack >> 8);

    // CO2 (Bytes 5 y 6)
    datos_payload[5] = (uint8_t)(CO2_pack & 0xFF);
    datos_payload[6] = (uint8_t)(CO2_pack >> 8);

    //Bateria (Bytes 7 y 8)
    datos_payload[7] = (uint8_t)(Bat_pack & 0xFF);
    datos_payload[8] = (uint8_t)(Bat_pack >> 8);
  //-------------------------------------------

  //-------------------------------------------
  // Emitir el beacon con datos
  //-------------------------------------------
    elPublicador.laEmisora.emitirDatosMultiples(datos_payload, sizeof(datos_payload));
  
    esperar( 30000 );

    elPublicador.laEmisora.detenerAnuncio();
  //-------------------------------------------

  elPuerto.escribir( "---- loop(): acaba **** " );
  elPuerto.escribir( cont );
  elPuerto.escribir( "\n" );
  
} // loop ()
// --------------------------------------------------------------

