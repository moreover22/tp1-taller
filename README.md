# TP 1 - Protocolo DBUS | Taller 7542

## Trabajo práctico 1 - Implementación del protocolo DBUS 

### Taller de programación 75.42 - FIUBA

- Alumno: Agustín Emanuel More
- Padrón: 102914
- Repositorio github: https://github.com/moreover22/tp1-taller

Contenidos
---


Introducción
---

El trabajo práctico tiene como objetivo dominar el manejo de sockets, archivos y cadenas en C. Para ello se deberá implementar parte del protocolo [DBUS] (Desktop Bus) que comunica procesos dentro del sistema operativo. Además, se deben conseguir implementaciones abstractas y encapsuladas.


El protocolo DBUS
---

El protocolo brinda una serie de reglas que al momento de codificar las mensajes que serán intercambiados entre los procesos. El mismo cuanta con un `header` y un `body`.

El `header` contiene:
 - En los primeros 4 bytes, información del protocolo y el _endianness_ del mensaje (esto es, si es _little endian_ o _big endian_). En este tp se trató completamete la información en _little endian_ (aunque el programa es compatible en ambas _endianness_).

 Ejemplo de de primeros cuatro bytes del `header`:
 
 ```
 |6C 01 00 01             |                         |  |l...            |
 ```

[Aclaración: En el panel de la izquierda, se encuentra los bytes expresados en hexadecimal y a la derecha su correspondiente equivalente en ASCII (si el caracter no tiene significado en la explicación, se representa con '.')].

Donde el primer `6C` corresponde al carácter en [ASCII] 'l'. Luego, `01 00 01` corresponden a `llamada a método`, `sin flags` y `versión del 1 protocolo` respectivamente. Estos primeros bytes permanecerán constantes para esta implementación del protoco.

- Pasados esos bytes, se encuentran tres campos importantes:

```
 |6C 01 00 01 D6 00 00 00 | 06 00 00 00 A6 00 00 00 |  |l................|
```

Los primeros 4 bytes fueron analizados previamente, los siguientes `D6 00 00 00` expresa el número D6<sub>16</sub> (expresado en _little endian_) que equivale a 214<sub>10</sub>, esto indica que el cuerpo del mensaje (el que contiene los parámetros), tiene dicha longitud medida en bytes. Los siguientes 4 bytes o palabra `06 00 00 00`, se trata del id del mensaje, en este caso 6<sub>16</sub> = 6<sub>10</sub>.

- Después de estos 16 bytes o 4 palabras de 4 bytes, se cuenta con parametros definidas por el protocolo. Sin entrar en detalle son de la forma:

```
 |06 01 73 00 12 00 00 00 |                         |  |..s.....        |
```

Donde el primer byte, `06`, indica que es un argumento de tipo `Destino`, el caracter 's' (73<sub>16</sub> en ASCII) indica que será un argumento de tipo cadena (para esta implementación siempre será el caso). Los siguientes 4 bytes mostrarán la longitud del argumento, en este caso 12<sub>16</sub> = 18<sub>10</sub>. Luego de estas dos palabras, se encuentra el argumento en sí:


```
 |06 01 73 00 12 00 00 00 | 74 61 6C 6C 65 72 2E 64 |  |..s.....taller.d|
 |62 75 73 2E 70 61 72 61 | 6D 73 00 00 00 00 00 00 |  |bus.params......|
```

Como se ve, se completa con ceros hasta completar dos palabras (completar a 8 bytes), esto es un requerimiento que establece el protocolo y deben ser contados en la longitud del `header` salvo para el último argumento.

- Para los siguientes argumentos, `Ruta`, `Interfaz` y `Método` el procedimiento es análogo. Finalmente, la `Firma`, que es opcional, cambia ligeramente el formato, los primeros 4 bytes se forman de la misma manera, pero los siguientes bytes consisten en un byte indicando la cantidad de parametros que tiene el método y un carácter 's', por cada uno.

- El `body` cuenta con un estructura más simple, una palabra indicando la longitud del parámetro y el parámetro en cuestión finalizado con un carácter nulo.

El protocolo se encuentra más detallado en la documentación [DBUS], o en el enunciado del tp.

De este análisis del protocolo, se tomaron las siguientes decisiones:
- Como los primeros 16 bytes contienen la información del tamaño del resto del cuerpo, se procede a hacer un `recv` de esos primeros bytes en un buffer estático y luego en base a esa información ya sé cuántos bytes tengo que recibir después, sin tener que esperar a que cierren el socket del otro lado. Además de poder reservar la memoria justa para almacenar los mensajes.



<!--
abstraccion del socket
read del fuffer del archivo
interpret from server
buffer static buffer dinamico
client - server

 -->

Referencias
---
[DBUS]: https://www.freedesktop.org/wiki/Software/dbus/
- Especificaciones DBUS: https://www.freedesktop.org/wiki/Software/dbus/

[ASCII]: https://www.rapidtables.com/code/text/ascii-table.html
