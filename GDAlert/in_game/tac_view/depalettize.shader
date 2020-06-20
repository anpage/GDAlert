shader_type canvas_item;

uniform sampler2D palette_tex;

void fragment() {
	float pal_index = texture(TEXTURE,UV).r;
	vec4 col = texture(palette_tex,vec2(pal_index,0));
	// Convert from 6-bit to 8-bit
	col *= 4.;
	COLOR = col;
}
