#version 300 es
layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform float uShrinkFactor;  // Shrink factor from 0 (fully shrunk to center) to 1 (original shape)
uniform mat4 uViewMatrix;
uniform mat4 uProjection;


in vec4 vColor[];

out vec4 gColor;

vec3 Verts[3];
vec3 Center;

void EmitShrunkVertex(int i, float shrink)
{
	vec3 point = mix(Center, Verts[i], shrink);
	gl_Position = uProjection * uViewMatrix * vec4(point, 1.0);
	gColor = vColor[i];
	EmitVertex();
}

void main()
{
	Verts[0] = gl_in[0].gl_Position.xyz;
	Verts[1] = gl_in[1].gl_Position.xyz;
	Verts[2] = gl_in[2].gl_Position.xyz;
	
	Center = (Verts[0]+Verts[1]+Verts[2]) / 3;
	if(Center.x > 0)
	{
	
	EmitShrunkVertex(0, uShrinkFactor);
	EmitShrunkVertex(1, uShrinkFactor);
	EmitShrunkVertex(2, uShrinkFactor);
	EndPrimitive();
	
	Verts[0] -= normalize(Verts[0]) * 0.8;
	Verts[1] -= normalize(Verts[1]) * 0.8;
	Verts[2] -= normalize(Verts[2]) * 0.8;
	
	EmitShrunkVertex(0, uShrinkFactor/2);
	EmitShrunkVertex(1, uShrinkFactor/2);
	EmitShrunkVertex(2, uShrinkFactor/2);
	EndPrimitive();
	}

}
