// -- mode: c++ --
// MedidorO3.h (ULPSM O3 → ppm - Versión CORREGIDA con Calibración)

#ifndef MEDIDOR_O3_H_INCLUIDO
#define MEDIDOR_O3_H_INCLUIDO

#include <Arduino.h>
#include <math.h> // Necesario para usar fabsf()

// ===================== TUS CONSTANTES DE CONFIGURACIÓN (O3_...) =====================
#ifndef O3_PIN_VGAS
#define O3_PIN_VGAS A5 
#endif
#ifndef O3_PIN_VREF
#define O3_PIN_VREF A4 
#endif

#ifndef O3_VDD
#define O3_VDD 1.20f      // Voltaje de alimentación del sensor/ADC
#endif

#ifndef O3_ADC_BITS
#define O3_ADC_BITS 12      // Resolución del ADC (12 bits)
#endif

// Constantes de calibración (dentro del .h para ser accesibles por la lógica de medición)
const float SENSIBILIDAD_SENSOR = -44.26f;  // Tu sensibilidad original (nA/ppm)
const float GAIN_TIA = 499.0f;            // Ganancia del amplificador (Ω)


// ===================== NUEVAS CONSTANTES DE CORRECCIÓN (de la memoria) =====================
// Estos son los valores calculados en tu PDF (Sección 2.2)

//const float CORRECCION_SLOPE = 0.8729f;   // (Ajuste de Ganancia, 1/m')
const float CORRECCION_SLOPE = 1.0f;
//const float CORRECCION_OFFSET = 0.1607f; // (Ajuste de Desplazamiento, -b/m')
const float CORRECCION_OFFSET = 0.0f; // (Ajuste de Desplazamiento, -b/m')

// =========================================================================================

const int O3_SIMULADO[] = {100, 650, 45, 1200, 950, 800, 300}; 
const int NUM_O3_VALORES = sizeof(O3_SIMULADO) / sizeof(O3_SIMULADO[0]);


// ===================== Clase de medida O3 =====================
class MedidorO3 {
public:
    // Constructor
    explicit MedidorO3() {} 

    void iniciarMedidor(int nAvg = 50) {
        // Lee el pin de referencia (Vr) varias veces y guarda el promedio
        // Este valor (Vref_base) representa el 0 ppm para la placa en ese momento.
        _Vref_base = leerVolt(O3_PIN_VREF, nAvg);
    }

    float getVrefBase() const {
        return _Vref_base; 
    }

    float leerVgas(int nAvg = 10) { 
        return leerVolt(O3_PIN_VGAS, nAvg); 
    }

    //Función Ozono simulado para alertas
    float medirPPMSimulado() {
        // Generar un índice aleatorio
        int indiceAleatorio = random(0, NUM_O3_VALORES);
        
        // Obtener el valor (está multiplicado por 1000) y dividirlo por 1000.0f
        float valorSimuladoRaw = (float)O3_SIMULADO[indiceAleatorio];
        
        return valorSimuladoRaw / 1000.0f;
    }


    // Función medirPPM() (Llamada en loop, AHORA CON CORRECCIÓN)
    float medirPPM() {
        float Vg = leerVolt(O3_PIN_VGAS, 10);
        //float Vr = leerVolt(O3_PIN_VREF, 10);
        
        // Diferencia de voltaje BRUTA (V)
        float deltaV = Vg - _Vref_base;
        
        // Cálculo del denominador (GAIN_TIA * S * 1e-6)
        float denominador = GAIN_TIA * SENSIBILIDAD_SENSOR * 1e-6f; 
        
        float ppm_bruto = 0.0f;
        if (denominador != 0.0f) {
            // Conversión BRUTA (este es el "Valor_Medido_Bruto" o "y_medido")
            ppm_bruto = deltaV / denominador;
        }

        // Aplicar valor absoluto (fabsf)
        ppm_bruto = fabsf(ppm_bruto); 
        
        
        // ========= APLICAR CORRECCIÓN DE LA MEMORIA =========
        // Esta es tu fórmula: Valor_Corregido = (Valor_Medido_Bruto * 0.8729) + 0.1607
        float ppm_corregido = (ppm_bruto * CORRECCION_SLOPE) + CORRECCION_OFFSET;

        if (ppm_corregido < 0.0f) {
            ppm_corregido = 0.0f;
        }


        return ppm_corregido;
    }
    
private:

    float _Vref_base = 0.0f;

    float leerVolt(int pin, int nAvg = 10) {
        long acc = 0;
        for (int i=0; i<nAvg; ++i) { acc += analogRead(pin); delay(2); }
        const float raw = (float)acc / nAvg;
        const float fullScale = (float)((1 << O3_ADC_BITS) - 1); 
        return (raw * O3_VDD) / fullScale;
    }
};

#endif // MEDIDOR_O3_H_INCLUIDO

