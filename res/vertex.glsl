#version 330 core

//Signs for the directions along the horizontal and
//vertical vectors to expand the points along, indexed
//by vertex id
const int signs[8] = int[8](
	1, 1,
	-1, 1,
	1, -1,
	-1, -1
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

	//Get the normal for the plane so that it's oriented facing the camera
	vec3 n = normalize(pos - eye_pos.xyz);
	//Now determine the planes horizontal and vertical vectors using +Y as upish
	vec3 horiz = normalize(cross(vec3(0, 1, 0), n));
	vec3 vert = normalize(cross(horiz, n));
	vec4 h = vec4(horiz, 0);
	vec4 v = vec4(vert, 0);

	//Expand out this vertex to its point on the quad
	gl_Position = vec4(pos, 1) + signs[gl_VertexID * 2] * h + signs[gl_VertexID * 2 + 1] * v;
	//Transform and project the quad
	gl_Position = proj * view * gl_Position;
}

