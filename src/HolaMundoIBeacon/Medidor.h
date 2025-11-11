// -*- mode: c++ -*-

#ifndef MEDIDOR_H_INCLUIDO
#define MEDIDOR_H_INCLUIDO

// ------------------------------------------------------
// ------------------------------------------------------

// Valores de CO2 (en ppm)
const int CO2_SIMULADO[] = {400, 550, 850, 1000, 1250, 1300, 950};
const int NUM_CO2_VALORES = sizeof(CO2_SIMULADO) / sizeof(CO2_SIMULADO[0]);

// Valores de Temperatura (multiplicados por 10: 22.0°C -> 220)
const int TEMP_SIMULADA[] = {220, 245, 266, 289, 235, 68, 353}; 
const int NUM_TEMP_VALORES = sizeof(TEMP_SIMULADA) / sizeof(TEMP_SIMULADA[0]);

// Valores de bateria (%)
const int BAT_SIMULADA[] = {98, 87, 80, 75, 50, 30, 15}; 
const int NUM_BAT_VALORES = sizeof(BAT_SIMULADA) / sizeof(BAT_SIMULADA[0]);

class Medidor {

  // .....................................................
  // .....................................................
private:

public:

  // .....................................................
  // constructor
  // .....................................................
  Medidor(  ) {
  } // ()

  // .....................................................
  // .....................................................
  void iniciarMedidor() {
	// las cosas que no se puedan hacer en el constructor, if any
  } // ()

  // .....................................................
  // .....................................................
  int medirCO2() {//verde menos de 800, naranja 800 a 1200, rojo mas de 1200
	// Generar un índice aleatorio [0, NUM_CO2_VALORES - 1]
    int indiceAleatorio = random(0, NUM_CO2_VALORES); 
    return CO2_SIMULADO[indiceAleatorio];
  } // ()

  // .....................................................
  // .....................................................
  int medirTemperatura() {//
	// Generar un índice aleatorio [0, NUM_TEMP_VALORES - 1]
    int indiceAleatorio = random(0, NUM_TEMP_VALORES);
        // Devolvemos el valor raw (multiplicado por 10)
    return TEMP_SIMULADA[indiceAleatorio];
  } // ()

  // .....................................................
  // .....................................................
  int medirBateria() {
	// Generar un índice aleatorio [0, NUM_BAT_VALORES - 1]
    int indiceAleatorio = random(0, NUM_BAT_VALORES);
        
        // Devolvemos el porcentaje de batería simulado
    return BAT_SIMULADA[indiceAleatorio];
  } // ()

	
}; // class

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
#endif
