#include<iostream>
#include<stack>
#include<fstream>
#include <iomanip>
#include <string>
#include <math.h>

using namespace std;

struct Vector{
    float x, y, z;
};

float dotProduct(Vector vec1, Vector vec2) {
    return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

Vector crossProduct(Vector vec1, Vector vec2) {
    Vector result;
    result.x = vec1.y * vec2.z - vec1.z * vec2.y;
    result.y = vec1.z * vec2.x - vec1.x * vec2.z;
    result.z = vec1.x * vec2.y - vec1.y * vec2.x;
    return result;
}

//Rodrigues' rotation formula
Vector calculateR(Vector x, Vector a, float theta) {
    theta = theta * M_PI / 180.0;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    float dotProductAx = dotProduct(a, x);
    Vector crossProductAx = crossProduct(a, x);

    Vector result;
    result.x = cosTheta * x.x + (1.0 - cosTheta) * dotProductAx * a.x + sinTheta * crossProductAx.x;
    result.y = cosTheta * x.y + (1.0 - cosTheta) * dotProductAx * a.y + sinTheta * crossProductAx.y;
    result.z = cosTheta * x.z + (1.0 - cosTheta) * dotProductAx * a.z + sinTheta * crossProductAx.z;
    return result;
}

class Matrix {
private:
    float data[4][4];
    int row, col;

public:
    Matrix(int row, int col) {
        this->row = row;
        this->col = col;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; ++j) {
                data[i][j] = 0;
            }
        }
    }

    Matrix(){
        row=4;
        col=4;
        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                data[i][j]=0;
            }
        }
    }


    void set(int i, int j, float value) {
        if (i >= 0 && i < row && j >= 0 && j < col) {
            data[i][j] = value;
        }
    }

    void setIdentityMatrix(){
        for(int i=0; i< row; i++){
            for(int j=0; j<col; j++){
                if(i==j)
                    data[i][j]=1;
                else
                    data[i][j]=0;
            }
        }
    }

    void setTranslationMatrix(float tx, float ty, float tz){
        setIdentityMatrix();
        data[0][3]= tx;
        data[1][3]= ty;
        data[2][3]= tz;
    }

    void setScalingMatrix(float sx, float sy, float sz){
        setIdentityMatrix();
        data[0][0]= sx;
        data[1][1]= sy;
        data[2][2]= sz;
    }

    void setRotationMatrix(Vector c1, Vector c2, Vector c3){
        setIdentityMatrix();
        data[0][0]= c1.x;
        data[0][1]= c2.x;
        data[0][2]= c3.x;
        data[1][0]= c1.y;
        data[1][1]= c2.y;
        data[1][2]= c3.y;
        data[2][0]= c1.z;
        data[2][1]= c2.z;
        data[2][2]= c3.z;

    }

    float get(int i, int j) {
        if (i >= 0 && i < row && j >= 0 && j < col) {
            return data[i][j];
        }
        return 0.0; // Return 0 if indices are out of bounds
    }

    void display(){
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                cout << data[i][j] << " ";
            }
            cout << endl;
        }
    }

    Matrix transformPoint(Matrix x){
        //multiply 4*4 matrix with 4*1 matrix
        Matrix result(4,1);
        for(int i=0; i<4; i++){
            for(int j=0; j<1; j++){
                result.data[i][j]=0;
                for(int k=0; k<4; k++){
                    result.data[i][j]+= data[i][k]*x.data[k][j];
                }
            }
        }
        if(result.data[3][0] !=1){
            result.data[0][0]= result.data[0][0]/result.data[3][0];
            result.data[1][0]= result.data[1][0]/result.data[3][0];
            result.data[2][0]= result.data[2][0]/result.data[3][0];
            result.data[3][0]= result.data[3][0]/result.data[3][0];
        }
        return result;
       
    }

    Matrix multiplication(Matrix x){
        //multiply 4*4 matrix with 4*4 matrix
        Matrix result(4,4);
        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                result.data[i][j]=0;
                for(int k=0; k<4; k++){
                    result.data[i][j]+= data[i][k]* x.data[k][j];
                }
                if(result.data[i][j]<0.0000001 && result.data[i][j]>-0.0000001)
                    result.data[i][j]=0;
            }
        }
        return result;
    }
};

Matrix viewTransformation(Vector eye, Vector look, Vector up){
    Vector l, r, u;
    //calculate l
    l.x= look.x-eye.x;
    l.y= look.y-eye.y;
    l.z= look.z-eye.z;
    //normalize l
    float magnitude= sqrt(l.x*l.x + l.y*l.y + l.z*l.z);
    l.x= l.x/magnitude;
    l.y= l.y/magnitude;
    l.z= l.z/magnitude;
    //calculate r
    r= crossProduct(l, up);
    //normalize r
    magnitude= sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
    r.x= r.x/magnitude;
    r.y= r.y/magnitude;
    r.z= r.z/magnitude;
    //calculate u
    u= crossProduct(r, l);
    //calculate view matrix
    Matrix viewMatrix(4,4);
    Matrix translationMatrix(4,4);
    translationMatrix.setTranslationMatrix(-eye.x, -eye.y, -eye.z);
    Matrix rotationMatrix(4,4);
    rotationMatrix.setIdentityMatrix();
    rotationMatrix.set(0,0, r.x);
    rotationMatrix.set(0,1, r.y);
    rotationMatrix.set(0,2, r.z);
    rotationMatrix.set(1,0, u.x);
    rotationMatrix.set(1,1, u.y);
    rotationMatrix.set(1,2, u.z);
    rotationMatrix.set(2,0, -l.x);
    rotationMatrix.set(2,1, -l.y);
    rotationMatrix.set(2,2, -l.z);
    viewMatrix= rotationMatrix.multiplication(translationMatrix);
    return viewMatrix;

}

Matrix projectionMatrix(float fovY, float aspectRatio, float near, float far){
    Matrix projectionMatrix(4,4);
    float fovX= fovY*aspectRatio;
    float t= near*tan(fovY/2 * M_PI / 180.0);
    float r= near*tan(fovX/2 * M_PI / 180.0);
    projectionMatrix.set(0,0, near/r);
    projectionMatrix.set(1,1, near/t);
    projectionMatrix.set(2,2, -(far+near)/(far-near));
    projectionMatrix.set(2,3, -(2*far*near)/(far-near));
    projectionMatrix.set(3,2, -1);
    projectionMatrix.set(3,3, 0);
    return projectionMatrix;
}


int main(){
    ifstream sceneFile("scene.txt");
    ofstream outputFile("stage1.txt");
    ofstream outputFile2("stage2.txt");
    ofstream outputFile3("stage3.txt");
    Vector eye, look, up;
    float fovY, aspectRatio, near, far;
    sceneFile>>eye.x>>eye.y>>eye.z;
    sceneFile>>look.x>>look.y>>look.z;
    sceneFile>>up.x>>up.y>>up.z;
    sceneFile>>fovY>>aspectRatio>>near>>far;

    outputFile<<fixed<<setprecision(7);
    outputFile2<<fixed<<setprecision(7);
    outputFile3<<fixed<<setprecision(7);
    
    stack <Matrix> matrixStack;
    Matrix currentMatrix(4,4);
    currentMatrix.setIdentityMatrix();

    //calculate view matrix
    Matrix viewMatrix(4,4);
    viewMatrix= viewTransformation(eye, look, up);

    //calculate projection matrix
    Matrix projection(4,4);
    projection= projectionMatrix(fovY, aspectRatio, near, far);

    string command;
    while(1){
        sceneFile>>command;
        if(command=="triangle")
        {
            Matrix p1(4,1), p2(4,1), p3(4,1);
            float x, y, z;
            float w=1;
            sceneFile>>x>>y>>z;
            p1.set(0,0,x);
            p1.set(1,0,y);
            p1.set(2,0,z);
            p1.set(3,0,w);
            sceneFile>>x>>y>>z;
            p2.set(0,0,x);
            p2.set(1,0,y);
            p2.set(2,0,z);
            p2.set(3,0,w);
            sceneFile>>x>>y>>z;
            p3.set(0,0,x);
            p3.set(1,0,y);
            p3.set(2,0,z);
            p3.set(3,0,w);

            //stage 1
            //transform points
            p1= currentMatrix.transformPoint(p1);
            p2= currentMatrix.transformPoint(p2);
            p3= currentMatrix.transformPoint(p3);
            outputFile<<p1.get(0,0)<<" "<<p1.get(1,0)<<" "<<p1.get(2,0)<<endl;
            outputFile<<p2.get(0,0)<<" "<<p2.get(1,0)<<" "<<p2.get(2,0)<<endl;
            outputFile<<p3.get(0,0)<<" "<<p3.get(1,0)<<" "<<p3.get(2,0)<<endl;
            outputFile<<endl;

            //stage 2
            //transform points
            p1= viewMatrix.transformPoint(p1);
            p2= viewMatrix.transformPoint(p2);
            p3= viewMatrix.transformPoint(p3);
            outputFile2<<p1.get(0,0)<<" "<<p1.get(1,0)<<" "<<p1.get(2,0)<<endl;
            outputFile2<<p2.get(0,0)<<" "<<p2.get(1,0)<<" "<<p2.get(2,0)<<endl;
            outputFile2<<p3.get(0,0)<<" "<<p3.get(1,0)<<" "<<p3.get(2,0)<<endl;
            outputFile2<<endl;

            //stage 3
            //transform points
            p1= projection.transformPoint(p1);
            p2= projection.transformPoint(p2);
            p3= projection.transformPoint(p3);
            outputFile3<<p1.get(0,0)<<" "<<p1.get(1,0)<<" "<<p1.get(2,0)<<endl;
            outputFile3<<p2.get(0,0)<<" "<<p2.get(1,0)<<" "<<p2.get(2,0)<<endl;
            outputFile3<<p3.get(0,0)<<" "<<p3.get(1,0)<<" "<<p3.get(2,0)<<endl;
            outputFile3<<endl;
        }
        else if(command=="translate"){
            float tx, ty, tz;
            sceneFile>>tx>>ty>>tz;
            Matrix translationMatrix(4,4);
            translationMatrix.setTranslationMatrix(tx, ty, tz);
            currentMatrix= currentMatrix.multiplication(translationMatrix);
             
        }
        else if(command=="scale"){
            float sx, sy, sz;
            sceneFile>>sx>>sy>>sz;
            //initialize scaling matrix
            Matrix scalingMatrix(4,4);
            scalingMatrix.setScalingMatrix(sx, sy, sz);
            currentMatrix= currentMatrix.multiplication(scalingMatrix);
        }
        else if(command=="rotate"){
            float angle;
            Vector a;
            sceneFile>>angle>>a.x>>a.y>>a.z;
            //normalize a
            float magnitude= sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
            a.x= a.x/magnitude;
            a.y= a.y/magnitude;
            a.z= a.z/magnitude;
            Vector c1, c2, c3;
            c1= calculateR({1,0,0}, a, angle);
            c2= calculateR({0,1,0}, a, angle);
            c3= calculateR({0,0,1}, a, angle);
            Matrix rotationMatrix(4,4);
            rotationMatrix.setRotationMatrix(c1, c2, c3);
            currentMatrix= currentMatrix.multiplication(rotationMatrix);

        }
        else if(command=="push"){
            matrixStack.push(currentMatrix);
        }
        else if(command=="pop"){
            currentMatrix= matrixStack.top();
            matrixStack.pop();
        }
        else if(command=="end"){
            break;
        }
    }
    sceneFile.close();
    outputFile.close();
    return 0;   
}