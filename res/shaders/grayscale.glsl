#version 120
#ifdef GL_ES
precision mediump float;
#endif

//from https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson3

uniform sampler2D al_tex;
//uniform vec2 resolution;

varying vec4 varying_color;
varying vec2 varying_texcoord;

//Radius of vignette, where 0.5 is a circle fitting the screen
//const float RADIUS = 0.75;

//softness of vignette, between 0.0 and 1.0
//const float SOFTNESS = 0.45;

//not doing sepia thanks

void main() {
	//sample texture
	vec4 texColor = texture2D(al_tex, varying_texcoord);

	// VIGNETTE
	// find center
	//vec2 position = (gl_FragCoord.xy / resolution.xy) - vec2(0.5);

	//determine vector length of center position
	//float len = length(position);

	// smoothstep creates a nice vignette
	//float vignette = smoothstep(RADIUS, RADIUS-SOFTNESS, len);

	// apply with 50% opacity
	//texColor.rgb = mix(texColor.rgb, texColor.rgb * vignette, 0.5);

	// GRAYSCALE

	// NTSC conversion weights apparently
	float gray = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));

	// skipping sepia

	gl_FragColor = vec4(gray, gray, gray, 1.0);
}

