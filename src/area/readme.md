# src/area

The area source folder contains the Area class (which defines dungeons, towns, and other regions in the game that the player can explore), as well as the Tile
class (Tiles are the component blocks that make up an Area), and anything directly related to these classes.

* **area.cpp** - The Area class defines... well, *areas* of the game world where the gameplay takes place. Each Area consists of a square grid of Tiles.

* **shadowcast.cpp** - Shadowcasting algorithm, used to calculate line-of-sight and basic lighting.

* **tile.cpp** - The Tile class defines the blocks that make up the game world, be it floors, walls, doors, or whatever.
