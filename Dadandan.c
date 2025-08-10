#include <GL/glut.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <math.h> 
#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif 

// �r�Ƌr�̊p�x�i�A�j���[�V�����p�j 
float armAngle = 0.0f;
float bodyAngle = 0.0f;
float bodyPosition = 0.0f;
float rightThighLocalAngle = 0.0f;
float leftThighLocalAngle = 0.0f;
int armDirection = -1;
int legDirection = 1;
int armWaiting = 0; // 0:���쒆, 1:�グ�����đҋ@�� 
int legPhase = 0; // ���̃t�F�[�Y 
float rightLegAngle = 0.0f; // �E���̊p�x 
float leftLegAngle = 0.0f;  // �����̊p�x 
float rightThighAngle = 0.0f; // �E�����̊p�x 
float leftThighAngle = 0.0f;  // �������̊p�x 
int legWait = 0; // ���̓������~�߂�t���O 0:�����쒆, 1:���X�g�b�v�� 
float shadowScale = 1.3; // �e�̑傫��

// ���L�p�t�F�[�Y�Ǘ� 
enum { SQUAT_NONE, SQUAT_DOWN, SQUAT_UP };
int squatPhase = SQUAT_NONE;

// UFO�֘A�̕ϐ�
float ufoPositionX = 25.0f; // UFO��X���W�i�����ʒu�͉�ʉE�O�j
float ufoPositionY = 7.0f; // UFO��Y���W
float ufoPositionZ = -18.0f; // UFO��Z���W�i���{�b�g�̌��j
float ufoSpeedX = 0.7f;     // UFO��X�����ւ̑��x
int ufoFlying = 0;          // UFO����s�����ǂ��� (0:��~, 1:��s��)
int ufoMovementPhase = 0;   // UFO�̓��� 0:����, 1:�����E��, 2:�Ăэ���
float ufoScale = 0.0f; // UFO�̑傫��

// �e�N�X�`���}�b�s���O
#define	imageWidth 256
#define	imageHeight 256
unsigned char texImage[imageHeight][imageWidth][3];
GLuint textureID_shibafu;

// �}�E�X����
unsigned char mouseFlag = GL_FALSE;		// �����Ă��邩�ǂ���
int	xStart, yStart;				// �X�^�[�g�|�W�V����
double xAngle = 0.0, yAngle = 0.0;	// �A���O��

// �~�������֐�(�ڂ̂���) 
void drawCircle(float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); // ���S 
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        glVertex3f(x, y, 0.0f); // Z�������ʂɕ`�� 
    }
    glEnd();
}

// ==== ���{�b�g��`�� ==== 
void drawDadandan() {
    GLUquadric* robotQuad = gluNewQuadric();
    gluQuadricNormals(robotQuad, GLU_SMOOTH);

    // ==== �n�ʂɉe ====
    glPushMatrix();
    glColor3f(0.2, 0.2, 0.2); // ��
    glTranslatef(0.0, -1.0, 0.0);
    glTranslatef(0.0, 0.0, -3.0);
    glTranslatef(-2.0, 0.0, 0.0);
    glRotatef(90, 1, 0, 0);
    glScalef(shadowScale, 1.0, 1.0);
    gluDisk(robotQuad, 0.0, 2.0, 20, 1);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.2, 0.2, 0.2); // ��
    glTranslatef(0.0, -1.0, 0.0);
    glTranslatef(0.0, 0.0, -3.0);
    glTranslatef(-1.5, 0.0, 0.0);
    glRotatef(90, 1, 0, 0);
    glScalef(1.2, 1.0, 1.0);
    gluDisk(robotQuad, 0.0, 2.0, 20, 1);
    glPopMatrix();

    // === �� === 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);// �D�F 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glTranslatef(0.0, 2.4, 0.0);
    glScalef(1.1, 1.0, 1.0);   // ���ɒ����ȉ~�` 
    glutSolidSphere(1.0, 30, 30); // �� 
    glPopMatrix();

    // ���� 
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);// �� 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glRotatef(-90, 0, 1, 0);
    glTranslatef(0.0, 2.4, 0.0);
    gluCylinder(robotQuad, 1.025, 1.025, 0.02, 20, 20); // ���a1.025�A����0.02�̉~�� 
    glPopMatrix();

    // �l�W 
    glPushMatrix();
    glColor3f(0.1, 0.1, 0.1); // �l�W�F 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glTranslatef(0.05, 1.55, 1.0);

    float screwSpacing = 0.3; // �����m�̊Ԋu 
    float list[6] = { -0.4, -0.1, 0.0, 0.0, 0.0, -0.2 };
    for (int i = 0; i < 6; ++i) {
        glPushMatrix();
        glTranslatef(0.0, screwSpacing * i, list[i]); // �c�ɕ��ׂ� 
        glutSolidSphere(0.03, 16, 16);
        glPopMatrix();
    }
    glPopMatrix();

    // �� 
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// �� 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.0, 0.0, 1.9);
    gluCylinder(robotQuad, 1.005, 1.1, 0.35, 20, 20); //���a�͋��̂ɉ��킹��A����0.35�̉~�� 
    glPopMatrix();

    // ���� 
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);// �� 
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.0, 0.0, 2.1);
    gluCylinder(robotQuad, 1.075, 1.075, 0.02, 20, 20); // ���a1.075�A����0.02�̉~�� 
    glPopMatrix();

    // === �ځi���E�j ===
    glPushMatrix();
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);

    for (int side = -1; side <= 1; side += 2) {
        // �Ԃ���
        glPushMatrix();
        glColor3f(0.8, 0.0, 0.0); // ��
        glTranslatef(0.4f * side, 2.55f, 1.0f);
        drawCircle(0.2f, 40);
        glPopMatrix();

        // �����n�C���C�g
        glPushMatrix();
        glColor3f(1.0, 0.9, 0.9); // ��
        glTranslatef(0.39f * side, 2.54f, 1.0001f);
        drawCircle(0.13f, 40);
        glPopMatrix();

        // �������E
        glPushMatrix();
        glColor3f(0.0, 0.0, 0.0); // ��
        glTranslatef(0.36f * side, 2.54f, 1.0002f);
        drawCircle(0.07f, 40);
        glPopMatrix();
    }
    glPopMatrix(); // �ڂ̋��ʂ̕ϊ����I��


    // === ���́i�~���j === 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(0.0, bodyPosition, 0.0);
    glRotatef(bodyAngle, 0, 0, 1);
    glTranslatef(0.0, 0.65, 0.0);
    glRotatef(-90, 1, 0, 0); // Z�����ɗ��Ă� 
    gluCylinder(robotQuad, 0.4, 0.4, 1.5, 20, 20); // ���a0.4�A����1.5�̉~�� 

    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(0.0, 0.0, -0.2);
    gluCylinder(robotQuad, 0.2, 0.4, 0.2, 20, 20); // �㔼�a0.4�A�����a0.2�A����0.2�̉~�� 
    glPopMatrix();

    // === �r�i���E�j === 
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        // �� 
        glColor3f(0.45, 0.3, 0.75);
        glTranslatef(0.0, bodyPosition, 0.0);
        glRotatef(bodyAngle, 0, 0, 1);
        glTranslatef(0.35 * side, 1.35, 0.0);
        glRotatef(armAngle * side, 0, 0, 1);
        glutSolidSphere(0.2, 20, 20);

        // ��̘r 
        glPushMatrix();
        glColor3f(0.4, 0.4, 0.4);
        glRotatef(60 * side, 0, 0, 1);
        glTranslatef(0.1 * side, -0.45, 0.0);
        glScalef(0.15, 0.7, 0.15);
        glutSolidCube(1.0);
        glPopMatrix();

        // �I 
        glColor3f(0.45, 0.3, 0.75);
        glRotatef(30 * side, 0, 0, 1);
        glTranslatef(0.45 * side, -0.55, 0.0);
        glutSolidSphere(0.125, 20, 20);

        // �O�r 
        glPushMatrix();
        glColor3f(0.4, 0.4, 0.4);
        glRotatef(-80 * side, 0, 0, 1);
        glTranslatef(-0.025 * side, 0.35, 0.0);
        glScalef(0.15, 0.7, 0.15);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        // ��o���h 
        glColor3f(0.45, 0.3, 0.75);// �� 
        glRotatef(10 * side, 0, 0, 1);
        glTranslatef(0.6 * side, 0.025, 0.0);
        glRotatef(90 * side, 0, 1, 0);
        gluCylinder(robotQuad, 0.1, 0.1, 0.1, 20, 20); // ���a0.1�A����0.1�̉~�� 
        glPopMatrix();

        // ��̂Ђ�i����ɑO�r�̖��[�Ɂj 
        glPushMatrix();
        glRotatef(-40 * side, 0, 0, 1);
        glTranslatef(0.6 * side, 0.7, 0.0);
        glRotatef(90, 0, 0, 1);
        glScalef(1.7, 1.7, 1.7);

        int segments = 40;
        float innerR = 0.07;
        float outerR = 0.15;
        float startAngle = 30.0; // �J��������邽�߃J�b�g 
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
            glVertex3f(xOuter, yOuter, 0.05); // ��� 
            glVertex3f(xInner, yInner, 0.05);
            glVertex3f(xOuter, yOuter, -0.05); // ��� 
            glVertex3f(xInner, yInner, -0.05);
        }
        glEnd();

        glPopMatrix();
        glPopMatrix();
    }


    // === �E�� === 
    glPushMatrix();
    glRotatef(rightThighAngle, 0, 0, 1); // �Ҋ֐߂̉�] 

    // ������ 
    glColor3f(0.4, 0.4, 0.4);
    glTranslatef(0.5, 0, 0);      // 1. ���_����]�̒��S�܂ňړ� 
    glRotatef(rightThighLocalAngle, 0, 0, -1); // 2. ��] 
    glTranslatef(-0.5, 0, 0);   // 3. ���̍��W�n�ɖ߂� 
    glTranslatef(0.15, 0.65, 0.0); // �ʒu�ړ� 
    glRotatef(-270, 1, 0, 0);
    glRotatef(55, 0, 1, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 0.8, 20, 20); // ������ 
    glTranslatef(0.0, 2.0, 0.0); // �ʒu�ړ� 
    glPopMatrix();

    glPushMatrix();// �ӂ���͂���艺�𓮂��� 
    glTranslatef(0, rightLegAngle, 0); // �ӂ���͂����㉺ 

    glTranslatef(0.65, -0.5, 0.0); // �r�̈ʒu 
    glTranslatef(0.0, -0.875, 0.0);

    // �ӂ���͂� 
    glTranslatef(0.09, 0.7, 0.0); // �ʒu�ړ� 
    glRotatef(-90, 1, 0, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 1.0, 20, 20); // ���a0.15�A����1.0�̉~�� 
    glTranslatef(0.0, 0.0, 1.0); // �r�̉��[�ֈړ� 

    // ���̍b 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5); // �D�F 
    glTranslatef(0.0, 0.0, -1.0); // �r�̉��ɔz�u 
    gluCylinder(robotQuad, 0.5, 0.5, 0.2, 20, 20);

    glPushMatrix();// ��� 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1); // ���a0.5�̉~��`�� 
    glPopMatrix();

    glPushMatrix(); // ��� 
    glTranslatef(0.0, 0.0, 0.2); // �~���̍����ɍ��킹�Ĉړ� 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1);
    glPopMatrix();
    glPopMatrix();

    // ���o���h 
    glColor3f(0.45, 0.3, 0.75);// �� 
    glTranslatef(0.0, 0.0, -0.6); // �r�̉��[�ֈړ� 
    gluCylinder(robotQuad, 0.151, 0.151, 0.2, 20, 20); // ���a0.151�A����0.2�̉~�� 
    glPopMatrix();

    // === ���� ===  
    glPushMatrix();
    glRotatef(leftThighAngle, 0, 0, 1); // �Ҋ֐߂̉�] 
    glTranslatef(-0.5, 0, 0);      // 1. ���_����]�̒��S�܂ňړ� 
    glRotatef(leftThighLocalAngle, 0, 0, 1); // 2. ��] 
    glTranslatef(0.5, 0, 0);   // 3. ���̍��W�n�ɖ߂� 

    // ������ 
    glColor3f(0.4, 0.4, 0.4);
    glTranslatef(-0.15, 0.65, 0.0); // �ʒu�ړ� 
    glRotatef(-270, 1, 0, 0);
    glRotatef(-55, 0, 1, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 0.8, 20, 20); // ������ 
    glTranslatef(0.0, 2.0, 0.0); // �ʒu�ړ� 
    glPopMatrix();

    glPushMatrix();// �ӂ���͂���艺�𓮂��� 
    glTranslatef(0, leftLegAngle, 0); // �ӂ���͂����㉺ 

    glTranslatef(-0.65, -0.5, 0.0); // �r�̈ʒu 
    glTranslatef(0.0, -0.875, 0.0);

    // �ӂ���͂� 
    glTranslatef(-0.09, 0.7, 0.0); // �ʒu�ړ� 
    glRotatef(-90, 1, 0, 0);
    gluCylinder(robotQuad, 0.15, 0.15, 1.0, 20, 20); // ���a0.15�A����1.0�̉~�� 
    glTranslatef(0.0, 0.0, 1.0); // �r�̉��[�ֈړ� 

    // ���̍b 
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5); // �D�F 
    glTranslatef(0.0, 0.0, -1.0); // �r�̉��ɔz�u 
    gluCylinder(robotQuad, 0.5, 0.5, 0.2, 20, 20);

    glPushMatrix(); // ��� 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1); // ���a0.5�̉~��`�� 
    glPopMatrix();

    glPushMatrix(); // ��� 
    glTranslatef(0.0, 0.0, 0.2); // �~���̍����ɍ��킹�Ĉړ� 
    gluDisk(robotQuad, 0.0, 0.5, 20, 1);
    glPopMatrix();
    glPopMatrix();

    // ���o���h 
    glColor3f(0.45, 0.3, 0.75);// �� 
    glTranslatef(0.0, 0.0, -0.6); // �r�̉��[�ֈړ� 
    gluCylinder(robotQuad, 0.151, 0.151, 0.2, 20, 20); // ���a0.151�A����0.2�̉~�� 
    glPopMatrix();
}

// ==== UFO��`�� ====
void drawUFO() {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    glPushMatrix();
    // ==== �n�ʂɉe ====
    glColor3f(0.2, 0.2, 0.2); // ��
    glScalef(ufoScale * 1.5, ufoScale, ufoScale);
    glTranslatef(ufoPositionX- ufoPositionY, -1.0, ufoPositionZ);
    glRotatef(90, 1, 0, 0);
    gluDisk(quad, 0.0, 1.0, 20, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(ufoPositionX, ufoPositionY, ufoPositionZ); // UFO�S�̂̈ʒu
    glScalef(ufoScale, ufoScale, ufoScale);

    // �~��
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// ��
    glTranslatef(0.0f, -0.6f, 0.0f);
    glRotatef(-90, 1, 0, 0); // Z�����ɗ��Ă� 
    gluCylinder(quad, 1.6, 0.9, 0.3, 20, 20); // �㔼�a0.4�A����0.5�̉~�� 
    glPopMatrix();

    // �h�[���i�����j
    glPushMatrix();
    glColor3f(0.5f, 0.7f, 1.0f); // ���F
    glScalef(0.9f, 1.0f, 0.9f); // ���������ɏ����L�΂�
    gluSphere(quad, 1.0, 30, 30); // ���a1.0�̋��Ńh�[�����쐬
    glPopMatrix();

    // �~��
    glPushMatrix();
    glColor3f(0.6, 0.3, 0.58);// �Ԏ�
    glTranslatef(0.0f, -1.1f, 0.0f);
    glRotatef(-90, 1, 0, 0); // Z�����ɗ��Ă� 
    gluCylinder(quad, 0.85, 0.85, 0.75, 20, 20); // ���a0.9�A����0.8�̉~�� 
    glPopMatrix();

    // �~����
    glPushMatrix();
    glColor3f(0.6, 0.3, 0.58);// �Ԏ�
    glTranslatef(0.0f, -1.0f, 0.0f);
    glScalef(0.95f, 0.2f, 0.95f);
    gluSphere(quad, 1.0, 30, 30); // ���a1.0�̋���ׂ��ĉ~�Ղɂ���
    glPopMatrix();


    // �A���e�i
    // 1�{�ڂ̃A���e�i
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// ��
    glRotatef(10, 0, 0, 1);
    glTranslatef(-0.4, 0.85, 0.0f); // �h�[����Y�̒��_��X�����ɂ��炵�Ĕz�u
    glRotatef(-90, 1, 0, 0); // Z������Ɍ�������AY�������ɗ��Ă�
    gluCylinder(quad, 0.1, 0.05, 0.6, 10, 10); // �~��
    // �A���e�i�̐�[�̋�
    glTranslatef(0.0f, 0.0f, 0.5); // �~���̐�[�Ɉړ�
    glutSolidSphere(0.15, 10, 10); // �����傫�߂̋�
    glPopMatrix();

    // 2�{�ڂ̃A���e�i
    glPushMatrix();
    glColor3f(0.45, 0.3, 0.75);// ��
    glRotatef(-10, 0, 0, 1);
    glTranslatef(0.4, 0.85, 0.0f); // �h�[����Y�̒��_��X�����ɂ��炵�Ĕz�u
    glRotatef(-90, 1, 0, 0); // Z������Ɍ�������AY�������ɗ��Ă�
    gluCylinder(quad, 0.1, 0.05, 0.6, 10, 10); // �~��
    // �A���e�i�̐�[�̋�
    glTranslatef(0.0f, 0.0f, 0.5); // �~���̐�[�Ɉړ�
    glutSolidSphere(0.15, 10, 10); // �����傫�߂̋�
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
    // === shibafu.ppm �̐ݒ� ===
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



// ==== �w�i ==== 
// �R 
void drawMountains(float radius, float baseY, int count) {
    int count2 = 0;
    for (int i = 0; i < count; ++i) {
        float theta = (2.0f * M_PI * i) / count; // 0�`2�΂ŕ��z 
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        float width = 10.0f;   // �R�̉��� 
        float height = 0.0f;  // �R�̍��� 

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
        
        // �R�̌������~���̒��S�Ɍ����� 
        float dirX = -cos(theta);
        float dirZ = -sin(theta);

        // �O�p�`�̒�ӗ��[ 
        float bx1 = x + (width / 2) * sin(theta);
        float bz1 = z - (width / 2) * cos(theta);
        float bx2 = x - (width / 2) * sin(theta);
        float bz2 = z + (width / 2) * cos(theta);

        // ���_�͒�ӂ̒������璆�S������height������ 
        float tx = x + dirX * height * 0.2f; // ���������� 
        float ty = baseY + height;
        float tz = z + dirZ * height * 0.2f;

        glColor3f(0.42, 0.46, 0.82); // �R�F 
        glBegin(GL_TRIANGLES);
        glVertex3f(bx1, baseY, bz1); // ��Ӎ� 
        glVertex3f(bx2, baseY, bz2); // ��ӉE 
        glVertex3f(tx, ty, tz);      // ���_ 
        glEnd();
    }
}

// �_ 
void drawClouds(float radius, float baseY, int count) {
    for (int i = 0; i < count; ++i) {
        float theta = (2.0f * M_PI * i) / count + (i % 2) * 0.4f;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        glPushMatrix();
        glTranslatef(x, baseY, z);
        // �_�����S�����ɉ�] 
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
    glDisable(GL_LIGHTING); // ���C�e�B���O���ꎞ������ 

    // �n�ʁi�e�N�X�`���}�b�s���O����j
    glEnable(GL_TEXTURE_2D); // 2D�e�N�X�`���}�b�s���O��L���ɂ���
    glBindTexture(GL_TEXTURE_2D, textureID_shibafu);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    // �e���_�Ƀe�N�X�`�����W�����蓖�Ă�
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, -1.01f, -30.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, -1.01f, -30.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, -1.01f, 30.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, -1.01f, 30.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D); // 2D�e�N�X�`���}�b�s���O�𖳌��ɂ���

    // �傫�ȉ~���i��j 
    GLUquadric* wall = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, -10.0f, 0.0f);
    glRotatef(-90, 1, 0, 0); // Z��������Y�������� 
    glColor3f(0.77, 0.95, 1.0); // �������F 
    gluCylinder(wall, 30.0, 30.0, 50.0, 64, 1); // ���a20, ����16 
    glPopMatrix();
    gluDeleteQuadric(wall);

    //�V��i��j 
    glPushMatrix();
    glColor3f(0.77, 0.95, 1.0); // �������F 
    glTranslatef(0.0f, 16.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    gluDisk(wall, 0.0, 30.0, 64, 1);
    glPopMatrix();

    // �w�i�̕`�悪�I�������A���C�e�B���O���ēx�L���ɂ��� 
    glEnable(GL_LIGHTING);

    // �R�Ɖ_�̓��C�e�B���O�L���� 
    float radius = 30.0f;
    drawMountains(radius - 1.5f, -1.01f, 20);
    drawClouds(radius - 0.5f, 7.0f, 19);
}

// ==== ���� ==== 
void Light() {
    // ���z�̂悤�ȕ��s����
    float light0_position[] = { 1.0, 1.5, 0.5, 0.0 };
    float light0_ambient[] = { 0.5, 0.5, 0.45, 1.0 };
    float light0_diffuse[] = { 1.0, 1.0, 0.9, 1.0 };
    float light0_specular[] = { 0.8, 0.8, 0.8, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    glEnable(GL_LIGHT0);

    // �O���[�o���Ȋ�����ݒ�  
    GLfloat global_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

// ==== �^�C�}�[�R�[���o�b�N ==== 
void armDownStart(int v) {
    armDirection = -1;
    armWaiting = 0;
}
void resumeLeg(int v) {
    legWait = 0;
}

// ==== �A�j���[�V���� ==== 
void animate(int value) {
    // == �r�̓��� == 
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

    // == �̂̋��L�^�� == 
    const float thighSquatMax = 8.0f;
    const float squatMin = -0.15f;
    const float squatSpeed = 0.025f;

    if (squatPhase == SQUAT_DOWN) { // �㔼�g�������� 
        bodyPosition -= squatSpeed;
        rightThighLocalAngle -= 0.25f;
        leftThighLocalAngle -= 0.25f;
        if (bodyPosition <= squatMin) {
            bodyPosition = squatMin;
            squatPhase = SQUAT_UP;
        }
    }
    else if (squatPhase == SQUAT_UP) { // �㔼�g���グ�� 
        bodyPosition += squatSpeed;
        rightThighLocalAngle += 0.25f;
        leftThighLocalAngle += 0.25f;;
        if (bodyPosition >= 0.0f) {
            bodyPosition = 0.0f;
            squatPhase = SQUAT_NONE;
        }
    }
    if (squatPhase == SQUAT_DOWN || squatPhase == SQUAT_UP) {
        // ���L�̐[���ɉ����đ������̊p�x��ω� 
        float t = (bodyPosition - 0.0f) / (squatMin - 0.0f); // t: 0�i��j�` 1�i���j 
        rightThighLocalAngle = thighSquatMax * -t;
        leftThighLocalAngle = thighSquatMax * -t;
    }
    // ���L���I�������0�ɖ߂� 
    if (squatPhase == SQUAT_NONE) {
        rightThighLocalAngle = 0.0f;
        leftThighLocalAngle = 0.0f;
    }

    // == ���̓��� == 
    const float liftMax = 0.2f;
    const float UpSpeed = 0.005f;
    const float DownSpeed = 0.075f;
    const float thighLiftMax = 7.0f;
    const float thighUpSpeed = 0.3f;
    const float thighDownSpeed = 3.0f;
    const float bodyMax = 8.0f;

    if (!legWait) {
        switch (legPhase) {
        case 0: // ���������E���グ�J�n 
            squatPhase = SQUAT_DOWN; // ���L�X�^�[�g 
            rightLegAngle = 0.0f;
            leftLegAngle = 0.0f;
            rightThighAngle = 0.0f;
            leftThighAngle = 0.0f;
            legPhase = 1;
            legWait = 1;
            glutTimerFunc(200, resumeLeg, 0); // 0.2�b�ҋ@ 
            break;

        case 1: // �E���グ��{�E������ 
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

        case 2: // �E��������{�E������ 
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

        case 3: // �������������グ�J�n 
            squatPhase = SQUAT_DOWN; // ���L�X�^�[�g 
            rightLegAngle = 0.0f;
            leftLegAngle = 0.0f;
            rightThighAngle = 0.0f;
            leftThighAngle = 0.0f;
            legPhase = 4;
            legWait = 1;
            glutTimerFunc(200, resumeLeg, 0); // 0.2�b�ҋ@ 
            break;

        case 4: // �����グ��{�������� 
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

        case 5: // ����������{�������� 
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

    // == UFO�̓��� ==
    if (ufoFlying) {
        switch (ufoMovementPhase) {
        case 0: // X�����������畉�����ֈړ�
            ufoPositionX -= ufoSpeedX;
            ufoPositionY -= 0.005f;
            ufoScale += 0.005f;
            if (ufoPositionX <= -6.0f) { // ���{�b�g�̍����܂ŗ�����t�F�[�Y1��
                ufoMovementPhase = 1;
            }
            break;
        case 1: // ��������X���������֖߂�
            ufoPositionX += ufoSpeedX * 0.3f; // �߂鑬�x�͒x�߂�
            ufoPositionY -= 0.08f;
            ufoPositionZ += 0.2f;
            ufoScale += 0.02f;
            if (ufoPositionX >= 0.0f) { // �����߂�����t�F�[�Y2��
                ufoMovementPhase = 2;
            }
            break;
        case 2: // �Ă�X���������ֈړ����A��ʊO��
            ufoPositionX -= ufoSpeedX;
            ufoPositionY -= 0.005f;
            ufoScale -= 0.01f;
            if (ufoPositionX <= -25.0f) { // ��ʊO�ɏo�����s��~
                ufoFlying = 0;
                ufoMovementPhase = 0; // ����̂��߂Ƀt�F�[�Y�����Z�b�g
                ufoPositionX = 25.0f; // ����̂��߂ɏ����ʒu�����Z�b�g
                ufoPositionZ = -18.0f; // Z���W�����Z�b�g
            }
            break;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, animate, 0);
}

// ==== �L�[�{�[�h���͊֐� ====
void myKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'u': // 'u' �L�[����������UFO���s�J�n
    case 'U':
        if (!ufoFlying) {
            ufoFlying = 1;
            ufoMovementPhase = 0; // �����̃t�F�[�Y��������
            ufoPositionX = 25.0f; // ��ʉE�[����J�n
            ufoPositionY = 7.0f;
            ufoPositionZ = -15.0f; // ���{�b�g�̌��
            ufoScale = 0.0f;
        }
        break;
    case 27: // ESC�L�[�ŏI��
        exit(0);
        break;
    }
    glutPostRedisplay(); // �L�[���͂ōĕ`���v��
}

// ==== ���j���[ ====
void getValueFromMenu(int value) {
    switch (value) {
    case 1: // UFO���΂�
        if (!ufoFlying) {
            ufoFlying = 1;
            ufoMovementPhase = 0; // �����̃t�F�[�Y��������
            ufoPositionX = 25.0f; // ��ʉE�[����J�n
            ufoPositionY = 7.0f;
            ufoPositionZ = -15.0f; // ���{�b�g�̌��
            ufoScale = 0.0f;
        }
        break;
    case 2: // �`����I������
        exit(0);
        break; \
    }
    glutPostRedisplay(); // ���j���[�I����ɍĕ`���v��
}

void mySetMenu() {
    glutCreateMenu(getValueFromMenu);
    glutAddMenuEntry("Call Baikinman", 1); // ����1: UFO���΂�
    glutAddMenuEntry("Finish", 2); // ����2: �`����I������
    glutAttachMenu(GLUT_RIGHT_BUTTON); // �E�N���b�N�Ń��j���[��\��
}

// ==== �}�E�X���� ====
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

// ==== ������ ==== 
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

// ==== �E�B���h�E�T�C�Y�ύX�� ==== 
void myReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -5.0); // �J�����𓮂���
}

// ==== �\���֐� ==== 
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef(xAngle, 1.0, 0.0, 0.0);
    glRotatef(yAngle, 0.0, 1.0, 0.0);

    // �J�����ʒu�ݒ� 
    gluLookAt(
        0.0, 1.0, 15.0,// ���ʂ���0,0,15�A�΂߂���5,5,5 
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0
    );

    Light();

    drawDadandan();
    drawBackground();
    // UFO�͔�s���̂ݕ`��
    if (ufoFlying) {
        drawUFO();
    }
    glutSwapBuffers();
}

// ==== ���C���֐� ==== 
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

