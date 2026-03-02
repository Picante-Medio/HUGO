# Idea 1 - Componentes y sistemas
Podríamos tener un componente InputListener con un array de booleanos de tamaño `SDLK_COUNT`, indicando para cada entrada de SDLK si dicha acción está pulsada. Un sistema de input entonces escribiría sobre esta lista cada vez que na acción se pulsara, algo así:

```cpp
struct InputListener {
    std::array<bool, SDLK_COUNT> isPressed;
}
```

Esto no rompe ECS, pero tiene el problema de que estamos constantemente escribiendo la misma información en todos los componentes. No es ideal

# Idea 2 - gestor de input global basado en teclas
Visto que la idea de antes escribía la misma información en múltiples componentes, el siguiente paso obvio es crear un gestor global que tenga métodos para detectar si una tecla ha sido pulsada o soltada, algo de este estilo:

```cpp
class InputHandler { //Singleton 
    std::array<bool, SDLK_COUNT> isPressed;
public:
    bool isPressed(SDLKcode key) { return isPressed[key]; }
    bool wasJustPressed(SDLKcode key) { ... }
    bool wasJustReleased(SDLKcode key) { ... }
}
```

Esto resuelve el problema explicado anteriormente

# Idea 3 - gestor de ACCIONES global
Ya puestos a crear una clase singleton, podemos crear una un poco más especiaizada, que nos permita crear "acciones" y asignar inputs a dichas acciones. Esto se parece bastante al input system "nuevo" de unity y a la forma que tiene godot de gestionar entrada, sería algo así:

```cpp

class InputHandler { // Singleton
    std::unordered_map<std::string, std::vector<SDLKcode>> actions;
    std::array<bool, SDLK_COUNT> isPressed;

public:

    void registerAction(std::string actionName, std::vector<SDLKcode> assignedKeys);
    bool isActionPressed(std::string actionName);
    bool wasActionJustPressed(std::string actionName);
    bool wasActionJustReleased(std::string actionName);
}
```

Esta implementación es más cómoda pero es algo más lenta por el uso de mapas y porque la posibilidad de tener la misma tecla asignada a varias acciones hace que se tengan que hacer múñtiples comprobaciones. Si en lugar de esto limitásemos cada tecla a una sola acción podríamos tener una implementación más parecida a esta idea:


```cpp

class InputHandler { // Singleton
    std::array<std::string, SDLK_COUNT> actionOfKey;
    std::unordered_map<std::string, bool> actionPressed;
public:
    // Si una tecla aparece dos veces, se sobreescribe su acción asociada
    void registerAction(std::string actionName, std::vector<SDLKcode> keys);
    bool isActionPressed(std::string actionName);
    bool wasActionJustPressed(std::string actionName);
    bool wasActionJustReleased(std::string actionName);
}
```

Aquí ganamos bastante en eficiencia, pero perdemos algo de flexibilidad en las acciones. Ambas formas de implementación tienen sus casos de uso y sus limitaciones
