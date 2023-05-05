// built off examples in book and in assignment videos
// Vertex shader program
var VSHADER_SOURCE = `
  precision mediump float;
  attribute vec4 a_Position;
  attribute vec2 a_UV;
  attribute vec3 a_Normal;
  varying vec2 v_UV;
  varying vec3 v_Normal;
  varying vec4 v_VertPos;
  uniform mat4 u_ModelMatrix;
  uniform mat4 u_GlobalRotateMatrix;
  uniform mat4 u_ViewMatrix;
  uniform mat4 u_ProjectionMatrix;
    void main() {
      gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_GlobalRotateMatrix * u_ModelMatrix * a_Position;
      v_UV=a_UV;
      v_Normal=a_Normal;
      v_VertPos= u_ModelMatrix*a_Position;
    }`


// Fragment shader program
var FSHADER_SOURCE =`
  precision mediump float;
  varying vec2 v_UV;
  varying vec3 v_Normal;
  uniform vec4 u_FragColor;
  uniform sampler2D u_Sampler0;
  uniform sampler2D u_Sampler1;
  uniform int u_whichTexture;
  uniform vec3 u_lightPos;
  uniform vec3 u_cameraPos;
  varying vec4 v_VertPos;
  uniform bool u_lightOn;
    void main() {
    	//use normal
      if (u_whichTexture == -3) {
        gl_FragColor = vec4((v_Normal+1.0)/2.0, 1.0);
      } 	
      //use color
      else if (u_whichTexture == -2) {
        gl_FragColor = u_FragColor;
      }
      //use UV debug color
      else if (u_whichTexture == -1) {
        gl_FragColor=vec4(v_UV,1.0,1.0);
      }
      //use texture0
      else if(u_whichTexture == 0){
        gl_FragColor=texture2D(u_Sampler0, v_UV);
      }
      //use texture1
      else if(u_whichTexture == 1){
        gl_FragColor=texture2D(u_Sampler1, v_UV);
      }
      else{
        gl_FragColor=vec4(1,.2,.2,1);
      }
      
      vec3 lightVector=u_lightPos-vec3(v_VertPos);
      float r=length(lightVector);

      //N dot L
      vec3 L=normalize(lightVector);
      vec3 N= normalize(v_Normal);
      float nDotL=max(dot(N,L),0.0);


      //reflection
      vec3 R = reflect(-L,N);

      //eye
      vec3 E= normalize(u_cameraPos-vec3(v_VertPos));

      //specular 
      float specular = pow(max(dot(E,R),0.0), 25.0);

      vec3 diffuse=vec3(gl_FragColor)*nDotL;
      vec3 ambient = vec3(gl_FragColor)*0.3;

      if (u_lightOn) {
      	gl_FragColor=vec4(specular + diffuse + ambient,1.0);
      }

    }`

// GLOBALS
let canvas;
let gl;
let a_Position;
let a_UV;
let u_FragColor;
let u_Size;
let u_ModelMatrix;
let u_ProjectionMatrix;
let u_ViewMatrix;
let u_GlobalRotateMatrix;
let u_whichTexture;
let u_Sampler0;
let u_Sampler1;
let u_lightPos;
let u_cameraPos;
let u_lightOn;

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

  a_Normal = gl.getAttribLocation(gl.program, 'a_Normal');
  if (a_Normal < 0) {
    console.log('Failed to get the storage location of a_Normal');
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

  u_ProjectionMatrix = gl.getUniformLocation(gl.program, 'u_ProjectionMatrix');
  if (!u_ProjectionMatrix) {
    console.log('Failed to get the storage location of u_ProjectionMatrix');
    return;
  }

  u_ViewMatrix = gl.getUniformLocation(gl.program, 'u_ViewMatrix');
  if (!u_ViewMatrix) {
    console.log('Failed to get the storage location of u_ViewMatrix');
    return;
  }

  u_GlobalRotateMatrix = gl.getUniformLocation(gl.program, 'u_GlobalRotateMatrix');
  if (!u_GlobalRotateMatrix) {
    console.log('Failed to get the storage location of u_GlobalRotateMatrix');
    return;
  }

  u_Sampler0 = gl.getUniformLocation(gl.program,'u_Sampler0');
  if (!u_Sampler0) {
    console.log('Failed to get the storage location of u_Sampler0');
    return;
  }
  u_Sampler1 = gl.getUniformLocation(gl.program,'u_Sampler1');
  if (!u_Sampler1) {
    console.log('Failed to get the storage location of u_Sampler1');
    return;
  }

  u_whichTexture = gl.getUniformLocation(gl.program,'u_whichTexture');
  if (!u_whichTexture) {
    console.log('Failed to get the storage location of u_whichTexture');
    return;
  }

  u_lightPos = gl.getUniformLocation(gl.program,'u_lightPos');
  if (!u_lightPos) {
    console.log('Failed to get the storage location of u_lightPos');
    return;
  }

  u_cameraPos = gl.getUniformLocation(gl.program,'u_cameraPos');
  if (!u_cameraPos) {
    console.log('Failed to get the storage location of u_cameraPos');
    return;
  }
  u_lightOn = gl.getUniformLocation(gl.program,'u_lightOn');
  if (!u_lightOn) {
    console.log('Failed to get the storage location of u_lightOn');
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
let g_normalOn=false;
let g_lightPos=[3,2.5,-1.5];
let g_lightOn=true;
let g_lightAni=true;
function addActionsForHtmlUI(){
  document.getElementById("normalOn").onclick=function(){g_normalOn=true;};
  document.getElementById("normalOff").onclick=function(){g_normalOn=false;};
  document.getElementById("lightOn").onclick=function(){g_lightOn=true;};
  document.getElementById("lightOff").onclick=function(){g_lightOn=false;};
  document.getElementById("lightAniOn").onclick=function(){g_lightAni=true;};
  document.getElementById("lightAniOff").onclick=function(){g_lightAni=false;};
  document.getElementById("lightSlideX").addEventListener('input', function(){g_lightPos[0]=-this.value/100; renderScene();});
  document.getElementById("lightSlideY").addEventListener('input', function(){g_lightPos[1]=-this.value/100; renderScene();});
  document.getElementById("lightSlideZ").addEventListener('input', function(){g_lightPos[2]=-this.value/100; renderScene();});


  document.getElementById("legSlide").addEventListener('input', function(){g_legAngle=this.value; renderScene();});
  document.getElementById("tailConSlide").addEventListener('input', function(){g_tailConnectorAngle=this.value; renderScene();});
  document.getElementById("tailSlide").addEventListener('input', function(){g_tailAngle=this.value; renderScene();});
  document.getElementById("animationWalkOn").onclick=function(){g_walkAnimation=true;};
  document.getElementById("animationWalkOff").onclick=function(){g_walkAnimation=false;};
  document.getElementById("animationTailOn").onclick=function(){g_TailAnimation=true;};
  document.getElementById("animationTailOff").onclick=function(){g_TailAnimation=false;};
}

function initTextures() {
  

  
  var dirt = new Image();  // Create the image object
  if (!dirt) {
    console.log('Failed to create the image object');
    return false;
  }
  // Register the event handler to be called on loading an image
  dirt.onload = function(){ sendTextureToTEXTURE0(dirt); requestAnimationFrame(tick);};
  // Tell the browser to load an image
  dirt.src = 'block.jpg';


  var grass = new Image();  // Create the image object
  if (!grass) {
    console.log('Failed to create the image object');
    return false;
  }
  // Register the event handler to be called on loading an image
  grass.onload = function(){ sendTextureToTEXTURE1(grass); requestAnimationFrame(tick);};
  // Tell the browser to load an image
  grass.src = 'grass.jpg';


  return true;
}

function sendTextureToTEXTURE0(image) {
  var texture = gl.createTexture();   // Create a texture object
  if (!texture) {
    console.log('Failed to create the texture object');
    return false;
  }
  gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1); // Flip the image's y axis
  // Enable texture unit0
  gl.activeTexture(gl.TEXTURE0);
  // Bind the texture object to the target
  gl.bindTexture(gl.TEXTURE_2D, texture);

  // Set the texture parameters
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
  // Set the texture image
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, image);
  
  // Set the texture unit 0 to the sampler
  gl.uniform1i(u_Sampler0, 0);
  
  // gl.clear(gl.COLOR_BUFFER_BIT);   // Clear <canvas>

  // gl.drawArrays(gl.TRIANGLE_STRIP, 0, n); // Draw the rectangle
}

function sendTextureToTEXTURE1(image) {
  var texture = gl.createTexture();   // Create a texture object
  if (!texture) {
    console.log('Failed to create the texture object');
    return false;
  }
  gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1); // Flip the image's y axis
  // Enable texture unit0
  gl.activeTexture(gl.TEXTURE1);
  // Bind the texture object to the target
  gl.bindTexture(gl.TEXTURE_2D, texture);

  // Set the texture parameters
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
  // Set the texture image
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, image);
  
  // Set the texture unit 0 to the sampler
  gl.uniform1i(u_Sampler1, 1);
  
  // gl.clear(gl.COLOR_BUFFER_BIT);   // Clear <canvas>

  // gl.drawArrays(gl.TRIANGLE_STRIP, 0, n); // Draw the rectangle
}

let camera

function main() {

  setupWebGL();
  connectVariablesToGLSL();
  
  
  // Register function (event handler) to be called on a mouse press
  addActionsForHtmlUI();
  camera= new Camera();
  generateBlocks();
  document.onkeydown =keydown;

  initTextures();

  // Specify the color for clearing <canvas>
  gl.clearColor(0.0, 0.0, 0.0, 1.0);
  // Clear <canvas>
  //gl.clear(gl.COLOR_BUFFER_BIT);
  
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
  var lightX=document.getElementById('lightSlideX');
  lightX.value=g_lightPos[0]*-100;
  var lightY=document.getElementById('lightSlideY');
  lightY.value=g_lightPos[1]*-100;
  var lightZ=document.getElementById('lightSlideZ');
  lightZ.value=g_lightPos[2]*-100;
  
}

function updateAnimationAngles(){
  if (g_walkAnimation) {
    g_legAngle=(45*Math.sin(g_seconds));
  }
  if (g_TailAnimation) {
    g_tailConnectorAngle=(45*Math.sin(g_seconds));
    g_tailAngle=(Math.abs(160*Math.sin(g_seconds)));
  }
  if(g_lightAni){
  	g_lightPos[0]=g_lightPos[0]-.1;
    if ( g_lightPos[0]<-8) {
    	g_lightPos[0]=8;
    	g_lightPos[1]-=1;
    	if (g_lightPos[1]<=0) {
    		g_lightPos[1]=3;
    	}
    }
   }
}

function keydown(ev){
  if (ev.keyCode==87) {//forward
    camera.moveForward();
  }
  if (ev.keyCode==83) {//back
    camera.moveBackwards();
  }
  if (ev.keyCode==68) {//right
    camera.moveRight();
  }
  if (ev.keyCode==65) {//left
    camera.moveLeft();
  }
  if (ev.keyCode==81) {//Q turn left
    camera.panLeft();
  }
  if (ev.keyCode==69) {//E turn right
    camera.panRight();
  }
}


var g_shapeList=[];

function renderScene(){
  //camera 
  


  var globalRotMat=new Matrix4().rotate(g_globalAngle,0,1,0);
  gl.uniformMatrix4fv(u_GlobalRotateMatrix, false, globalRotMat.elements);
  
  //update view matrix 
  gl.uniformMatrix4fv(u_ViewMatrix, false, camera.viewMatrix.elements);

  gl.uniformMatrix4fv(u_ProjectionMatrix, false, camera.projectionMatrix.elements);



  // Clear <canvas>
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
  gl.clear(gl.COLOR_BUFFER_BIT);
  //pass light pos to glsl
  gl.uniform3f(u_lightPos, g_lightPos[0],g_lightPos[1],g_lightPos[2]);
  //pass camera position to glsl
  gl.uniform3f(u_cameraPos, camera.eye.elements[0],camera.eye.elements[1],camera.eye.elements[2]);
  //pass light status
  gl.uniform1i(u_lightOn, g_lightOn);
  //Start drawing stuff

  //light
  var light= new Cube();
  if (g_normalOn) {
  	light.textureNum=-3;
  }else{
  	light.textureNum=-2;	
  }
  
  light.color=[2,2,0,1];
  light.matrix.translate( g_lightPos[0],g_lightPos[1],g_lightPos[2]);
  light.matrix.scale(-.4,-.4,-.4);
  light.matrix.translate(-.5, -.5, -.5);
  light.render();
 



  //floor
  var floor= new Cube();
  if (g_normalOn) {
  	floor.textureNum=-3;
  }else{
  	floor.textureNum=1;	
  }
  
  floor.color=[1.0,0.0,0.0,1.0];
  floor.matrix.translate(0,-.75, 0.0);
  floor.matrix.scale(12,0,12);
  floor.matrix.translate(-.5, 0, -.5);
  floor.render();
 

  var sky= new Cube();
  sky.color=[ .678, .847, .902,1.0];
  if (g_normalOn) {
  	sky.textureNum=-3;
  }else{
  	sky.textureNum=-2;
  }
  sky.matrix.scale(-12,-12,-12);
  sky.matrix.translate(-.5,-.5,-.5);
  sky.render();
  //draw all the cubes in shapeList
  for (var i =0; i <  g_shapeList.length; i++) {
    g_shapeList[i].render();
  }

  //draw dog from last asg
  //Start Animal

  var head = new Cube();
  if (g_normalOn) {
  	head.textureNum=-3;
  }else{
  	head.textureNum=-2;	
  }
  head.color=[.54,.54,.54,1.0];
  head.matrix.translate(-.33,-.25,-0.35);
  // head.matrix.rotate(-5,1,0,0);
  head.matrix.scale(0.66, .6, .5);
  head.render();

  var face = new Cube();
  if (g_normalOn) {
  	face.textureNum=-3;
  }else{
  	face.textureNum=-2;	
  }
  face.color=[.85,.85,.85,1.0];
  face.matrix.translate(-.25,-.25,-.55);
  // face.matrix.rotate(-5,1,0,0);
  face.matrix.scale(.5, .5, .3);
  face.render();

  var body = new Cube();
  if (g_normalOn) {
  	body.textureNum=-3;
  }else{
  	body.textureNum=-2;	
  }
  body.color=[.85,.85,.85,1.0];
  body.matrix.translate(-.25,-.25,0);
  // body.matrix.rotate(-5,1,0,0);
  body.matrix.scale(.5, .5, .75);
  body.render();

  var mouth = new Cube();
  if (g_normalOn) {
  	mouth.textureNum=-3;
  }else{
  	mouth.textureNum=-2;	
  }
  mouth.color=[1,.94,.90,1.0];
  mouth.matrix.translate(-.13,-.23,-.7);
  // mouth.matrix.rotate(-5,1,0,0);
  mouth.matrix.scale(.25, .25, .3);
  mouth.render();

  var nose = new Cube();
  if (g_normalOn) {
  	nose.textureNum=-3;
  }else{
  	nose.textureNum=-2;	
  }
  nose.color=[0.1,0.1,0.1,1.0];
  nose.matrix.translate(-.04,-.06,-.71);
  // nose.matrix.rotate(-5,1,0,0);
  nose.matrix.scale(.08, .08, .01);
  nose.render();

  var lips = new Cube();
  if (g_normalOn) {
  	lips.textureNum=-3;
  }else{
  	lips.textureNum=-2;	
  }
  lips.color=[0.1,0.1,0.1,1.0];
  lips.matrix.translate(-.13,-.23,-.71);
  // lips.matrix.rotate(-5,1,0,0);
  lips.matrix.scale(.25, .08, .01);
  lips.render();

  var eye1 = new Cube();
  if (g_normalOn) {
  	eye1.textureNum=-3;
  }else{
  	eye1.textureNum=-2;	
  }
  eye1.color=[0.1,0.1,0.1,1.0];
  eye1.matrix.translate(.06,.02,-.6);
  // eye1.matrix.rotate(-5,1,0,0);
  eye1.matrix.scale(.1, .1, .1);
  eye1.render();

  var eye2 = new Cube();
  if (g_normalOn) {
  	eye2.textureNum=-3;
  }else{
  	eye2.textureNum=-2;	
  }
  eye2.color=[0.1,0.1,0.1,1.0];
  eye2.matrix.translate(-.16,.02,-.6);
  // eye2.matrix.rotate(-5,1,0,0);
  eye2.matrix.scale(.1, .1, .1);
  eye2.render();

  var ear1 = new Cube();
  if (g_normalOn) {
  	ear1.textureNum=-3;
  }else{
  	ear1.textureNum=-2;	
  }
  ear1.color=[1,1,1,1.0];
  ear1.matrix.translate(.04,.25,-.45);
  // ear1.matrix.rotate(-5,1,0,0);
  ear1.matrix.scale(.18, .2, .1);
  ear1.render();

  var ear2 = new Cube();
  if (g_normalOn) {
  	ear2.textureNum=-3;
  }else{
  	ear2.textureNum=-2;	
  }
  ear2.color=[1,1,1,1.0];
  ear2.matrix.translate(-.22,.25,-.45);
  // ear2.matrix.rotate(-5,1,0,0);
  ear2.matrix.scale(.18, .2, .1);
  ear2.render();

  var leg1 = new Cube();
  if (g_normalOn) {
  	leg1.textureNum=-3;
  }else{
  	leg1.textureNum=-2;	
  }
  leg1.color=[.3,.3,.3,1.0];
  leg1.matrix.translate(.23,-.2,-.15);
  leg1.matrix.rotate(-g_legAngle,1,0,0);
  leg1.matrix.scale(-.18, -.45, -.2);
  leg1.render();

  var leg2 = new Cube();
  if (g_normalOn) {
  	leg2.textureNum=-3;
  }else{
  	leg2.textureNum=-2;	
  }
  leg2.color=[.3,.3,.3,1.0];
  leg2.matrix.translate(-.05,-.2,-.15);
  leg2.matrix.rotate(g_legAngle,1,0,0);
  leg2.matrix.scale(-.18, -.45, -.2);
  leg2.render();

  var leg3 = new Cube();
  if (g_normalOn) {
  	leg3.textureNum=-3;
  }else{
  	leg3.textureNum=-2;	
  }
  leg3.color=[.3,.3,.3,1.0];
  leg3.matrix.translate(.23,-.2,.7);
  leg3.matrix.rotate(-g_legAngle,1,0,0);
  leg3.matrix.scale(-.18, -.45, -.2);
  leg3.render();

  var leg4 = new Cube();
  if (g_normalOn) {
  	leg4.textureNum=-3;
  }else{
  	leg4.textureNum=-2;	
  }
  leg4.color=[.3,.3,.3,1.0];
  leg4.matrix.translate(-.05,-.2,.7);
  leg4.matrix.rotate(g_legAngle,1,0,0);
  leg4.matrix.scale(-.18, -.45, -.2);
  leg4.render();


  var tailConnect = new Cube();
  if (g_normalOn) {
  	tailConnect.textureNum=-3;
  }else{
  	tailConnect.textureNum=-2;	
  }
  tailConnect.color=[.79,.79,.79,1.0];
  tailConnect.matrix.translate(-.09,-.05,.7);
  tailConnect.matrix.rotate(-g_tailConnectorAngle,0,1,0);
  var conCoord=new Matrix4(tailConnect.matrix);
  tailConnect.matrix.scale(.2, .2, .1);
  tailConnect.render();

  var tail = new Cube();
  if (g_normalOn) {
  	tail.textureNum=-3;
  }else{
  	tail.textureNum=-2;	
  }
  tail.color=[.88,.88,.88,1.0];
  tail.matrix=conCoord;
  tail.matrix.translate(.2,.2,.1);
  tail.matrix.rotate(-g_tailAngle,1,0,0);
  tail.matrix.scale(-.18, -.45, -.15);
  tail.render();
  
  var sphere1 = new Sphere();
  if (g_normalOn) {
  	sphere1.textureNum=-3;
  }else{
  	sphere1.textureNum=-2;	
  }
  
  sphere1.color=[1.0,0.0,0.0,1.0];
  sphere1.matrix.translate(1,-.5, -1);
  sphere1.matrix.scale(.25,.25,.25);
  sphere1.render();

  var box= new Cube();
  box.color=[ 1, .5, 0,1.0];
  if (g_normalOn) {
  	box.textureNum=-3;
  }else{
  	box.textureNum=-2;
  }
  
  box.matrix.translate(1,-.5,-1);
  box.matrix.scale(.50,.50,.50);
  box.render();
}

var g_map=[
   [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
   [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
]
function generateBlocks(){
  console.log("generateBlocks called once");
  //CREATES AND ADDS WALLS BASED ON THE 2D ARRAY ABOVE
  for (var x = 0; x <=10 ; x++) {
    for (var y = 0 ; y <=10; y++) {
      if (g_map[x][y]>=1) {
        
        var body= new Cube();
        body.textureNum=0;
        body.matrix.translate(x-5,-.6,y-5);
        g_shapeList.push(body);
        var body= new Cube();
        body.textureNum=0;
        body.matrix.translate(x-5,.4,y-5);
        g_shapeList.push(body);


      }
    }
  }
  for (var x = 0; x <=10 ; x++) {
    for (var y = 0 ; y <=10; y++) {
      if (x != 6 && y!=6 && x != 4 && y!=4 && x != 5 && y!=5) {
        var roll = Math.floor(Math.random()*10); 
        console.log(roll);
        if (roll<5) {
        var body= new Cube();
        body.textureNum=0;
        body.matrix.translate(x-5,-.4,y-5);
        body.matrix.scale(.5,.5,.5);
        g_shapeList.push(body);
      	}
      }
    }
  }
}