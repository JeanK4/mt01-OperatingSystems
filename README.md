# Simulador de Planificador MLFQ

Este repositorio contiene un **simulador del planificador Multi-Level Feedback Queue (MLFQ)** implementado en **C++** como parte de un proyecto de Sistemas Operativos.  
El programa lee un archivo de texto con la definición de procesos, simula la ejecución bajo tres esquemas diferentes de MLFQ y genera los resultados (tiempo de espera, retorno, finalización y respuesta) en archivos de salida.

---

## 📂 Estructura del proyecto

- `main.cpp` → Punto de entrada, ejecuta los tres esquemas de planificación y guarda los resultados en archivos.
- `scheduler.h` → Definición de clases (`Process`, `QueuePolicy`, `RRPolicy`, `SJFPolicy`, `STCFPolicy`, `MLFQScheduler`).
- `scheduler.cpp` → Implementación de la lógica de planificación y simulación.
- `mlfq001.txt` → Archivo de entrada de ejemplo con definición de procesos.
- `scheme1output.txt`, `scheme2output.txt`, `scheme3output.txt` → Ejemplos de salidas generadas por el simulador.
- `informe.pdf` → Informe del proyecto, que describe el algoritmo, ejemplos de entrada y resultados de la simulación.

---

## 📑 Informe

El informe completo está disponible aquí:  
[Descargar PDF](./informe.pdf)

Incluye:
- Descripción del algoritmo de planificación MLFQ.  
- Dos ejemplos de entrada (`mlfq001.txt` y `mlfq002.txt`).  
- Resultados de la simulación para los tres esquemas.  

---

## 🎥 Video de Presentación

Un video corto de demostración está disponible aquí:  
[Enlace a Drive](https://javerianacaliedu-my.sharepoint.com/:v:/g/personal/jkbuitragoo_javerianacali_edu_co/EUr1iIG1145Akqjuw14B-8ABE4QPqk6kn6NBz7kv9WaGJA?nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJPbmVEcml2ZUZvckJ1c2luZXNzIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXciLCJyZWZlcnJhbFZpZXciOiJNeUZpbGVzTGlua0NvcHkifX0&e=UYYDJj)

---
