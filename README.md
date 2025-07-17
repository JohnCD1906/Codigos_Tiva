✍️ Autor
Cesareo Dolores John Brian: 
Estudiante e ingeniero interesado en sistemas embebidos y microcontroladores.

# Códigos_Tiva

Repositorio con ejemplos y proyectos para programar la tarjeta **Tiva TM4C1294NCPDT** usando **Code Composer Studio (CCS)**. Incluye ejemplos en C y en lenguaje ensamblador.
Este repositorio está hecho para el IDE Code Composer Studio v12.6.0 de Texas Instruments que cuenta con una integración con Git

---

## 📂 Estructura

| Carpeta                  | Contenido                                                                                  |
|--------------------------|--------------------------------------------------------------------------------------------|
| `Proyecto_Final/`           | Proyecto completo con múltiples tareas, uso de GPIO, timers e interrupciones            |
| `UART/`                     | Código de prueba de UART usando Tiva                                                    |
| `ADC/`                      | Lectura analógica con el módulo ADC                                                     |
| `Ensamblador/`              | Código en ensamblador para TM4C1294NCPDT                                                |
| `Interrupciones_Prioridad/` | Manejadores de interrupciones con diferentes prioridades                                |
| `Proyecto_BCD/`             |Conmuta un led a una frecuencia especifica del timer, lee la temperatura y usa un BCD    |
|`SPI/`                       |Código de prueba del protocolo de comunicación                                           |
|`I2C/`                       |Código de prueba del protocolo de comunicación                                           |

---

## ⚙️ Requisitos

- Code Composer Studio (CCS)
- Driverlib y TivaWare instalados
- Tarjeta Tiva TM4C1294NCPDT
- Cable micro USB
- Drivers de la Tiva (Stellaris ICDI)

---

## 🚀 ¿Cómo usar los proyectos?
A continuación se explican dos métodos para abrir estos proyectos en Code Composer Studio (CCS):

---

### 🔁 Opción 1: Importar desde Git (recomendado)

Esta opción es ideal si estás trabajando con GitHub directamente desde CCS. **Requiere Code Composer Studio v12.6.0 o superior** y **Git 2.46.0 o superior**.

#### 🔧 Pasos:

1. Abre **Code Composer Studio**.
2. En el menú **Archivo > Importar > Git**, busca y selecciona la opción:
Projects from Git
3.  En la ventana que aparece, elige:
Clonar URI
4.  En el campo **URI**, pega esta dirección:
https://github.com/JohnCD1906/Codigos_Tiva.git
5.Si el repositorio es **privado**, GitHub requerirá autenticación. Debido a actualizaciones de seguridad, **ya no puedes usar tu contraseña normal**. Debes generar un **Token de Acceso Personal** (PAT).

#### 🛡️ ¿Cómo generar un token?

GitHub ha reemplazado las contraseñas por tokens para proteger mejor tu cuenta. Para crear uno:

🔗 Guía oficial:  
[https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens#creating-a-personal-access-token-classic](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens#creating-a-personal-access-token-classic)

> ⚠️ Copia y guarda tu token al crearlo, ya que no podrás verlo de nuevo. Este token funcionará como tu contraseña en Git.

6. Una vez que completes la autenticación, CCS clonará el repositorio en tu computadora.

⚠️ Notas adicionales
-Asegúrate de tener TivaWare correctamente instalado y configurado si algún proyecto usa driverlib.
-Revisa que tu tarjeta Tiva esté conectada por USB y que los drivers Stellaris ICDI estén instalados.


📃 Licencia
Este proyecto es libre. Puedes copiar, modificar y usar con fines educativos.
