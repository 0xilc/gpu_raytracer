#version 460 core
#define INFINITY 1e30
#define RECURSION_MAX_DEPTH 5
out vec4 FragColor;
in vec2 fragPos;

uniform int u_ScreenWidth;
uniform int u_ScreenHeight;

struct BVHNode {
    vec3 minBounds;
    int leftChild;
    vec3 maxBounds;
    int rightChild;
    int primitiveIndex;
    float pad[3];
};

struct Primitive {
    vec4 vertexData[3];
    int materialId;          
    int type;
    float pad;
};

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 mirror;
	float phong_exponent;
	int isMirror;
	float pad[2];
};

struct Light {
	vec3 position;
	int lightsSize;
	vec3 intensity;
	float pad2;
};

layout(std140, binding = 1) uniform CameraData {
    vec4 u_Position;
    vec4 u_Front;
    vec4 u_Up;
    vec4 u_PlaneCenter;
};

layout(std430, binding = 2) buffer BVHBuffer {
    BVHNode BVHNodes[];
};

layout(std430, binding = 3) buffer Primitives {
    Primitive primitiveNodes[];
};

layout(std430, binding = 4) buffer Materials {
    Material materials[];
};

layout(std430, binding = 5) buffer Lights {
    Light lights[];
};


struct Ray {
    vec3 origin;
    vec3 direction;
};

struct HitRecord {
    vec3 hitPoint;
    vec3 normal;
    int materialId;
    float t;
};

bool Hit(Ray ray, Primitive primitive, out HitRecord hitRecord) {
    // triangle
    if (primitive.type == 0) {
        vec3 e1 = primitive.vertexData[1].xyz - primitive.vertexData[0].xyz;
        vec3 e2 = primitive.vertexData[2].xyz - primitive.vertexData[0].xyz;
        vec3 h = cross(ray.direction, e2);
        float a = dot(e1, h);
        if (a > -0.00001 && a < 0.00001)
            return false;
        float f = 1.0 / a;
        vec3 s = ray.origin - primitive.vertexData[0].xyz;
        float u = f * dot(s, h);
        if (u < 0.0 || u > 1.0)
            return false;
        vec3 q = cross(s, e1);
        float v = f * dot(ray.direction, q);
        if (v < 0.0 || u + v > 1.0)
            return false;
        float t = f * dot(e2, q);
        if (t > 0.00001) {
            hitRecord.hitPoint = ray.origin + ray.direction * t;
            hitRecord.normal = normalize(cross(e1, e2));
            hitRecord.materialId = primitive.materialId;
            hitRecord.t = t;
            return true;
        }
        return false;
    }
    // sphere
    else {
        vec3 center = primitive.vertexData[0].xyz;
        float radius = primitive.vertexData[1].x;
        vec3 oc = ray.origin - center;
        float a = dot(ray.direction, ray.direction);
        float b = dot(oc, ray.direction);
        float c = dot(oc, oc) - radius * radius;
        float discriminant = b * b - a * c;
        if (discriminant > 0) {
            float temp = (-b - sqrt(discriminant)) / a;
            if (temp < 0.00001)
                temp = (-b + sqrt(discriminant)) / a;
            if (temp > 0.00001) {
                hitRecord.t = temp;
                hitRecord.hitPoint = ray.origin + ray.direction * temp;
                hitRecord.normal = normalize(hitRecord.hitPoint - center);
                hitRecord.materialId = primitive.materialId;
                return true;
            }
        }
        return false;
    }
}

bool aabbIntersect(Ray ray, vec3 minBounds, vec3 maxBounds, out float tmin, out float tmax) {
    vec3 invDir = 1.0 / ray.direction;
    vec3 t0s = (minBounds - ray.origin) * invDir;
    vec3 t1s = (maxBounds - ray.origin) * invDir;
    vec3 tSmall = min(t0s, t1s);
    vec3 tBig = max(t0s, t1s);
    tmin = max(max(tSmall.x, tSmall.y), tSmall.z);
    tmax = min(min(tBig.x, tBig.y), tBig.z);
    return tmax > max(0.0, tmin);
}

void BVHHit(Ray ray, out HitRecord hitRecord)
{ 
    hitRecord.t = INFINITY;

    int stack[128];
    int stackPointer = 0;
    stack[stackPointer++] = 0;

    while (stackPointer > 0) {
        int nodeIndex = stack[--stackPointer];
        BVHNode node = BVHNodes[nodeIndex];

        float tmin, tmax;
        if(aabbIntersect(ray, node.minBounds, node.maxBounds, tmin, tmax)) {
            if (node.primitiveIndex >= 0) {
                Primitive primitive = primitiveNodes[node.primitiveIndex];
                HitRecord tempRecord;
                if (Hit(ray, primitive, tempRecord)) {
                    if (tempRecord.t < hitRecord.t) {
                        hitRecord = tempRecord;
                    }
                }
            } else {
                if (node.leftChild >= 0) {
                    stack[stackPointer++] = node.leftChild;
                }
                if (node.rightChild >= 0) {
                    stack[stackPointer++] = node.rightChild;
                }
            }
        }
    }
}

struct ShadingStackElement {
    Ray ray;
    HitRecord hitRecord;
    int depth;
    vec3 mirrorCoefficient;
};

vec3 blinnPhong(Ray ray, HitRecord hitRecord)
{
    ShadingStackElement shadingStack[RECURSION_MAX_DEPTH + 1];
    shadingStack[0].ray = ray;
    shadingStack[0].hitRecord = hitRecord;
    shadingStack[0].depth = 0;
    shadingStack[0].mirrorCoefficient = vec3(1.0);
    int stackPointer = 1;
    vec3 resultColor = vec3(0.0);

    while(stackPointer > 0)
    {  
        stackPointer--;
        if (shadingStack[stackPointer].depth > RECURSION_MAX_DEPTH) break;

        // Local variables
        Ray ray = shadingStack[stackPointer].ray;
        HitRecord hitRecord = shadingStack[stackPointer].hitRecord;
        int depth = shadingStack[stackPointer].depth;
        vec3 mirrorCoefficient = shadingStack[stackPointer].mirrorCoefficient;

        Material material = materials[hitRecord.materialId - 1];
        if (material.isMirror == 1) {
            vec3 reflectedDir = reflect(ray.direction, hitRecord.normal);
            Ray reflectedRay;
            reflectedRay.origin = hitRecord.hitPoint + hitRecord.normal * 0.0001;
            reflectedRay.direction = reflectedDir;

            HitRecord reflectedHitRecord;
            reflectedHitRecord.t = INFINITY;
            BVHHit(reflectedRay, reflectedHitRecord);
            if (reflectedHitRecord.t < INFINITY) {
                shadingStack[stackPointer].ray = reflectedRay;
                shadingStack[stackPointer].hitRecord = reflectedHitRecord;
                shadingStack[stackPointer].depth = depth + 1;
                shadingStack[stackPointer].mirrorCoefficient = material.mirror.rgb;
                stackPointer++;
            } 
        }
        
        vec3 ambient = material.ambient.rgb;
        vec3 diffuse = material.diffuse.rgb;
        vec3 specular = material.specular.rgb;

        vec3 hitPoint = hitRecord.hitPoint;
        vec3 normal = hitRecord.normal;
        vec3 viewDir = normalize(ray.origin - hitPoint);
        
        for (int i = 0; i < lights[0].lightsSize; i++)
        {
            vec3 wi = lights[i].position - hitPoint;
            float dist = length(wi);
            wi = normalize(wi);

            Ray shadowRay;
            shadowRay.origin = hitPoint + normal * 0.0001;
            shadowRay.direction = wi;
            HitRecord shadowHitRecord;
            shadowHitRecord.t = INFINITY;
            BVHHit(shadowRay, shadowHitRecord);
            vec3 addition = vec3(0.0);

            if (shadowHitRecord.t == INFINITY) 
            {
                // Diffuse
                float cosTheta = max(0.0, dot(normal, wi));
                addition += diffuse * lights[i].intensity * (cosTheta / (dist * dist));

                // Specular component
                vec3 wo = normalize(ray.origin - hitPoint);
                vec3 h = normalize(wi + wo);
                float cosAlpha = max(0.0, dot(normal, h));
                addition += specular * lights[i].intensity * (pow(cosAlpha, material.phong_exponent) / (dist * dist));
                resultColor += addition * mirrorCoefficient;
            }
        }

    }

    return resultColor / 255;
}

void main() {
    vec3 right = normalize(cross(u_Front.xyz, u_Up.xyz));
    vec3 up = normalize(vec3(u_Up.xyz));
    vec2 uv = fragPos + vec2(-0.5, -0.5);
    vec2 distanceFromCenter = uv * vec2(u_ScreenWidth, u_ScreenHeight);
    vec3 screenPoint = u_PlaneCenter.xyz + right * distanceFromCenter.x + up * distanceFromCenter.y;

    Ray ray;
    ray.origin = u_Position.xyz;
    ray.direction = normalize(screenPoint - u_Position.xyz);
    
    vec3 resultColor = vec3(0.0);
    HitRecord hitRecord; 
    BVHHit(ray, hitRecord);

    if (hitRecord.t < INFINITY) {
        resultColor = blinnPhong(ray, hitRecord);    
    }
    
    FragColor = vec4(resultColor, 1.0);
}
