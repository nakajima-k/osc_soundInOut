#version 120

uniform vec2 resolution;
uniform float volL;
uniform float volR;
uniform float time;
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

  right =st.y - 0.8;
  right += sin(st.x * 14.0) * volR/10.;

  float y = step(0.5,st.x);
  float pct = plot(st,y);

  vec3 color2 = vec3(1.0 - pow(abs(right),0.1));

  vec3 color = vec3(1.0 - pow(abs(left),0.1))+color2;

  color = fract(color);
  // color = (pct)* vec3(y)+pct * color;
  // color = pct * vec3(y)+pct * color;

	gl_FragColor = vec4(color,1.0);
}
