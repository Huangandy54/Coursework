class Cube{
  constructor(){
    this.type='cube';
    //this.position=[0.0,0.0,0.0];
    this.color=[1.0,1.0,1.0,1.0];
    //this.size=5.0;
    //this.segments = 10;
    this.matrix = new Matrix4();
  }

  render(){
    var rgba = this.color;
    
    // Pass the color of a point to u_FragColor variable
    gl.uniform4f(u_FragColor, rgba[0], rgba[1], rgba[2], rgba[3]);

    //pass matrix to u_ModelMatrix attrib
    gl.uniformMatrix4fv(u_ModelMatrix, false, this.matrix.elements);
    // Draw

    // Pass the color of a point to u_FragColor variable
    // gl.uniform4f(u_FragColor, rgba[0]*.9, rgba[1]*9, rgba[2]*.9, rgba[3]);
    //left
    drawTriangle3DUV([0,0,0,  0,0,1,  0,1,1],[1,1,1,1,1,1]);
    drawTriangle3DUV([0,0,0,  0,1,1,  0,1,0],[1,1,1,1,1,1]);
    //right
    drawTriangle3DUV([1,1,1,  1,0,0,  1,1,0],[1,1,1,1,1,1]);
    drawTriangle3DUV([1,0,0,  1,1,1,  1,0,1],[1,1,1,1,1,1]);
    //front
    drawTriangle3DUV([1,1,0,  0,0,0,  0,1,0],[1,1,1,1,1,1]);
    drawTriangle3DUV([1,1,0,  1,0,0,  0,0,0],[1,1,1,1,1,1]);
    //back
    drawTriangle3DUV([0,1,1,  0,0,1,  1,0,1],[1,1,1,1,1,1]);
    drawTriangle3DUV([1,1,1,  0,1,1,  1,0,1],[1,1,1,1,1,1]);
    //bottom 
    drawTriangle3DUV([1,0,1,  0,0,0,  1,0,0],[1,1,1,1,1,1]);
    drawTriangle3DUV([1,0,1,  0,0,1,  0,0,0],[1,1,1,1,1,1]);
    //top
    drawTriangle3DUV([1,1,1,  1,1,0,  0,1,0],[1,1,1,1,1,1]);
    drawTriangle3DUV([1,1,1,  0,1,0,  0,1,1],[1,1,1,1,1,1]);

  }
}

