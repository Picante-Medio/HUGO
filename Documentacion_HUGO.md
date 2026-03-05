# Documento de arquitectura del motor H.U.G.O
- Escrito por los integrantes de 'Picante Medio' para la asignatura PIII del GDV. 
- Año 2026
- Explicación del nombre: HUGO is a unique game originator (sigla recursiva)

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
    - Viewport

- **Input**
    - InputListener

- **Audio**
    - AudioPlayer (contiene información de cada audio)
    - Audio3D
    - AudioListener

- **UI**
    - TextLabel
    - Button
    - Image
    - Container

## Sistemas de H.U.G.O
- RenderSystem
- InputSystem
- AudioSystem
- UISystem

# Arquitectura del core del motor

## Game
Clase de inicialización. Es responsable de cargar la biblioteca dinámica del juego en cuestión, inicializar todos los módulos de dependencias y cargar los datos de juego (como archivos chat de contenido de escenas).

Una vez inicializado el juego mantiene el bucle de tiempo y se compensa pérdida de frames en caso de que sea necesario.

### Scene Parser
Clase auxiliar encargada de leer los archivos `.chat` (mapas) de escenas, validarlos y devolver un objeto Scene con la escena construida. 

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
| Bullet3   | 3.25.5  | Físicas y detección de colisiones                                           |


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
Para permitir la comunicación del motor con librerías externas como OGRE3D, Physx, o Miniaudio, se ha pensado en la implementación de dichas funcionalidades dentro de los sistemas de las escenas de juego, formando parte de la arquitectura ECS pensada. Estos sistemas serían los principales representantes de dichas librerías, y servirían como capa intermedia de comunicación entre el mundo de nuestro motor y la funcionalidad que proporciona cada librería.

## Funcionalidad de los sistemas
Hacen uso de la información que contienen los componentes. Por ejemplo, RenderSystem (sistema de renderizado que hace uso de Ogre3D), recorrería todos las entidades que cuentan con una posición y una malla, y las actualizaría en su propio mundo para así mostrarse correctamente en pantalla. AudioSystem cargará archivos de audio en memoria (efectos de sonido) o streaming (canciones), cambiar parámetros y reproducir sonidos.

### AudioSystem
**Componentes**  
Este sistema ofrece dos componentes por defecto: AudioComponent y Audio3DComponent, siendo el primero aquello para modificar parámetros genéricos de un audio y lo otro para cambiar valores de un audio 3D como (factor de Doppler, distancia mínima y máxima, etc.). 

Además, tiene un archivo llamado ```AudioTags.h``` que contiene todas las etiquetas que ofrece el sistema (reproducir/parar un sonido, cambiar valores de un audio específico).

**Implementación del sistema**  
Para este sistema, se han implementado tres estructuras auxiliares:
- ```AudioInfo```: contiene el puntero inteligente de ma_sound y un booleano para indicar si está usado (necesario para el pool).
- ```AudioEntry```: incluye nombre de un audio, ruta relativa del archivo y el tipo de audio (SOUND2D, SOUND3D, MUSIC). La diferencia entre sound y music es que los audios de tipo sound se cargan en memoria mientras que music se reproduce en streaming.
- ```AudioPool```: incorpora un vector de AudioInfo (polifonía) y un índice para actualizar (Round-Robin).

En adición, hemos utilizado tres estructuras de datos: 
- ```audios``` :vector de datos de un audio (AudioEntry)
- ```pools```: vector de pool de audios (uno por cada audio)
- ```nameToID```: unordered_map de nombre-audioID (para acceder de forma más eficiente a un sonido que genera Miniaudio)

Es importante anotar que un objeto de tipo ```ma_sound``` no puede reproducirse a la vez, por eso hemos optado por añadir un vector para cada audio. 

Para acceder a un objeto ```ma_sound``` específico:
1. Reusar una voz libre previamente creada 
2. Si todas las voces están ocupadas, introducimos una voz nueva si el tamaño actual del vector no supera un límite, o descartamos una posición siguiendo la política Round-Robin.

**Bucle principal de este sistema**
```cpp
// la complejidad de cada bucle es, como máximo, queueSize (un número pequeño)
// aunque se añade la complejidad nos ofrece más riqueza y funcionalidad al sistema

//query para cambios en audios 3D
for (auto& pack->query<Audio3DInfoComponent, 
                       TransformComponent, 
                       Audio3DDirtyTag>()){
    process3DChanges(audio, tr, tag);
    //se puede coger cualquier componente porque comparten la entidad
    removeComponent<Audio3DDirtyTag>(tag.entity);
}

//query para cambios genéricos de audios
for (auto& pack->query<AudioComponent, 
                       AudioDirtyTag>()){
    processChanges(audio, tag);
    removeComponent<AudioDirtyTag>(tag.entity);
}

//query para reproducir audios
for (auto& pack->query<AudioComponent, 
                       AudioPlayTag>()){
    playAudio(audio.id);
    removeComponent<AudioPlayTag>(tag.entity);
}

//query para parar audios
for (auto& pack->query<AudioComponent, 
                       AudioStopTag>()){
    stopAudio(audio.id);
    removeComponent<AudioStopTag>(tag.entity);
}
```
**Carga de audio**  
Los audios a incluir al motor se leerán desde un archivo .txt llamado ```ÀudioList.txt``` que tiene el siguiente formato: 
``` txt
Cada línea sería un audio
# son comentarios
; separa cada campo
Sintaxis: nombre;ruta_relativa;tipo
tipo-> 0: sonido 2D; 1: sonido 3D; 2: música
```
Se llama en ```Core``` una función de ```AudioSystem``` para cargar todos los audios. 

## Forma de comunicación con estos sistemas
Una forma de introducir estas notificaciones es a través de **tags**. Los tags son simplemente componentes que un sistema añade a una entidad para que otro sistema lo detecte y haga algo en consecuencia. 

```cpp
// En un sistema cualquiera
EntityID msg = ECS.createEntity();
ECS.addComponent<UpdatedMeshMessageComponent>(msg, ...)
...

// En el OgreSystem
ECS.foreach<UpdatedMeshMessageComponent>([]() { /* Actualizar la malla de la entidad en Ogre3D */ })
```

### Ventajas de esta implementación
Al implementar las funcionalidades externas de las librerías como sistemas dentro de nuestra propia arquitectura ECS, se garantizan y facilitan varias cosas:

* No se rompe la arquitectura ECS al meter estas funcionalidades. Es decir, no se trata de forma distinta a estas funcionalidades y se garantiza la ejecución ordenada de sistemas que está pensada para el bucle principal.

* Como tentativa, se encapsularía toda la funcionalidad de cada librería en un sólo sistema, lo que provoca que el seguimiento de la comunicación con cada librería sea más cómodo e intuitivo.

* Se reutiliza código ya hecho. Al tratarse de implementar sistemas, no hace falta programar una infraestructura distinta para estas funcionalidades ni mantenerla, sino que se reutiliza una parte fundamental de la arquitectura.

## RenderSystem
Es el sistema que se encarga de gestionar el renderizado del motor. Conecta las entidades de HUGO con Ogre, permitiendo visualizar en pantalla las entidades del motor que tenga el RenderComponent, CameraComponent, ViewportComponent y LightComponent. Es el responsable de crear el SceneManager de Ogre.


## Responsabilidades Principales

- Crear y eliminar objetos de Ogre (entidades, sceneNode, luces, cámaras, viewports).
- Sincronizar el transform de las entidades en Ogre.
- Gestionar cámaras, viewports, materiales y el tamaño de la ventana.

## Componentes Relacionados

El RenderSystem trabaja con los siguientes componentes que pueden ser añadidos a las entidades:

| Componente | Descripción |
|------------|-------------|
| `RenderComponent` | Permite asociar un mesh y material a una entidad para que sea renderizada |
| `CameraComponent` | Define los parámetros de una cámara (FOV, planos near/far, tipo de proyección) |
| `ViewportComponent` | Define un viewport de renderizado (dimensiones, color de fondo, cámara asociada) |
| `LightComponent` | Define una luz (tipo, colores, atenuación, parámetros de sombras) |
| `TransformComponent` | Define la posición, rotación y escala de la entidad |

### Mapas de Correspondencia
El sistema utiliza mapas para relacionar las entidades HUGO con sus contrapartes en Ogre3D:

```cpp
std::unordered_map<EntityID, CameraInfo> cameraMap;     // Entidad -> Información de cámara
std::unordered_map<EntityID, ViewportInfo> viewportMap; // Entidad -> Información de viewport
std::unordered_map<EntityID, LightInfo> lightMap;       // Entidad -> Información de luz
```
## Estructuras Internas de Almacenamiento

Internamente, el sistema almacena los objetos de Ogre en structs, y utiliza estructuras de datos para relacionar cada entidad de HUGO con su objeto de Ogre.:

### CameraInfo
```cpp
struct CameraInfo {
    Ogre::Camera* ogreCamera = nullptr;   // Cámara de Ogre
    Ogre::SceneNode* node = nullptr;      // Nodo de escena que contiene la cámara
};
```
### OgreObject
```cpp
struct OgreObject {
    HUGO::OgreObjectID id;                    // Identificador único del objeto 
    Ogre::Entity* ogreEntity = nullptr;       // Entidad de Ogre (mesh)
    Ogre::SceneNode* ogreSceneNode = nullptr; // Nodo de escena
};
```

Para gestionar las entidades renderizables, se ha utilizado un TAD propio, llamada PagedArray, que consiste en un vector de arrays. Cada entidad renderizable tiene un OgreObjectID que corresponda a un objeto de Ogre. Y la correspondencia entre el ID y la posición del PagedArray se realiza mediante un vector idToIndex. Además, se utiliza un vector freeOgreObjectID para reutilizar IDs.

Faltan los mensajes de cuando se crean las entidades del render system desde fuera.

## Simulación de físicas
El caso de las físicas es distinto. Debido a la naturaleza de la librería física es necesario establecer un orden de ejecución muy concreto que no es posible garantizar si se tratan las físicas desde un sistema de ECS. Por esa misma razón hemos decidido implementarlas con una clase abstracta **Simulación**. 

### Implementación

**Interfaces**
- IPhysicsModule
- IphysicsSimulation

**Estructuras**
- BulletUtils
- Shape
    - BoxColliderInfo
    - SphereColliderInfo
    - CapsuleColliderInfo
    - CylinderColliderInfo
    - ConeColliderInfo
- RigidBodyComponent
- StaticRBTag
- KinematicRBTag

**Clases**
- BulletModule
- BulletSimulation
- HugoMotionState

### Pipeline de físicas a nivel de usuario
Para crear un cuerpo físico el usuario debe crear una estructura llamada ``Shape`` que recibe hasta cinco tipos distintos de información para crear una forma:

- Caja
- Esfera
- Cápsula
- Cilindro
- Cono

Esta información es representada a través de cinco estructuras que recibe como argumento la clase Shape.

>**Nota**: La clase ``Shape`` solo es capaz de recibir una estructura de información. Puede ser cualquiera de las cinco pero solo una de ellas por ``Shape`` (p.ej: una estructura ``Shape`` no puede tener un BoxColliderInfo y un CapsuleColliderInfo a la vez pero es posible hacer dos ``Shape`` una con un tipo de información y otra con otro). Para ello hace uso de ``std::variant``.

Cuando se crea un ``RigidbodyComponent`` se le asigna una ``Shape`` para definir su forma y tamaño. Esta diferenciación entre la forma y el cuerpo permite asociar a distintos cuerpos la misma forma y el mismo tamaño. En la simulación física, todos esos cuerpos también comparten los mismos datos de su forma. De esta forma el usuario puede reaprovechar una misma forma para varios cuerpos.

Para hacer que los cuerpos sean **Kinemáticos** o **Estáticos** se deben añadir las tags ``KinematicRBTag`` y ``StaticRBTag`` respectivamente a la entidad a la que se le añada el ``RigidbodyComponent`` inmediatamente después de hacerlo. Si se añade la tag en cualquier otro momento no se puede garantizar que se vaya a crear un cuerpo con las propiedades deseadas.

### IPhysicsSimulation
Esta es una interfaz que permite a cualquier usuario implementar la simulación como desee. Tiene métodos virtuales para destruir la simulación que esté gestionando ahora mismo y para hacer un step en el mundo de físicas.

> **Nota**: Un step es una simulación de un tiempo x (normalmente milesimas) que hace el mundo de físicas para simular el movimiento físico de los objetos afectados por el mundo en ese tiempo.

Nuestra implementación de la interfaz es ``BulletSimulation``. Esta interfaz contiene un mundo de Bullet (btWorld) y se encarga de inicializarlo y ejecutar su step cuando sea necesario. Además tiene un registro de todas las formas y todos los cuerpos que hay en la simulación con un ID asociado al componente ``RigidbodyComponent`` de HUGO que tenga una entidad asociada.

En cada frame se encarga de actualizar la información de los rigidbodies de bullet con respecto a los de HUGO si es que han cambiado, actualiza los transforms de bullet en función de los de HUGO para cuerpos kinemáticos y hace uso de una clase ``HugoMotionState`` que hereda de la interfaz ``MotionState`` de bullet creada especificamente para coordinar de forma automática los transforms de físicas con los de renderizado. Esta clase ``HugoMotionState`` es gestionada por la librería de bullet.

La sincronización que no ocurre a través de ``HugoMotionState`` se lleva a cabo usando queries de ECS de la escena asociada a la simulación. La simulación es también la encargada de crear un rigidbody y una shape de bullet si durante la sincronización encuentra un ``RigidbodyComponent`` de HUGO que no tiene uno de bullet asociado (es decir, si su ID es inválido).

### IPhysicsModule
Esta es una interfaz que se encarga de gestionar las simulaciones (IPhysicsSimulation) del motor. Tiene una lista de simulaciones activas e inactivas y normalmente habrá siempre una simulación por escena. Tiene métodos virtuales para crear una nueva simulación, destruirla, activarla, desactivarla y ejecutar el step de todas las simulaciones activas.

Nuestra implementación de la interfaz es ``BulletModule`` que simplemente se encarga de aplicar todas las gestiones ya mencionadas sobre las ``BulletSimulation`` registradas, usando cuando sea necesario los métodos que implementa la librería de bullet.

``BulletModule`` se inicializa en ``Core`` y se ejecutan sus métodos cuando es necesario para actualizar todas las simulaciones activas cuando corresponda.

### BulletUtils
Una estructura con conversiones de vectores y cuaterniones del motor al de la librería de físicas y viceversa para garantizar la buena comunicación entre ambos. Es necesario implementar nuevas transformaciones si se quisiera utilizar otro motor de físicas que usara vectores/cuaterniones distintos a los del motor.

# Pipeline de generación de contenido

## Modelos 3D
Para cargar modelos y animaciones en OGRE, la librería necesita archivos con extensión .mesh, .skeleton y .material. La herramienta que utilizaremos para la generación de modelos con texturas es Blender, así que los importaremos directamente en este formato desde Blender usando el plugin blender2Ogre.

## Imágenes
A la hora de usar imágenes, hemos distinguido dos tipos de ellas: vectoriales y no vectoriales.
- **Vectoriales:** El formato que usaremos será svg. Se emplearán este tipo de imágenes únicamente en elementos escalables de la UI, como botones. OGRE3D soporta svg de forma nativa
- **No vectoriales:** Los formatos permitidos son .png, .jpg, .bmp. OGRE3D soporta estos tipos de archivos.

## Audio
Los archivos de audio que admite Miniaudio son .mp3, .wav y .flac. Serán los únicos archivos de audio del motor. 

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
