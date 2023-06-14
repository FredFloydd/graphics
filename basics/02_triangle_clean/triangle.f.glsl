void main(void) {
  gl_FragColor[0] = 0.0;
  gl_FragColor[1] = 0.3;
  gl_FragColor[2] = 0.1;
  gl_FragColor[3] = floor(mod(gl_FragCoord.y, 2.0));
}

