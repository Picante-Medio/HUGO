# Guías de estilo de código


## Nomenclatura
Se asignarán siempre nombres en inglés.
- Variables: se empleará camelCase.  
    ```
    myVar
    ``` 
- Funciones: se empleará camelCase.  
    ```
    myFunction()
    ```
- Clases: se empleará PascalCase.
    ```
    MyClass
    ```
    Para nombrar componentes y sistemas, se añadirá ``Component`` o ``System`` según corresponda al final del nombre, para facilitar identificar su propósito.
    ```
    TransformComponent, HealthSystem
    ```
- Macros y constantes: se empleará SCREAMING_SNAKE_CASE.
    ```
    INT_MAX
    const double PI = 3.14159264;
    ```  

No comenzar nunca un nombre por ```_```, ya que puede dar problemas por colisionar con nombres reservados para el compilador y la librería estándar.  
  

## Clases
Las clases tendrán la forma:
```
#include <...>
#include "..."

class MyClass
{

    private:

    int m_data;

    protected:

    ...

    public:

    MyClass(int data)
    : m_data(data)
    {

    }

    // this function does something
    int myFunction(); 

};
```
Por lo general, los métodos y atributos de clase tendrán el mayor nivel de protección posible. Es decir, serán privados salvo que tengan que ser protegidos, y serán protegidos salvo que tengan que ser públicos.

No se repetirán las secciones public, protected y private, de modo que los métodos y atributos de un mismo nivel de protección estarán todos agrupados en la misma sección.

## Comentarios
El código se comentará siempre en inglés, siendo consistentes con la nomenclatura de clases, funciones y variables.

## Punteros
No se utilizarán punteros crudos (salvo en casos específicos). Esto es, nunca escribir ```new``` y ```delete```. Utilizar siempre en su lugar punteros inteligentes.

Utilizar ```nullptr``` para punteros nulos, en lugar de ````NULL```` o ```0```.

## Asserts
Los asserts no se evalúan en modo release. No escribir código significativo (es decir, que altere el comportamiento del programa) dentro de un assert, pues el comportamiento del programa se vería alterado entre ```debug``` y ```release```.

## Keyword ``auto``
El uso de la palabra clave ``auto`` se reservará exclusivamente para iteradores y expansión de tuplas. Para cualquier otro tipo de variables se escribirá explícitamente su tipo.

## Casting
Se evitará el C-Style casting: ``(int) myNum``, por ser arriesgado al ignorar las comprobaciones de seguridad del compilador.  
Se utlilizarán ``static_cast<>``, ``dynamic_cast<>``, ``reinterpret_cast<>`` y ``const_cast<>`` en su lugar.

## Archivos de encabezado y namespaces
Los archivos de encabezado (.h) comenzarán usando la directiva ``#pragma once``.
Por norma general, el código del proyecto irá en namespaces propios y no en el ámbito global, para evitar conflictos de nombres.  
Estructura de un archivo de encabezado:
```
#pragma once

#include <...>
#include "..."

namespace myNamespace {

    //Código del archivo de encabezado
    ...
}

```
No debe escribirse nunca ```using namespace ...``` en un archivo .h.


