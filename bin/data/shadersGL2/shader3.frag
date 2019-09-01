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


void main()
{

  // float windowWidth = 1024.0;
  // float windowHeight = 768.0;

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

  vec3 colorR = vec3(1.0 - pow(abs(right),0.1));
  vec3 colorL = vec3(1.0 - pow(abs(left),0.1));

  vec3 color = colorR + colorL;
  // color = fract(color*5.);
  if (shadermode == 0) {
    color = (1.0-pct)*color;
  } else if (shadermode == 1){
    color = (1.0-pct)-color;
  }  else {
    color = (1.0-pct)/color;
  }


	gl_FragColor = vec4(color,1.0);
}
