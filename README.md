vsbillboards
=
A demo of quickly rendering camera-facing 3d billboards using the vertex shader.

High Level Explanation
-
The quads are expanded out from a single point in the vertex shader to form a full square.
This is done by finding the horizontal and vertical vectors spanning the plane perpindicular
to the eye direction and translating the position along these vectors in position/negative
directions based on its vertex id.

This lets us create these billboards easily while only having to send the center points of
the quads along with any other per-instance attributes we may want (sprite ids, colors, w/e).

A video of this demo running can be found on [youtube](http://youtu.be/7W_KPzwMBCU)

Controls for the demo
-
- w/s - forward/back
- a/d - strafe left/right
- q/e - strafe up/down
- r/f - roll clockwise/counterclockwise
- click + drag - move camera

Notes
-
I used two buffers for the instance attributes since I didn't want to deal with interleaved offsets when
writing the data but for best performance you really should use a single interleaved buffer.

The billboards will spin about some if you move about while looking at them with your view direction
almost parallel to +/-Y. This is because the shader assumes +Y can be used as an up vector that is
somewhat perpindicular to the viewing direction. It's possible to fix this by detecting the singularity
when computing the horizontal vector. I messed with this a bit but this resulted in the billboards snapping
when the axes switched, so I've left it out until I can work out something better.

Dependencies
-
- [SDL2](http://libsdl.org/)
- [glm](http://glm.g-truc.net/)
- [lfwatch](https://github.com/Twinklebear/lfwatch) (downloaded by CMake)
- [glLoadGen](https://bitbucket.org/alfonse/glloadgen/wiki/Home) (included)

