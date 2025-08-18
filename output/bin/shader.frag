uniform sampler2D texture;
uniform sampler2D texture2;
uniform float time;
uniform float waveStrength;
uniform float scale;
uniform float alpha;

void main() {
    vec2 coord = gl_TexCoord[0].xy;
    coord.x += sin(time + coord.y * 10.0) * waveStrength;
    coord=coord*scale;
    vec4 c1=texture2D(texture,coord);
    vec4 c2=texture2D(texture2,coord);
    vec4 mixed=mix(c1,c2,alpha);
    gl_FragColor = mixed;
    
}