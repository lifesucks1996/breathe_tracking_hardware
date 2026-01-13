/**
 * @file Medidor.h
 * @brief Clase para la gestión de sensores analógicos y simulados.
 * @author Rocio
 * @date 11/11/2025
 * @details Este archivo contiene la lógica para calcular concentraciones de O3 
 * mediante un sensor electroquímico, así como la estimación del nivel de batería
 * y la generación de datos simulados para CO2 y Temperatura.
 */

#ifndef MEDIDOR_H_INCLUIDO
#define MEDIDOR_H_INCLUIDO

#include <Arduino.h>
#include <math.h>

// ===================== CONSTANTES DE CONFIGURACIÓN (O3) =====================

#ifndef O3_PIN_VGAS
#define O3_PIN_VGAS A5 ///< Pin para el voltaje de gas del sensor de ozono.
#endif
#ifndef O3_PIN_VREF
#define O3_PIN_VREF A4 ///< Pin para el voltaje de referencia del sensor de ozono.
#endif

#ifndef O3_VDD
#define O3_VDD 1.20f      ///< Voltaje de alimentación del sensor/ADC (V).
#endif

#ifndef O3_ADC_BITS
#define O3_ADC_BITS 12      ///< Resolución del ADC para ozono (12 bits).
#endif

/** @brief Sensibilidad del sensor electroquímico en nA/ppm. */
const float SENSIBILIDAD_SENSOR = -44.26f;  
/** @brief Resistencia de ganancia del amplificador de transimpedancia (TIA) en Ohmios. */
const float GAIN_TIA = 499.0f;    

// ===================== CONSTANTES DE CORRECCIÓN =====================

const float CORRECCION_SLOPE = 1.0f;  ///< Factor de escala para ajuste de ganancia (m').
const float CORRECCION_OFFSET = 0.0f; ///< Desplazamiento para ajuste de línea base (b').

// ===================== DATOS SIMULADOS =====================

/// @brief Array de valores de Ozono simulados (ppm * 1000).
const int O3_SIMULADO[] = {100, 650, 45, 1200, 950, 800, 300}; 
const int NUM_O3_VALORES = sizeof(O3_SIMULADO) / sizeof(O3_SIMULADO[0]);

/// @brief Array de valores de CO2 simulados (ppm).
  //const int CO2_SIMULADO[] = {400, 550, 850, 1000, 1250, 1300, 950};
const int CO2_SIMULADO[] = {1400, 1350, 1360, 1280, 1250, 1300, 1450};
const int NUM_CO2_VALORES = sizeof(CO2_SIMULADO) / sizeof(CO2_SIMULADO[0]);

/// @brief Array de temperaturas simuladas (ºC * 10).
  //const int TEMP_SIMULADA[] = {220, 245, 266, 289, 235, 68, 353, 360, 400, 320};
const int TEMP_SIMULADA[] = {320, 345, 366, 389, 335, 368, 353, 360, 400, 320};
const int NUM_TEMP_VALORES = sizeof(TEMP_SIMULADA) / sizeof(TEMP_SIMULADA[0]);

/// @brief Array de porcentajes de batería simulados.
  //const int BAT_SIMULADA[] = {98, 87, 80, 75, 50, 30, 15, 77, 63}; 
const int BAT_SIMULADA[] = {15, 14, 12, 10, 8, 7, 3, 1}; 
const int NUM_BAT_VALORES = sizeof(BAT_SIMULADA) / sizeof(BAT_SIMULADA[0]);

// ===================== CONSTANTES BATERÍA REAL =====================

const float BATT_MAX_VOLTS = 4.20f; ///< Voltaje LiPo al 100%.
const float BATT_MIN_VOLTS = 3.30f; ///< Voltaje LiPo al 0%.
const float VDD = 3.30f;            ///< Referencia de voltaje del sistema (V).
const int ADC_BITS = 10;            ///< Resolución ADC para batería.

/**
 * @class Medidor
 * @brief Se encarga de la adquisición y procesamiento de datos ambientales.
 * * Esta clase encapsula tanto las lecturas de los pines analógicos como
 * el procesamiento matemático para convertir voltajes en unidades físicas.
 */
class Medidor {

private:
  float _Vref_base = 0.0f; ///< Valor de calibración inicial de VREF.

  /**
   * @brief Lee el voltaje de un pin analógico promediando varias muestras.
   * @param pin Pin analógico a leer.
   * @param nAvg Número de muestras para el promedio.
   * @return Voltaje calculado en Voltios.
   */
  float leerVolt(int pin, int nAvg = 10) {
    long acc = 0;
    for (int i=0; i<nAvg; ++i) { acc += analogRead(pin); delay(2); }
    const float raw = (float)acc / nAvg;
    const float fullScale = (float)((1 << O3_ADC_BITS) - 1); 
    return (raw * O3_VDD) / fullScale;
  }

public:

  /**
   * @brief Constructor por defecto.
   */
  Medidor(  ) {
  }

  /**
   * @brief Calibra el medidor obteniendo el voltaje de referencia inicial.
   * @param nAvg Número de muestras para la calibración (por defecto 50).
   */
  void iniciarMedidor(int nAvg = 50) {
       _Vref_base = leerVolt(O3_PIN_VREF, nAvg);
  }
  
  /**
   * @brief Calcula el porcentaje de carga de la batería real.
   * @details Utiliza un divisor de tensión 2:1 en el pin A6.
   * @return Porcentaje de batería (0-100).
   */
  int medirBateria() {
      const int nAvg = 10;
      long rawAcc = 0;
      for (int i = 0; i < nAvg; ++i) { 
          rawAcc += analogRead(PIN_A6); 
          delay(1); 
      }
      const float rawAvg = (float)rawAcc / nAvg;
      const float fullScale = (float)((1 << ADC_BITS) - 1); 
      float measuredVolts = (rawAvg * VDD) / fullScale;
      
      // Multiplicar por 2 por el divisor de voltaje del hardware
      float battVolts = measuredVolts * 2.0f; 
      
      float battRange = BATT_MAX_VOLTS - BATT_MIN_VOLTS;
      float percent = ((battVolts - BATT_MIN_VOLTS) / battRange) * 100.0f;

      if (percent > 100.0f) percent = 100.0f;
      if (percent < 0.0f) percent = 0.0f;

      return (int)percent;
  }

  /**
   * @brief Obtiene una medida de CO2 de los datos simulados.
   * @return Valor de CO2 en ppm.
   */
  int medirCO2() {
    int indiceAleatorio = random(0, NUM_CO2_VALORES); 
    return CO2_SIMULADO[indiceAleatorio];
  }

  /**
   * @brief Obtiene una medida de temperatura de los datos simulados.
   * @return Temperatura (ºC * 10).
   */
  int medirTemperatura() {
    int indiceAleatorio = random(0, NUM_TEMP_VALORES);
    return TEMP_SIMULADA[indiceAleatorio];
  }
  
  /**
   * @brief Retorna el voltaje de referencia base calibrado al inicio.
   * @return Voltaje en Voltios.
   */
  float getVrefBase() const {
        return _Vref_base; 
  }

  /**
   * @brief Lee el voltaje actual en el pin Vgas del sensor de O3.
   * @param nAvg Número de muestras para el promedio.
   * @return Voltaje en Voltios.
   */
  float leerVgas(int nAvg = 10) { 
        return leerVolt(O3_PIN_VGAS, nAvg); 
  }

  /**
   * @brief Obtiene una medida de Ozono de los datos simulados.
   * @return Valor de ozono en ppm (float).
   */
  float medirPPMSimulado() {
    int indiceAleatorio = random(0, NUM_O3_VALORES);
    float valorSimuladoRaw = (float)O3_SIMULADO[indiceAleatorio];      
    return valorSimuladoRaw / 1000.0f;
  }

  /**
   * @brief Realiza la medición real de Ozono (O3) en ppm.
   * @details Calcula la diferencia entre Vgas y Vref, convierte a corriente y 
   * aplica las constantes de sensibilidad y corrección (slope/offset).
   * @return Concentración de ozono corregida en ppm.
   */
  float medirPPM() {
    float Vg = leerVolt(O3_PIN_VGAS, 10);
    float deltaV = Vg - _Vref_base;
        
    float denominador = GAIN_TIA * SENSIBILIDAD_SENSOR * 1e-6f; 
        
    float ppm_bruto = 0.0f;
    if (denominador != 0.0f) {
      ppm_bruto = deltaV / denominador;
    }

    ppm_bruto = fabsf(ppm_bruto); 
        
    // Ajuste lineal final
    float ppm_corregido = (ppm_bruto * CORRECCION_SLOPE) + CORRECCION_OFFSET;

    if (ppm_corregido < 0.0f) {
      ppm_corregido = 0.0f;
    }

    return ppm_corregido;
  }
  
}; // class

#endif
