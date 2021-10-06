//�ҏW�F�c��c��, ���Ԏ闢
#include "DxLib.h"
#include "Shooting.h"
#include <math.h>
#define PI acos(-1)

/*���C���֐�*/
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(TRUE); // �t���X�N���[������E�B���h�E���[�h�ɕύX
	if (DxLib_Init() == -1)		// �c�w���C�u��������������
		return -1;			// �G���[���N�����璼���ɏI��
	SetDrawScreen(DX_SCREEN_BACK);//�`���𗠉�ʂɐݒ肷��

	/*�����ɏ���������������*/
	int count = 0;
	KeyInput keys;//�L�[���͌��ʂ��i�[����ꏊ

	Graphic* G_player = new Graphic("player.png", 69, 120, 3, 3);
	Graphic* G_fairy = new Graphic("fairy.png", 168, 150, 2, 2);
	Player p(CX, CY + 100, 4, G_player, 3);
	Enemy fairy1(CX - 100, CY - 100, 4, G_fairy, 0, 2);
	Enemy fairy2(CX + 100, CY - 100, 4, G_fairy, 2, 2, 20, 20);
	EnemyList Enemys;//�G���X�g
	Enemys.add(fairy1);//�d���̒ǉ�
	Enemys.add(fairy2);//�ԗd���̒ǉ�

	BulletList BList_p;//�v���C���[�̒e���X�g

    /*���C�����[�v*/
	while (1) {
		if (ProcessMessage() != 0) {//���b�Z�[�W����
			break;//�~�{�^���Ń��[�v�I��
		}
		ClearDrawScreen();//��ʂ�����
		clsDx();//printfDx�ŏo�͂�������������[�v���Ƃɏ���
		keys.update();//�L�[�̉�����Ԃ��擾

		/*�ȉ��ɕ`�揈��������*/
		p.draw(keys, count);
		BList_p.draw();
		Enemys.draw(count);


		/*�ȉ��ɍX�V����������*/
		p.update(keys);
		p.createShot(keys, &BList_p, count);
		BList_p.calc(count);
		Enemys.collision_with_PlayerShot(&BList_p);


		ScreenFlip();//����ʂ�\��ʂɔ��f
		count++;
	}

	BList_p.delAll();//�v���C���[�̒e�����ׂď���
	DxLib_End(); // DX���C�u�����I������
	return 0;
}

/*�ȉ��Ɋ֐��̒�`������*/
Graphic::Graphic(const char* FileName, int Width, int Height, int numX, int numY, double ExRate) {
	filepath = (TCHAR*)malloc(sizeof("graphic/") + sizeof(FileName));
	strcpyDx(filepath, "graphic/");
	strcatDx(filepath, FileName);
	width = Width; height = Height; numSliceX = numX; numSliceY = numY;
	setExRate(ExRate);

	//�T�C�Y���̉摜�n���h�����m��
	int num = numSliceX * numSliceY;
	handle = (int*)malloc(sizeof(int) * (num + 1));
	if (num == 1)
		handle[0] = LoadGraph(filepath);
	else
		LoadDivGraph(filepath, num, numSliceX, numSliceY, width / numSliceX, height / numSliceY, handle);
}
void Graphic::draw(int x, int y, int index, double angle) {
	DrawRotaGraph(x, y, exRate, angle, handle[index], 1);//�������ɓǂ݂��񂾃O���t�B�b�N�̕`��(��]����)
}
void Graphic::setExRate(double ExRate) {
	exRate = ExRate;
	size_x = (width / numSliceX) / 2 * exRate;
	size_y = (height / numSliceY) / 2 * exRate;
}

Character::Character(int X, int Y, int V, Graphic* Image, int firstIndex, int hp, int maxhp) {
	x = X;
	y = Y;
	v = V;
	graph = Image;
	index = firstIndex;
	HP = hp;
	maxHP = maxhp;
}
void Character::draw() {
	graph->draw(x, y, index, angle);
}

void Player::update(KeyInput keys) {
	if (keys.nowKeys[KEY_INPUT_UP] == PUSH)
		y -= v;
	if (keys.nowKeys[KEY_INPUT_DOWN] == PUSH)
		y += v;
	if (keys.nowKeys[KEY_INPUT_LEFT] == PUSH)
		x -= v;
	if (keys.nowKeys[KEY_INPUT_RIGHT] == PUSH)
		x += v;

	if (y > MAX_Y - graph->size_y)//���[���z���Ă���
		y = MAX_Y - graph->size_y;
	else if (y < MIN_Y + graph->size_y)//��[���z���Ă���
		y = MIN_Y + graph->size_y;
	if (x > MAX_X - graph->size_x)//�E�[���z���Ă���
		x = MAX_X - graph->size_x;
	else if (x < MIN_X + graph->size_x)//���[���z���Ă���
		x = MIN_X + graph->size_x;
}
void Player::draw(KeyInput keys, int count) {
	int frame = (count / 5) % 3; // 5count���ƂɃR�}���ω�(�S����3�R�})

	index = 3;//���E�L�[�������Ă��Ȃ��Ƃ�
	if (keys.nowKeys[KEY_INPUT_LEFT] == PUSH)
		index = 6;
	if (keys.nowKeys[KEY_INPUT_RIGHT] == PUSH)
		index = 0;
	index += frame;

	graph->draw(x, y, index, angle);
}

void Player::createShot(KeyInput keys, BulletList* BList_p, int count, int interval) {
	static int sCount = 0;//���˃^�C�~���O�̊
	int isPush = keys.getState(KEY_INPUT_SPACE) == PUSH;//SPACE�L�[����������
	int isPull = keys.getState(KEY_INPUT_SPACE) == PULL;//SPACE�L�[�𗣂�����
	int isEnoughInterval = sCount + interval <= count;//�A�ł��Ă��Ȃ���
	if (isPull)sCount = count;//���˃^�C�~���O�̊���X�V
	if (isPush && isEnoughInterval) sCount = count;//���˃^�C�~���O�̊���X�V
	if (!keys.nowKeys[KEY_INPUT_SPACE]) return;//SPACE�L�[�������Ă��Ȃ�������NG
	if (!((count - sCount) % interval == 0)) return;//�C���^�[�o������������NG
	//�ȉ��̓V���b�g�̌���Ɛ���
	Bullet b1 = { x - 10, y, 270, 3 ,COLOR::white, 0 };//�e�f�[�^��p��
	Bullet b2 = { x + 10, y, 270, 3 ,COLOR::white, 0 };//�e�f�[�^��p��
	BList_p->add(b1);//�e��ǉ�
	BList_p->add(b2);//�e��ǉ�
}

void Enemy::update() {
	//�܂������
	x += v;//x�����̈ړ�
	y += v;//y�����̈ړ�

	if (HP > maxHP) {//����HP���ő�HP�𒴂��Ă����璼��
		HP = maxHP;//�i�߂�
	}
	/*
	if (hp > 0 && isInWall(itr->e.pos.x, itr->e.pos.y, 100)) {//�����Ă��邩�͈͓��ɂ���ꍇ
		itr = itr->next;//�i�߂�
	}
	else {//hp��0�����܂��͉�ʂ͈̔͊O�ɂ���Ƃ�
		itr = delEnemy(itr);//�Y���̓G��Elist�������
	}
	*/
}
void Enemy::draw(int count) {
	int frame = (count / 10) % frameSum; // 10count���ƂɃR�}���ω�

	index = index_min + frame;
	graph->draw(x, y, index, angle);
}


int Btwn(int p1, int p, int p2) {
	if (p2 > p1 && (p1 <= p) && (p <= p2))
		return 1;
	if (p2 < p1 && (p2 <= p) && (p <= p1))
		return 1;
	return 0;
}
template<typename T>
void pushBack(T** node, T** head, T** tail) {
	(*node)->next = NULL;//�ǉ��m�[�h�̎���NULL

	if (*head == NULL) //��̃��X�g�̏ꍇ
		*head = *node;//�w�b�h�͒ǉ��m�[�h���w��
	if (*tail)//�����̃m�[�h�����ɂ���Ƃ�
		(*tail)->next = *node;//�O�m�[�h�̐��ǉ��m�[�h��
	(*node)->before = *tail;//�ǉ��m�[�h�̑O��O�m�[�h��
	*tail = *node;//Tail�͍Ō�̃m�[�h���w��
}
template<typename T>
T* deleteNode(T** node, T** head, T** tail) {
	T* pReturn = (*node)->next;//���̃m�[�h���w���|�C���^�̗p��
	if ((*node)->before)//�m�[�h���擪�łȂ��Ƃ�
		(*node)->before->next = pReturn;//��O�̎������̃m�[�h�ɂ���
	else
		*head = pReturn;//�w�b�h�̎w���m�[�h�����ɂ���

	if (pReturn)//p���������w���Ȃ��Ƃ�
		pReturn->before = (*node)->before;//���̃m�[�h����2�O�̃m�[�h���Ȃ�
	else//p���������w���Ƃ�
		*tail = (*node)->before;//�e�[���͑O�̃m�[�h���w��

	free(*node);//�m�[�h������
	return pReturn;//���̃|�C���^��Ԃ�

}
template<typename T>
void deleteAllNode(T** head, T** tail) {
	T* p, * pnext;//�|�C���^��2�p��
	pnext = *head;//�w�b�h�̃m�[�h���w��
	while (pnext) {//�m�[�h���Ȃ��Ȃ�܂ŌJ��Ԃ�
		p = pnext;//p��i�߂�
		pnext = p->next;//pnext��i�߂�
		free(p);//p�̃m�[�h���J��
	}
	*head = NULL;//�k��
	*tail = NULL;//�k��
}

void BulletList::draw() {
	Bullet* itr = head;//�擪�|�C���^����n�߂�
	while (itr != NULL) {//�����܂Ń|�C���^��i�߂�
		//�~�̕`��
		DrawCircle(itr->x, itr->y, itr->r, itr->color, 1);
		itr = itr->next;//�i�߂�
	}
}
void BulletList::calc(int count) {
	Bullet* itr = head;//�擪�|�C���^����n�߂�
	while (itr != NULL) {//�����܂Ń|�C���^��i�߂�
		double rad = itr->angle * PI / 180;//�x���@���ʓx�@

		switch (itr->ptn) {
		case 0://�ʏ�e
			itr->x += 5 * cos(rad);//x�����̈ړ�
			itr->y += 5 * sin(rad);//y�����̈ړ�
			break;
		}

		//�͈̓`�F�b�N
		if (Btwn(-20, itr->x, MAX_X + 20) && Btwn(-20, itr->y, MAX_Y + 20)) {//��ʓ��ɂ���Ƃ�
			itr = itr->next;//�i�߂�
		}
		else {//��ʊO+20�ɂ���Ƃ�
			itr = del(itr);//�Y���̒e������
		}

	}
}
void BulletList::add(Bullet b) {
	//�V����node�f�[�^
	Bullet* pnode = (Bullet*)malloc(sizeof(Bullet));
	*pnode = b;//�V���ȃm�[�h�Ƀf�[�^������
	pushBack(&pnode, &head, &tail);//�����ɒǉ�
}
Bullet* BulletList::del(Bullet* target) {
	return deleteNode(&target, &head, &tail);
}
void BulletList::delAll() {
	deleteAllNode(&head, &tail);
}

void EnemyList::draw(int count) {
	ENode* itr = head;//�擪�|�C���^����n�߂�
	while (itr != NULL) {//�����܂Ń|�C���^��i�߂�
		itr->data.draw(count);//�`��
		itr = itr->next;//�i�߂�
	}
}
void EnemyList::update(int count) {
	ENode* itr = head;//�擪�|�C���^����n�߂�
	while (itr != NULL) {//�����܂Ń|�C���^��i�߂�
		itr->data.update();//�X�V
		itr = itr->next;//�i�߂�
	}
}
void EnemyList::add(Enemy b) {
	ENode* pnode = (ENode*)malloc(sizeof(ENode));//�V����node�f�[�^
	pnode->data = b;//�V���ȃm�[�h�Ƀf�[�^������
	pushBack(&pnode, &head, &tail);//�����ɒǉ�
}
ENode* EnemyList::del(ENode* target) {
	return deleteNode(&target, &head, &tail);
}
void EnemyList::delAll() {
	deleteAllNode(&head, &tail);
}

void EnemyList::collision_with_PlayerShot(BulletList* b) {
	ENode* itrE = head;//�G���X�g�̐擪�|�C���^
	Bullet* itrB = b->getHead();//�e���X�g�̐擪�|�C���^
	while (itrE != NULL) {//�����܂Ń|�C���^��i�߂�
		while (itrB != NULL) {//�����܂Ń|�C���^��i�߂�
			if ((Btwn(itrE->data.x - itrE->data.graph->size_x, itrB->x + itrB->r, itrE->data.x + itrE->data.graph->size_x) ||
				Btwn(itrE->data.x - itrE->data.graph->size_x, itrB->x - itrB->r, itrE->data.x + itrE->data.graph->size_x)) &&
				(Btwn(itrE->data.y - itrE->data.graph->size_y, itrB->y + itrB->r, itrE->data.y + itrE->data.graph->size_y) ||
				Btwn(itrE->data.y - itrE->data.graph->size_y, itrB->y - itrB->r, itrE->data.y + itrE->data.graph->size_y))) {
				itrB = b->del(itrB);//�������Ă�����e������
				itrE->data.HP -= 1;
			}
			else {
				itrB = itrB->next;//�i�߂�
			}
		}
		if (itrE->data.HP <= 0) {
			itrE = del(itrE);//HP��0�ȉ��ɂȂ��Ă�����G������
		}
		else {
			itrE = itrE->next;//����ȊO�Ȃ炻�̂܂ܐi�߂�
		}
		itrB = b->getHead();
	}
}