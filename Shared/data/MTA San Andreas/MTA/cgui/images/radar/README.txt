MTA:SA F11 Radar Map - Custom Images

You can replace the F11 map images by creating custom versions with the "-custom" suffix:

Default files:
- map_1024.png (1024x1024)
- map_2048.png (2048x2048)

To use custom images, create:
- map_1024-custom.png (1024x1024)
- map_2048-custom.png (2048x2048)

Place them in this folder. MTA will use -custom files if they exist.
Server scripts can override these with setPlayerMapImage().
