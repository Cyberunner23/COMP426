

__kernel void update_ball(__global float* masses, __global uint* radii, __global float2* positions, __global float2* velocities, __global float* gravity, __global float* deltaT, __global uint* WinSize)
{
    int index = get_global_id(0);

    // Update velocity
    velocities[index].y += *gravity * *deltaT;

    // Update Position
    positions[index].x += velocities[index].x * *deltaT;
    positions[index].y += velocities[index].y * *deltaT;

    // Ensure its still on screen
    positions[index].x = positions[index].x < (float)*WinSize - radii[index] ? positions[index].x : (float)*WinSize - radii[index];
    positions[index].x = positions[index].x > (float)radii[index] ? positions[index].x : (float)radii[index];

    positions[index].y = positions[index].y < (float)*WinSize - radii[index] ? positions[index].y : (float)*WinSize - radii[index];
    positions[index].y = positions[index].y > (float)radii[index] ? positions[index].y : (float)radii[index];
}

bool collides_with_edge_x(float2 position, uint radius, uint WinSize)
{
    return (unsigned int)position.x - radius <= 0 || (unsigned int)position.x + radius >= WinSize;
}

bool collides_with_edge_y(float2 position, uint radius, uint WinSize)
{
    return (unsigned int)position.y - radius <= 0 || (unsigned int)position.y + radius >= WinSize;
}

void handle_wall_collision(float2 position, uint radius, uint WinSize, __global float2* velocity)
{
    if (collides_with_edge_x(position, radius, WinSize))
    {
        float vX = (*velocity).x;
        (*velocity).x = -vX;
    }

    if (collides_with_edge_y(position, radius, WinSize))
    {
        float vY = (*velocity).y;
        (*velocity).y = -vY;
    }
}

void handle_ball_ball_collision(__global float* masses, __global uint* radii, __global float2* positions, __global float2* velocities, uint ballIndex1, uint ballIndex2)
{
    float2 delta;
    delta.x = positions[ballIndex1].x - positions[ballIndex2].x;
    delta.y = positions[ballIndex1].y - positions[ballIndex2].y;

    float r = (float)radii[ballIndex1] + (float)radii[ballIndex2];
    float dist2 = dot(delta, delta);

    if (dist2 >= r * r)
    {
        return;
    }

    float d = length(delta);

    float2 mtd;
    if (d != 0.0f)
    {
        mtd = delta * ((((float)radii[ballIndex1] + (float)radii[ballIndex2]) - d)/d);
    }
    else
    {
        d = (float)radii[ballIndex1] +  (float)radii[ballIndex2] - 1.0f;
        delta.x = radii[ballIndex1] + radii[ballIndex2];
        delta.y = 0.0f;
        mtd.x = delta.x * ((((float)radii[ballIndex1] + (float)radii[ballIndex2]) - d)/d);
        mtd.y = delta.y * ((((float)radii[ballIndex1] + (float)radii[ballIndex2]) - d)/d);
    }

    float im1 = 1 / masses[ballIndex1]; // inverse mass quantities
    float im2 = 1 / masses[ballIndex1];

    positions[ballIndex1] = positions[ballIndex1] + (mtd * (im1 / (im1 + im2)));
    positions[ballIndex2] = positions[ballIndex2] - (mtd * (im1 / (im1 + im2)));

    float2 v;
    v.x = velocities[ballIndex1].x - velocities[ballIndex2].x;
    v.y = velocities[ballIndex1].y - velocities[ballIndex2].y;

    float vn = dot(v, normalize(mtd));

    if (vn > 0.0f) return;

    float i = (-(1.0f + 0.85f) * vn) / (im1 + im2);
    float2 impulse;
    impulse.x = mtd.x * i * 0.001f;
    impulse.y = mtd.y * i * 0.001f;

    velocities[ballIndex1].x = velocities[ballIndex1].x + (impulse.x * im1);
    velocities[ballIndex1].y = velocities[ballIndex1].y + (impulse.y * im1);

    velocities[ballIndex1].x = velocities[ballIndex2].x - (impulse.x * im2);
    velocities[ballIndex1].y = velocities[ballIndex2].y - (impulse.y * im2);
}

__kernel void handle_collisions(__global float* masses, __global uint* radii, __global float2* positions, __global float2* velocities, __global uint* count, __global uint* WinSize)
{
    int i = get_global_id(0);

    handle_wall_collision(positions[i], radii[i], *WinSize, &velocities[i]);

    for(int j = i + 1; j < *count; j++)
    {
        handle_ball_ball_collision(masses, radii, positions, velocities, i, j);
    }
}