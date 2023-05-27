# src/entity

The entity source folder contains the Entity class, which defines all objects that inhabit the game world -- monsters, items, chests, and so on.
Entity has multiple derived classes, which represent more specialized forms.

* **entity.cpp** - The Entity class defines tangible ~things~ in the game world, including the player, items, and monsters.

* **item.cpp** - The Item class is a type of Entity we can pick up and use.

* **mobile.cpp** - The Mobile class is a kind of Entity that can move around and fight. This includes monsters and the player character.

* **player.cpp** - The Player class is a kind of Entity that belongs to the player. It derives from Mobile, so that we can use some of the same code.
