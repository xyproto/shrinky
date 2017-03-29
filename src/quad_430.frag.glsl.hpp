static const char *g_shader_fragment_quad = ""
#if defined(USE_LD)
"quad_430.frag.glsl"
#else
"#version 430\n"
"layout(location=0)uniform vec3[4] e;"
"in vec2 i;"
"out vec4 r;"
"void main()"
"{"
"vec2 o=i;"
"if(e[3].g>1.)o.r*=e[3].g;"
"else o.g/=e[3].g;"
"vec3 i=normalize(e[1]),t=normalize(cross(i,e[2])),v=normalize(o.r*t+o.g*normalize(cross(t,i))+i);"
"float l=dot(-e[0],v),c=1.+sin(e[3].r/4444.)*.1;"
"vec3 s=l*v+e[0];"
"float n=dot(s,s);"
"if(n<=c)"
"{"
"vec3 o=(l-sqrt(c*c-n*n))*v+e[0];"
"r=vec4(o*dot(o,vec3(1.)),1.);"
"}"
"else r=vec4(.0,.0,.0,1.);"
"}"
#endif
"";
