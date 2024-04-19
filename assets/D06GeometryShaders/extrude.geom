#version 300 es
precision highp float;
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in vec3 vPosition[3];
in vec3 vNormals[3];
in vec2 vTextureCoordinates[3];

out vec2 fTextureCoordinates;
out float fLightIntensity;

uniform float uExtrudeFactor;
uniform bool uFlat;
uniform mat4 uProjection;
uniform mat4 uViewMatrix;

void main() 
{
    vec3 normal;
    if (uFlat) 
    {
        // Compute flat normal from triangle vertices
        vec3 edge1 = vPosition[1] - vPosition[0];
        vec3 edge2 = vPosition[2] - vPosition[0];
        normal = normalize(cross(edge1, edge2));
    } 

    // Base triangle
    for (int i = 0; i < 3; i++) 
    {
        gl_Position = uProjection * uViewMatrix * vec4(vPosition[i], 1.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 0.0;  // Base is dark
        EmitVertex();
    }
    EndPrimitive();

    // Sides (extruded as triangle strip)
    for (int i = 0; i < 3; i++) 
    {
        int next = (i + 1) % 3;
    
        // Determine extrusion direction for the current vertex
        vec3 currentExtrudeDirection = uFlat ? normal : normalize(vNormals[i]);
        vec3 nextExtrudeDirection    = uFlat ? normal : normalize(vNormals[next]);
    
        // Original vertex i
        gl_Position = uProjection * uViewMatrix * vec4(vPosition[i], 1.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = (uExtrudeFactor == 0.0) ? 1.0 : 0.0; // If extrude factor is 0, use light intensity for top
        EmitVertex();

        // Extruded vertex i
        gl_Position = uProjection * uViewMatrix * vec4(vPosition[i] + currentExtrudeDirection * max(uExtrudeFactor, 0.001), 1.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0;  // Top is light
        EmitVertex();

        // Original vertex next 
        gl_Position = uProjection * uViewMatrix * vec4(vPosition[next], 1.0);
        fTextureCoordinates = vTextureCoordinates[next];
        fLightIntensity = (uExtrudeFactor == 0.0) ? 1.0 : 0.0; // If extrude factor is 0, use light intensity for top
        EmitVertex();
    
        // Extruded vertex next
        gl_Position = uProjection * uViewMatrix * vec4(vPosition[next] + nextExtrudeDirection * max(uExtrudeFactor, 0.001), 1.0);
        fTextureCoordinates = vTextureCoordinates[next];
        fLightIntensity = 1.0;  
        EmitVertex();
    }

    // Close the strip with the first two vertices to form a loop
    EndPrimitive();

    // Top triangle
    for (int i = 0; i < 3; i++) 
    {
        gl_Position = uProjection * uViewMatrix * vec4(vPosition[i] + (uFlat ? normal : normalize(vNormals[i])) * max(uExtrudeFactor, 0.001), 1.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0;
        EmitVertex();
    }   
    EndPrimitive();
}
