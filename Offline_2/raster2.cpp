#include<iostream>
#include<stack>
#include<fstream>
#include <iomanip>
#include <string>

using namespace std;

struct Point{
    float x, y, z;
};

Point transformPoint(Point p, float matrix[4][4]){
    Point newPoint;
    newPoint.x= p.x*matrix[0][0]+p.y*matrix[0][1]+p.z*matrix[0][2]+matrix[0][3];
    newPoint.y= p.x*matrix[1][0]+p.y*matrix[1][1]+p.z*matrix[1][2]+matrix[1][3];
    newPoint.z= p.x*matrix[2][0]+p.y*matrix[2][1]+p.z*matrix[2][2]+matrix[2][3];
    return newPoint;
}

void printMatrix(float mat[4][4]){
    cout<<"Matrix:"<<endl;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            cout<<mat[i][j]<<" ";
        }
        cout<<endl;
    }
}

// Function to multiply two matrices
void matrixMultiplication(float A[][4], float B[][4], float result[][4]) {
    float temp[4][4];
    //copy B to temp
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            temp[i][j]= B[i][j];
        }
    }
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i][j] += A[i][k] * temp[k][j];
            }
        }
    }
    cout<<"Result:"<<endl;
    printMatrix(result);
}



int main(){
    ifstream sceneFile("scene.txt");
    ofstream outputFile("stage1.txt");
    Point eye, look, up;
    float fovY, aspectRatio, near, far;
    sceneFile>>eye.x>>eye.y>>eye.z;
    sceneFile>>look.x>>look.y>>look.z;
    sceneFile>>up.x>>up.y>>up.z;
    sceneFile>>fovY>>aspectRatio>>near>>far;
    //set precision to 6 decimal places
    outputFile<<fixed<<setprecision(6);
    
    //calculate the modelview matrix
    //initialize empty stack that will store matrix
    stack <float[4][4]> matrixStack;
   // initialize identity matrix
    float currentMatrix[4][4] ={{1,0,0,0},
                            {0,1,0,0},
                            {0,0,1,0},
                            {0,0,0,1}};
    //push identity matrix to stack
    //matrixStack.push(currentMatrix);
    string command;
    while(1){
        sceneFile>>command;
        if(command=="triangle")
        {
            Point p1, p2, p3;
            sceneFile>>p1.x>>p1.y>>p1.z;
            sceneFile>>p2.x>>p2.y>>p2.z;
            sceneFile>>p3.x>>p3.y>>p3.z;
            //transform points
            p1= transformPoint(p1, currentMatrix);
            p2= transformPoint(p2, currentMatrix);
            p3= transformPoint(p3, currentMatrix);
            //write to file
            outputFile<<p1.x<<" "<<p1.y<<" "<<p1.z<<endl;
            outputFile<<p2.x<<" "<<p2.y<<" "<<p2.z<<endl;
            outputFile<<p3.x<<" "<<p3.y<<" "<<p3.z<<endl;
            outputFile<<endl;
        }
        else if(command=="translate"){
            //read tx, ty, tz
            float tx, ty, tz;
            sceneFile>>tx>>ty>>tz;
            //initialize translation matrix
            float translationMatrix[4][4]={{1,0,0,tx},
                                           {0,1,0,ty},
                                           {0,0,1,tz},
                                           {0,0,0,1}};
            //multiply translation matrix with top of stack
            matrixMultiplication(translationMatrix, currentMatrix, currentMatrix);
             
        }
        else if(command=="scale"){
            //read sx, sy, sz
            float sx, sy, sz;
            sceneFile>>sx>>sy>>sz;
            //initialize scaling matrix
            float scalingMatrix[4][4]={{sx,0,0,0},
                                           {0,sy,0,0},
                                           {0,0,sz,0},
                                           {0,0,0,1}};
            //multiply scaling matrix with top of stack
            printMatrix(scalingMatrix);
            printMatrix(currentMatrix);
            matrixMultiplication(scalingMatrix, currentMatrix, currentMatrix);
        }
        else if(command=="rotate"){
            //read angle, x, y, z
            float angle, ax, ay, az;
            sceneFile>>angle>>ax>>ay>>az;
            //initialize rotation matrix
            float rotationMatrix[4][4];

        }
        else if(command=="push"){
            matrixStack.push(currentMatrix);
        }
        else if(command=="pop"){
            // currentMatrix= matrixStack.top();
            // matrixStack.pop();
        }
        else if(command=="end"){
            break;
        }
    }
    sceneFile.close();
    outputFile.close();


    
}