#include <GL/glut.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <math.h> 
#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif 

// 腕と脚の角度（アニメーション用） 
float armAngle = 0.0f;
float bodyAngle = 0.0f;
float bodyPosition = 0.0f;
float rightThighLocalAngle = 0.0f;
float leftThighLocalAngle = 0.0f;
int armDirection = -1;
int legDirection = 1;
int armWaiting = 0; // 0:動作中, 1:上げきって待機中 
int legPhase = 0; // 今のフェーズ 
float rightLegAngle = 0.0f; // 右足の角度 
float leftLegAngle = 0.0f;  // 左足の角度 
float rightThighAngle = 0.0f; // 右ももの角度 
float leftThighAngle = 0.0f;  // 左ももの角度 
int legWait = 0; // 足の動きを止めるフラグ 0:足動作中, 1:足ストップ中 
float shadowScale = 1.3; // 影の大きさ

// 屈伸用フェーズ管理 
enum { SQUAT_NONE, SQUAT_DOWN, SQUAT_UP };
int squatPhase = SQUAT_NONE;

// UFO関連の変数
float ufoPositionX = 25.0f; // UFOのX座標（初期位置は画面右外）
float ufoPositionY = 7.0f; // UFOのY座標
float ufoPositionZ = -18.0f; // UFOのZ座標（ロボットの後ろ）
float ufoSpeedX = 0.7f;     // UFOのX方向への速度
int ufoFlying = 0;          // UFOが飛行中かどうか (0:停止, 1:飛行中)
int ufoMovementPhase = 0;   // UFOの動き 0:左へ, 1:少し右へ, 2:再び左へ
float ufoScale = 0.0f; // UFOの大きさ

// テクスチャマッピング
#define	imageWidth 256
#define	imageHeight 256
unsigned char texImage[imageHeight][imageWidth][3];
GLuint textureID_shibafu;

// マウス操作
unsigned char mouseFlag = GL_FALSE;		// 動いているかどうか
int	xStart, yStart;				// スタートポジション
double xAngle = 0.0, yAngle = 0.0;	// アングル

// 円を書く関数(目のため) 
void drawCircle(float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); // 中心 
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        glVertex3f(x, y, 0.0f); // Z方向正面に描画 
    }
    glEnd();
}

// ==== ロボットを描画 ==== 
void drawDadandan() {
    GLUquadric* robotQuad = gluNewQuadric();
    gluQuadricNormals(robotQuad, GLU_SMOOTH);

    // ==== 地面に影 ====
    glPushMatrix();
    glColor3f(0.2, 0.2, 0.2); // 黒
    glTranslatef(0.0, -1.0, 0.0);
    glTranslatef(0.0, 0.0, -3.0);
    glTranslatef(-2.0, 0.0, 0.0);
    glRotatef(90, 1, 0, 0);
    glScalef(shadowScale, 1.0, 1.0);
    gluDisk(robotQuad, 0.0, 2.0, 20, 1);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.2, 0.2, 0.2); // 黒
    glTranslatef(0.0, -1.0, 0.0);
    glTranslatef(0.0, 0.0, -3.0);
    glTranslatef(-1.5, 0.0, 0.0);
    glRotatef(90, 1, 0, 0);
    glScalef(1.2, 1.0, 1.0);
    gluDisk(robotQuad, 0.0, 2.0, 20, 1);
    glPopMatrix();

    // === 頭 === 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);// 灰色 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glTranslatef(0.0, 2.4, 0.0);
    glScalef(1.1, 1.0, 1.0);   // 横に長い楕円形 
    glutSolidSphere(1.0, 30, 30); // 球 
    glPopMatrix();

    // 頭線 
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);// 黒 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glRotatef(-90, 0, 1, 0);
    glTranslatef(0.0, 2.4, 0.0);
    gluCylinder(robotQuad, 1.025, 1.025, 0.02, 20, 20); // 半径1.025、高さ0.02の円柱 
    glPopMatrix();

    // ネジ 
    glPushMatrix();
    glColor3f(0.1, 0.1, 0.1); // ネジ色 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glTranslatef(0.05, 1.55, 1.0);

    float screwSpacing = 0.3; // 球同士の間隔 
    float list[6] = { -0.4, -0.1, 0.0, 0.0, 0.0, -0.2 };
    for (int i = 0; i < 6; ++i) {
        glPushMatrix();
        glTranslatef(0.0, screwSpacing * i, list[i]); // 縦に並べる 
        glutSolidSphere(0.03, 16, 16);
        glPopMatrix();
    }
    glPopMatrix();

    // 口 
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// 紫 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.0, 0.0, 1.9);
    gluCylinder(robotQuad, 1.005, 1.1, 0.35, 20, 20); //半径は球体に沿わせる、高さ0.35の円柱 
    glPopMatrix();

    // 口線 
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);// 黒 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.0, 0.0, 2.1);
    gluCylinder(robotQuad, 1.075, 1.075, 0.02, 20, 20); // 半径1.075、高さ0.02の円柱 
    glPopMatrix();

    // === 目（左右） ===
    glPushMatrix();
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);

    for (int side = -1; side <= 1; side += 2) {
        // 赤い目
        glPushMatrix();
        glColor3f(0.8, 0.0, 0.0); // 赤
        glTranslatef(0.4f * side, 2.55f, 1.0f);
        drawCircle(0.2f, 40);
        glPopMatrix();

        // 白いハイライト
        glPushMatrix();
        glColor3f(1.0, 0.9, 0.9); // 白
        glTranslatef(0.39f * side, 2.54f, 1.0001f);
        drawCircle(0.13f, 40);
        glPopMatrix();

        // 黒い瞳孔
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0); // 黒
        glTranslatef(0.36f * side, 2.54f, 1.0002f);
        drawCircle(0.07f, 40);
        glPopMatrix();
    }
    glPopMatrix(); // 目の共通の変換を終了


    // === 胴体（円柱） === 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glTranslatef(0.0, 0.65, 0.0);
    glRotatef(-90, 1, 0, 0); // Z方向に立てる 
    gluCylinder(robotQuad, 0.4, 0.4, 1.5, 20, 20); // 半径0.4、高さ1.5の円柱 

    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(0.0, 0.0, -0.2);
    gluCylinder(robotQuad, 0.2, 0.4, 0.2, 20, 20); // 上半径0.4、下半径0.2、高さ0.2の円柱 
    glPopMatrix();

    // === 腕（左右） === 
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        // 肩 
        glColor3f(0.45, 0.3, 0.75);
        glTranslatef(0.0, bodyPosition, 0.0);
        glRotatef(bodyAngle, 0, 0, 1);
        glTranslatef(0.35 * side, 1.35, 0.0);
        glRotatef(armAngle * side, 0, 0, 1);
        glutSolidSphere(0.2, 20, 20);

        // 二の腕 
        glPushMatrix();
        glColor3f(0.4, 0.4, 0.4);
        glRotatef(60 * side, 0, 0, 1);
        glTranslatef(0.1 * side, -0.45, 0.0);
        glScalef(0.15, 0.7, 0.15);
        glutSolidCube(1.0);
        glPopMatrix();

        // 肘 
        glColor3f(0.45, 0.3, 0.75);
        glRotatef(30 * side, 0, 0, 1);
        glTranslatef(0.45 * side, -0.55, 0.0);
        glutSolidSphere(0.125, 20, 20);

        // 前腕 
        glPushMatrix();
        glColor3f(0.4, 0.4, 0.4);
        glRotatef(-80 * side, 0, 0, 1);
        glTranslatef(-0.025 * side, 0.35, 0.0);
        glScalef(0.15, 0.7, 0.15);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        // 手バンド 
        glColor3f(0.45, 0.3, 0.75);// 紫 
        glRotatef(10 * side, 0, 0, 1);
        glTranslatef(0.6 * side, 0.025, 0.0);
        glRotatef(90 * side, 0, 1, 0);
        gluCylinder(robotQuad, 0.1, 0.1, 0.1, 20, 20); // 半径0.1、高さ0.1の円柱 
        glPopMatrix();

        // 手のひら（さらに前腕の末端に） 
        glPushMatrix();
        glRotatef(-40 * side, 0, 0, 1);
        glTranslatef(0.6 * side, 0.7, 0.0);
        glRotatef(90, 0, 0, 1);
        glScalef(1.7, 1.7, 1.7);

        int segments = 40;
        float innerR = 0.07;
        float outerR = 0.15;
        float startAngle = 30.0; // 開口部を作るためカット 
        float endAngle = 330.0;

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float angle = startAngle + (endAngle - startAngle) * i / segments;
            float rad = angle * M_PI / 180.0;
            float xOuter = cos(rad) * outerR;
            float yOuter = sin(rad) * outerR;
            float xInner = cos(rad) * innerR;
            float yInner = sin(rad) * innerR;

            glColor3f(0.4, 0.4, 0.4);
            glVertex3f(xOuter, yOuter, 0.05); // 上面 
            glVertex3f(xInner, yInner, 0.05);
            glVertex3f(xOuter, yOuter, -0.05); // 底面 
            glVertex3f(xInner, yInner, -0.05);
        }
        glEnd();

        glPopMatrix();
        glPopMatrix();
    }


    // === 右足 === 
    glPushMatrix();
    glRotatef(rightThighAngle, 0, 0, 1); // 股関節の回転 

    // 太もも 
    glColor3f(0.4, 0.4, 0.4);
    glTranslatef(0.5, 0, 0);      // 1. 原点を回転の中心まで移動 
    glRotatef(rightThighLocalAngle, 0, 0, -1); // 2. 回転 
    glTranslatef(-0.5, 0, 0);   // 3. 元の座標系に戻す 
    glTranslatef(0.15, 0.65, 0.0); // 位置移動 
    glRotatef(-270, 1, 0, 0);
    glRotatef(55, 0, 1, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 0.8, 20, 20); // 太もも 
    glTranslatef(0.0, 2.0, 0.0); // 位置移動 
    glPopMatrix();

    glPushMatrix();// ふくらはぎより下を動かす 
    glTranslatef(0, rightLegAngle, 0); // ふくらはぎを上下 

    glTranslatef(0.65, -0.5, 0.0); // 脚の位置 
    glTranslatef(0.0, -0.875, 0.0);

    // ふくらはぎ 
    glTranslatef(0.09, 0.7, 0.0); // 位置移動 
    glRotatef(-90, 1, 0, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 1.0, 20, 20); // 半径0.15、高さ1.0の円柱 
    glTranslatef(0.0, 0.0, 1.0); // 脚の下端へ移動 

    // 足の甲 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5); // 灰色 
    glTranslatef(0.0, 0.0, -1.0); // 脚の下に配置 
    gluCylinder(robotQuad, 0.5, 0.5, 0.2, 20, 20);

    glPushMatrix();// 底面 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1); // 半径0.5の円を描画 
    glPopMatrix();

    glPushMatrix(); // 上面 
    glTranslatef(0.0, 0.0, 0.2); // 円柱の高さに合わせて移動 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1);
    glPopMatrix();
    glPopMatrix();

    // 足バンド 
    glColor3f(0.45, 0.3, 0.75);// 紫 
    glTranslatef(0.0, 0.0, -0.6); // 脚の下端へ移動 
    gluCylinder(robotQuad, 0.151, 0.151, 0.2, 20, 20); // 半径0.151、高さ0.2の円柱 
    glPopMatrix();

    // === 左足 ===  
    glPushMatrix();
    glRotatef(leftThighAngle, 0, 0, 1); // 股関節の回転 
    glTranslatef(-0.5, 0, 0);      // 1. 原点を回転の中心まで移動 
    glRotatef(leftThighLocalAngle, 0, 0, 1); // 2. 回転 
    glTranslatef(0.5, 0, 0);   // 3. 元の座標系に戻す 

    // 太もも 
    glColor3f(0.4, 0.4, 0.4);
    glTranslatef(-0.15, 0.65, 0.0); // 位置移動 
    glRotatef(-270, 1, 0, 0);
    glRotatef(-55, 0, 1, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 0.8, 20, 20); // 太もも 
    glTranslatef(0.0, 2.0, 0.0); // 位置移動 
    glPopMatrix();

    glPushMatrix();// ふくらはぎより下を動かす 
    glTranslatef(0, leftLegAngle, 0); // ふくらはぎを上下 

    glTranslatef(-0.65, -0.5, 0.0); // 脚の位置 
    glTranslatef(0.0, -0.875, 0.0);

    // ふくらはぎ 
    glTranslatef(-0.09, 0.7, 0.0); // 位置移動 
    glRotatef(-90, 1, 0, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 1.0, 20, 20); // 半径0.15、高さ1.0の円柱 
    glTranslatef(0.0, 0.0, 1.0); // 脚の下端へ移動 

    // 足の甲 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5); // 灰色 
    glTranslatef(0.0, 0.0, -1.0); // 脚の下に配置 
    gluCylinder(robotQuad, 0.5, 0.5, 0.2, 20, 20);

    glPushMatrix(); // 底面 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1); // 半径0.5の円を描画 
    glPopMatrix();

    glPushMatrix(); // 上面 
    glTranslatef(0.0, 0.0, 0.2); // 円柱の高さに合わせて移動 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1);
    glPopMatrix();
    glPopMatrix();

    // 足バンド 
    glColor3f(0.45, 0.3, 0.75);// 紫 
    glTranslatef(0.0, 0.0, -0.6); // 脚の下端へ移動 
    gluCylinder(robotQuad, 0.151, 0.151, 0.2, 20, 20); // 半径0.151、高さ0.2の円柱 
    glPopMatrix();
}

// ==== UFOを描画 ====
void drawUFO() {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    glPushMatrix();
    // ==== 地面に影 ====
    glColor3f(0.2, 0.2, 0.2); // 黒
    glScalef(ufoScale * 1.5, ufoScale, ufoScale);
    glTranslatef(ufoPositionX- ufoPositionY, -1.0, ufoPositionZ);
    glRotatef(90, 1, 0, 0);
    gluDisk(quad, 0.0, 1.0, 20, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(ufoPositionX, ufoPositionY, ufoPositionZ); // UFO全体の位置
    glScalef(ufoScale, ufoScale, ufoScale);

    // 円盤
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// 紫
    glTranslatef(0.0f, -0.6f, 0.0f);
    glRotatef(-90, 1, 0, 0); // Z方向に立てる 
    gluCylinder(quad, 1.6, 0.9, 0.3, 20, 20); // 上半径0.4、高さ0.5の円柱 
    glPopMatrix();

    // ドーム（半球）
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 1.0f); // 水色
    glScalef(0.9f, 1.0f, 0.9f); // 高さ方向に少し伸ばす
    gluSphere(quad, 1.0, 30, 30); // 半径1.0の球でドームを作成
    glPopMatrix();

    // 円柱
    glPushMatrix();
    glColor3f(0.6, 0.3, 0.58);// 赤紫
    glTranslatef(0.0f, -1.1f, 0.0f);
    glRotatef(-90, 1, 0, 0); // Z方向に立てる 
    gluCylinder(quad, 0.85, 0.85, 0.75, 20, 20); // 半径0.9、高さ0.8の円柱 
    glPopMatrix();

    // 円柱下
    glPushMatrix();
    glColor3f(0.6, 0.3, 0.58);// 赤紫
    glTranslatef(0.0f, -1.0f, 0.0f);
    glScalef(0.95f, 0.2f, 0.95f);
    gluSphere(quad, 1.0, 30, 30); // 半径1.0の球を潰して円盤にする
    glPopMatrix();


    // アンテナ
    // 1本目のアンテナ
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// 紫
    glRotatef(10, 0, 0, 1);
    glTranslatef(-0.4, 0.85, 0.0f); // ドームのYの頂点にX方向にずらして配置
    glRotatef(-90, 1, 0, 0); // Z軸を上に向けた後、Y軸方向に立てる
    gluCylinder(quad, 0.1, 0.05, 0.6, 10, 10); // 円柱
    // アンテナの先端の球
    glTranslatef(0.0f, 0.0f, 0.5); // 円柱の先端に移動
    glutSolidSphere(0.15, 10, 10); // 少し大きめの球
    glPopMatrix();

    // 2本目のアンテナ
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// 紫
    glRotatef(-10, 0, 0, 1);
    glTranslatef(0.4, 0.85, 0.0f); // ドームのYの頂点にX方向にずらして配置
    glRotatef(-90, 1, 0, 0); // Z軸を上に向けた後、Y軸方向に立てる
    gluCylinder(quad, 0.1, 0.05, 0.6, 10, 10); // 円柱
    // アンテナの先端の球
    glTranslatef(0.0f, 0.0f, 0.5); // 円柱の先端に移動
    glutSolidSphere(0.15, 10, 10); // 少し大きめの球
    glPopMatrix();

    glPopMatrix();
    gluDeleteQuadric(quad);
}

#define	imageWidth 256
#define	imageHeight 256

unsigned char texImage[imageHeight][imageWidth][3];

void readPPMImage(char* filename)
{
    FILE* fp;
    int  ch, i;

    if (fopen_s(&fp, filename, "r") != 0) {
        fprintf(stderr, "Cannot open ppm file %s\n", filename);
        exit(1);
    }
    /*
    for (i = 0; i < 4; i++){
        while (1){
            if ((ch = fgetc(fp)) != '#') break;
            fgets((char*)texImage, 1024, fp);
            while(isspace(ch)) ch = fgetc(fp);
        }
        while (!isspace(ch)) ch = fgetc(fp)
        if (i < 3){
            while (isspace(ch)) ch = fgetc(fp);
        }
    }*/
    fread(texImage, 1, imageWidth * imageHeight * 3, fp);
    fclose(fp);
}

void setUpTexture(void)
{
    // === shibafu.ppm の設定 ===
    glGenTextures(1, &textureID_shibafu);
    glBindTexture(GL_TEXTURE_2D, textureID_shibafu);

    readPPMImage("shibafu.ppm");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
}



// ==== 背景 ==== 
// 山 
void drawMountains(float radius, float baseY, int count) {
    int count2 = 0;
    for (int i = 0; i < count; ++i) {
        float theta = (2.0f * M_PI * i) / count; // 0～2πで分布 
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        float width = 10.0f;   // 山の横幅 
        float height = 0.0f;  // 山の高さ 

        if (count2 % 3 == 0) {
            height = 4.5f;
            width = 11.0f;
        }
        else if (count2 % 2 == 0) {
            height = 3.0f;
            width = 13.0f;
        }
        else {
            height = 5.5f;
            width = 10.0f;
        }
        count2 += 1;
        
        // 山の向きを円柱の中心に向ける 
        float dirX = -cos(theta);
        float dirZ = -sin(theta);

        // 三角形の底辺両端 
        float bx1 = x + (width / 2) * sin(theta);
        float bz1 = z - (width / 2) * cos(theta);
        float bx2 = x - (width / 2) * sin(theta);
        float bz2 = z + (width / 2) * cos(theta);

        // 頂点は底辺の中央から中心方向にheightだけ上 
        float tx = x + dirX * height * 0.2f; // 少し内向き 
        float ty = baseY + height;
        float tz = z + dirZ * height * 0.2f;

        glColor3f(0.42, 0.46, 0.82); // 山色 
        glBegin(GL_TRIANGLES);
        glVertex3f(bx1, baseY, bz1); // 底辺左 
        glVertex3f(bx2, baseY, bz2); // 底辺右 
        glVertex3f(tx, ty, tz);      // 頂点 
        glEnd();
    }
}

// 雲 
void drawClouds(float radius, float baseY, int count) {
    for (int i = 0; i < count; ++i) {
        float theta = (2.0f * M_PI * i) / count + (i % 2) * 0.4f;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        glPushMatrix();
        glTranslatef(x, baseY, z);
        // 雲も中心方向に回転 
        glRotatef(-theta * 180.0f / M_PI + 90.0f, 0, 1, 0);
        glColor3f(1.0, 1.0, 1.0);
        glScalef(2.3, 1.0, 1.0);
        glutSolidSphere(0.8, 16, 8);
        glTranslatef(0.8, 0.1, 0.0);
        glutSolidSphere(0.5, 16, 8);
        glTranslatef(-1.6, 0.1, 0.0);
        glutSolidSphere(0.5, 16, 8);
        glPopMatrix();
    }
}

void drawBackground() {
    glDisable(GL_LIGHTING); // ライティングを一時無効化 

    // 地面（テクスチャマッピングあり）
    glEnable(GL_TEXTURE_2D); // 2Dテクスチャマッピングを有効にする
    glBindTexture(GL_TEXTURE_2D, textureID_shibafu);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    // 各頂点にテクスチャ座標を割り当てる
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, -1.01f, -30.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, -1.01f, -30.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, -1.01f, 30.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, -1.01f, 30.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D); // 2Dテクスチャマッピングを無効にする

    // 大きな円柱（空） 
    GLUquadric* wall = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, -10.0f, 0.0f);
    glRotatef(-90, 1, 0, 0); // Z軸方向をY軸方向に 
    glColor3f(0.77, 0.95, 1.0); // 薄い水色 
    gluCylinder(wall, 30.0, 30.0, 50.0, 64, 1); // 半径20, 高さ16 
    glPopMatrix();
    gluDeleteQuadric(wall);

    //天井（空） 
    glPushMatrix();
    glColor3f(0.77, 0.95, 1.0); // 薄い水色 
    glTranslatef(0.0f, 16.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    gluDisk(wall, 0.0, 30.0, 64, 1);
    glPopMatrix();

    // 背景の描画が終わったら、ライティングを再度有効にする 
    glEnable(GL_LIGHTING);

    // 山と雲はライティング有効化 
    float radius = 30.0f;
    drawMountains(radius - 1.5f, -1.01f, 20);
    drawClouds(radius - 0.5f, 7.0f, 19);
}

// ==== 光源 ==== 
void Light() {
    // 太陽のような平行光源
    float light0_position[] = { 1.0, 1.5, 0.5, 0.0 };
    float light0_ambient[] = { 0.5, 0.5, 0.45, 1.0 };
    float light0_diffuse[] = { 1.0, 1.0, 0.9, 1.0 };
    float light0_specular[] = { 0.8, 0.8, 0.8, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    glEnable(GL_LIGHT0);

    // グローバルな環境光を設定  
    GLfloat global_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

// ==== タイマーコールバック ==== 
void armDownStart(int v) {
    armDirection = -1;
    armWaiting = 0;
}
void resumeLeg(int v) {
    legWait = 0;
}

// ==== アニメーション ==== 
void animate(int value) {
    // == 腕の動き == 
    if (!armWaiting) {
        armAngle += 5.0f * armDirection;
        if (armDirection == 1 && armAngle > 15.0f) {
            armAngle = 15.0f;
            armWaiting = 1;
            glutTimerFunc(580, armDownStart, 0);
        }
        if (armDirection == -1 && armAngle < -35.0f) {
            armAngle = -35.0f;
            armDirection = 1;
        }
    }

    // == 体の屈伸運動 == 
    const float thighSquatMax = 8.0f;
    const float squatMin = -0.15f;
    const float squatSpeed = 0.025f;

    if (squatPhase == SQUAT_DOWN) { // 上半身を下げる 
        bodyPosition -= squatSpeed;
        rightThighLocalAngle -= 0.25f;
        leftThighLocalAngle -= 0.25f;
        if (bodyPosition <= squatMin) {
            bodyPosition = squatMin;
            squatPhase = SQUAT_UP;
        }
    }
    else if (squatPhase == SQUAT_UP) { // 上半身を上げる 
        bodyPosition += squatSpeed;
        rightThighLocalAngle += 0.25f;
        leftThighLocalAngle += 0.25f;;
        if (bodyPosition >= 0.0f) {
            bodyPosition = 0.0f;
            squatPhase = SQUAT_NONE;
        }
    }
    if (squatPhase == SQUAT_DOWN || squatPhase == SQUAT_UP) {
        // 屈伸の深さに応じて太ももの角度を変化 
        float t = (bodyPosition - 0.0f) / (squatMin - 0.0f); // t: 0（上）～ 1（下） 
        rightThighLocalAngle = thighSquatMax * -t;
        leftThighLocalAngle = thighSquatMax * -t;
    }
    // 屈伸が終わったら0に戻す 
    if (squatPhase == SQUAT_NONE) {
        rightThighLocalAngle = 0.0f;
        leftThighLocalAngle = 0.0f;
    }

    // == 足の動き == 
    const float liftMax = 0.2f;
    const float UpSpeed = 0.005f;
    const float DownSpeed = 0.075f;
    const float thighLiftMax = 7.0f;
    const float thighUpSpeed = 0.3f;
    const float thighDownSpeed = 3.0f;
    const float bodyMax = 8.0f;

    if (!legWait) {
        switch (legPhase) {
        case 0: // 両足つき→右足上げ開始 
            squatPhase = SQUAT_DOWN; // 屈伸スタート 
            rightLegAngle = 0.0f;
            leftLegAngle = 0.0f;
            rightThighAngle = 0.0f;
            leftThighAngle = 0.0f;
            legPhase = 1;
            legWait = 1;
            glutTimerFunc(200, resumeLeg, 0); // 0.2秒待機 
            break;

        case 1: // 右足上げる＋右太もも 
            rightLegAngle += UpSpeed;
            rightThighAngle += thighUpSpeed;
            rightThighLocalAngle = -(rightLegAngle / liftMax) * thighLiftMax;
            if (bodyAngle < bodyMax) {
                bodyAngle += 0.2f;
                shadowScale += 0.005;
                if (bodyAngle > bodyMax) bodyAngle = bodyMax;
            }
            if (rightLegAngle >= liftMax) {
                rightLegAngle = liftMax;
                rightThighAngle = thighLiftMax;
                legPhase = 2;
            }
            break;

        case 2: // 右足下げる＋右太もも 
            rightLegAngle -= DownSpeed;
            rightThighAngle -= thighDownSpeed;
            rightThighLocalAngle = -(rightLegAngle / liftMax) * thighLiftMax;
            if (bodyAngle > -bodyMax) {
                bodyAngle -= 3.0f;
                shadowScale -= 0.005;
                if (bodyAngle < -bodyMax) bodyAngle = -bodyMax;
            }
            if (rightLegAngle <= 0.0f) {
                rightLegAngle = 0.0f;
                rightThighAngle = 0.0f;
                legPhase = 3;
            }
            break;

        case 3: // 両足つき→左足上げ開始 
            squatPhase = SQUAT_DOWN; // 屈伸スタート 
            rightLegAngle = 0.0f;
            leftLegAngle = 0.0f;
            rightThighAngle = 0.0f;
            leftThighAngle = 0.0f;
            legPhase = 4;
            legWait = 1;
            glutTimerFunc(200, resumeLeg, 0); // 0.2秒待機 
            break;

        case 4: // 左足上げる＋左太もも 
            leftLegAngle += UpSpeed;
            leftThighAngle -= thighUpSpeed;
            leftThighLocalAngle = -(leftLegAngle / liftMax) * thighLiftMax;
            if (bodyAngle > -bodyMax) {
                bodyAngle -= 0.2f;
                shadowScale -= 0.005;
                if (bodyAngle < -bodyMax) bodyAngle = -bodyMax;
            }
            if (leftLegAngle >= liftMax) {
                leftLegAngle = liftMax;
                leftThighAngle = -thighLiftMax;
                legPhase = 5;
            }
            break;

        case 5: // 左足下げる＋左太もも 
            leftLegAngle -= DownSpeed;
            leftThighAngle += thighDownSpeed;
            leftThighLocalAngle = -(leftLegAngle / liftMax) * thighLiftMax;
            if (bodyAngle < bodyMax) {
                bodyAngle += 3.0f;
                shadowScale += 0.005;
                if (bodyAngle > bodyMax) bodyAngle = bodyMax;
            }
            if (leftLegAngle <= 0.0f) {
                leftLegAngle = 0.0f;
                leftThighAngle = 0.0f;
                legPhase = 0;
            }
            break;
        }
    }

    // == UFOの動き ==
    if (ufoFlying) {
        switch (ufoMovementPhase) {
        case 0: // X軸正方向から負方向へ移動
            ufoPositionX -= ufoSpeedX;
            ufoPositionY -= 0.005f;
            ufoScale += 0.005f;
            if (ufoPositionX <= -6.0f) { // ロボットの左側まで来たらフェーズ1へ
                ufoMovementPhase = 1;
            }
            break;
        case 1: // 少しだけX軸正方向へ戻る
            ufoPositionX += ufoSpeedX * 0.3f; // 戻る速度は遅めに
            ufoPositionY -= 0.08f;
            ufoPositionZ += 0.2f;
            ufoScale += 0.02f;
            if (ufoPositionX >= 0.0f) { // 少し戻ったらフェーズ2へ
                ufoMovementPhase = 2;
            }
            break;
        case 2: // 再びX軸負方向へ移動し、画面外へ
            ufoPositionX -= ufoSpeedX;
            ufoPositionY -= 0.005f;
            ufoScale -= 0.01f;
            if (ufoPositionX <= -25.0f) { // 画面外に出たら飛行停止
                ufoFlying = 0;
                ufoMovementPhase = 0; // 次回のためにフェーズをリセット
                ufoPositionX = 25.0f; // 次回のために初期位置をリセット
                ufoPositionZ = -18.0f; // Z座標もリセット
            }
            break;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, animate, 0);
}

// ==== キーボード入力関数 ====
void myKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'u': // 'u' キーを押したらUFOを飛行開始
    case 'U':
        if (!ufoFlying) {
            ufoFlying = 1;
            ufoMovementPhase = 0; // 動きのフェーズを初期化
            ufoPositionX = 25.0f; // 画面右端から開始
            ufoPositionY = 7.0f;
            ufoPositionZ = -15.0f; // ロボットの後ろ
            ufoScale = 0.0f;
        }
        break;
    case 27: // ESCキーで終了
        exit(0);
        break;
    }
    glutPostRedisplay(); // キー入力で再描画を要求
}

// ==== メニュー ====
void getValueFromMenu(int value) {
    switch (value) {
    case 1: // UFOを飛ばす
        if (!ufoFlying) {
            ufoFlying = 1;
            ufoMovementPhase = 0; // 動きのフェーズを初期化
            ufoPositionX = 25.0f; // 画面右端から開始
            ufoPositionY = 7.0f;
            ufoPositionZ = -15.0f; // ロボットの後ろ
            ufoScale = 0.0f;
        }
        break;
    case 2: // 描画を終了する
        exit(0);
        break; \
    }
    glutPostRedisplay(); // メニュー選択後に再描画を要求
}

void mySetMenu() {
    glutCreateMenu(getValueFromMenu);
    glutAddMenuEntry("Call Baikinman", 1); // 項目1: UFOを飛ばす
    glutAddMenuEntry("Finish", 2); // 項目2: 描画を終了する
    glutAttachMenu(GLUT_RIGHT_BUTTON); // 右クリックでメニューを表示
}

// ==== マウス操作 ====
void myMouseMotion(int x, int y)
{
    int		xdis, ydis;
    double	a = 0.2;

    if (mouseFlag == GL_FALSE) return;
    xdis = x - xStart;
    ydis = y - yStart;
    xAngle += (double)ydis * a;
    yAngle += (double)xdis * a;

    if (xAngle > 89.0) xAngle = 89.0;
    if (xAngle < -89.0) xAngle = -89.0;

    xStart = x;
    yStart = y;
    glutPostRedisplay();
}

void myMouseFunc(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        xStart = x;
        yStart = y;
        mouseFlag = GL_TRUE;
    }
    else {
        mouseFlag = GL_FALSE;
    }
}

// ==== 初期化 ==== 
void myInit(char* progname)
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(progname);

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

// ==== ウィンドウサイズ変更時 ==== 
void myReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -5.0); // カメラを動かす
}

// ==== 表示関数 ==== 
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef(xAngle, 1.0, 0.0, 0.0);
    glRotatef(yAngle, 0.0, 1.0, 0.0);

    // カメラ位置設定 
    gluLookAt(
        0.0, 1.0, 15.0,// 正面から0,0,15、斜めから5,5,5 
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0
    );

    Light();

    drawDadandan();
    drawBackground();
    // UFOは飛行中のみ描画
    if (ufoFlying) {
        drawUFO();
    }
    glutSwapBuffers();
}

// ==== メイン関数 ==== 
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    myInit(argv[0]);
    setUpTexture();
    mySetMenu();
    glutKeyboardFunc(myKeyboard);
    glutMouseFunc(myMouseFunc);
    glutMotionFunc(myMouseMotion);
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutTimerFunc(16, animate, 0);
    glutMainLoop();
    return 0;
}

