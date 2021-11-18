#include"Camera.h"

BEGIN_P3D_NS

Camera::Camera() {
	setViewParams({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f });
	setProjParams(60.0f, 1.0f, 0.1, 100.0f);
}
void Camera::setViewParams(const Vec3f& e,const Vec3f& d,const Vec3f& u){
    eyePos = e;

	dstPos = d;
    viewMat = Mat4f::getLookAt(e,d,u);

	forwardVec = (d - e).normalize();// {viewMat[2].x, viewMat[2].y, viewMat[2].z};
    upVec = {viewMat[1].x,viewMat[1].y,viewMat[1].z};
    rightVec = {viewMat[0].x,viewMat[0].y,viewMat[0].z};

}
void Camera::setProjParams(float fov,float asp,float nearP,float farP){
    this->fov = fov;
    this->asp = asp;
    nearPlane = nearP;
    farPlane = farP;

    projMat = Mat4f::getPerspective(fov,asp,nearP,farP);
}
void Camera::setOrthParams(float32 w, float h, float32 nearZ, float32 farZ) {
	orthW = w;
	orthH = h;
	orthNearZ = nearZ;
	orthFarZ = farZ;
	asp = w / h;
	orthMat = Mat4f::getOrthognal(-orthW * 0.5, orthW*0.5, -0.5*orthH, orthH*0.5, nearZ, farZ);

}

void Camera::setOrthWidth(float32 w) {
	orthW = w;
	orthH = w / asp;
	orthMat = Mat4f::getOrthognal(-orthW * 0.5, orthW*0.5, -0.5*orthH, orthH*0.5, orthNearZ, orthFarZ);
}

void Camera::move(float right,float forward){
    eyePos += (rightVec * right + forwardVec * forward);
    viewMat = Mat4f::getLookAt(eyePos,eyePos + forwardVec,upVec);
}

void Camera::rotate(float pitch,float yaw){

    Vec3f tmpV = Quaternionf::rotateVec(forwardVec,upVec,yaw).normalize();

    Vec3f trV = upVec.cross(tmpV).normalize();

    forwardVec = Quaternionf::rotateVec(tmpV,trV,pitch).normalize();
    
    setViewParams(eyePos,eyePos+forwardVec,upVec);
}

void Camera::print() {
    if (type == CAMERA_TYPE_PRO) {
        std::cout << "camera type: Projective,";
    }
    else
        std::cout << "camera type: Orth,";
    std::cout<< "camera Pos " << eyePos.x << " " << eyePos.y << " " << eyePos.z << ",look vec: ";
    forwardVec.print();
}
END_P3D_NS