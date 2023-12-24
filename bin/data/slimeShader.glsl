#version 430 

layout(binding = 1, rgba32f) uniform image2D TrailMap;
uniform int width;
uniform int height;
uniform int numSlimes;
uniform float deltaTime;
uniform float moveSpeed;
uniform float diffuseWeight;
uniform float decayRate;
uniform float sensorAngleOffset;
uniform int sensorSize;
uniform float sensorOffsetDistance;
uniform float turnSpeed;

uint hash(uint state)
{
    state = (state ^ 61) ^ (state >> 16);
    state *= 9;
    state = state ^ (state >> 4);
    state *= 0x27d4eb2d;
    state = state ^ (state >> 15);
    return state;
}

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
struct Slime {
    vec2 position;
    float angle;
};

layout(std430, binding = 0) buffer SlimeBuffer {
    Slime slimes[];
};

float scaleToRange01(uint value) {
    return float(value) / 4294967295.0;
}

float trailScent(vec2 agentPosition, float agentAngle, float sensorAngleOffset) {
    float sensorAngle = agentAngle + sensorAngleOffset;
    vec2 sensorDir = vec2(cos(sensorAngle), sin(sensorAngle));
    vec2 sensorCenter = agentPosition + sensorDir * sensorOffsetDistance;
    float sum = 0.0;

    // Loop over a square grid around the sensor center
    for (int offsetX = -sensorSize; offsetX <= sensorSize; offsetX++) {
        for (int offsetY = -sensorSize; offsetY <= sensorSize; offsetY++) {
            int posx = min(width - 1, max(0, int(sensorCenter.x) + offsetX));
			int posy = min(height - 1, max(0, int(sensorCenter.y) + offsetY));

            // Check if the position is within the bounds of the trailMap
            if (posx >= 0 && posx < width && posy >= 0 && posy < height) {
                vec4 trailColor = imageLoad(TrailMap, ivec2(posx, posy));
                sum += (trailColor.r + trailColor.g + trailColor.b) / 3.0; // Averaging RGB values for brightness
            }
        }
    }
    return sum;
}

vec2 circleCenter = vec2(width / 2.0, height / 2.0);
float circleRadius = min(width, height) / 3.0;

void updateSlime(uint id) {
    Slime slime = slimes[id];
    uint random = hash(uint(slimes[id].position.y) * width + uint(slimes[id].position.x) + hash(id.x * 10));

    float weightAhead = trailScent(slimes[id].position, slimes[id].angle, 0.0);
    float weightLeft = trailScent(slimes[id].position, slimes[id].angle, sensorAngleOffset);
    float weightRight = trailScent(slimes[id].position, slimes[id].angle, -sensorAngleOffset);

    float randomSteerStrength = scaleToRange01(random);

    if (weightAhead >= weightLeft && weightAhead >= weightRight) {
        // Keep angle the same
        slimes[id].angle += 0;
    }
    else if (weightAhead < weightLeft && weightAhead < weightRight) {
        slimes[id].angle += (randomSteerStrength - 0.5) * 2 * turnSpeed * deltaTime;
    }
    else if (weightRight > weightLeft) {
        slimes[id].angle -= randomSteerStrength * turnSpeed * deltaTime;
    }
    else if (weightLeft > weightRight) {
        slimes[id].angle += randomSteerStrength * turnSpeed * deltaTime;
    }

    vec2 direction = vec2(cos(slimes[id].angle), sin(slimes[id].angle));
    vec2 newPos = slimes[id].position + direction * moveSpeed;

    // Window Boundary
    if (newPos.x < 0 || newPos.x >= width || newPos.y < 0 || newPos.y >= height) {
        newPos = clamp(newPos, vec2(0.0), vec2(width, height));
        slimes[id].angle = scaleToRange01(random) * 2.0 * 3.14159; 
    }

    // Circle Boundary
    //if (distance(newPos, circleCenter) > circleRadius) {
        // Find the nearest point on the circle's edge
    //    vec2 toCenter = normalize(circleCenter - newPos);
    //    newPos = circleCenter - toCenter * circleRadius;

        // Stick to the circle edge
        //slimes[id].angle = atan(toCenter.y, toCenter.x) + 3.14159;
        // Bounce off the circle edge
     //   slimes[id].angle = scaleToRange01(random) * 2.0 * 3.14159;
    //}

    slimes[id].position = newPos;
}

void main() {
    uint id = gl_GlobalInvocationID.x;

    if (id >= numSlimes) {
        return;
    }
    

    // Draw trail
    updateSlime(id);
    vec2 newPos = slimes[id].position;

    imageStore(TrailMap, ivec2(newPos), vec4(0.0, 1.0, 1.8, 1.5)); // Trail map is marked with 1.0 to indicate the trail
}