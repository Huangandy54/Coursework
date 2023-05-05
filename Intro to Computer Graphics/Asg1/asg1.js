// built off examples in book and in assignment videos
// Vertex shader program
var VSHADER_SOURCE = `
  attribute vec4 a_Position;
  uniform float u_Size;
    void main() {
      gl_Position = a_Position;
      gl_PointSize = u_Size;
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

function setupWebGL(){
  // Retrieve <canvas> element
  canvas = document.getElementById('webgl');

  // Get the rendering context for WebGL
  gl = canvas.getContext("webgl",{preserveDrawingBuffer: true});
  if (!gl) {
    console.log('Failed to get the rendering context for WebGL');
    return;
  }

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

  // Get the storage location of u_FragColor
  u_FragColor = gl.getUniformLocation(gl.program, 'u_FragColor');
  if (!u_FragColor) {
    console.log('Failed to get the storage location of u_FragColor');
    return;
  }
  u_Size = gl.getUniformLocation(gl.program,'u_Size');
  if (!u_Size) {
    console.log('Failed to get the storage location of u_Size');
    return;
  }
}

const POINT=0;
const TRIANGLE=1;
const CIRCLE=2;

let selectedColor=[1.0,0.0,0.0,1.0];
let selectedSize=5;
let selectedSegCount=10;
let selectedType=POINT;
function addActionsForHtmlUI(){
  document.getElementById("ClearCanvas").onclick=function(){g_shapeList=[]; renderAllShapes();};

  document.getElementById("SquareSel").onclick=function(){selectedType=POINT;};
  document.getElementById("TriangleSel").onclick=function(){selectedType=TRIANGLE;};
  document.getElementById("CircleSel").onclick=function(){selectedType=CIRCLE;};


  document.getElementById("Red").addEventListener('mouseup', function(){selectedColor[0]=this.value/255;});
  document.getElementById("Green").addEventListener('mouseup', function(){selectedColor[1]=this.value/255;});
  document.getElementById("Blue").addEventListener('mouseup', function(){selectedColor[2]=this.value/255;});
  document.getElementById("Alpha").addEventListener('mouseup', function(){selectedColor[3]=this.value/100;});
 
  document.getElementById("Size").addEventListener('mouseup', function(){selectedSize=this.value;});
  document.getElementById("SegCount").addEventListener('mouseup', function(){selectedSegCount=this.value;});

}

function main() {
  setupWebGL();
  connectVariablesToGLSL();
  
  
  // Register function (event handler) to be called on a mouse press
  addActionsForHtmlUI();
  canvas.onmousemove = function(ev){if(ev.buttons==1){click(ev)}};

  // Specify the color for clearing <canvas>
  gl.clearColor(0.0, 0.0, 0.0, 1.0);

  // Clear <canvas>
  gl.clear(gl.COLOR_BUFFER_BIT);
}
var g_shapeList=[];


function click(ev) {
  let [x,y]=convertCoordinateEventToGL(ev);
  //get color, size info from HTML 
  
  let point;
  switch(selectedType){
    case POINT:
      point=new Point();
      break;0.
    case TRIANGLE:
      point=new Triangle();
      break;
    case CIRCLE:
      point=new Circle();
      point.segments=selectedSegCount;
      break;
    default:
      point=new Point();

  }
  
  point.position=[x,y];
  point.color=selectedColor.slice();
  point.size=selectedSize;
  g_shapeList.push(point);

  //draw all shapes
  renderAllShapes();
}


function convertCoordinateEventToGL(ev){
  var x = ev.clientX; // x coordinate of a mouse pointer
  var y = ev.clientY; // y coordinate of a mouse pointer
  var rect = ev.target.getBoundingClientRect();

  x = ((x - rect.left) - canvas.width/2)/(canvas.width/2);
  y = (canvas.height/2 - (y - rect.top))/(canvas.height/2);

  return ([x,y]);
}

function renderAllShapes(){
  // Clear <canvas>
  gl.clear(gl.COLOR_BUFFER_BIT);

  var len = g_shapeList.length;
  for(var i = 0; i < len; i++) {
    g_shapeList[i].render();
  }
}