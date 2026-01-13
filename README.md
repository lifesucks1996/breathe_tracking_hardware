# Sistema de Monitorización Ambiental BLE 🌿

Este proyecto consiste en un nodo sensor basado en la placa **Adafruit Bluefruit (nRF52840)** diseñado para medir la calidad del aire y transmitir los datos mediante balizas Bluetooth (Beacons).

## 🚀 Funcionalidades
- **Medición de Ozono (O3):** Cálculo real mediante sensor electroquímico y ajuste por software.
- **Sensores Ambientales:** Monitorización de CO2 y Temperatura (datos simulados/reales).
- **Gestión de Energía:** Lectura del voltaje de la batería LiPo mediante divisor de tensión.
- **Transmisión BLE:** Publicación de datos en formato iBeacon con payload personalizado.
- **Feedback Visual:** Sistema de alertas mediante secuencias de LED.

## 🛠️ Estructura del Código
El código está organizado de forma modular para facilitar su mantenimiento:
- `Medidor.h`: Lógica de adquisición y procesado de señales analógicas.
- `Publicador.h`: Empaquetado de datos y gestión de la lógica de anuncios.
- `EmisoraBLE.h`: Abstracción de la pila Bluetooth Low Energy.
- `ServicioEnEmisora.h`: Gestión de perfiles GATT (servicios y características).
- `LED.h` / `PuertoSerie.h`: Utilidades de interfaz y depuración.

## 📊 Formato del Payload BLE
Los datos se emiten en un paquete de 9 bytes con el siguiente formato:
| Byte 0 | Bytes 1-2 | Bytes 3-4 | Bytes 5-6 | Bytes 7-8 |
|:---:|:---:|:---:|:---:|:---:|
| `0xAA` | O3 (ppb) | Temp (ºC*10) | CO2 (ppm) | Batería (%) |

## 📚 Documentación
Este proyecto está completamente documentado siguiendo el estándar **Doxygen**. 
Para generar la documentación:
1. Asegúrate de tener instalado Doxygen.
2. Ejecuta `doxygen Doxyfile` en la raíz del proyecto.
3. Abre `html/index.html` para ver la documentación técnica en tu navegador.

## 🔧 Instalación
1. Clona este repositorio: `git clone https://github.com/tu-usuario/tu-repositorio.git`
2. Abre el proyecto en **Arduino IDE** o **VS Code + PlatformIO**.
3. Asegúrate de instalar la librería `Adafruit Bluefruit nRF52`.
4. Carga el archivo `HolaMundoIBeacon.ino` en tu placa.

---
**Desarrollado por:** Rocio  
**Fecha:** Enero 2026
