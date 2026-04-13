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
    - Panel
    - Text
    - Button
    - InputField
    - Slider

- **Partículas**
    - ParticleEmmiter

## Sistemas de H.U.G.O
- RenderSystem
- InputSystem
- AudioSystem
- UISystem
- ParticleSystem

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
| OGRE3D    | 14.5.0  | Gráficos 3D (modelos, animaciones, shaders, generación de ventana, layouts, partículas) |
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
Para permitir la comunicación del motor con librerías externas como OGRE3D, Bullet, o Miniaudio, se ha pensado en la implementación de dichas funcionalidades dentro de los sistemas de las escenas de juego, formando parte de la arquitectura ECS pensada. Estos sistemas serían los principales representantes de dichas librerías, y servirían como capa intermedia de comunicación entre el mundo de nuestro motor y la funcionalidad que proporciona cada librería.

## Funcionalidad de los sistemas
Hacen uso de la información que contienen los componentes. Por ejemplo, RenderSystem (sistema de renderizado que hace uso de Ogre3D), recorrería todos las entidades que cuentan con una posición y una malla, y las actualizaría en su propio mundo para así mostrarse correctamente en pantalla. AudioSystem cargará archivos de audio en memoria (efectos de sonido) o streaming (canciones), cambiar parámetros y reproducir sonidos.

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

### AudioSystem
**Componentes**  
El sistema utiliza tres componentes principales:
``` cpp
HCOMPONENT(AudioComponent)
    HUGO::AudioTypes type;
    HUGO::String audioName;
    HUGO::Float volume;
    HUGO::Bool loop;
    HUGO::Float pitch;
    double startTime;
    HUGO::Bool isPlaying;

    //identificador del audio (asignado por el módulo de audio assets)
    HUGO::AudioID audioId;

    //sonido en activo
    ma_sound* activePtr = nullptr;
    
    //callback cuando se acaba el sonido
    std::function<void(HUGO::AudioID)> onEnd; 
    
END_HCOMPONENT()
```

``` cpp
HCOMPONENT(Audio3DInfoComponent)
    // distancia de atenuación
    HUGO::Float minDistance;
    HUGO::Float maxDistance;

    // velocidad para el efecto doppler
    HUGO::Vector3 velocity;
    HUGO::Float dopplerFactor;

    // orientación para el cono
    HUGO::Vector3 direction;

    // rolloff
    HUGO::Float rolloff;

    // cono de emisión
    HUGO::Float coneInnerAngle;
    HUGO::Float coneOuterAngle;
    HUGO::Float coneOuterGain;
END_HCOMPONENT()
```

``` cpp
HCOMPONENT(AudioListenerComponent)
    // velocidad para el cálculo de doopler
    HUGO::Vector3 listenerVelocity;
    // volumen máster para este listener
    HUGO::Float masterVolume;
END_HCOMPONENT()
```

**Implementación del sistema**  
Para este sistema, se han implementado tres estructuras auxiliares:
- ***AudioInfo***
    ``` cpp
    struct AudioInfo {
        std::shared_ptr<Audio> ptr;
        bool inUse = false;
    };
    ```
    - Representa una voz individual.
    - `inUse` indica si está siendo utilizada actualmente.
    - usa `shared_ptr` para gestionar el lifetime del `ma_sound`.
- ***AudioPool***
    ``` cpp
    struct AudioPool {
        std::vector<AudioInfo> voices;
        size_t stealIndex = 0;
    };
    ```
    - Cada audio tiene su propio pool.
    - Permite polifonía
    - stealIndex implementa round-robin voice stealing.

En adición, hemos utilizado una estructuras de datos: 
- ```audios``` :vector de datos de un audio.
- ```pools```: vector de pool de audios (uno por cada audio).

Es importante anotar que un objeto de tipo ```ma_sound``` no puede reproducirse a la vez, por eso hemos optado por añadir un vector para cada audio. 

Para acceder a un objeto ```ma_sound``` específico:
1. Reusar una voz libre previamente creada 
2. Si todas las voces están ocupadas, introducimos una voz nueva si el tamaño actual del vector no supera un límite, o descartamos una posición siguiendo la política Round-Robin.

**Bucle principal de este sistema**
```cpp
void AudioSystem::update(double)
{
    // liberar los sonidos terminados de reproducir
    for (auto& pack : scene.query<AudioComponent>())
    {
        AudioComponent& c = pack.getConst<AudioComponent&>();
        if (c.activePtr && !ma_sound_is_playing(c.activePtr))
        {
            pack.modify<AudioComponent>([](AudioComponent& a){
                a.activePtr = nullptr;
                a.isPlaying = false;
            });
        }
    }
}

void AudioSystem::init(){
    engine = MiniaudioModule::Instance().getMiniaudioEngine();
    
    // configurar el tamaño correcto del mapa de sonidos
    ensurePoolSize();

    // conectar los eventos de creación de componentes
    scene.onAddComponent<AudioComponent>().connect(this, &AudioSystem::onAddAudioComponent);
    scene.onAddComponent<Audio3DInfoComponent>().connect(this, &AudioSystem::onAdd3DAudioComponent);

    // conectar los eventos de modificación de componente
    scene.onUpdateComponent<TransformComponent>().connect(this, &AudioSystem::onAn3DAudioMove);
    scene.onUpdateComponent<AudioComponent>().connect(this, &AudioSystem::onUpdateAudioComponent);
    scene.onUpdateComponent<Audio3DInfoComponent>().connect(this, &AudioSystem::onUpdate3DAudioComponent);
    scene.onUpdateComponent<AudioListenerComponent>().connect(this, &AudioSystem::onUpdateAudioListener);

}

// crear un puntero ma_sound para poder usar posteriormente
void AudioSystem::onAddAudioComponent(AudioComponent &c)
{
    acquireVoice(c);
}
#pragma endregion

// actualizar cambios relacionados con el audio genérico
void AudioSystem::onUpdateAudioComponent(AudioComponent &c)
{
    processChanges(c);
}

// actualizar cambios relacionados con el audio 3d
void AudioSystem::onUpdate3DAudioComponent(Audio3DInfoComponent &c)
{
    EntityID id = c.entity;
    if (!scene.hasComponent<AudioComponent>(id)) return;

    const AudioComponent& a = scene.getComponentConst<AudioComponent>(id);
    process3DChanges(a, c);
}

//actualizar audio listener
void AudioSystem::onUpdateAudioListener(AudioListenerComponent &l)
{
    EntityID id = l.entity;
    if (scene.hasComponent<TransformComponent>(id)){
        const TransformComponent& tr = scene.getComponentConst<TransformComponent>(id);
        updateListeners(l, tr);
    }
}

// por seguridad, convierto el tipo de audio a 3d
void AudioSystem::onAdd3DAudioComponent(Audio3DInfoComponent &c)
{
    scene.modifyComponent<AudioComponent>(c.entity, [](AudioComponent& a){
        a.type = HUGO::AudioTypes::SOUND3D;
    });
}

// actualizar la posición de un sonido 3d cuando se ha movido
void AudioSystem::onAn3DAudioMove(TransformComponent &tr)
{
    if (scene.hasComponent<AudioComponent>(tr.entity) && scene.hasComponent<Audio3DInfoComponent>(tr.entity)){
        const AudioComponent& a = scene.getComponentConst<AudioComponent>(tr.entity);
        set3DPosition(a.activePtr, tr.getPosition());
    }
}
```
**Carga de audio**  
Se realiza una búsqueda recursiva desde una ruta relativa base y se guardan el nombre del sonido, la ruta relativa correspondiente y se le asigna un ID exclusivo. Este diseño permite más libertad al usuario de organizar los assets de audio. También permite al usuario crear dos tipos de audio (por ejemplo, 2D y 3D) usando una sola ruta.

Los audios a incluir al motor se guardarán en un diccionario donde se puede acceder tanto por su nombre como por un identificador único desde cualquier sitio del motor usando el módulo de `AudioAssetsModule`.  

Para consultar información relacionada con los assets de audio, se crea un archivo ```AudioAssetsDebug.log``` que tiene el siguiente formato: 
``` log
AUDIO ASSETS DEBUG
========================================
Generated: Apr 12 2026 22:40:15
Base Path: ../Resources/audios/
Total Assets: 2
========================================

1. lucky
   ID: 0
   Path: ../Resources/audios//music\lucky.mp3
   SUCCESS

2. risa
   ID: 1
   Path: ../Resources/audios//sfx\risa.wav
   SUCCESS

========================================
SUMMARY
========================================
Total Assets: 2
Files Found: 2
Files Missing: 0
========================================
```
Dicho módulo se inicializa en ```Core```. 

## RenderSystem
Es el sistema que se encarga de gestionar el renderizado del motor. Conecta las entidades de HUGO con Ogre, permitiendo visualizar en pantalla las entidades del motor que tenga el RenderComponent, CameraComponent, ViewportComponent y LightComponent. Es el responsable de crear el SceneManager de Ogre.


### Responsabilidades Principales

- Crear y eliminar objetos de Ogre (entidades, sceneNode, luces, cámaras, viewports).
- Sincronizar el transform de las entidades en Ogre.
- Gestionar cámaras, viewports, materiales y el tamaño de la ventana.

### Componentes Relacionados

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
### Estructuras Internas de Almacenamiento

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
El módulo de físicas se carga junto con el resto de módulos en el core del motor. Se encarga de cargar una simulación física por escena y almacenarlas para intercambiarlas y simular la escena activa en el momento.

### Implementación

**Interfaces**
- IPhysicsModule
- IphysicsSimulation

**Estructuras**
- HugoPhysics
- ColliderComponent
- RigidBodyComponent

**Clases**
- BulletModule
- BulletSimulation
- HUGOMotionState
- HUGODispatcher

### HugoPhysics

Archivo que define en enumerados y structs tanto las formas que puede tener un objeto físico en **HUGO** y su información (**structs** con el sufijo "``ColliderInfo``") como los tipos posibles de cuerpos rígidos (**dinámico**, **estático** y **kinemático**).

### ColliderComponent

El ``ColliderComponent`` es el componente de las físicas de **HUGO** que se usa para definir la forma de un objeto físico. Al ser inicializado debe definirse su forma accediendo a su parámetro ``info`` o de lo contrario no se cargará ninguna forma. Un objeto físico no puede cambiar de forma, por lo que **la forma de un collider se tiene que definir obligatoriamente al añadir el componente** o no funcionará. Si se cambia el parámetro de info después de la inicialización del componente **no ocurrirá nada**. Además de su información, un collider también define las siguientes propiedades físicas:

- Restitución
- Fricción
- Offset de posición y rotación

Los atributos del ``ColliderComponent`` solamente deben ser accedidos a través de getters y setters definidos en la estructura. Esto se debe a que cuando se modifican los valores del cuerpo físico de **HUGO** también deben modificarse los de **bullet**. Para ello existen una serie de flags que se activan cuando el usuario modifica alguno de los valores del **Rigidbody** utilizando sus setters. De esta forma, en cada frame, actualiza los valores en **bullet** en función de las flags.

También es posible dibujar la forma que tiene el **Collider** seteando su atributo ``useGizmo`` a ``true``. Esto es especialmente útil para hacer pruebas y ver el tamaño real del **Collider**. La forma en la que se dibuja se describe más adelante.

### RigidbodyComponent

Un **Rigidbody**, o un cuerpo rígido, es la forma que tiene **HUGO** y otros muchos motores de definir la forma y las propiedades de objetos que son afectados por el sistema de físicas (gravedad, velocidad, fuerzas, etc.). Para añadir un **RigidBody** a una entidad es necesario que tenga antes un ``ColliderComponent`` que defina su forma. **Si a una entidad se le añade un RigidBody sin tener un Collider, no ocurrirá nada**. En la inicialización del componente se debe definir el tipo de cuerpo que se está creando (**dinámico**, **estático** o **kinemático**, por defecto **dinámico**) utilizando el método ``setBodyType``. El cuerpo se crea del tipo especificado. **Si se cambia el tipo de cuerpo después de haberlo inicializado, no ocurrirá nada**. En nuestro motor un **Rigidbody** define las siguientes propiedades físicas:

- Masa
- Damping lineal y angular
- Velocidad lineal y angular
- Gravedad

Al igual que el ``ColliderComponent`` y por los mismos motivos, sus atributos deben ser accedidos a través de sus getters y setters.

También existen setters para aplicarle impulso y/o fuerza a un **Rigidbody** en el frame en el que se setea dicha fuerza/impulso.

### Sleep y Active

Los colliders tienen dos posibles estados: **Active** e **Inactive**.  
Los cuerpos definen un estado a mayores: **Asleep**.

- **Active** es el estado por defecto. Si un **Collider** está activo, se actualizará en cada frame y se procesarán las colisiones que tenga. 

- **Asleep** es un estado pensado para ahorrar recursos. Si un cuerpo no se mueve más de un umbral en un tiempo determinado, **bullet** considerará que ese cuerpo está quieto y no lo actualizará. De esta forma no se procesan datos sobre una entidad que no cambia. **El estado del RigidBody pasa a ser Activo en cuanto actualiza alguna de alguna forma alguna de sus propiedades**. Las propiedades de un RigidBody pueden cambiar porque:

    - El usuario cambia la propiedadcon su setter
    - El RigidBody colisiona contra otro
    - El usuario aplica una fuerza/impulso sobre el rigidbody

    Es posible dormir un **RigidBody** manualmente y setear a mano el umbral lineal y angular a partir del cual un objeto se duerme con los setters ``setAsleep``, ``setLinearSleepingThreshold`` y ``setAngularSleepingThreshold`` respectivamente.

- **Inactive** dejará al **Collider** en un estado en el que no se actualiza en ningún frame. Sigue siendo posible cambiar los valores del **Collider** y que estén cambiados al reactivarlo, pero mientras esté en este estado **el RigidBody asociado a dicho collider no se va a ver afectado por ningún tipo de fuerza/impulso y el Collider no va a detectar colisiones de ningún tipo**. Se puede activar/desactivar el Collider con el método ``SetActive``.

>**Nota**: un **RigidBody** inactivo no se puede poner en estado dormido usando ``setAsleep``, es necesario activarlo primero para poder dormirlo. Esto debido a que el estado **Asleep** es innecesario si un objeto está inactivo.

### Colisiones

Para la gestión de colisiones, **HUGO** tiene una clase ``HUGODispatcher`` que se encarga de actualizar en cada frame una propiedad del ``ColliderComponent`` llamada ``collisionInfo``, accesible a través del método ``getCollisionInfo``. Esta propiedad es una lista de ids de las **entidades** con las que está colisionando un **Collider**. En cada frame ``HUGODispatcher`` recorre todas las colisiones que ha habido en el mundo fisico y añade a cada **Collider** que haya chocado con otro el id de la **entidad** a la que pertenece. Por supuesto, una colisión solo se procesa si ambas entidades tienen **Colliders**. Por lo tanto se puede asegurar que todas las entidades registradas en el ``collisionInfo`` de un ``ColliderComponent`` tienen un **Collider** en el frame en el que han sido registradas.

La lista se actualiza en cada frame. Esto facilita las interacciones entre entidades al colisionar. Por ejemplo, imaginemos que queremos hacer en un sistema que cambie el parametro "bar" de todas las entidades con "FooComponent" que choquen contra un objeto especifico.

````cpp

void init(){

    objectId = scene->createEntity();

    // ... añade Transform, Collider, etc.

    EntityID e1 = scene->createEntity();

    // ... añade Transform, Collider, FooComponent, etc.

    EntityID e2 = scene->createEntity();

    // ... añade Transform, Collider, NO FooComponent, etc.
}

void update(dt){
    
    for(auto collisionId : scene->getComponent<ColliderComponent>(objectId).getCollisionInfo()){
        if(scene->hasComponent<FooComponent>(collisionId))
            scene->getComponent<FooComponent>(collisionId).bar += 10;
            // Solo se aplica este cambio sobre e1
    }
}
````

### Debug

El sistema de **Debug** forma parte del sistema de **Render**. Comprueba en todos los **Colliders** el parámetro ``useGizmo`` y si está a ``true`` utiliza un **material** de cables para dibujar la forma del collider utilizando la información de ``info`` en ``ColliderComponent`` y su posición.

### Pipeline de físicas a nivel de usuario
Para crear una entidad con un componente físico el usuario debe añadirle un componente ``ColliderComponent`` e inicializarlo con una forma **al menos**. Opcionalmente se pueden inicializar más valores con sus setters y getters. Por ejemplo:

```cpp
#include "HugoPhysics.h"
#include "ColliderComponent.h"

EntityId e1 = scene->createEntity();

// Añadimos transform

scene->AddComponent<ColliderComponent>(e1, [](ColliderComponent &col){
    col.info = HUGO::BoxColliderInfo(HUGO::Vector3(0.5f, 0.5f, 0.5f));
    // Esto le da una forma de caja, si queremos otra forma usaremos otro
    // ColliderInfo
});
```

Si queremos que una entidad tenga un **RigidBody** debemos añadir **a una entidad que ya tenga un Collider añadido como ya se ha explicado** un componente ``RigidBodyComponent`` de la forma que sigue:

```cpp
#include "HugoPhysics.h"
#include "ColliderComponent.h"
#include "RigidBodyCOmponent.h"

EntityId e1 = scene->createEntity();

// Añadimos transform y collider

scene->AddComponent<RigidBodyComponent>(e1, [](RigidBodyComponent& rb){
    rb->setBodyType(HUGO::BodyType::STATIC);
    // Por defecto el BodyType de un RigidBody es HUGO::BodyType::DYNAMIC
    // Si queremos que el tipo de cuerpo sea otro es necesario
    // especificarlo de esta forma
});
```

No es necesario añadir el ``RigidBodyComponent`` inmediatamente después del ``ColliderComponent``. Mientras se añada primero el **Collider** y después el **RigidBody**, se puede añadir en cualquier momento y se actualizará de forma correspondiente.

>**Nota**: Se pueden crear desde **Hugodot** objetos con físicas siempre que se defina el parámetro info de ``ColliderComponent`` como ya hemos definido y se añada el **Collider** antes que el **RigidBody**. 

### IPhysicsSimulation
Esta es una interfaz que permite a cualquier usuario implementar la simulación como desee. Tiene métodos virtuales para destruir la simulación que esté gestionando ahora mismo y para hacer un step en el mundo de físicas.

> **Nota**: Un step es una simulación de un tiempo x (normalmente milesimas) que hace el mundo de físicas para simular el movimiento físico de los objetos afectados por el mundo en ese tiempo.

Nuestra implementación de la interfaz es ``BulletSimulation``. Esta interfaz contiene un mundo de Bullet (btWorld) y se encarga de inicializarlo y ejecutar su step cuando sea necesario. Además tiene un registro de todas las formas y todos los cuerpos que hay en la simulación con un ID asociado a sus componentes ``ColliderComponent`` y ``RigidbodyComponent`` de HUGO que tenga una entidad asociada.

En cada frame se encarga de actualizar la información de las entidades de bullet con respecto a los de HUGO si es que han cambiado. En el caso de los **RigidBodies** además, actualiza los transforms de bullet en función de los de HUGO para cuerpos kinemáticos y hace uso de una clase ``HugoMotionState`` que hereda de la interfaz ``MotionState`` de bullet creada especificamente para coordinar de forma automática los transforms de físicas con los de renderizado. Esta clase ``HugoMotionState`` es gestionada por la librería de bullet.

La sincronización que no ocurre a través de ``HugoMotionState`` se lleva a cabo usando queries de ECS de la escena asociada a la simulación. La simulación es también la encargada de crear un rigidbody y/o un collisionObject con su shape de bullet si durante la sincronización encuentra un ``RigidbodyComponent`` o un ``ColliderComponent`` de HUGO, respectivamente, que no tiene uno de bullet asociado (es decir, si su ID es inválido).

**Los Colliders se comportan como objetos kinemáticos por defecto. Esto es porque no tienen un cuerpo físico que simular y por lo tanto la simulación física no puede aplicarle fuerzas, velocidad, etc.**

### IPhysicsModule
Esta es una interfaz que se encarga de gestionar las simulaciones (IPhysicsSimulation) del motor. Tiene una lista de simulaciones activas e inactivas y normalmente habrá siempre una simulación por escena. Tiene métodos virtuales para crear una nueva simulación, destruirla, activarla, desactivarla y ejecutar el step de todas las simulaciones activas.

Nuestra implementación de la interfaz es ``BulletModule`` que simplemente se encarga de aplicar todas las gestiones ya mencionadas sobre las ``BulletSimulation`` registradas, usando cuando sea necesario los métodos que implementa la librería de bullet.

``BulletModule`` se inicializa en ``Core`` y se ejecutan sus métodos cuando es necesario para actualizar todas las simulaciones activas cuando corresponda.

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
