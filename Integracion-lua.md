# Objetivo
Lo que queríamos lograr a través del uso de lua en nuestro motor era:
- Que lua tuviera acceso a funciones declaradas por el motor o el usuario en C++
- Que desde C++ se pudieran ejecutar funciones declaradas por el usuario en lua
- Que un intérprete de lua se ejecutase junto con el motor para llamar a todos estos bindings, en forma de consola de comandos

# Integración
La capa de integración con lua en nuestro motor está contenida en la clase `LuaBinder`. `LuaBinder` es un singleton inicializado por el motor y compartido a la dll del juego durante el linkado dinámico. A través de LuaBinder, tanto el motor como el usuario pueden declarar funciones y métodos en C++ que quedarán registrados como funciones en Lua para su posterior uso en scripts o desde la consola de comandos.

## Registro de funciones
Para registrar una *función* que no pertenece a ninguna clase, se puede usar el método `registerFunc`. Su declaración es la siguiente:
```cpp
template<typename ReturnType, typename ...ArgTypes>
void registerFunc(ReturnType(*function)(ArgTypes...), HUGO::String luaName);
```

`registerFunc` es genérico en el tipo de retorno de la función (`ReturnType`) y también en los tipos de los argumentos de la misma (`...ArgTypes`). Además del puntero a función, el método recibe `luaName`, que es el nombre que se le da a la función global de lua que llama a la implementación de la función registrada. Se puede registrar una función en lua de la siguiente manera:

```cpp
int sumar(int a, int b) { return a+b; }

LuaBinder::getInstance().registerFunc<int, int, int>(sumar, "sumar");
```

## Registro de métodos
Para registrar un *método* propio de una instancia de una clase, se puede usar el método `registerMethod`. Su declaración es la siguiente:
```cpp
template<typename ClassType, typename ReturnType, typename... ArgTypes>
void registerMethod(ClassType& obj, ReturnType(ClassType::*fn)(ArgTypes...), HUGO::String name);
```

`registerMethod` es muy parecido a `registerFunc`, solo que también necesita un objeto del tipo `ClassType`, que es el dueño del método que se va a llamar en la función global de lua. Debido a esto, si el método a registrar es de un sistema, debe llamarse a `registerMethod` en el método `onLoad` del mismo. Aquí un ejemplo con la función `playAudioLUA` del `AudioSystem`:

```cpp
void AudioSystem::onLoad(){
    LuaBinder& luaBinder = LuaBinder::getInstance();
    luaBinder.registerMethod<AudioSystem, void, int, int>(*this, &AudioSystem::playAudioLUA, "playAudio");
}
```

## Llamadas a lua desde C++
Para ejecutar código de lua desde C++, se pueden usar las funciones `interpretString` y `getFromString`. Ambas interpretan una `HUGO::String` dada como código válido de lua y le piden al intérprete que lo ejecute, la diferencia es que `getFromString<T>` devuelve un valor del tipo T si este es el tipo de retorno de la instrucción llamada de lua. Por ejemplo:

```cpp
LuaBinder::getInstance().interpretString("print('hello, world!')")
auto five = LuaBinder::getInstance().getFromString("sumar(3, 2)")
```
Nótese que se ha llamado a una función previamente registrada en C++.

Ambas funciones pueden errar si el código no es válido para interpretarse en Lua y, además, `getFromString` fallará si el código interpretado no ha devuelto un valor o el valor devuelto no es del tipo dado. Para más información sobre errores se recomienda leer [[manejo-de-errores]].

## Detección de scripts del usuario
Para hacer accesibles archivos de lua al intérprete en tiempo de ejecución se usará el método `requireFolder`:
```cpp
void requireFolder(HUGO::String folderPath);
```

El usuario puede hacer scripts de lua que interactúen con el motor si son llamados, para ello debe crear archivos `.lua` en algún lugar de su juego **que solamente contengan funciones globales**. Si esto se cumple, y todos los scripts están en la misma carpeta, una sola llamada al método `requireFolder` hará todas las funciones declaradas accesibles a `LuaBinder` y, por lo tanto, a la consola de comandos.

Por defecto, el motor hace `requireFolder` en `src/lua/`, pero no asume nada sobre dónde almacenará el usuario sus scripts.

# Limitaciones
Puesto que lua solo tiene 5 tipos: `number`, `string`, `boolean`, `function` y `table`, hemos tenido que limitar los tipos registrables en LuaBinder a `int`, `HUGO::Int`, `float`, `double`, `HUGO::Float`, `bool`, `std::string`, `char\*` y `HUGO::String`.

Para registrar una función o método que tenga argumentos o tipo de retorno distintos, se recomienda crear una función análoga, que convierta tipos válidos a tipos no válidos, cuyo nombre tenga el sufijo *LUA*. Por ejemplo, `playAudioLUA`:
```cpp
void AudioSystem::playAudioLUA(int id, int type) {
    if (type >= HUGO::AudioTypes::LAST_TYPE){
        return;
    }
    playAudio(static_cast<HUGO::AudioID>(id), static_cast<HUGO::AudioTypes>(type));
}
```

# ¿Dónde se usa lua?
Lua puede usarse para hacer comportamiento para los juegos, creando entidades y componentes o llamando a funciones de audio, animaciones, etc. aunque, realmente, el uso principal de lua es en *debug de los juegos*, gracias a la consola de comandos que permite interpretar lua en tiempo de ejecución.

# Consola de comandos

