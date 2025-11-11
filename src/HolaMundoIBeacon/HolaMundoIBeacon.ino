// -*-c++-*-

// --------------------------------------------------------------
//
// Jordi Bataller i Mascarell
// 2019-07-07
//
// --------------------------------------------------------------

// https://learn.sparkfun.com/tutorials/nrf52840-development-with-arduino-and-circuitpython

// https://stackoverflow.com/questions/29246805/can-an-ibeacon-have-a-data-payload

// --------------------------------------------------------------
// --------------------------------------------------------------
#include <bluefruit.h>

#undef min // vaya tela, están definidos en bluefruit.h y  !
#undef max // colisionan con los de la biblioteca estándar

// --------------------------------------------------------------
// --------------------------------------------------------------
#include "LED.h"
#include "PuertoSerie.h"

// --------------------------------------------------------------
// --------------------------------------------------------------
namespace Globales {
  
  LED elLED ( /* NUMERO DEL PIN LED = */ 7 );

  PuertoSerie elPuerto ( /* velocidad = */ 115200 ); // 115200 o 9600 o ...

  // Serial1 en el ejemplo de Curro creo que es la conexión placa-sensor 
};

// --------------------------------------------------------------
// --------------------------------------------------------------
#include "EmisoraBLE.h"
#include "Publicador.h"
#include "Medidor.h"
#include "MedidorO3.h"


// --------------------------------------------------------------
// --------------------------------------------------------------
namespace Globales {

  Publicador elPublicador;

  Medidor elMedidor;

  MedidorO3 elMedidorO3;

}; // namespace

// --------------------------------------------------------------
// --------------------------------------------------------------
void inicializarPlaquita () {

  pinMode(O3_PIN_VGAS, INPUT); // A5 default
  pinMode(O3_PIN_VREF, INPUT); // A4 default

} // ()

// --------------------------------------------------------------
// setup()
// --------------------------------------------------------------
void setup() {

  Globales::elPuerto.esperarDisponible();

  // 
  // 
  // 
  inicializarPlaquita();
  randomSeed(analogRead(0));

  // Suspend Loop() to save power
  // suspendLoop();

  // 
  // 
  // 
  Globales::elPublicador.encenderEmisora();

  // Globales::elPublicador.laEmisora.pruebaEmision();
  
  // 
  // 
  // 
  Globales::elMedidor.iniciarMedidor();

  // 
  // 
  // 
  esperar( 1000 );

  Globales::elMedidorO3.iniciarMedidor();

  float vref_calibrado = Globales::elMedidorO3.getVrefBase();
  Globales::elPuerto.escribir( "Vref Calibracion (V): " );
  Globales::elPuerto.escribir( vref_calibrado ); 
  Globales::elPuerto.escribir( "\n" );
  // 
  // 
  // 
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

  float Vg_lectura = elMedidorO3.leerVgas(10);
  float Vref_base = elMedidorO3.getVrefBase();
  float DeltaV = Vg_lectura - Vref_base;

  //float valorO3 = elMedidorO3.medirPPM();
  int valorCO2 = elMedidor.medirCO2();
  int valorTemperatura = elMedidor.medirTemperatura();
  int valorBateria = elMedidor.medirBateria();
  float valorO3_float = elMedidorO3.medirPPMSimulado();

  //Medición y muestra ozono
    
    // Muestro el valor de O3 por el Puerto Serie
    elPuerto.escribir( "O3 (ppm): " );
    //elPuerto.escribir( valorO3); // Asumiendo que PuertoSerie::escribir tiene sobrecarga para float
    elPuerto.escribir( valorO3_float);
    elPuerto.escribir( "\n" );
    
    // Si tuvieras una función para publicar O3 (por ejemplo, BLE), la llamarías aquí:
    // elPublicador.publicarO3( (int)(valorO3 * 1000), cont, 1000 ); // Ejemplo: publicar el valor multiplicado por 1000 para pasar float a int
 
  // 
  
  // 1. Convertir los valores a enteros para empaquetar
    //uint16_t O3_pack = (uint16_t)(valorO3 * 1000.0f); 
    uint16_t O3_pack = (uint16_t)(valorO3_float * 1000.0f);
    uint16_t Temp_pack = (uint16_t)valorTemperatura; 
    uint16_t CO2_pack = (uint16_t)valorCO2; 
    uint16_t Bat_pack = (uint16_t)valorBateria;

    // 2. Crear el array de 7 bytes: [ID] [O3_L] [O3_H] [TEMP_L] [TEMP_H] [CO2_L] [CO2_H]
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
    // CO2 (Bytes 5 y 6)
    datos_payload[7] = (uint8_t)(Bat_pack & 0xFF);
    datos_payload[8] = (uint8_t)(Bat_pack >> 8);

    // 3. Emitir el anuncio con la nueva función
    elPublicador.laEmisora.emitirDatosMultiples(datos_payload, sizeof(datos_payload));
    
    esperar( 120000 );

    elPublicador.laEmisora.detenerAnuncio();



  // 
  // 
  // 
  elPuerto.escribir( "---- loop(): acaba **** " );
  elPuerto.escribir( cont );
  elPuerto.escribir( "\n" );
  
} // loop ()
// --------------------------------------------------------------
// --------------------------------------------------------------
// --------------------------------------------------------------
// --------------------------------------------------------------
