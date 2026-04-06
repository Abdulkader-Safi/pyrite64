#version 450

// Fullscreen triangle — no vertex buffer needed
// Invoke with 3 vertices, covers entire screen
vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

layout(location = 0) out vec2 fragUV;

void main() {
    fragUV = positions[gl_VertexIndex] * 0.5 + 0.5;
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
