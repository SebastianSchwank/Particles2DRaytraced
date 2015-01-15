#ifdef GL_ES
// Set default precision to high
precision highp int;
precision highp float;
#endif

//RT2DShader

varying vec2 v_texcoord;

uniform sampler2D Lines;

uniform sampler2D Particles;
uniform int numParticles;
uniform int numParameters;

uniform sampler2D randTex;
uniform float seedX;
uniform float seedY;

uniform int slope;

const float pi = 3.14159265359;

uniform int shaderMode;

// Unpacking a [0-1] float value from a 4D vector where each component was a 8-bits integer
float unpack(const vec4 value)
{
   const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
   return(dot(value, bitSh));
}
//synthesizing TexelFetch
vec4 texelFetch(sampler2D tex,const vec2 coord, const ivec2 size){
    vec2 fCoord = vec2((2.0*coord.x + 1.0)/(2.0*float(size.x)),(2.0*coord.y + 1.0)/(2.0*float(size.y)));
    return texture2D(tex,fCoord);
}

float clampFl(float value){
    if(value > 1.0) return 1.0;
    return value;
}

void main()
{
    vec4 renderedImagePixel = vec4(0.0,0.0,0.0,0.0);

    if(shaderMode == 0){

        for(int i = 0; i < numParticles; i++){
            float xP = unpack(texelFetch(Particles,vec2(float(i),3.0),ivec2(numParticles,numParameters)));
            float yP = unpack(texelFetch(Particles,vec2(float(i),2.0),ivec2(numParticles,numParameters)));
            float brightness = unpack(texelFetch(Particles,vec2(float(i),1.0),ivec2(numParticles,numParameters)));
            vec4  color = texelFetch(Particles,vec2(float(i),0.0),ivec2(numParticles,numParameters));

            brightness = brightness * 0.005*(1.0/distance(vec2(xP,yP),v_texcoord));
            renderedImagePixel += color * brightness;
        }
    }

    if(shaderMode == 1){
            vec4 Param = texture2D(Particles,v_texcoord);

            vec2 randCoord = vec2(v_texcoord.x+seedX,v_texcoord.y+seedY);
            vec4 randomParam = texture2D(randTex,randCoord);

            renderedImagePixel = Param + (randomParam-vec4(0.5,0.5,0.5,0.5))/100.0;
    }

    if(shaderMode == 2){

    }


    //renderedImagePixel = texture2D(Particles,v_texcoord);


    gl_FragColor = renderedImagePixel;

}

