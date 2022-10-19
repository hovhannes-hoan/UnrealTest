Task Description:

	StartUp UI
		Enter name
		● This name should be displayed above every player in the 3d Space.
	Joint the Multiplayer (Local)
		The map will have a lever, that on interacting with it will open or close a large
		door. Any player can interact with it.
		● Try performing most of the tasks in C++
		● Showcase the use of exposing C++ in Blueprints.
		● Handling of Replication and RPC
		
My solution:
	Game starts on the Empty map showing the BP_LobbyHUD.
	The player has to enter the name in UI and has an option to host a server 
		or join an existing game (through specifying ip address) as a client.
	Once done the player travels to the ThirdPersonExampleMap that contains a lever 
	and a big orange door on the right side of the lever.
	To interact with the lever player has to get close enough and press E keyboard button.
	Once lever reaches it's position the door will start opening.
	
	- Most of the code is written in C++, in blueprints I have left only a small functionality
	related with navigating through the UI menu and menu button presses.
	- Blueprints invoke methods defined in C++ for example to host a server or join a game as a client
	- Replication and RPC is done for lever and door interactions.
	- To show player name above the character I am using a widget component attached to the player character.
	  As an alternative solution the responsibility of drawing the player names can be put on HUD class
	  which could use the Project method to draw the label widget on the 2D space based on the character's
	  location in 3D space.