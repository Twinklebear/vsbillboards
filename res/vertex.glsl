#version 330 core

const vec2 quad[4] = vec2[4](
	vec2(-1, -1),
	vec2(1, -1),
	vec2(-1, 1),
	vec2(1, 1)
);

//Viewing matrices and eye pos
//the eye pos is used to make the billboards face the camera
layout(std140) uniform Viewing {
	mat4 view;
	mat4 proj;
	vec4 eye_pos;
};

//Colors for the vertices, indexed by sprite_id + glVertexID
//This could be other sprite attributes that you need, eg. uv coordinates
//for the sprite textures, etc.
layout(std140) uniform Colors {
	//We have "4" types of sprites and a color for each vertex per sprite
	vec4 colors[16];
};

layout(location = 0) in vec3 pos;
layout(location = 1) in int sprite_id;

out vec4 fcolor;

void main(void){
	//Select the color (uv, w/e) for this sprite and vertex
	fcolor = colors[sprite_id * 4 + gl_VertexID];

	//Expand out this vertex to its point on the quad
	gl_Position = vec4(pos.xy + quad[gl_VertexID], pos.z, 1);
	//Transform and project the quad
	mat4 modified_view = view;
	modified_view[0] = vec4(1, 0, 0, 0);
	modified_view[1] = vec4(0, 1, 0, 0);
	modified_view[2] = vec4(0, 0, 1, 0);
	gl_Position = proj * modified_view * gl_Position;
}

