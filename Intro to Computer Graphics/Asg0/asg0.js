// DrawRectangle.js
function main() {
// Retrieve <canvas> element <- (1)
var canvas = document.getElementById('example');
if (!canvas) {
console.log('Failed to retrieve the <canvas> element');
return;
}
// Get the rendering context for 2DCG <- (2)
var ctx = canvas.getContext('2d');
//function to draw a vector
//draw black background 
ctx.fillStyle = 'rgba(0, 0, 0, 1.5)'; 
ctx.fillRect(0,0,400,400);


const button1= document.getElementById("button1");
//handleDrawEvent() didnt know how to put in a separate function
button1.onclick=function(){
    ctx.fillRect(0,0,400,400);
    let temp1=[document.getElementById("x1").value,document.getElementById("y1").value,0];
    // console.log(temp1);
    let v1= new Vector3(temp1);
    // console.log(v1);
    drawVector(ctx,v1,"red");
    let temp2=[document.getElementById("x2").value,document.getElementById("y2").value,0];
    // console.log(temp2);
    let v2= new Vector3(temp2);
    // console.log(v2);
    drawVector(ctx,v2,"blue");
};
// console.log("after button was made");
//
const button2= document.getElementById("button2");
button2.onclick=function(){
    //draw & get info about the original 2 vectors
    ctx.fillRect(0,0,400,400);
    // console.log("clicked");
    let temp1=[document.getElementById("x1").value,document.getElementById("y1").value,0];
    // console.log(temp1);
    let v1= new Vector3(temp1);
    // console.log(v1);
    drawVector(ctx,v1,"red");
    let temp2=[document.getElementById("x2").value,document.getElementById("y2").value,0];
    // console.log(temp2);
    let v2= new Vector3(temp2);
    // console.log(v2);
    drawVector(ctx,v2,"blue");
    var scalarTemp;

    let chosenOp= document.getElementById("op-select").value;
    switch(chosenOp){
        case "Add":
            // console.log("add");
            v1.add(v2);
            drawVector(ctx,v1,"green");
            break;
        case "Subtract":
            // console.log("sub");
            v1.sub(v2);
            drawVector(ctx,v1,"green");
            break;
        case "Multiply":
            // console.log("mult");
            scalarTemp=document.getElementById("scalar").value;
            v1.mul(scalarTemp);
            drawVector(ctx,v1,"green");
            v2.mul(scalarTemp);
            drawVector(ctx,v2,"green");
            break;
        case "Divide":
            // console.log("div");
            scalarTemp=document.getElementById("scalar").value;
            v1.div(scalarTemp);
            drawVector(ctx,v1,"green");
            v2.div(scalarTemp);
            drawVector(ctx,v2,"green");
            break;
        case "Magnitude":
            console.log("Magnitude v1: ",v1.magnitude());
            console.log("Magnitude v2: ",v2.magnitude());
            break;
        case "Normalize":
            v1.normalize();
            v2.normalize();
            drawVector(ctx,v1,"green");
            drawVector(ctx,v2,"green");
            break;
        case "Angle between":
            angleBetween(v1,v2);
            break;
        case "Area":
            areaTriangle(v1,v2);
            break;
        default:
            console.log("OP NOT FOUND");
    }
}



}

function drawVector(ctx,name,color){
    //create vector
    ctx.beginPath();
    ctx.strokeStyle=color;
    ctx.moveTo(200,200);
    ctx.lineTo(200+name.elements[0]*20,200-name.elements[1]*20);
    ctx.stroke();
}

function angleBetween(v1, v2){
    let dotAns=Vector3.dot(v1,v2);
    let v1Mag= v1.magnitude();
    let v2Mag= v2.magnitude();
    //console.log(dotAns);
    dotAns=dotAns/(v1Mag*v2Mag);
    //console.log(dotAns);
    console.log("Angle: ", Math.acos(dotAns)*(180/Math.PI));
}

function areaTriangle(v1, v2){
    let crossAns=Vector3.cross(v1,v2);
    let area=crossAns.magnitude();
    area=area/2;
    console.log("Area of the triangle: ", area);
}