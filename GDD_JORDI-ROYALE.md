# **Jordi Royale**

Picante Medio  
2025-2026
---
**ÍNDICE**

- [**1. Resumen**](#1-resumen)

	- [**1.1. Descripción**](#11-descripción)

	- [**1.2. Género**](#12-género)

	- [**1.3. Público objetivo**](#13-público-objetivo)

- [**2. Historia y contexto del juego**](#2-historia-y-contexto-del-juego)

- [**3. Gameplay**](#3-gameplay)

	- [**3.1. Objetivo del juego**](#31-objetivo-del-juego)

	- [**3.2. Flujo del juego**](#32-flujo-del-juego)

- [**4. Mecánicas**](#4-mecánicas)

	- [**4.1. El jugador**](#41-el-jugador)

		- [**4.1.1. Movimiento**](#411-movimiento)

		- [**4.1.2.Estadísticas base**](#412-estadísticas-base)
	
	- [**4.2. Los Enemigos**](#42-los-enemigos)

		- [**4.2.1. Movimiento**](#421-movimiento)

		- [**4.2.2. Estadísticas base**](#422-estadísticas-base)

		- [**4.2.3. Aparición**](#423-aparición)

		- [**4.2.4. Características**](#424-características)

	- [**4.3. Las armas**](#43-las-armas)

	- [**4.4. Mejoras**](#44-mejoras)

	- [**4.5. Entorno**](#45-entorno)
		
		- [**4.5.1. Items**](#451-items)
		
		- [**4.5.2. Mapa**](#452-mapa)

- [**5. Estética**](#5-estética)

	- [**5.1. Ambientación**](#51-ambientación)

	- [**5.2. Arte Grafico**](#52-arte-grafico)

	- [**5.3. Audio**](#53-audio)

- [**6. Interfaces**](#6-interfaces)

- [**7. Referencias**](#7-referencias)
---
# **1. Resumen**

# ***1.1. Descripción*** 

Juego estilo Magic Survival y similares (p.ej: Vampire Survivors o Deep Rock Galactic: Survivor) donde el jugador se enfrenta a hordas de enemigos a través armas automatizadas, es decir, que atacan sin recibir un input.  
La dificultad de las hordas es incremental conforme van llegando. Además, el jugador puede adquirir habilidades/armas nuevas o mejorar las que ya tiene derrotando enemigos y subiendo de nivel. El único control real que tiene el jugador es el movimiento del personaje y las mejoras que decide escoger.

# ***1.2. Género***

Roguelike, arcade y acción 

# ***1.3. Público objetivo*** 

Jugadores jóvenes (12+)

# **2. Historia y contexto del juego**

Después de haber llegado a la luna Jordi se encuentra en una de las catástrofes más grandes con las que se ha encontrado, miles de clones suyos zombificados que llegaron desde júpiter. Ahora su misión es acabar con todos ellos antes de que acaben con él.

# **3. Gameplay**

# ***3.1. Objetivo del juego***

El objetivo es sobrevivir el mayor tiempo posible esquivando y acabando con todos los enemigos que pueda. La partida termina cuando el jugador pierde toda la vida.

# ***3.2. Flujo del juego***

Al abrir el juego se abre el menú principal, donde se da a escoger al jugador entre iniciar una partida o salir del juego con los botones Comenzar y Salir.

Si el jugador decide comenzar la partida se encontrará con un arma en un mapa donde aparecerán de forma pseudoaleatoria los enemigos. Conforme el jugador vaya matando enemigos irá subiendo de nivel y consiguiendo nuevas armas y mejoras de característica.

La partida acabará cuando los enemigos maten al jugador, entonces al jugador le saldrá el menú de derrota con las estadísticas generales de la partida (tiempo de partida, enemigos eliminados, etc…)

# **4. Mecánicas**

## ***4.1. El jugador***

### ***4.1.1. Movimiento***

El jugador se mueve en todas las direcciones con las teclas WASD, puede saltar con la barra espaciadora y puede hacer un dash con el click izquierdo del ratón. Con el ratón cambia la dirección de la cámara.

### ***4.1.2. Estadísticas base*** 

Las estadísticas base son una serie de datos que definen todos los parámetros del jugador, entre estas están:

- **Vida máxima**: toda la vida que puede llegar a acumular el jugador  
- **Velocidad:** la velocidad a la que se mueve el jugador  
- **Cadencia:** la velocidad a la que ataca el jugador  
- **Daño:** el daño base del que parten todas las armas


## ***4.2. Los enemigos***

### ***4.2.1. Movimiento***

Los enemigos pueden moverse o no. Si se mueven lo harán en línea recta directamente hacia el jugador.

### ***4.2.2. Estadísticas base***

Comparten estadísticas con el jugador.

### ***4.2.3 Aparición***

Los enemigos apareceran en una posición aleatoria dentro de un rango a una distancia media del jugador.

### ***4.2.4. Características***

Los enemigos pueden tener hasta 2 características/comportamientos distintos:

- Avanzar hacia el jugador, en cuyo caso le harán daño al tocarlo  
- Disparar proyectiles hacia el jugador


## ***4.3. Las armas***

El jugador comienza siempre con un arma inicial (escogida aleatoriamente) y podrá conseguir más en el transcurso de la partida.   
Hay varios tipos de armas:

- Cuerpo a cuerpo: afectan a los enemigos más cercanos.  
- A distancia: pueden llegar a los enemigos que estén más lejos.   
- De área: afectan a todos los enemigos que se encuentren en ese área.

Las armas hacen daño en función del daño base del jugador y su cadencia.

## ***4.4 Mejoras***

Las mejoras son habilidades que se podrán escoger al subir de nivel, al igual que las armas, y proporcionan al jugador una ventaja en su misión de sobrevivir.

## ***4.5. Entorno***

### ***4.5.1. Items***

Los enemigos podrán soltar uno de dos items de forma aleatoria al morir:

- Item de curación  
- Experiencia


### ***4.5.2. Mapa***

El mapa es limitado. El jugador puede navegarlo dentro de sus límites, al llegar a los bordes del mapa el jugador no podrá seguir avanzando. Los enemigos solo aparecen dentro los límites del mapa.

# **5. Estética**

## ***5.1. Ambientación***

Cambiará en función de la historia y el contexto del juego. Ahora mismo, distintas partes de la luna. Montañas, cráteres y demás. Muchas partículas.

## ***5.2. Arte Grafico***

   [Moodboard](https://pin.it/22uL4zGgo) 

## ***5.3. Audio***

Cambiará en función de la historia y el contexto del jugo.

# **6. Interfaces**

## ***6.1. HUD***

Durante el juego, en la pantalla aparecerá datos que informarán al jugador sobre su estado actual:

- **Reloj:** muestra el tiempo transcurrido desde el inicio de la partida.  
- **Indicador de nivel de experiencia:** muestra tu nivel actual y cuánto te falta para llegar al siguiente.

## ***6.2. Menús***

Los menús del juego son sencillos por lo que no tienen mucha más funcionalidad que permitir al jugador comenzar y terminar partidas:

- **Menú principal:** tiene dos botones, jugar y salir, que permiten al jugador comenzar la partida o salir del juego  
- **Menú de final de partida:** muestra las estadísticas generales de la partida como el tiempo transcurrido, los enemigos matados o el nivel alcanzado. También permite al jugador volver al menú principal o volver a intentarlo. 

# **7. Referencias**

El juego toma referencias de Jordi Llinares y su estética cambiante y transgresiva a través del pasar de los años y las épocas. Se centra mucho en la acción, el metal, Valencia, Perú, y la cebolla encurtida.

En cuanto a juegos bebe mucho de la fórmula de Magic Survival y todos los juegos similares como Brotato, Vampire Survivors, Deep Rock Galactic Survivor, 10 minutes until dawn etc…  
Centrado en el movimiento constante, oleadas masivas de enemigos y disparos con muchos efectos estimulantes para el cortex prefrontal.