//system
uniform float time;
//
uniform sampler2D tex_grass;
uniform sampler2D tex_blocks;
uniform sampler2D tex_water;
uniform sampler2D tex_mapmask;
uniform sampler2D tex_noise;
uniform sampler2D tex_normal;
uniform sampler2D texGrassMask;


uniform float grassSpeed;
uniform float grassWave;
uniform vec4 light;
uniform float blocks_scale;
uniform float grass_scale;
uniform float water_scale;
uniform float normal_scale;
uniform float noise_scale;
uniform float normalInt;

float valueTo1(float inputValue, float value, float precision) {
    float diff = abs(value - inputValue);
    float scaled = diff / precision;
    float floored = floor(scaled);
    return 1.0 - clamp(floored, 0.0, 1.0);
}
float mapTo01(float x, float a, float b) {
    // 处理区间反向的情况（如 b < a）
    float minVal = min(a, b);
    float maxVal = max(a, b);
    
    // 处理区间长度为0的特殊情况
    if (minVal == maxVal) {
        return 0.5; // 当 a == b 时返回中间值
    }
    
    // 线性映射并钳制到 [0, 1]
    float t = (x - minVal) / (maxVal - minVal);
    return clamp(t, 0.0, 1.0);
}
float map_range(float x, float a, float b){
    return ((b - a) / 2) * x + (a + b) / 2;
}
    
void main() {
    vec2 coord = gl_TexCoord[0].xy;
    vec3 lightVector=normalize(light.xyz);
    vec2 maskUV=coord;
    vec2 noiseUV=coord*noise_scale;
    vec2 blocksUV=coord*blocks_scale;
    vec2 grassUV=vec2(coord.x*grass_scale,coord.y*grass_scale*2 );
    vec2 normalUV=coord*normal_scale+sin(time*8)/100;
    float noiseValue=texture2D(tex_noise,noiseUV);
    vec2 waterUV=coord*water_scale+time/(20)+(noiseValue)/noise_scale;

    float animValue=texture2D(texGrassMask,grassUV).x;
    vec2 grassUVFix=vec2(grassUV.x+animValue.x*sin(time*grassSpeed)/grassWave,grassUV.y );
    
    float maskValue=texture2D(tex_mapmask,maskUV);
    vec4 cGrass=texture2D(tex_grass,grassUVFix);
    vec4 cBlocks=texture2D(tex_blocks,blocksUV);
    vec4 cWater=texture2D(tex_water,waterUV);
    
    vec3 normalVec=texture2D(tex_normal,normalUV).xyz*2-1;
    float lightInt=map_range(dot(lightVector,normalVec),normalInt,1);

    vec4 mixGB=mix(cGrass*lightInt,cBlocks,valueTo1(maskValue,0.2,0.01));
    vec4 mixGBW=mix(mixGB,cWater,valueTo1(maskValue,0.3,0.01));
    gl_FragColor = mixGBW;
    
}