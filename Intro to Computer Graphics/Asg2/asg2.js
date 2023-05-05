// built off examples in book and in assignment videos
// Vertex shader program
var VSHADER_SOURCE = `
  attribute vec4 a_Position;
  uniform float u_Size;
  uniform mat4 u_ModelMatrix;
  uniform mat4 u_GlobalRotateMatrix;
  attribute vec2 a_UV;
  varying vec2 v_UV;
    void main() {
      gl_Position = u_GlobalRotateMatrix * u_ModelMatrix * a_Position;
      gl_PointSize = u_Size;
      v_UV=a_UV;
    }`


// Fragment shader program
var FSHADER_SOURCE =`
  precision mediump float;
    uniform vec4 u_FragColor;
    void main() {
      gl_FragColor = u_FragColor;
    }`

// GLOBALS
let canvas;
let gl;
let a_Position;
let u_FragColor;
let u_Size;
let u_ModelMatrix;
let u_GlobalRotateMatrix;
let a_UV;

function setupWebGL(){
  // Retrieve <canvas> element
  canvas = document.getElementById('webgl');

  // Get the rendering context for WebGL
  gl = canvas.getContext("webgl",{preserveDrawingBuffer: true});
  if (!gl) {
    console.log('Failed to get the rendering context for WebGL');
    return;
  }
  gl.enable(gl.DEPTH_TEST);
}

function connectVariablesToGLSL(){
  // Initialize shaders
  if (!initShaders(gl, VSHADER_SOURCE, FSHADER_SOURCE)) {
    console.log('Failed to intialize shaders.');
    return;
  }

  // // Get the storage location of a_Position
  a_Position = gl.getAttribLocation(gl.program, 'a_Position');
  if (a_Position < 0) {
    console.log('Failed to get the storage location of a_Position');
    return;
  }
  a_UV = gl.getAttribLocation(gl.program, 'a_UV');
  if (a_UV < 0) {
    console.log('Failed to get the storage location of a_UV');
    return;
  }
  // Get the storage location of u_FragColor
  u_FragColor = gl.getUniformLocation(gl.program, 'u_FragColor');
  if (!u_FragColor) {
    console.log('Failed to get the storage location of u_FragColor');
    return;
  }

  u_ModelMatrix = gl.getUniformLocation(gl.program, 'u_ModelMatrix');
  if (!u_ModelMatrix) {
    console.log('Failed to get the storage location of u_ModelMatrix');
    return;
  }

  u_GlobalRotateMatrix = gl.getUniformLocation(gl.program, 'u_GlobalRotateMatrix');
  if (!u_GlobalRotateMatrix) {
    console.log('Failed to get the storage location of u_GlobalRotateMatrix');
    return;
  }

  u_Size = gl.getUniformLocation(gl.program,'u_Size');
  if (!u_Size) {
    console.log('Failed to get the storage location of u_Size');
    return;
  }
  //Set initial value for matrix to identity
  var identityM = new Matrix4();
  gl.uniformMatrix4fv(u_ModelMatrix, false, identityM.elements);

}

let g_globalAngle=0;
let g_legAngle=0;
let g_tailConnectorAngle=0;
let g_tailAngle=30;
let g_walkAnimation=false;
let g_TailAnimation=false;
function addActionsForHtmlUI(){
  document.getElementById("angleSlide").addEventListener('input', function(){g_globalAngle=this.value; renderScene();});
  document.getElementById("legSlide").addEventListener('input', function(){g_legAngle=this.value; renderScene();});
  document.getElementById("tailConSlide").addEventListener('input', function(){g_tailConnectorAngle=this.value; renderScene();});
  document.getElementById("tailSlide").addEventListener('input', function(){g_tailAngle=this.value; renderScene();});
  document.getElementById("animationWalkOn").onclick=function(){g_walkAnimation=true;};
  document.getElementById("animationWalkOff").onclick=function(){g_walkAnimation=false;};
  document.getElementById("animationTailOn").onclick=function(){g_TailAnimation=true;};
  document.getElementById("animationTailOff").onclick=function(){g_TailAnimation=false;};
}

function main() {
  setupWebGL();
  connectVariablesToGLSL();
  
  
  // Register function (event handler) to be called on a mouse press
  addActionsForHtmlUI();

  // Specify the color for clearing <canvas>
  gl.clearColor(0.0, 0.0, 0.0, 1.0);
  // Clear <canvas>
  //gl.clear(gl.COLOR_BUFFER_BIT);
  requestAnimationFrame(tick);
}
var g_startTime=performance.now()/1000.0;
var g_seconds=performance.now()/1000.0-g_startTime;

function tick(){
  g_seconds=performance.now()/1000.0-g_startTime;
  updateAnimationAngles();
  updateSliders();
  renderScene();
  requestAnimationFrame(tick);
}

//simple function to update the sliders for control of specific joints
function updateSliders(){
  var yAni=document.getElementById('legSlide');
  yAni.value=g_legAngle;
  var tail1Ani=document.getElementById('tailConSlide');
  tail1Ani.value=g_tailConnectorAngle;
  var tail2Ani=document.getElementById('tailSlide');
  tail2Ani.value=g_tailAngle;
  
  
}

function updateAnimationAngles(){
  if (g_walkAnimation) {
    g_legAngle=(45*Math.sin(g_seconds));
  }
  if (g_TailAnimation) {
    g_tailConnectorAngle=(45*Math.sin(g_seconds));
    g_tailAngle=(Math.abs(160*Math.sin(g_seconds)));
  }
  
}



function renderScene(){
  
  var globalRotMat=new Matrix4().rotate(g_globalAngle,0,1,0);
  gl.uniformMatrix4fv(u_GlobalRotateMatrix, false, globalRotMat.elements);
  
  // Clear <canvas>
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
  gl.clear(gl.COLOR_BUFFER_BIT);


  //Start Animal

  var head = new Cube();
  head.color=[.54,.54,.54,1.0];
  head.matrix.translate(-.33,-.25,-0.35);
  // head.matrix.rotate(-5,1,0,0);
  head.matrix.scale(0.66, .6, .5);
  head.render();

  var face = new Cube();
  face.color=[.85,.85,.85,1.0];
  face.matrix.translate(-.25,-.25,-.55);
  // face.matrix.rotate(-5,1,0,0);
  face.matrix.scale(.5, .5, .3);
  face.render();

  var body = new Cube();
  body.color=[.85,.85,.85,1.0];
  body.matrix.translate(-.25,-.25,0);
  // body.matrix.rotate(-5,1,0,0);
  body.matrix.scale(.5, .5, .75);
  body.render();

  var mouth = new Cube();
  mouth.color=[1,.94,.90,1.0];
  mouth.matrix.translate(-.13,-.23,-.7);
  // mouth.matrix.rotate(-5,1,0,0);
  mouth.matrix.scale(.25, .25, .3);
  mouth.render();

  var nose = new Cube();
  nose.color=[0.1,0.1,0.1,1.0];
  nose.matrix.translate(-.04,-.06,-.71);
  // nose.matrix.rotate(-5,1,0,0);
  nose.matrix.scale(.08, .08, .01);
  nose.render();

  var lips = new Cube();
  lips.color=[0.1,0.1,0.1,1.0];
  lips.matrix.translate(-.13,-.23,-.71);
  // lips.matrix.rotate(-5,1,0,0);
  lips.matrix.scale(.25, .08, .01);
  lips.render();

  var eye1 = new Cube();
  eye1.color=[0.1,0.1,0.1,1.0];
  eye1.matrix.translate(.06,.02,-.6);
  // eye1.matrix.rotate(-5,1,0,0);
  eye1.matrix.scale(.1, .1, .1);
  eye1.render();

  var eye2 = new Cube();
  eye2.color=[0.1,0.1,0.1,1.0];
  eye2.matrix.translate(-.16,.02,-.6);
  // eye2.matrix.rotate(-5,1,0,0);
  eye2.matrix.scale(.1, .1, .1);
  eye2.render();

  var ear1 = new Cube();
  ear1.color=[1,1,1,1.0];
  ear1.matrix.translate(.04,.25,-.45);
  // ear1.matrix.rotate(-5,1,0,0);
  ear1.matrix.scale(.18, .2, .1);
  ear1.render();

  var ear2 = new Cube();
  ear2.color=[1,1,1,1.0];
  ear2.matrix.translate(-.22,.25,-.45);
  // ear2.matrix.rotate(-5,1,0,0);
  ear2.matrix.scale(.18, .2, .1);
  ear2.render();

  var leg1 = new Cube();
  leg1.color=[.3,.3,.3,1.0];
  leg1.matrix.translate(.23,-.2,-.15);
  leg1.matrix.rotate(-g_legAngle,1,0,0);
  leg1.matrix.scale(-.18, -.45, -.2);
  leg1.render();

  var leg2 = new Cube();
  leg2.color=[.3,.3,.3,1.0];
  leg2.matrix.translate(-.05,-.2,-.15);
  leg2.matrix.rotate(g_legAngle,1,0,0);
  leg2.matrix.scale(-.18, -.45, -.2);
  leg2.render();

  var leg3 = new Cube();
  leg3.color=[.3,.3,.3,1.0];
  leg3.matrix.translate(.23,-.2,.7);
  leg3.matrix.rotate(-g_legAngle,1,0,0);
  leg3.matrix.scale(-.18, -.45, -.2);
  leg3.render();

  var leg4 = new Cube();
  leg4.color=[.3,.3,.3,1.0];
  leg4.matrix.translate(-.05,-.2,.7);
  leg4.matrix.rotate(g_legAngle,1,0,0);
  leg4.matrix.scale(-.18, -.45, -.2);
  leg4.render();


  var tailConnect = new Cube();
  tailConnect.color=[.79,.79,.79,1.0];
  tailConnect.matrix.translate(-.09,-.05,.7);
  tailConnect.matrix.rotate(-g_tailConnectorAngle,0,1,0);
  var conCoord=new Matrix4(tailConnect.matrix);
  tailConnect.matrix.scale(.2, .2, .1);
  tailConnect.render();

  var tail = new Cube();
  tail.color=[.88,.88,.88,1.0];
  tail.matrix=conCoord;
  tail.matrix.translate(.2,.2,.1);
  tail.matrix.rotate(-g_tailAngle,1,0,0);
  tail.matrix.scale(-.18, -.45, -.15);
  tail.render();



  // var leftArm = new Cube();
  // leftArm.color = [1,1,0,1];
  // leftArm.matrix.setTranslate(0,-.5,0.0);
  // leftArm.matrix.rotate(-5,1,0,0);
  // leftArm.matrix.rotate(-g_legAngle,0,0,1);  
  // var yellowCoordMat=new Matrix4(leftArm.matrix);
  // leftArm.matrix.scale(.25,.7,.5);
  // leftArm.matrix.translate(-.5,0,0);
  // leftArm.render();


  // var box = new Cube();
  // box.color = [1,0,1,1];
  // box.matrix=yellowCoordMat;
  // box.matrix.translate(0,.65,0);
  // box.matrix.rotate(g_tailConnectorAngle,0,0,1);
  // box.matrix.scale(.3,.3,.2);
  // box.matrix.translate(-.5,0,-.001);
  // box.render();

}