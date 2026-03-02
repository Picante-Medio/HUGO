# Comandos que vamos a usar y qué hacen
- *git status:* Imprime en la consola el estado actual de los cambios con respecto al último commit y a la rama actual
- *git log:* Imprime la lista de los últimos commits de la actual rama
- *git add:* Pasa los archivos untrackeados al staging
- *git rm:* Quita archivos del staging y los untrackea
- *git stash:* Pasa todos los cambios del staging a un "montón especial" desde el que se pueden recuperar o borrar
- *git commit:* Añade todos los cambios del staging a un commit
- *git fetch:* Actualiza nuesta máquina con respecto al repositorio remoto
- *git pull:* Trae cambios que no tenemos del repositorio remoto a nuesta máquina
- *git push:* Sube todos los commits creados recientemente al repositorio remoto
- *git checkout:* Viaja a un commit por su rama o por su id
- *git merge:* Fusiona el estado de dos ramas en una

# Casos de uso y flags
## Cuando voy a empezar a trabajar
1. `git status` para asegurarme de que no tengo cambios.
    - Si los tengo y no los quiero mantener, `git stash` y luego `git stash clear`
    - Si los tengo y los quiero mantener, `git commit -m"<nombre del commit>"`

2. `git pull --rebase origin <nombre de la rama>` para actualizar mi copia con el remoto

## Cuando he hecho cambios que funcionan
1. `git add .`, seguido de `git commit -m "<mesaje>"` para crear el commit
2. `git pull --rebase origin <nombre de la rama>` para traer cambios del remoto usando rebase
    - Si el rebase falla, `git rebase --abort` seguido de `git pull origin` y arreglar el merge manualmente
3. `git push origin <nombre de la rama>` para subir los cambios a remoto

## Cuando quiero moverme a otra rama
1. `git fetch origin` para actualizar las ramas disponibles
2. `git branch -a` para mostrar las ramas disponibles por consola
3. `git checkout <nombre de la rama>` para ir a la rama
    - Si tenía cambios sin guardar, `git add .` seguido de `git stash` para almacenarlos en el stash temporalmente
    - Cuando vuelva a esta rama `git stash pop` para recuperar los cambios del stash

## Cuando he hecho cambios que no funcionan
1. `git add .` y `git commit -m"<mensaje>"` para guardar mis cambios
2. `git checkout -b <nuevaRama>` para moverme a una rama de "fix"
3. `git push origin <nuevaRama>` para almacenar mis cambios en la rama temporal
4. Seguir añadiendo commits hasta que compile sin warnings (cuanto menos commits me lleve mejor)

## Cuando he arreglado los cambios rotos de mi rama "fix"
Vamos a mergear nuestra rama fix con la rama de trabajo
1. `git checkout <ramaFix>` para asegurarnos de que estamos en la rama temporal
2. `git pull --rebase origin <ramaPrincipal>` para traer los cambios de la rama de trabajo
    - Si el rebase falla, `git rebase --abort` seguido de `git pull origin` y arreglar el merge manualmente
3. `git push origin` para guardar el estado fusionado en la rama temporal
4. `git checkout <ramaPrincipal>` seguido de `git merge <ramaFix>` y `git push origin <ramaPrincipal>` para mergear la rama fix y poder eliminarla

## Cuando me cargo el repositorio
1. Leer atentamente `git checkout -h` y hacer magia o pedir ayuda a Diego
2. Pedir perdón
