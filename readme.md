# Godot CPP - Reinforcement learning avec GDExtension

## Lancer le projet

1. Importer demo/project.godot dans godot > 4.3
2. Lancer avec F5

## Organisation

- demo : projet godot
- godot-cpp : bindings C++ pour GDExtension. Nécessaire pour build avec scons. Dans ce zip, la GDExtension est déjà compilée et présente dans le projet. La première compilation compile toutes les sources dans godot-cpp pour pouvoir étendre les classes natives.
- src : fichiers C++ qui composent la gdextension.