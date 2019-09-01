#version 120

uniform vec2 resolution;
uniform float volL;
uniform float volR;
uniform float time;
uniform float shadermode;
const int NUM = 20;
uniform float freq[512];

float plot(vec2 st,float pct) {
  return smoothstep(pct-0.04,pct,st.y) - smoothstep(pct,pct+0.04,st.y);
}

vec3 colorB = vec3(0);
vec3 colorW = vec3(1);
vec3 color1 = vec3(0.92,0.3,0.12);
vec3 color2 = vec3(0.2,0.141,0.61);

vec2 sphIntersect ( in vec3 ro, in vec3 rd ,in vec3 ce, float ra) {
  vec3 oc = ro -ce;
  float b = dot(oc,rd);
  float c = dot(oc,oc) - ra * ra;
  float h = b* b -c;
  if( h<0.0) return vec2(-1.0);
  h = sqrt(h);
  return vec2(-b-h,-b+h);
}


void main()
{

  vec2 st = gl_FragCoord.xy / resolution;

  float left;
  float right;

  left =st.y - 0.2;
  left += sin(st.x * 10.0) * volL/10.;
  // left += volL;

  right =st.y - 0.8;
  right += sin(st.x * 10.0) * volR/10.;

  if (volL>6.) {
    left += cos(st.x*5.)+pow(volL,4.);
    left = left*5.;
  }

  if (volR>6.) {
    right /= sin(st.y*5.)+pow(volR,4.);
    right = fract(right);
  }

  float y = step(abs(sin(volL+time)),st.x);
  y = y + step(abs(sin(volR+time)),st.x);
  float pct = plot(st,y);
  pct = pct + y;
  // pct = sin(time)+pct*cos(time)+y;
  vec3 pct3 = vec3(st.x);
  vec3 colorR = vec3(1.0 - pow(abs(right),0.1));
  vec3 colorL = vec3(1.0 - pow(abs(left),0.1));
  vec3 colorG = vec3(0.);
  vec3 color = mix(colorW,colorB,pct);

  //rect
  vec2 bl = step(vec2(0.1),st);
  pct = bl.x * bl.y;
  vec2 tr = step(vec2(0.1),1.0-st);
  pct *= tr.x * tr.y;

  // color = fract(color*5.);
  if (shadermode == 0) {
    color = (1.0-pct)*color;
  } else if (shadermode == 1){
    // color =vec3(0.);
    color = mix(color1,color2,pct3)+color;
    color = mix(color,vec3(0.8,0.,0.),pct3.r);
    color = mix(color,vec3(0,0.9,0.),pct3.g);
    color = mix(color,color2,pct3.b);
  }  else {
    color = (1.0-pct)/color;
  }

	gl_FragColor = vec4(color,1.0);
}
