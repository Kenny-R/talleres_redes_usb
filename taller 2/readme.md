# Solución del segundo taller de la materia Redes de computadoras (CI4835) de la USB
## Autores:
Este taller fue resuelto por los alumnos:
- Gabriela Panqueva (18-10671)
- Kenny Rojas (18-10595)
- Simon Puyosa (18-10717)
## Como ejecutar: 
Para ejecutar este taller se necesitan dos dispositivos con sistema operativo linux.

Primero debe compilar los scripts por separado usando el comando `gcc -o jarraChiquita jarraChiquita.c && gcc -o jarraGrande jarraGrande.c`. Luego cada dispositivo debe tener uno de los archivos ya compilados. 

Para el dispositivo que hará de jarra Grande debe ejecutar el siguiente comando `./jarraGrande [Hostname de la jarra pequeña]`. Análogamente se hace con el dispositivo que funcionara como jarra pequeña `./jarraChiquita [Hostname de la jarra grande]`.
