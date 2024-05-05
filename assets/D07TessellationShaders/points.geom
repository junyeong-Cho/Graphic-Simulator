#version 450


layout(lines)in;
layout(points,max_vertices=128)out;

void main()
{
    
    gl_PointSize=6.;
    gl_Position=gl_in[0].gl_Position;
    EmitVertex();
    
    for(int i=1; i < 127; ++i)
    {
        float t = float(i) / 127.;
        gl_PointSize = 1.;
        gl_Position = mix(gl_in[0].gl_Position, gl_in[1].gl_Position,t);
        EmitVertex();
    }
    
    gl_PointSize=6.;
    gl_Position=gl_in[1].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}
