#include <GLUT/glut.h>
#include <iostream>
#include <math.h>

using namespace std;

void display();
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void passiveMouseMotion(int x, int y);
void idle();
void drawSphere(GLfloat xx, GLfloat yy, GLfloat zz, GLfloat radius, GLfloat M, GLfloat N);
void lighting();
void myGlutBall(float radius, int numStacks, int numSides);
int width = 750, height = 750;
int degree = 0;
static bool paused = false,trans = false;
int X=30;
float Y=1;
static float e_revolution, e_rotate, m_revolution, m_rotate;

int main(int argc, char** argv)
{
    e_rotate=0;
    e_revolution=0;
    m_rotate=0;
    m_revolution=0;
    glutInit(&argc, argv);      //初始化
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);              //定義屏幕上顯示窗口大小
    glutInitWindowPosition(0, 0);                   //窗口位置
    glutCreateWindow("HW1");                        //創建窗口（視窗標題）
    
    glutDisplayFunc(display);           //註冊一個繪圖函數，只要windows要重畫，就要call這個function
    glutReshapeFunc(reshape);           //改變窗口大小
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);               //用來偵測滑鼠移動時的座標 負責處理滑鼠按著按鍵移動時
    glutPassiveMotionFunc(passiveMouseMotion); //負責處理滑鼠在視窗內移動時
    glutIdleFunc(idle);                        //程式空閒時，會調用 idle這個函式
    
    glutMainLoop();                            //讓glut程序進入事件循環
    
    return 0;
}

void display()
{
    //ModelView Matrix
    glMatrixMode(GL_MODELVIEW);     //選擇想要改變的矩陣：模型市景矩陣 -> 才能正確畫3D圖
    glLoadIdentity();               //將矩陣改成單位矩陣 -> 初始化
    gluLookAt(0.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);  //改view matrix
    //眼睛座標、相機座標eyeX,Y,Z, 物體的中心、焦點centerX,Y,Z, 頭頂的朝向upX,Y,Z
    //Projection Matrix
    glMatrixMode(GL_PROJECTION);    //選擇想要改變的矩陣：投影矩陣
    glLoadIdentity();
    gluPerspective(45, width / (GLfloat)height, 0.1, 1000); //透視投影
    //在y-z平面的角度（眼睛和模型的連線＆Y軸的夾角）/ 投影平面寬高比 / 延負z軸的近遠剪裁面->最近最遠能看到的距離
    //Viewport Matrix
    glViewport(0, 0, width, height);    //把視景體擷取的圖像顯示到屏幕上
    
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_CULL_FACE);        //remove back face
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);        //normalized normal
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   //設置底色
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     //將緩存清除為預先設定值
    
    lighting();
    //sun
    glPushMatrix();
    glColor3f(0.31f,0.22f,0.14f);
    drawSphere(0,0,0,6*Y,240,60);
    glPopMatrix();
    
    glRotatef(e_revolution, 0.0f, 1.0f, 0.0f);    //(angle,x,y,z) 旋轉軸經過原點，方向為 x,y,z
    glTranslatef(20.0, 0.0, 0.0);            //移動
    glRotatef(-e_revolution,0,1,0);
    //line
    glPushMatrix();
    glRotatef(23.5,0,0,1);
    glLineWidth(2.5);
    glColor3f(0.2f,0.2f,0.35f);
    glBegin(GL_LINES);
    glNormal3f(0.0, -4*Y, 0.0);
    glVertex3f(0.0, -4*Y, 0.0);
    glNormal3f(0.0, 4*Y, 0.0);
    glVertex3f(0, 4*Y, 0);
    glEnd();
    glPopMatrix();
    //earth
    glPushMatrix();
    glRotatef(23.5,0,0,1);
    glRotatef(e_rotate,0,1,0);
    glColor3f(0.35f,0.35f,0.50f);
    if(trans==false)    drawSphere(0,0,0,2*Y,360,180);
    else                drawSphere(0,0,0,2*Y,4,2);
    glPopMatrix();
    //moon
    glRotatef(m_revolution, 0.0f, 1.0f, 0.0f);
    glTranslatef(3.0, 0.0, 0.0);
    glRotatef(-m_revolution, 0.0f, 1.0f, 0.0f);
    glRotatef(m_rotate,0,1,0);
    glPushMatrix();
    glColor3f(0.5f,0.5f,0.5f);
    drawSphere(0,0,0,Y,240,60);
    glPopMatrix();
    
     
    glutSwapBuffers();      //畫完圖之後，變更顯示的緩衝區
}

void reshape(int _width, int _height) {
    width = _width;
    height = _height;
}



void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'p': case 'P':
            paused = !paused;
            break;
        case 'o': case 'O':
            trans = !trans;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
}

void mouseMotion(int x, int y) {
}

void passiveMouseMotion(int x, int y) {
}

void idle() {
    if(!paused){
        e_rotate=fmod((e_rotate+X),360.0f);
        e_revolution=fmod((e_revolution+X/365.0f),360.0f);
        m_rotate=fmod((e_rotate+X/28.0f),360.0f);
        m_revolution=fmod((m_revolution+X/28.0f),360.0f);
    }
    
    glutPostRedisplay();    //重繪影像，否則圖像只有游標、鍵盤改變時才更新影像
}
void lighting(){
    glEnable(GL_LIGHTING);  //啟用光罩功能
    GLfloat diffuse_color[]={1.0f,1.0f,1.0f,1.0f};
    GLfloat ambient_color[]={0.5f,0.5f,0.5f,1.0f};
    GLfloat position[]={0.0f,10.0f,0.0f,1.0f};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
}

#define PI 3.1415926
void drawSphere(GLfloat xx, GLfloat yy, GLfloat zz, GLfloat radius, GLfloat M, GLfloat N)
{
    float step_z = PI/M;        //縱向夾角 pi
    float step_xy = 2*PI/N;     //橫向夾角 2*pi
    float x[4],y[4],z[4];
    float angle_z = 0.0;
    float angle_xy = 0.0;
    int i=0, j=0;
    glBegin(GL_QUADS);  //繪製四邊形
    for(i=0; i<M; i++) {
        angle_z = i * step_z;
        for(j=0; j<N; j++) {
            angle_xy = j * step_xy;
            x[0] = radius * sin(angle_z) * cos(angle_xy);
            y[0] = radius * sin(angle_z) * sin(angle_xy);
            z[0] = radius * cos(angle_z);
            x[1] = radius * sin(angle_z + step_z) * cos(angle_xy);
            y[1] = radius * sin(angle_z + step_z) * sin(angle_xy);
            z[1] = radius * cos(angle_z + step_z);
            x[2] = radius * sin(angle_z + step_z) * cos(angle_xy + step_xy);
            y[2] = radius * sin(angle_z + step_z) * sin(angle_xy + step_xy);
            z[2] = radius * cos(angle_z + step_z);
            x[3] = radius * sin(angle_z) * cos(angle_xy + step_xy);
            y[3] = radius * sin(angle_z) * sin(angle_xy + step_xy);
            z[3] = radius * cos(angle_z);
            for(int k=0; k<4; k++) {
                glNormal3f(xx+x[k], yy+y[k], zz+z[k]);
                glVertex3f(xx+x[k], yy+y[k], zz+z[k]);   //設定座標點，它的顏色取決於glColor3f
                //定出4點來表示四邊形
            }
            
        }
        
    }
    glEnd();
    
}

