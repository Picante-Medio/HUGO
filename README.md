# Documento de arquitectura del motor H.U.G.O
- Escrito por los integrantes de 'Picante Medio' para la asignatura PIII del GDV. 
- Año 2026
- Explicación del nombre: *HUGO is a unique game originator* (sigla recursiva)

# Arquitectura ECS
El manejo de entidades del motor se hará a través del patrón de diseño **ECS (Entity Component System).**  Hemos elegido ECS frente a EC para tener mayor control sobre el orden de ejecución del código y también por motivos didácticos: nunca hemos usado ECS en un proyecto de esta escala, es una oportunidad para aprender.

Las **entidades** toman la forma de un identificador, que no necesita almacenar ningún tipo de información. 
```cpp
using EntityID = int;
EntityID ent;
```

Los **componentes** solo son responsables de almacenar el estado del juego. Son porciones de la información de las entidades.
```cpp
struct Component {
    EntityID myEntity;
public:
    Component(EntityID myEntity) : myEntity {}
};
```

Los **sistemas** alteran la información provista por los componentes, añadiendo la capa de funcionalidad. Guardan un puntero compartido a la escena a la que pertenecen.
```cpp
// Los sistemas operan sobre entidades y componentes, a través de las escenas
class System {
protected:
    std::shared_ptr<Scene> scene;
public:
  System(std::shared_ptr<Scene> scene): scene(scene) {}
  virtual void update(double dt) = 0;
  virtual void fixedUpdate(double dt) = 0;
};
```

Las **escenas** son los contenedores de componentes y sistemas.
- Implementan la estructura sparse-set en dos clases, `EntityTable` y `ComponentDenseTable`, para almacenar los componentes de cada entidad *contiguos en memoria*
- Almacenan los sitemas en un vector y los actualizan
- Crean estructuras iterables para recorrer entidades que tienen ciertos componentes (Instancias de la clase `Query`)
    - La única forma de ver o alterar el contenido de los componentes es a través de estas queries
```cpp
class Scene {
protected:
  EntityTable entityTable;
  ComponentDenseTable componentTable;
  std::vector<std::shared_ptr<System>> systems;

public:
    void update(float dt); //Llama a update de todos los sistemas en la lista
    void fixedUpdate(float dt); //Llama a fixedUpdate de todos los sistemas en la lista

    // Manejo de entidades
    EntityID createEntity();
    void removeEntity(EntityID entity);

    // Manejo de componentes
    template <typename T, typename... Ts>
    void addComponent(EntityID entity, Ts &&...args);

    template<tpename T>
    void removeComponent(EntityID entity);

    // Accesor a modo de iterador a los componentes
    template <typename... Ts> 
    Query<Ts...> query();
};
```
\
# Componentes y sistemas implementados en el motor

## Componentes de H.U.G.O
- **Generales**
    - Transform
    - Parent
    - Children

- **Física**
    - RigidBody
    - Collider (y variantes)

- **Render**
    - Mesh (que contiene el material)
    - SkyBox
    - ParticleGenerator
    - Camera
    - Animation
    - Light

- **Input**
    - InputListener

- **Audio**
    - SoundPlayer
    - SoundListener

- **UI**
    - TextLabel
    - Button
    - Image
    - Container

## Sistemas de H.U.G.O
- RenderSystem
- PhysicsSystem
- InputSystem
- AudioSystem
- UISystem

# Arquitectura del core del motor

## Game
Clase de inicialización. Es responsable de cargar la biblioteca dinámica del juego en cuestión, inicializar todos los módulos de dependencias y cargar los datos de juego (config.lua para configuración general del juego y .tscn de la primera escena, nombre indicado en el archivo de configuración).

Una vez inicializado el juego mantiene el bucle de tiempo y se compensa pérdida de frames en caso de que sea necesario.

### Scene Parser
Clase auxiliar encargada de leer los TSCN de escenas, validarlos y devolver un objeto Scene con la escena construida. 

## Factorías
Se definen dos clases que serviran como factorías de objetos externos:

- ComponentFactory
- SystemFactory

Estas factorías contienen métodos de generación de componentes y sistemas respectivamente. Los componentes y sistemas se registran en las factorías mediante las macros (ver siguiente apartado).

## Macros de creación de juegos
A la hora de crear un juego lo que el usuario del motor debe hacer es definir sistemas y componentes. 

Para crear componentes y sistemas en lugar de heredar de clases expuestas al usuario se definen las siguientes macros:

```cpp
COMPONENT(ComponentName) {
    /*
        Implementación como en cualquier struct
        ...
    */
}

SYSTEM(SystemName) {
    /*
        Implementación como en cualquier clase
        ...
    */
}
```

Esto permite que la implementación de las factorías no sea responsabilidad de quien crea el juego, ya que por debajo estas macros registran los componentes

## Gestor de recursos
Se define como clase. Es responsable de cargar los assets de juego y tiene métodos para acceder a ellos por nombre y tipo.

# Dependencias externas
Las librerías que utilizará el motor son las siguientes:
| Librería  | Versión | Uso                                                                         |
| --------- | ------- | --------------------------------------------------------------------------- |
| OGRE3D    | 14.5.0  | Gráficos 3D (modelos, animaciones, shaders, generación de ventana, layouts) |
| SDL2      | 2.32.10 | Input (como dependencia de OGRE3D)                                          |
| Miniaudio | 0.11.24 | Implementación de efectos de sonido y música                                |
| Bullet3   | 3.2.5   | Físicas y detección de colisiones                                           |


# Estuctura de directorios del motor
El proyecto se construye con el uso de cmake, la estructura del directorio del proyecto es la siguiente:
\
. \
├── *CMakeLists.txt* -> Fichero de compilación de cmake \
├── *build/* -> Archivos de generación del proyecto, creados por cmake (.vcxproj, CmakeFiles/, etc.) \
├── *bin/* -> Archivos compilados del código (ejecutables, .libs y archivos de depuración) \
├── *dependencies/* -> Librerías externas que se usan para construir el motor. Se genera al compilar \
└── *src/* -> Código fuente del motor

# Implementación de las librerías externas
Para permitir la comunicación del motor con librerías externas como OGRE3D, Physx, o FMOD, se ha pensado en la implementación de dichas funcionalidades dentro de los sistemas de las escenas de juego, formando parte de la arquitectura ECS pensada. Estos sistemas serían los principales representantes de dichas librerías, y servirían como capa intermedia de comunicación entre el mundo de nuestro motor y la funcionalidad que proporciona cada librería.

## Funcionalidad de los sistemas
Hacen uso de la información que contienen los componentes. Por ejemplo, RenderSystem (sistema de renderizado que hace uso de Ogre3D), recorrería todos las entidades que cuentan con una posición y una malla, y las actualizaría en su propio mundo para así mostrarse correctamente en pantalla. PhysicsSystem se encargaría de recorrer a todas las entidades que cuentan con un Rigidbody y actualizaría su posición en base a las posiciones simuladas dentro del mundo de Physx.

## Forma de comunicación con estos sistemas
Una forma de introducir estas notificaciones es a través de **tags**. Los tags son simplemente componentes que un sistema añade a una entidad para que otro sistema lo detecte y haga algo en consecuencia. 

```cpp
// En un sistema cualquiera
EntityID msg = sene.createEntity();
sene.addComponent<UpdatedMeshMessageComponent>(msg, ...)
...

// En el RenderSystem
for(auto message : scene.query<UpdatedMeshMessageComponent>())
    ./* Actualizar la malla de la entidad en Ogre3D */

```

### Ventajas de esta implementación
Al implementar las funcionalidades externas de las librerías como sistemas dentro de nuestra propia arquitectura ECS, se garantizan y facilitan varias cosas:

- No se rompe la arquitectura ECS al meter estas funcionalidades. Es decir, no se trata de forma distinta a estas funcionalidades y se garantiza la ejecución ordenada de sistemas que está pensada para el bucle principal.

- Como tentativa, se encapsularía toda la funcionalidad de cada librería en un sólo sistema, lo que provoca que el seguimiento de la comunicación con cada librería sea más cómodo e intuitivo.

- Se reutiliza código ya hecho. Al tratarse de implementar sistemas, no hace falta programar una infraestructura distinta para estas funcionalidades ni mantenerla, sino que se reutiliza una parte fundamental de la arquitectura.

# Pipeline de generación de contenido

## Modelos 3D
Para cargar modelos y animaciones en OGRE, la librería necesita archivos con extensión .mesh, .skeleton y .material. La herramienta que utilizaremos para la generación de modelos con texturas es Blender, así que los importaremos directamente en este formato desde Blender usando el plugin blender2Ogre.

## Imágenes
A la hora de usar imágenes, hemos distinguido dos tipos de ellas: vectoriales y no vectoriales.
- **Vectoriales:** El formato que usaremos será svg. Se emplearán este tipo de imágenes únicamente en elementos escalables de la UI, como botones. OGRE3D soporta svg de forma nativa
- **No vectoriales:** Los formatos permitidos son .png, .jpg, .bmp. OGRE3D soporta estos tipos de archivos.

## Audio
Los archivos de audio que Miniaudio permite son .mp3, .wav y .ogg. Serán los únicos archivos de audio del motor. 

## Mapas de Juego
Usaremos GodotEngine como editor de escenas 3D, pero en vez de usar sus archivos de esccena nativos `.tscn`, crearemos un plugin de Godot que traduzca el contenido del editor a un formato propio que hemos llamado `.chat`, siglas de *chat is HUGO's alternative to tscn* (otra sigla recursiva).
    - El plugin creará archivos de GDScript a partir de las declaraciones .h de los componentes del motor y los juegos.
    - Las entidades existirán en un mapa del nodo padre de las escenas.
    - Los sistemas existirán en un enum del nodo padre de las escenas.

El siguiente es un ejemplo de cómo se verá una escena en un archivo `.chat`
```chat
SCENE miEscena

ENTITY Sinbad 0

COMPONENT Nickname
    label STRING
        my name is Sinbad!

ENTITY Cube 1

COMPONENT Transform
    position VECTOR_3 0 1 3

COMPONENT Counter
    count INT 0
```
