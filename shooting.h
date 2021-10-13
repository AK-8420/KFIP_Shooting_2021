#pragma once
/*�ȉ��ɒ萔�̒�`*/
#define MAX_X 640//�Q�[����ʂ̍ő�x
#define MAX_Y 480//�Q�[����ʂ̍ő�y
#define MIN_X 0//�Q�[����ʂ̍ŏ�x
#define MIN_Y 0//�Q�[����ʂ̍ŏ�y
#define CX (MAX_X + MIN_X)/2 //���S��x
#define CY (MAX_Y + MIN_Y)/2 //���S��y

#define PUSH 1//�L�[���������u��
#define PULL -1//�L�[�𗣂����u��
#define NO_CHANGE 0//����ȊO�̏�ԁi�L�[�ω��Ȃ�)

namespace COLOR {
	static const int white = GetColor(255, 255, 255);
	static const int red = GetColor(255, 0, 0);
	static const int green = GetColor(0, 255, 0);
	static const int blue = GetColor(0, 0, 255);
	static const int gray = GetColor(128, 128, 128);
}

/*�ȉ��ɃN���X�̒�`*/
class KeyInput {//�L�[�{�[�h����
private:
	char preKeys[256];
public:
	char nowKeys[256];

	KeyInput() {//�R���X�g���N�^
		GetHitKeyStateAll(nowKeys);//���݂̃L�[�̉�����Ԃ��擾
	}
	void update() {
		memcpy(preKeys, nowKeys, sizeof(nowKeys));//�O�̃L�[�̉�����Ԃ��X�V
		GetHitKeyStateAll(nowKeys);//���݂̃L�[�̉�����Ԃ��擾
	}
	int getState(int where) {
		return nowKeys[where] - preKeys[where];//�p���X�̌��o���������u�ԂȂ�1,�������u�ԂȂ�-1���Ԃ�
	}
};

//�摜
class Graphic {
private:
	TCHAR* filepath;//�摜�̕ۑ��ꏊ
	int numSliceX, numSliceY;//�摜�̕�����
	int width, height;	//�摜�̏c���T�C�Y
	int* handle;//�摜�n���h��
public:
	int size_x, size_y;//������̉摜�T�C�Y(��1/2)
	double exRate;//�摜�̊g�嗦

	Graphic(const char* FileName, int Width, int Height, int numX, int numY, double ExRate = 1);
	void draw(int x, int y, int index = 0, double angle = 0);
	void setExRate(double exRate = 1);
};

//�e�̃f�[�^
typedef struct BulletData {
	int x, y;//���W
	double angle;//���ݐi�s�p�x
	int r; //���a
	int color;//�F
	int ptn;//�����p�^�[��ID
	struct BulletData* before;//�O�̃f�[�^�ւ̃|�C���^
	struct BulletData* next;//���̃f�[�^�ւ̃|�C���^
}Bullet;

//�e�̐����E����
class BulletList {
private:
	Bullet* head = NULL; //���X�g�̐擪���w���|�C���^
	Bullet* tail = NULL; //���X�g�̖������w���|�C���^

public:
	void draw();//�`��
	void calc(int count);//�v�Z (�����p�^�[���ŏꍇ����)
	void add(Bullet b);//�e�̒ǉ�(���X�g�̖����ɒǉ�)
	Bullet* del(Bullet* target);//�e�̍폜 (�ߒl�F�����w���|�C���^)
	void delAll();//���X�g�S�̂�����

	Bullet* getHead() { return head; }
	Bullet* getTail() { return tail; }
};


//�L�����N�^�[
class Character {
public:
	int x, y;//���W
	double v;//�ړ����x
	Graphic* graph;//�摜�f�[�^
	int index;	//�摜�n���h���̃C���f�b�N�X
	double angle = 0; //�摜�̉�]�p�x(rad)
	int HP, maxHP;//maxHP��HP�̍ő�l

	//�R���X�g���N�^
	Character(int X, int Y, int V, Graphic* Image, int firstIndex = 0, int hp = 1, int maxhp = 1);
	//�`��
	void draw();
};

//�v���C���[
class Player : public Character {
public:
	using Character::Character;//Character�N���X�̃R���X�g���N�^���Ă�

	//�X�V
	void update(KeyInput keys);
	//�`��
	void draw(KeyInput keys, int count);
	// ���e�̒ǉ�
	void createShot(KeyInput keys, BulletList* BList_p, int count, int interval = 5);
};

//�G
class Enemy : public Character {
private:
	int index_min; //�摜�̃C���f�b�N�X�͈͂̍ŏ��l
	int frameSum;  //�A�j���[�V�����̑��R�}��
public:
	int moveID;//�ړ��p�^�[��ID
	//�����ɍU���p�^�[�������i�������j

	Enemy(int X, int Y, int V, Graphic* Image, int Index_min = 0, int FrameSum = 1, int hp = 1, int maxhp = 1, int moveId = 0) :Character(X, Y, V, Image, Index_min, hp, maxhp) {
		index_min = Index_min;
		frameSum = FrameSum;
		moveID = moveId;
	}
	void update();//�X�V
	void draw(int count);//�`��
};

//�G���X�g�̃m�[�h
typedef struct EnemyNode{
	Enemy data;
	struct EnemyNode* before;
	struct EnemyNode* next;
}ENode;
//�G���X�g
class EnemyList {
private:
	ENode* head = NULL; //���X�g�̐擪���w���|�C���^
	ENode* tail = NULL; //���X�g�̖������w���|�C���^
public:
	void draw(int count);//�`��
	void update(int count);//�X�V
	void add(Enemy b);//���X�g�̖����ɒǉ�
	ENode* del(ENode* target);//�폜 (�ߒl�F�����w���|�C���^)
	void delAll();//���X�g�S�̂�����
	void collision_with_PlayerShot(BulletList* b);//�v���C���[�̒e�Ƃ̏Փˏ���
};


/*�ȉ��Ɋ֐��̃v���g�^�C�v�錾������*/
int Btwn(int p1, int p, int p2);// p1����p2�͈̔͂�p������Ƃ�1��Ԃ�

template<typename T>
void pushBack(T** node, T** head, T** tail);//head->...->tail�̖�����node��ǉ�

template<typename T>
T* deleteNode(T** node, T** head, T** tail);//node��head-tail�̃��X�g�������(node�̎��̃m�[�h��Ԃ�)

template<typename T>
void deleteAllNode(T** head, T** tail);//head-tail�̃��X�g�̃m�[�h�����ׂď���

template<typename T>
int isInSquare(T chara, int min_x, int min_y, int max_x, int max_y);//�L�����N�^�[���l�p�`�͈̔͂̒��ɂ��邩�`�F�b�N