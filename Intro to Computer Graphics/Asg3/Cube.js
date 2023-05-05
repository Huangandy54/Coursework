class Cube{
  constructor(){
    this.type='cube';
    //this.position=[0.0,0.0,0.0];
    this.color=[1.0,1.0,1.0,1.0];
    //this.size=5.0;
    //this.segments = 10;
    this.matrix = new Matrix4();
    this.textureNum=-2;
  }

  render(){
    var rgba = this.color;
    
    gl.uniform1i(u_whichTexture, this.textureNum);

    // Pass the color of a point to u_FragColor variable
    gl.uniform4f(u_FragColor, rgba[0], rgba[1], rgba[2], rgba[3]);

    //pass matrix to u_ModelMatrix attrib
    gl.uniformMatrix4fv(u_ModelMatrix, false, this.matrix.elements);
    // Draw

    //left
    drawTriangle3DUV([0,0,0,  0,0,1,  0,1,1], [0,0, 0,1, 1,1]);
    drawTriangle3DUV([0,0,0,  0,1,1,  0,1,0], [0,0, 1,1, 1,0]);
    //right
   // gl.uniform4f(u_FragColor, rgba[0]*9, rgba[1]*9, rgba[2]*9, rgba[3]);
    drawTriangle3DUV([1,1,1,  1,0,0,  1,1,0], [1,1, 0,0, 0,1]);
    drawTriangle3DUV([1,0,0,  1,1,1,  1,0,1], [0,0, 1,1, 1,0]);
    //front
   // gl.uniform4f(u_FragColor, rgba[0], rgba[1], rgba[2], rgba[3]);
    drawTriangle3DUV([1,1,0,  0,0,0,  0,1,0], [1,1, 0,0, 0,1]);
    drawTriangle3DUV([1,1,0,  1,0,0,  0,0,0], [1,1, 1,0, 0,0]);
    //back
   // gl.uniform4f(u_FragColor, rgba[0]*9, rgba[1]*9, rgba[2]*9, rgba[3]);
    drawTriangle3DUV([0,1,1,  0,0,1,  1,0,1], [0,0, 0,1, 1,1]);
    drawTriangle3DUV([1,1,1,  0,1,1,  1,0,1], [0,1, 0,0, 1,1]);
    //bottom 
   // gl.uniform4f(u_FragColor, rgba[0], rgba[1], rgba[2], rgba[3]);
    drawTriangle3DUV([1,0,1,  0,0,0,  1,0,0], [0,0, 1,1, 1,0]);
    drawTriangle3DUV([1,0,1,  0,0,1,  0,0,0], [0,0, 0,1, 1,1]);
    //top
   // gl.uniform4f(u_FragColor, rgba[0]*9, rgba[1]*9, rgba[2]*9, rgba[3]);
    drawTriangle3DUV([1,1,1,  1,1,0,  0,1,0], [0,0, 0,1, 1,1]);
    drawTriangle3DUV([1,1,1,  0,1,0,  0,1,1], [0,0, 1,1, 1,0]);

  }
}

