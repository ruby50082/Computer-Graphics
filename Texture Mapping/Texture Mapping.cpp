
#include <iostream>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <FreeImage.h>
#include <cmath>

#define PI 3.14159265358

using namespace std;

void display();
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void idle();

void init();
void lighting();
void init_texture();
void mySphere(float radius, int slice, int stack, bool index);

int width, height;
unsigned int texture_id;
int win_width = 400, win_height = 400;
int degree = 0;
static bool paused = false,trans = false;
int X=5;
float Y=1;
static float e_revolution, e_rotate, m_revolution, m_rotate;
GLuint id[2];

struct _AUX_RGBImageRec {
    unsigned long sizeX;
    unsigned long sizeY;
    unsigned char *data;
};

typedef struct _AUX_RGBImageRec AUX_RGBImageRec;

GLboolean LoadBmp(const char* filename,AUX_RGBImageRec* texture_image) {

    FIBITMAP* dib = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename,0);
    dib=FreeImage_ConvertTo32Bits(dib); //對於不是32位元的圖片，強制轉換至32位元，RGBA
    width = FreeImage_GetWidth(dib);    //獲取像素的長寬，以像素為單位
    height = FreeImage_GetHeight(dib);

    BYTE *pixels = (BYTE*) FreeImage_GetBits(dib);  //獲取數據指針
    int pix = 0;
    
    if (texture_image == NULL)
        return FALSE;
    
    texture_image->data = (BYTE *) malloc(width * height * 4);
    texture_image->sizeX = width;
    texture_image->sizeY = height;
    
    for (pix = 0; pix < width * height; pix++) {
        texture_image->data[pix * 4 + 0] = pixels[pix * 4 + 0];
        texture_image->data[pix * 4 + 1] = pixels[pix * 4 + 1];
        texture_image->data[pix * 4 + 2] = pixels[pix * 4 + 2];
        texture_image->data[pix * 4 + 3] = pixels[pix * 4 + 3];
    }
    
    FreeImage_Unload(dib);  //從內存中刪除此圖片
    
    return TRUE;
}

int LoadGLTextures()    //載入圖片，並轉換成紋理
{
    int Status=FALSE;
    AUX_RGBImageRec *textureImage, *textureImage2; //建立紋理儲存空間
    textureImage = (AUX_RGBImageRec*)malloc(sizeof(AUX_RGBImageRec));
    textureImage2 = (AUX_RGBImageRec*)malloc(sizeof(AUX_RGBImageRec));
    
    //載入位圖
    if ( LoadBmp("earth.jpg", textureImage) )
    {
        Status=TRUE;
        //GLuint id;
        glGenTextures(1, &id[0]); //建立紋理
        // 使用位圖生成的紋理
        glBindTexture(GL_TEXTURE_2D, id[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        // 生成纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureImage->sizeX, textureImage->sizeY, 0, GL_BGRA, GL_UNSIGNED_BYTE, textureImage->data);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
        
        //GL_TEXTURE_MIN_FILTER表示設定材質影像需要放大時該如何處理 線形濾波
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }
    if ( LoadBmp("moon.jpg", textureImage2) )
    {
        Status=TRUE;
        //GLuint id;
        glGenTextures(1, &id[1]); //建立紋理
        // 使用位圖生成的紋理
        glBindTexture(GL_TEXTURE_2D, id[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        // 生成纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureImage2->sizeX, textureImage2->sizeY, 0, GL_BGRA, GL_UNSIGNED_BYTE, textureImage2->data);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
        
        //GL_TEXTURE_MIN_FILTER表示設定材質影像需要放大時該如何處理 線形濾波
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }
    if (textureImage) // 紋理是否存在
    {
        if (textureImage->data) // 紋理圖像是否存在
        {
            free(textureImage->data); // 釋放紋理圖像所佔用的內存
        }
        free(textureImage); // 釋放圖像結構
    }
    if (textureImage2) // 紋理是否存在
    {
        if (textureImage2->data) // 紋理圖像是否存在
        {
            free(textureImage2->data); // 釋放紋理圖像所佔用的內存
        }
        free(textureImage2); // 釋放圖像結構
    }
    return Status;
}

void display()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);  //改view matrix
    //眼睛座標、相機座標eyeX,Y,Z, 物體的中心、焦點centerX,Y,Z, 頭頂的朝向upX,Y,Z
    glMatrixMode(GL_PROJECTION);    //選擇想要改變的矩陣：投影矩陣
    glLoadIdentity();
    gluPerspective(45, win_width / (GLfloat)win_height, 0.1, 1000); //透視投影
    //在y-z平面的角度（眼睛和模型的連線＆Y軸的夾角）/ 投影平面寬高比 / 延負z軸的近遠剪裁面->最近最遠能看到的距離
    glViewport(0, 0, win_width, win_height);    //把視景體擷取的圖像顯示到屏幕上
    
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_CULL_FACE);        //remove back face
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);        //normalized normal
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     //將緩存清除為預先設定值
    
    lighting();

    //earth
    glPushMatrix();
    glRotatef(23.5,0,0,1);
    glRotatef(e_rotate,0,1,0);
    if(trans==false){
        mySphere(Y,360,180,0);
    }
    else{
        mySphere(Y,4,2,0);
    }
    glPopMatrix();
    
    //moon
    glPushMatrix();
    glRotatef(m_revolution, 0.0f, 1.0f, 0.0f);
    glTranslatef(3*Y, 0.0, 0.0);
    glRotatef(-m_revolution, 0.0f, 1.0f, 0.0f);
    glRotatef(m_rotate,0,1,0);
    mySphere(0.5*Y,240,60,1);
    glPopMatrix();
    
    glFlush();
    
}

void myInit()
{
    
    LoadGLTextures();
    
    glEnable(GL_TEXTURE_2D);                 // 啟用紋理
    glShadeModel(GL_SMOOTH);                 // 啟用陰影平滑
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);    // 黑色背景
    glClearDepth(1.0f);                      // 設置深度緩存
    glEnable(GL_DEPTH_TEST);                 // 啟用深度測試
    glDepthFunc(GL_LEQUAL);                  // 深度測試的類型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // 真正精細的透視修正

}

void reshape(int _width, int _height) {
    win_width = _width;
    win_height = _height;
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
        m_rotate=fmod((m_rotate+X/28.0f),360.0f);
        m_revolution=fmod((m_revolution+X/28.0f),360.0f);
    }
    glutPostRedisplay();    //重繪影像，否則圖像只有游標、鍵盤改變時才更新影像
}

void lighting(){
    glEnable(GL_LIGHTING);
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

void mySphere(float radius,int slice, int stack, bool index) {
    double x, y, z, u, v;
    double slice_step = 2 * PI / slice, stack_step = PI / stack;
    if(index==0)    glBindTexture(GL_TEXTURE_2D,id[0]);
    else            glBindTexture(GL_TEXTURE_2D,id[1]);
    for (int i = 0; i < slice; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < stack + 1; j++) {
            x = radius * sin(j * stack_step) * cos(i * slice_step);
            y = radius * cos(j * stack_step);
            z = radius * sin(j * stack_step) * sin(i * slice_step);
            
            u = 1-double(i)/double(slice);
            v = 1-(double(j)/double(stack));
            glTexCoord2f(u,v);
            glNormal3d(x, y, z);
            glVertex3d(x, y, z);
            
            x = radius * sin(j * stack_step) * cos((i + 1) * slice_step);
            y = radius * cos(j * stack_step);
            z = radius * sin(j * stack_step) * sin((i + 1) * slice_step);
            
            u = 1-double(i+1)/double(slice);
            v = 1-(double(j)/double(stack));
            glTexCoord2f(u,v);
            glNormal3d(x, y, z);
            glVertex3d(x, y, z);
        }
        glEnd();
    }
}

int main(int argc, char**argv)
{
    e_rotate=0;
    e_revolution=0;
    m_rotate=0;
    m_revolution=0;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("HW2");
    
    myInit();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(passiveMouseMotion);
    glutIdleFunc(idle);
    
    glutMainLoop();
    
    return 0;
}

