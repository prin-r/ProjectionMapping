#version 120

// Sets the maximum number of iterations per pixel.
// Note: anything above 256 is a waste of energy,
//       because of the limited floating point precision.

uniform sampler2D tex;
uniform vec3 basisX;
uniform vec3 basisY;
uniform vec3 basisZ;

float clamp(float a) {
	if (a > 1.0f) return 1.0f;
	if (a < 0.0f) return 0.0f;
	return a;
}

void main() {
	//vec2 texCoord = vec2( 0 );
	vec2 myCoordinate2D = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y);

	vec3 myCoordinate3D = vec3(myCoordinate2D.x, myCoordinate2D.y, 1);
	vec3 transformation3D = myCoordinate3D.x * basisX + myCoordinate3D.y * basisY + myCoordinate3D.z * basisZ;
	float invZ = 1.0 / transformation3D.z;
	vec2 transformation2D = transformation3D.xy * invZ;
	
	vec4 color = texture2D(tex, transformation2D);

	if (transformation2D.x < 0 || transformation2D.x > 1 || transformation2D.y < 0 || transformation2D.y > 1) {
		color = vec4(0, 0, 0, 0);
	}

	gl_FragColor = color;// vec4(myCoordinate2D.x, myCoordinate2D.y, 0, 1);
}
















