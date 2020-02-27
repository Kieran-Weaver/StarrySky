# StarrySky
A platformer game, written in C++11, using GLFW.

It uses sprite-batching and texture atlases to recreate the experience of games from the 16-bit and 32-bit eras, but with a maintainable, modern code-base.

This game was designed from the ground up to have optimal performance. The performance goals are:
- To run at 60fps on a Raspberry Pi Zero
- To keep the total binary size less than 100 MB

GL/Sprite.h, GL/SpriteBatch.h, GL/Texture.h, GL/TextureAtlas.h, and GL/TextureAtlas.cpp are originally from https://github.com/ricanteja/Moony-SpriteBatch, but were rewritten for Starry Sky.

The original engine for Starry Sky was based off of https://github.com/pixelpicosean/PlatformerEngine, but is was also rewritten.

The engine uses texture atlases which are generated by https://github.com/Kieran-Weaver/crunch
