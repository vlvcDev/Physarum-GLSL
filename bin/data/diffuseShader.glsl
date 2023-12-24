#version 430

layout(binding = 1, rgba32f) uniform image2D TrailMap;
uniform int width;
uniform int height;
uniform float deltaTime;
uniform float diffuseWeight;
uniform float decayRate;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

void Diffuse(ivec2 texCoord) {
    if (texCoord.x < 0 || texCoord.x >= width || texCoord.y < 0 || texCoord.y >= height) {
        return;
    }

    vec4 sum = vec4(0.0);
    vec4 baseColor = imageLoad(TrailMap, texCoord);

    for (int offsetX = -1; offsetX <= 1; offsetX++) {
        for (int offsetY = -1; offsetY <= 1; offsetY++) {
            int sampleX = clamp(texCoord.x + offsetX, 0, width - 1);
            int sampleY = clamp(texCoord.y + offsetY, 0, height - 1);
            sum += imageLoad(TrailMap, ivec2(sampleX, sampleY));
        }
    }

    vec4 averageColor = sum / 9.0;
    float blendWeight = clamp(diffuseWeight * deltaTime, 0.0, 0.5);
    vec4 blurredColor = baseColor * (1.0 - blendWeight) + averageColor * blendWeight;
    vec4 finalColor = max(vec4(0.0), blurredColor - decayRate * deltaTime);

    imageStore(TrailMap, texCoord, finalColor);
}

void main() {
    ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
    Diffuse(texCoord);
}