# Improve GPU Volumetric Rendering

## Improve stability

In more cases, VTK detects unstable configurations (eg multi volume with incompatible configurations)
and disable the rendering in those cases.

## Improve performance

The shader composer now factorizes more data used during the raymarching loop.
You can also set more than 6 lights now. The system passing lights as uniforms
has also been improved.

## Add secondary rays

When the shading of a volume is enabled, you can now render shadows. A global illumination reach
parameter controls the scope of secondary rays (ambient or global).
