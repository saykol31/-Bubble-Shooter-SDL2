# Bubble Shooter — porte a Linux (SDL2)

Juego de disparar burbujas en C++17 con SDL2 (ventana 1280×800, redimensionable).
Se compila con CMake en Linux.

## Origen y atribución

Este proyecto **no es una obra original mía**. Lo descargué hace años, cuando
estaba empezando con SDL2, dentro de un paquete que reunía varios juegos
diferentes. El juego no traía más nombre que «juego» y no incluía ninguna
licencia visible.

Desconozco quién es su autor original. Los nombres internos de los recursos
(por ejemplo «dokme», «asli») sugieren un autor de habla persa, pero no tengo
datos para atribuirlo con certeza.

Si eres el autor original de este juego, o sabes quién es, abre un *issue* en
este repositorio y corrijo la atribución de inmediato — o retiro el
repositorio si así lo pides.

## Mi contribución

- Porte a Linux: el código original no funcionaba en Linux; lo adapté hasta
  dejarlo compilando y ejecutándose en Linux (reescritura y saneamiento del
  código, incluido `main.cpp`).
- Corrección de varios *bugs* del original.
- Construcción con CMake para Linux.

## Licencia

El código base no traía licencia conocida y su autoría no me pertenece, así
que **no aplico ninguna licencia propia al conjunto**: no puedo licenciar lo
que no es mío. Mi contribución (el porte a Linux y las correcciones) la ofrezco
libremente a quien use este repositorio en los mismos términos en que lo
encontró.

## Compilar

Requisitos (Debian/MX/Ubuntu): `build-essential`, `cmake` y las librerías de
desarrollo de SDL2: `libsdl2-dev`, `libsdl2-image-dev`, `libsdl2-ttf-dev`,
`libsdl2-mixer-dev`.

```bash
cmake -S . -B build
cmake --build build -j"$(nproc)"
```
