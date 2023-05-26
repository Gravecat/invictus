# src/entity

The entity source folder contains the Entity class, which defines all objects that inhabit the game world -- monsters, items, chests, and so on.
Entity has multiple derived classes, which represent more specialized forms.

* **entity.cpp** - The main Entity class, from which everything else in this source folder derives.

* **item.cpp** - The Item class is a type of Entity we can pick up and use.

* **mobile.cpp** - The Mobile class represents Entities which can move around and fight; generally represents living creatures, but can also represent automata.

* **player.cpp** - Derived from Mobile, the Player class specifically represents the player's character in the game world.
