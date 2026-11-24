# CardputerZero-Music (fork, rama `carpetas`)

Fork para uso propio de la app oficial `CardputerZero/Music` (remoto `upstream`); el trabajo va
en la rama `carpetas` de `baltamir1978/Music`. Lo que cambia respecto a la oficial y cómo se
compila e instala está en el `README.md` (primer apartado). Comentarios, documentación y
commits nuevos, en español (`~/Proyectos/CLAUDE.md`); el código heredado sigue en inglés.

## Compilar y comprobar

```sh
./bootstrap.sh                                   # dependencias en dependencies/ (no van a git)
cmake -S . -B build/desktop -G Ninja -DMUSIC_USE_SDL=ON -DBUILD_TESTING=ON
cmake --build build/desktop && ctest --test-dir build/desktop --output-on-failure
ZLIB_LIBRARY=$HOME/.local/share/cardputer-zero-sdk/sdk_bsp-src/usr/lib/aarch64-linux-gnu/libz.a \
    PACKAGE_SUFFIX=carpetas1 packaging/deb/package_deb.sh     # dist/*.deb
```

En el aparato (`ssh cpzero`): `sudo dpkg -i` del `.deb` (el paquete está retenido con
`apt-mark hold` para que la tienda no ponga la oficial encima). La versión está en
`project(... VERSION)` del `CMakeLists.txt`; el `.deb` se copia a
`~/Documentos/Software/Music-carpetas-<versión>.deb` sustituyendo al anterior.

## Mandar a la tele (0.2.0) y a Google Cast (0.3.0)

- `src/cast/dlna.*`, `gcast.*`, `dlna_caster.*` y `google_cast.*` son **los mismos ficheros que
  en ZeroGallery** (`src/model/` y `src/platform/`), igual que `tests/dlna_test.cpp`,
  `tests/gcast_test.cpp` y `tests/fixtures`. Cómo probar Cast desde `granja-pi`, en el
  `CLAUDE.md` de ZeroGallery. Un arreglo en uno se copia tal cual al otro (y se pasan las pruebas de
  los dos). Las decisiones y lo aprendido de cada tele están en el `CLAUDE.md` de ZeroGallery.
- Aquí la tele es otra salida de `PlaybackModel`: con `_casting`, `playTrack` manda la canción
  a la tele en vez de abrir miniaudio, `update` lee de la tele estado y posición
  (`updateFromTv`), y al empezar cada canción se le da la siguiente (`queueNextOnTv`, la misma
  elección que hace `playRelative` al acabar). Si la tele cambia sola, `adoptNextFromTv`; si no
  sabe encadenar, `take_finished()` y `playRelative(1, true)` como siempre.
- `V` es `music_key::Cast` (`KEY_V` en `music_keypad.cpp`, `v` en el simulador). La lista de
  teles es `views/cast_picker_view.*`, sobre `lv_layer_top()`; con ella abierta, `MusicApp`
  le pasa las teclas (`onCastPickerKey`).
- Probar sin tocar el aparato: ver la memoria «Driving the Cardputer remotely» (ZeroRemote y
  captura de `/dev/fb0`). El estado de la LG se mira desde el portátil con SOAP
  (`GetTransportInfo`, `GetPositionInfo`, `GetMediaInfo` → `NextURI`); los puertos de la LG
  cambian cada vez que se enciende: hay que leerlos de su descripción (buscarla por SSDP desde
  el aparato, que el portátil tiene `ufw`).

## Estado

- **0.3.0 (24-nov-2026)**: Google Cast (Chromecast, Google TV, Nest). El código compartido
  se probó con un Google Nest Hub de la granja (música con carátula, su final, pausa,
  volumen); la app, en el aparato con la LG. **Sin probar por Bruno.**
- **0.2.0 (24-nov-2026)**: música en la tele por DLNA. Probado por Claude desde el aparato con
  la LG de Bruno (ver README). **Sin probar por Bruno.**
- **0.1.8 (16-nov-2026)**: carpetas en vez de álbumes y aleatorio por defecto.
