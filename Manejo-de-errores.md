La versión propuesta previamente del manejo de errores era la siguiente:
```cpp
template<typename T>
struct ErrorOrValue {
    bool isError;
    std::string errorDescription;

    T value;
}
```

Esta implementación tiene los siguientes problemas: 
- No podemos tener variantes de error (no hay polimorfismo) a no ser que: heredemos de `ErrorOrValue`, lo que añade mucha información sobrante al error; o añadamos un enum que contenga las variantes, que ess muy incómodo de editar. En resumen, escala mal.
- Si ignoramos el booleano isError y usamos el valor igualmente, el programa tiene comportamiento indeterminado, porque T no tiene valor

Ambos problemas se pueden resolver de forma sencilla usando `std::variant`, que es básicamente un `union` de C, pero en la librería estándar, así que tira excepciones con mal uso.
La versión nueva del manejo de errores sería la siguiente:
```cpp
struct Error {
    virtual std::string description() = 0;
};

template<typename T>
class ErrorOrType {
    bool isError;
    std::variant<Error, T> info;

public:
    ErrorOrType(Error err) : isError(true), info(err) {}
    ErrorOrType(T value) : isError(false), info(value) {}

    bool isError() {return isError; }
    Error getError() {return std::get<Error>(info); };
    Error getValue() {return std::get<T>(info); };
};
```

ErrorOrValue pasa a ser una clase con variantes. De esta manera si no comprobamos primero que el error existe, se lanzará una excepción en tiempo de ejecución. Además (si no he leído mal), acceder a un valor sin usar `std::get` lanza un warning.

Si queremos crear errores específicos, podemos heredar de `Error` en vez de heredar de `ErrorOrValue,` implementando el método `description()`. Propongo dejar error como clase virtual pura y crear tipos de errores para el manejo. Un tipo útil para tener en el motor sería el StringError, para errores que solo necesitan descripción: 
```cpp
struct StringError {
    std::string errorDesc;
    std::string description() override { return errorDesc; }
}
```

Ahora una llamada a un método que devuelva error se ve así:
```cpp
//Este método lanza un error
ErrorOrType<int> fiveOrError() {
    ...
    if(...) return ErrorOrType<int>(StringError{"something bad has hapenned"});
    else return ErrorOrType<int>(5);
}

//Este método maneja un error pero no interrumpe el programa
void foo() {
    ErrorOrType<int> maybeFive = fiveOrError();

    if(maybeFive.isError()) {
        std::cout << "Five was not five\n Error: " << maybeFive.getError();
        return;
    }
    std::cout << "Five is " << maybeFive.getValue() << "!\n";
}
```

Como se pueden crear variantes de error, se pueden comparar errores con sus tipos para tratarlos de forma especial usando condicionales y `std::dynamic_cast`
```cpp
void foo() {
    ErrorOrType<int> maybeFive = fiveOrError();

    if(maybeFive.isError())
        if(std::dynamic_cast<CoolError>(maybeFive.getError())
            std::cout << "There was an error, but a very cool one!: " << maybeFive.getError();

        else 
            std::cout << "Five was not five\n Error: " << maybeFive.getError();

        return;
    }
    std::cout << "Five is " << maybeFive.getValue() << "!\n";
}
```
