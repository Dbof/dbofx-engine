DbofX Engine
============

The DbofX Engine is a 3D game engine built to facilitate the creation of 3D games and graphics.
The engine works with DirectX 9, which is pretty old right now, since DX11 already exists, but still widely used within a lot of projects.

This project was started in 2010 and was worked on since then. 

The current features will be released in the future, same with a good documentation on how to use it.

Features:
- Fully coded in C++, pretty easy to use
- Easy to set up
- Common settings such as transformation and rendering is done for you automatically
- Memory is handled internally and does not require any management
- Same for any ressources, textures or object models, no management needed, no memory leaks.
- Easy DirectX setup, most of the work is done for you
- Uses its own window, but can be integrated in any other custom window

Meshes:
- Basic 3D model support in .x Format
- Every model is handled the same way, so move methods can be applied to a whole group of objects
- Models can be attached to scene nodes for easier handling of mesh groups.
- Load textures from image files or from DDS files
- Support for transparent objects
- Support for animated models with keyframe animations
- Support for skinned meshes and skeletal animation 
- 2D sprite support and billboards

Scene:
- Fully flexible scene management
- Camera can be managed like any other 3D model, rotation, viewport settings and much more
- Hierarchical scene graph with scene nodes, objects can be attached to each other
- Support directional light, spot lights and point lights, fully customizable and automatically managed for you

Special effects:
- Contains easy-to-use particle system with very precise control over what is drawn and how
- Support for skyboxes in DDS format
- Billboarding for 2D sprites
- Support for automatic terrain rendering using heightmaps with access to height data
- Transparency and alpha blending fully handled for you
- Supports vertex and pixel fog effects
- Support for 2D onscreen text, useful for HUD screens

Misc:
- Fast and easy to use math functions and containers
- Fully integrated logging/error system with automatic console outputs
- Windows console is fully integrated into the engine
