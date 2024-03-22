# Music Credits

https://pixabay.com/music/techno-trance-the-dying-110458/

https://pixabay.com/music/upbeat-a-dark-desolate-world-15695/

https://pixabay.com/music/synthwave-boss-fight-143121/

https://pixabay.com/music/upbeat-the-final-boss-battle-158700/

https://pixabay.com/music/beats-the-longest-night-of-this-winter-158699/

# General Work Credits
This section lists the work done by each people involved in this project
## St4rman
- Core Physics all the collisions OBBBs, Capsules, core gameplay loop
- Core tech Game flow - syncing client and server across levels and making sure theyre syncing well
- Core player input class wrapper
- Core Level loading tool development on unity, Mesh loading with level reader - Takes data from unity to json and then puts it in the game itself
- Level Manager, Input Manager, Player component, Medal system in level Manager - Static class singleton patterns allowing anywhere
- Post processing speedlines shader - Generative noise glsl 4 directional
- lava shader, UI fire shaders, Lava Quad - noise and shader animation
- Shine shader on medal , Noise shader on timer bar
- Core Player cam animations, Core UI system integration  - Set up with player input
- Timer class for stage timer 
- Debug Class - time costs of functionalities and memory
- main menu rewrite 
- UI programming - lua

## HeavyMetalGameDev
- Component system
- Physics broadphase rewrite using Sort & Sweep.
- Collision layer matrix for objects and raycasts.
- Physics materials.
- Deferred rendering implementation.
- Animation system including frame interpolation, animation transitions and transitioning through midposes. This builds upon Joe's networking system to make sure all player animations are consistent between clients.
- Level transitioning. This involved a large amount of code rewriting, as most of the code up to that point did not consider what would happen when the game should go to a new level. Includes networking level loading i.e server tells clients what level to load, waiting for all clients to load into the game etc. Managing memory for this, making sure all memory is freed.
- Disabling of player's controls at end of level and in countdown.
- Camera FOV changing based on speed and grappling.
- Gameplay components like moving platforms including being able to stand on them, grapple them.
- Level editor tools in Unity for visualising moving platforms and spikes, selecting meshes for objects.
- Level select on lobby screen that is consistent between clients.
- Hooking up some functionality to networking: speed lines, grappling sway.
- Rework of game flow of both server & client, to have a countdown, playing state, and an end timer before the next level is loaded. Includes networking these details.
- Player texturing including different textures per client.
- Made a few levels.
- Changes to grapple functionality: disabling grappling of damaging objects.
- Debug screen collision count.
- Improvement to Adam's cinematic camera: made it transition to the correct player at the end of the camera track.
- Various bugfixes

## Odycious
- Took ownership of version control
- Created Trigger Volumes - Start, End, Checkpoint, Death
- Functionality for Win/Lose conditions
- Made the build level function work between client and server
- Multithreaded loading screen
- Bug fixing
- Made death sound using rFGX
- Physics testing

## Joerseven

- Multi-threaded networking / created the server-client architecture that the game uses.
- Structured the project and created the base cmake files for the project, adding the libraries used like RAudio, Freetype and more through CMake.
- Used the freetype library with opengl to make a text renderer
- Added entt to add signal powered code throughout the repo.
- Created the UI framework for the game, a main canvas with layers that act as a pushdown automata system,
- a custom position system that lets the users specify position and size relative to the screen or in pixels. Added events that can be hooked into for each of the UI elements
- Embedded Lua to use with the UI if people wanted to.
- Created a TweenManager class for easy UI animations.
- Fixed the broken Matrix3 class.
- Created the Visual side of the grapple, modelling the chains and the trident in maya, adding them into the game, using instanced rendering for the chains.
- Created a smaller state-based system for the player movement.
- Created the toon outlines using sobel edge detection on the depth texture.
- Used 'tinyobj' along with a custom function to load OBJ files into OpenGL, to give use more flexibility over just .MSH files.
- Implemented Fast Approximate Anti Aliaiasing as a post process pass to remove the nasty jaggies.

## JunkyX1122

Mainly polish stuff.
- Creation of initial state machines for client and server.
- Implementation of sound system and sound effects.
- Sound loading "Threading".
- Camera movement animations.
  - Makes use of packets from server I guess.
- Base UI functionality (Ed did the redesigns) for single and multiplayer.
  - Timer.
  - Timer bar.
  - Medals.
  - Start and End screen.
  - Pause screen + game pause singleplayer.
- Updating canvas elements to use transforms.
- Fixed OBB colission.
- Pause screen.
- Springs.

## AdSand
- Debug: Memory usage, Camera information
- Continous Integration setup with jenkins
- Swinging objects, Added to unity level creator, Added to level loader
- Particles, GPU Instanced, Load on client, Added to renderer
- Music manager, Music pausing on pause menu
- Grapple, Deactivates when hitting lava in grapple state

## ikozoom & bruceshi
- stage timer
- components (raycast enemy, breaking block, bridge)
- live ops testing
- level design
