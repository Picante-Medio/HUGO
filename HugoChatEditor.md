# HugoChatEditor

## Descripcion

`HugoChatEditor` es una herramienta en Godot 4.6 orientada a editar contenido de chats y sincronizar datos del proyecto HUGO usando un plugin de editor.

Su configuracion principal vive en:

- `tools/HugoChatEditor/project.godot`
- `tools/HugoChatEditor/addons/HugosChatPlugin/plugin.cfg`
- `tools/HugoChatEditor/addons/HugosChatPlugin/hugos_chat_plugin.gd`

## Integracion en Godot

En `project.godot`:

- Se habilita el plugin `res://addons/HugosChatPlugin/plugin.cfg`.
- Se define `hugo/chats_path` para apuntar a la carpeta de chats.

## Flujo del plugin

El script `hugos_chat_plugin.gd` implementa un `EditorPlugin` con este comportamiento:

1. Al activar plugin, conecta `scene_saved`.
2. Al entrar al arbol, registra plugin de inspector y agrega un dock de configuracion.
3. Al guardar escena:
   - Si se mantiene presionada la tecla `R`, ejecuta sincronizacion de scripts (`ScriptsSyncronizer.sync()`).
   - Si no, escribe la escena/chat (`SceneWriter.write()`).
4. Al salir, limpia inspector plugin y controles del dock/toolbar.

## Estructura relevante

Directorios y archivos utiles dentro de `tools/HugoChatEditor`:

- `addons/HugosChatPlugin/` (plugin de editor)
- `ChatsTest/` (ejemplos de archivos `.chat`)
- `Components/` (componentes GDScript)
- `Prefabs/` (escenas reutilizables)
- `Escena.chat`, `SCENE_TRACKER.chat` (artefactos de chat)

## Uso rapido

1. Abrir `tools/HugoChatEditor` como proyecto Godot 4.6.
2. Verificar que el plugin este habilitado en `Project Settings`.
3. Editar contenido/escenas segun flujo del editor.
4. Guardar escena normalmente para escritura, o guardar con `R` para sincronizar scripts.