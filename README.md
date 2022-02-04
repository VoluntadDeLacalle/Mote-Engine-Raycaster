<h1>Mote Engine Raycaster</h1>
<h2>Made by William Lacalle</h2>

<h3>Mote Game Engine:<h3> 
<p>https://github.com/jmarshallstewart/MoteGameEngine</p>

<h3>Lode's Computer Graphics Tutorial:<h3>
<p>https://lodev.org/cgtutor/raycasting.html</p>
<p>Reading this tutorial inspired me to learn more about Raycasting and convert their C++ code into functioning Lua code! Check it out!</p>

<h3>Raycast Map Demo<h3>
<p>In this demo, and the tutorial listed above, player movement was added to allow the player to explore the area they create. Use the WASD keys to move around the 3D environment. The player may be reset to their spawning position using the R key.</p>

<h3>Raycast Map Editor</h3>
<p>I decided to also add a map editor, which the original tutorial did not have. To load the map editor rather than the map demo, you must open the config.lua file and change the per-project loaded script to "mapEditor.lua". In this editor you are presented with a 2D grid that represents your map, and 6 squares to the right of said grid. To navigate the tiles of the grid, use the WASD keys. To change the current tile you have selected, press TAB until the arrow on the right is pointing to your preferred color (Note: The first color is clear, this means that no wall will render in that spot.). To check your new map, simply change the loaded script back to the "raycastMapDemo.lua" and you are ready to run the software!
<br><tab>-Be wary of editing in this software, it saves the map file every time a change is made.
<br><tab>-The light, teal tile with a black square is the starting position of the player and cannot be edited nor changed.</p>

<h3>Changes made to the Mote Engine:</h3>
<p><tab>-Added a glue function akin to the "Debug.Log()" function from Unity. This allows the user to pass a string from your Lua code for debugging.
<br><tab>-Added several glue functions for drawing lines and shapes.</p>