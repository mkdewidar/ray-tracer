# ray-tracer

Implementation of a ray tracer following the [ray tracing in a X books](https://raytracing.github.io/).

## TODO:

- The optimisation in 3.10 in "Ray tracing the next week"
- Lesson 5 of "Ray tracing the next week"
- All of "Ray tracing the rest of your life"

Other ideas I had:

- Improve my logging system to make it easier to debug issues
- Experiment with different diffuse implementations, for example one based on the lambertian material that occasionally absorbs instead of always reflecting.
- Use gif-h or msf_gif to generate gifs rather than just static images.
- Feels like there is some unexpected grainy-ness in the middle?
- Implement a tostring (overload of <<) for every class
- Update to v4 of the book, some changes I seem to notice are:
    - Image origin is top left instead of bottom right.
    - The pixel grid is inset in the viewport.
    - A pixel is a disk instead of a square? see `pixel_sample_disk` in the github source.
    - Potentially different depth of field implementation?