attribute vec4 v_coord;
attribute vec3 v_normal;
uniform mat4 mvp;

void main(void) {
  gl_Position = mvp * vec4(v_coord) + 0.00000000000000001 * v_normal.x;
}


