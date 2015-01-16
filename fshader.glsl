#ifdef GL_ES
// Set default precision to high
precision highp int;
precision highp float;
#endif

//RT2DShader

varying vec2 v_texcoord;

uniform sampler2D Lines;
uniform int numLines;
uniform int numParametersL;

uniform sampler2D Particles;
uniform int numParticles;
uniform int numParametersP;

uniform sampler2D randTex;
uniform float seedX;
uniform float seedY;

uniform sampler2D renderedImage;

//NIY
uniform float mouseX;
uniform float mouseY;

uniform int width;
uniform int height;

uniform int samples;

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
//Calculate intersection between a Vector and a Line
bool lineSegmentIntersection(vec2 r0, vec2 r1, vec2 a, vec2 b)
{
    vec2 s1, s2;
    s1 = r1 - r0;
    s2 = b - a;

    highp float s, t;
    s = (-s1.y * (r0.x - a.x) + s1.x * (r0.y - a.y)) / (-s2.x * s1.y + s1.x * s2.y);
    t = (s2.x * (r0.y - a.y) - s2.y * (r0.x - a.x)) / (-s2.x * s1.y + s1.x * s2.y);

    if ((s >= 0.0) && (s <= 1.0) && (t >= 0.0) && (t <= 1.0))
    {
        // Collision detected
        // Return the point of intersection
        float xI = r0.x + (t * s1.x);
        float yI = r0.y + (t * s1.y);
        float dist = sqrt((r0.x-xI)*(r0.x-xI)+(r0.y-yI)*(r0.y-yI));
        //return vec3(xI, yI, dist);
        return true;
    }

    //return vec3(0,0,-1.0); // No collision
    return false;
}


float clampFl(float value){
    if(value > 1.0) return 1.0;
    return value;
}

void main()
{
    vec4 renderedImagePixel = texture2D(renderedImage,v_texcoord);

    if(shaderMode == 1){
        vec4 pixelColor = vec4(0.0,0.0,0.0,0.0);

        vec4 randVal = texture2D(randTex,vec2(v_texcoord.x + seedX,v_texcoord.y + seedY));
        vec2 pos = vec2(v_texcoord.x + randVal.x/float(width/4),v_texcoord.y + randVal.y/float(height/4)) ;

        for(int i = 0; i < numParticles; i++){
            float xP = unpack(texelFetch(Particles,vec2(float(i),3.0),ivec2(numParticles,numParametersP)));
            float yP = unpack(texelFetch(Particles,vec2(float(i),2.0),ivec2(numParticles,numParametersP)));
            float brightness = unpack(texelFetch(Particles,vec2(float(i),1.0),ivec2(numParticles,numParametersP)));
            vec4  color = texelFetch(Particles,vec2(float(i),0.0),ivec2(numParticles,numParametersP));

            for(int j = 0; j < numLines; j++){
                float x1 = unpack(texelFetch(Lines,vec2(float(j),3.0),ivec2(numLines,numParametersL)));
                float y1 = unpack(texelFetch(Lines,vec2(float(j),2.0),ivec2(numLines,numParametersL)));
                vec2 a = vec2(x1,y1);
                float x2 = unpack(texelFetch(Lines,vec2(float(j),1.0),ivec2(numLines,numParametersL)));
                float y2 = unpack(texelFetch(Lines,vec2(float(j),0.0),ivec2(numLines,numParametersL)));
                vec2 b = vec2(x2,y2);

                vec2 r1 = vec2(xP,yP);
                vec2 r2 = pos;

                if(lineSegmentIntersection(a,b,r1,r2)){
                    brightness = 0.0;
                    j = numLines;
                }
            }


            brightness = brightness * 0.055*(1.0/distance(vec2(xP,yP),pos))*(1.0+sin(distance(vec2(xP,yP),pos)*300));
            pixelColor += (color * brightness);
        }

        renderedImagePixel = ( pixelColor + renderedImagePixel*samples) / (samples+1);
    }


    gl_FragColor = renderedImagePixel;

}

