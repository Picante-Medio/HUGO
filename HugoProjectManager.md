# HugosProjectManager

## Descripcion

`HugosProjectManager` es una herramienta en Godot 4.6 para crear proyectos nuevos con estructura base, configuracion inicial y addons descomprimidos automaticamente.

Se apoya principalmente en:

- `tools/HugosProjectManager/project.godot`
- `tools/HugosProjectManager/scenes/hugo_project_manager.tscn`
- `tools/HugosProjectManager/scripts/hugo_project_manager.gd`

## Objetivo

Facilitar la creacion de un proyecto HUGO listo para usar, evitando pasos manuales repetitivos como:

- crear carpetas base,
- generar `project.godot`,
- generar `game.config`,
- crear una escena principal inicial,
- extraer addons desde `addons.zip`.

## Flujo de uso

En la interfaz principal (`hugo_project_manager.tscn`):

1. Escribir nombre del proyecto.
2. Seleccionar carpeta destino.
3. Pulsar `Crear proyecto`.

Validaciones implementadas:

- El nombre no puede estar vacio.
- La ruta destino debe existir.
- No debe existir ya una carpeta con el mismo nombre.

Si todo es valido, se ejecuta `_create_project()`.

## Que genera

### Carpetas base

Se crean las carpetas:

- `Resources`
- `MyComponents`
- `Systems`
- `Chats`
- `Prefabs`
- `Scenes`

### Archivos iniciales

1. `project.godot`
   - Nombre del proyecto (`config/name`).
   - Activacion del plugin `HugosChatPlugin`.

2. `game.config`
   - Linea 1: nombre del proyecto.
   - Linea 2: ruta de chats (`../Chats`).
   - Linea 3: escena principal (`Scenes/main_scene.tscn`).

3. `Scenes/main_scene.tscn`
   - Crea un nodo `SCENE_TRACKER` con script `scene_tracker.gd` del addon.

### Addons

`addons.zip` se abre con `ZIPReader` y su contenido se extrae en la raiz del nuevo proyecto, creando directorios y archivos segun corresponda.

## Persistencia de proyectos

El manager guarda y recupera rutas de proyectos en `user://savegame.save`:

- `add_project(path)` agrega y guarda.
- `remove_project(path)` elimina y guarda.
- `load_save()` restaura proyectos existentes y los instancia visualmente.

La representacion visual usa `project_entity.tscn` instanciado dentro de `world`.

## Escena principal del manager

`hugo_project_manager.tscn` incluye:

- Panel de creacion (nombre, ruta, boton de busqueda, boton crear).
- `RichTextLabel` para estado con mensajes de error/exito.
- `FileDialog` para seleccionar directorios.
- Una `escena` 2D con elementos fisicos y lectura visual de proyectos.