## Note

This software uses Box2D physics engine (v2.4.0) by erin catto: https://github.com/erincatto/box2d
It is located in src/external/

I had to change some header file paths in the source files to make it work, other than that, it is the same one that
can be found from the git repo.

Idea for map editor and block layout (not copied code though) from Andrew: https://www.youtube.com/watch?v=DF5i6bZ3Rns

controls:

	- 'q' or 'esc'				to QUIT
	- 'm'						to toggle between PLAY and EDITOR modes (editor is only for game creator ;)

	PLAY:
		- 'a' or 'left arrow' 	to tilt left
		- 'd' or 'right arrow'	to tilt right

		- 'space' 				to jump
		- 'left mouse' 			to throw lance
	
	EDITOR:
		- 'w' or 'up arrow'		to move view up
		- 's' or 'down arrow'	to move view down
		- 'a' or 'left arrow'	to move view left
		- 'd' or 'right'		to move view right

		- 'left mouse'			to place a block
		- 'right mouse'			to remove a block

		- ','					to save a map as "newMap"
		- '.'					to load "newMap"
