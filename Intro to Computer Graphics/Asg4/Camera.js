class Camera{
	constructor(){
		this.viewMatrix=new Matrix4();
		this.projectionMatrix= new Matrix4();
		this.eye=new Vector3([0,0,-3]);
		this.at=new Vector3([0,0,100]);
		this.up=new Vector3([0,1,0]);
		this.fov=120;

		this.viewMatrix.setLookAt(this.eye.elements[0],this.eye.elements[1],this.eye.elements[2]
			,this.at.elements[0],this.at.elements[1],this.at.elements[2]
			,this.up.elements[0],this.up.elements[1],this.up.elements[2]);

		
		//projection matrix
		this.projectionMatrix.setPerspective(this.fov, canvas.width/canvas.height, 0.1,1000);
		

	}
	moveForward(){
		var f = new Vector3();
		f.set(this.at);
		f.sub(this.eye);
		f.normalize();
		f.mul(.25);
		this.at=this.at.add(f);
		this.eye=this.eye.add(f);
		this.viewMatrix.setLookAt(this.eye.elements[0],this.eye.elements[1],this.eye.elements[2]
			,this.at.elements[0],this.at.elements[1],this.at.elements[2]
			,this.up.elements[0],this.up.elements[1],this.up.elements[2]);
	}

	moveBackwards(){
		var f = new Vector3();
		f.set(this.at);
		f.sub(this.eye);
		f.normalize();
		f.mul(.25);
		this.at=this.at.sub(f);
		this.eye=this.eye.sub(f);
		this.viewMatrix.setLookAt(this.eye.elements[0],this.eye.elements[1],this.eye.elements[2]
			,this.at.elements[0],this.at.elements[1],this.at.elements[2]
			,this.up.elements[0],this.up.elements[1],this.up.elements[2]);
	}
	moveLeft(){
		var f = new Vector3();
		f.set(this.at);
		f.sub(this.eye);
		var s = Vector3.cross(this.up,f);
		s.normalize();
		s.mul(.25);
		this.at=this.at.add(s);
		this.eye=this.eye.add(s);
		this.viewMatrix.setLookAt(this.eye.elements[0],this.eye.elements[1],this.eye.elements[2]
			,this.at.elements[0],this.at.elements[1],this.at.elements[2]
			,this.up.elements[0],this.up.elements[1],this.up.elements[2]);
	}
	moveRight(){
		var f = new Vector3();
		f.set(this.at);
		f.sub(this.eye);
		var s = Vector3.cross(f, this.up);
		s.normalize();
		s.mul(.25);
		this.at=this.at.add(s);
		this.eye=this.eye.add(s);
		this.viewMatrix.setLookAt(this.eye.elements[0],this.eye.elements[1],this.eye.elements[2]
			,this.at.elements[0],this.at.elements[1],this.at.elements[2]
			,this.up.elements[0],this.up.elements[1],this.up.elements[2]);
	}
	//https://piazza.com/class/k8dgpa9va9z6in?cid=227 
	panLeft(){
		var f = new Vector3();
		f.set(this.at);
		f.sub(this.eye);
		var tempRotationMatrix= new Matrix4();
		tempRotationMatrix.setRotate(5, this.up.elements[0], this.up.elements[1], this.up.elements[2]);
		var f_prime=new Vector3();
		f_prime=tempRotationMatrix.multiplyVector3(f);
		var tempEye=new Vector3();
		tempEye=tempEye.set(this.eye);
		this.at=tempEye.add(f_prime);
		this.viewMatrix.setLookAt(this.eye.elements[0],this.eye.elements[1],this.eye.elements[2]
			,this.at.elements[0],this.at.elements[1],this.at.elements[2]
			,this.up.elements[0],this.up.elements[1],this.up.elements[2]);
	}
	panRight(){
		var f = new Vector3();
		f.set(this.at);
		f.sub(this.eye);
		var tempRotationMatrix= new Matrix4();
		tempRotationMatrix.setRotate(-5, this.up.elements[0], this.up.elements[1], this.up.elements[2]);
		var f_prime=new Vector3();
		f_prime=tempRotationMatrix.multiplyVector3(f);
		var tempEye=new Vector3();
		tempEye=tempEye.set(this.eye);
		this.at=tempEye.add(f_prime);
		this.viewMatrix.setLookAt(this.eye.elements[0],this.eye.elements[1],this.eye.elements[2]
			,this.at.elements[0],this.at.elements[1],this.at.elements[2]
			,this.up.elements[0],this.up.elements[1],this.up.elements[2]);
	}
}