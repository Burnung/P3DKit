#version 450 core
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Generates vertices of a triangle that covers the whole screen in clip space.


layout(location=0) out vec2 screenPosition;


void main()
{

	if(gl_VertexID == 0) {
		screenPosition = vec2(1.0, 2.0);
		gl_Position = vec4(1.0, 3.0, 0.0, 1.0);
	}
	else if(gl_VertexID == 1) {
		screenPosition = vec2(-1.0, 0.0);
		gl_Position = vec4(-3.0, -1.0, 0.0, 1.0);
	}
	else /* if(gl_VertexID == 2) */ {
		screenPosition = vec2(1.0, 0.0);
		gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
	}
}
