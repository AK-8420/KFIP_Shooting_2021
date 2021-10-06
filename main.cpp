//編集：田上慶治, 勝間朱里
#include "DxLib.h"
#include "Shooting.h"
#include <math.h>
#define PI acos(-1)

/*メイン関数*/
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(TRUE); // フルスクリーンからウィンドウモードに変更
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
		return -1;			// エラーが起きたら直ちに終了
	SetDrawScreen(DX_SCREEN_BACK);//描画先を裏画面に設定する

	/*ここに初期化処理を書く*/
	int count = 0;
	KeyInput keys;//キー入力結果を格納する場所

	Graphic* G_player = new Graphic("player.png", 69, 120, 3, 3);
	Graphic* G_fairy = new Graphic("fairy.png", 168, 150, 2, 2);
	Player p(CX, CY + 100, 4, G_player, 3);
	Enemy fairy1(CX - 100, CY - 100, 4, G_fairy, 0, 2);
	Enemy fairy2(CX + 100, CY - 100, 4, G_fairy, 2, 2, 20, 20);
	EnemyList Enemys;//敵リスト
	Enemys.add(fairy1);//青妖精の追加
	Enemys.add(fairy2);//赤妖精の追加

	BulletList BList_p;//プレイヤーの弾リスト

    /*メインループ*/
	while (1) {
		if (ProcessMessage() != 0) {//メッセージ処理
			break;//×ボタンでループ終了
		}
		ClearDrawScreen();//画面を消す
		clsDx();//printfDxで出力した文字列をループごとに消す
		keys.update();//キーの押下状態を取得

		/*以下に描画処理を書く*/
		p.draw(keys, count);
		BList_p.draw();
		Enemys.draw(count);


		/*以下に更新処理を書く*/
		p.update(keys);
		p.createShot(keys, &BList_p, count);
		BList_p.calc(count);
		Enemys.collision_with_PlayerShot(&BList_p);


		ScreenFlip();//裏画面を表画面に反映
		count++;
	}

	BList_p.delAll();//プレイヤーの弾をすべて消す
	DxLib_End(); // DXライブラリ終了処理
	return 0;
}

/*以下に関数の定義を書く*/
Graphic::Graphic(const char* FileName, int Width, int Height, int numX, int numY, double ExRate) {
	filepath = (TCHAR*)malloc(sizeof("graphic/") + sizeof(FileName));
	strcpyDx(filepath, "graphic/");
	strcatDx(filepath, FileName);
	width = Width; height = Height; numSliceX = numX; numSliceY = numY;
	setExRate(ExRate);

	//サイズ分の画像ハンドルを確保
	int num = numSliceX * numSliceY;
	handle = (int*)malloc(sizeof(int) * (num + 1));
	if (num == 1)
		handle[0] = LoadGraph(filepath);
	else
		LoadDivGraph(filepath, num, numSliceX, numSliceY, width / numSliceX, height / numSliceY, handle);
}
void Graphic::draw(int x, int y, int index, double angle) {
	DrawRotaGraph(x, y, exRate, angle, handle[index], 1);//メモリに読みこんだグラフィックの描画(回転あり)
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

	if (y > MAX_Y - graph->size_y)//下端を越えている
		y = MAX_Y - graph->size_y;
	else if (y < MIN_Y + graph->size_y)//上端を越えている
		y = MIN_Y + graph->size_y;
	if (x > MAX_X - graph->size_x)//右端を越えている
		x = MAX_X - graph->size_x;
	else if (x < MIN_X + graph->size_x)//左端を越えている
		x = MIN_X + graph->size_x;
}
void Player::draw(KeyInput keys, int count) {
	int frame = (count / 5) % 3; // 5countごとにコマが変化(全部で3コマ)

	index = 3;//左右キーを押していないとき
	if (keys.nowKeys[KEY_INPUT_LEFT] == PUSH)
		index = 6;
	if (keys.nowKeys[KEY_INPUT_RIGHT] == PUSH)
		index = 0;
	index += frame;

	graph->draw(x, y, index, angle);
}

void Player::createShot(KeyInput keys, BulletList* BList_p, int count, int interval) {
	static int sCount = 0;//発射タイミングの基準
	int isPush = keys.getState(KEY_INPUT_SPACE) == PUSH;//SPACEキーを押したか
	int isPull = keys.getState(KEY_INPUT_SPACE) == PULL;//SPACEキーを離したか
	int isEnoughInterval = sCount + interval <= count;//連打していないか
	if (isPull)sCount = count;//発射タイミングの基準を更新
	if (isPush && isEnoughInterval) sCount = count;//発射タイミングの基準を更新
	if (!keys.nowKeys[KEY_INPUT_SPACE]) return;//SPACEキーを押していなかったらNG
	if (!((count - sCount) % interval == 0)) return;//インターバル内だったらNG
	//以下はショットの決定と生成
	Bullet b1 = { x - 10, y, 270, 3 ,COLOR::white, 0 };//弾データを用意
	Bullet b2 = { x + 10, y, 270, 3 ,COLOR::white, 0 };//弾データを用意
	BList_p->add(b1);//弾を追加
	BList_p->add(b2);//弾を追加
}

void Enemy::update() {
	//まだ空っぽ
	x += v;//x方向の移動
	y += v;//y方向の移動

	if (HP > maxHP) {//現在HPが最大HPを超えていたら直す
		HP = maxHP;//進める
	}
	/*
	if (hp > 0 && isInWall(itr->e.pos.x, itr->e.pos.y, 100)) {//生きているかつ範囲内にいる場合
		itr = itr->next;//進める
	}
	else {//hpが0か負または画面の範囲外にいるとき
		itr = delEnemy(itr);//該当の敵をElistから消す
	}
	*/
}
void Enemy::draw(int count) {
	int frame = (count / 10) % frameSum; // 10countごとにコマが変化

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
	(*node)->next = NULL;//追加ノードの次はNULL

	if (*head == NULL) //空のリストの場合
		*head = *node;//ヘッドは追加ノードを指す
	if (*tail)//末尾のノードが既にあるとき
		(*tail)->next = *node;//前ノードの先を追加ノードに
	(*node)->before = *tail;//追加ノードの前を前ノードに
	*tail = *node;//Tailは最後のノードを指す
}
template<typename T>
T* deleteNode(T** node, T** head, T** tail) {
	T* pReturn = (*node)->next;//一個先のノードを指すポインタの用意
	if ((*node)->before)//ノードが先頭でないとき
		(*node)->before->next = pReturn;//一個前の次を一個先のノードにする
	else
		*head = pReturn;//ヘッドの指すノードを一個先にする

	if (pReturn)//pが末尾を指さないとき
		pReturn->before = (*node)->before;//一個先のノードから2個前のノードをつなぐ
	else//pが末尾を指すとき
		*tail = (*node)->before;//テールは前のノードを指す

	free(*node);//ノードを消す
	return pReturn;//次のポインタを返す

}
template<typename T>
void deleteAllNode(T** head, T** tail) {
	T* p, * pnext;//ポインタを2つ用意
	pnext = *head;//ヘッドのノードを指す
	while (pnext) {//ノードがなくなるまで繰り返す
		p = pnext;//pを進める
		pnext = p->next;//pnextを進める
		free(p);//pのノードを開放
	}
	*head = NULL;//ヌル
	*tail = NULL;//ヌル
}

void BulletList::draw() {
	Bullet* itr = head;//先頭ポインタから始める
	while (itr != NULL) {//末尾までポインタを進める
		//円の描画
		DrawCircle(itr->x, itr->y, itr->r, itr->color, 1);
		itr = itr->next;//進める
	}
}
void BulletList::calc(int count) {
	Bullet* itr = head;//先頭ポインタから始める
	while (itr != NULL) {//末尾までポインタを進める
		double rad = itr->angle * PI / 180;//度数法→弧度法

		switch (itr->ptn) {
		case 0://通常弾
			itr->x += 5 * cos(rad);//x方向の移動
			itr->y += 5 * sin(rad);//y方向の移動
			break;
		}

		//範囲チェック
		if (Btwn(-20, itr->x, MAX_X + 20) && Btwn(-20, itr->y, MAX_Y + 20)) {//画面内にいるとき
			itr = itr->next;//進める
		}
		else {//画面外+20にいるとき
			itr = del(itr);//該当の弾を消す
		}

	}
}
void BulletList::add(Bullet b) {
	//新しいnodeデータ
	Bullet* pnode = (Bullet*)malloc(sizeof(Bullet));
	*pnode = b;//新たなノードにデータを入れる
	pushBack(&pnode, &head, &tail);//末尾に追加
}
Bullet* BulletList::del(Bullet* target) {
	return deleteNode(&target, &head, &tail);
}
void BulletList::delAll() {
	deleteAllNode(&head, &tail);
}

void EnemyList::draw(int count) {
	ENode* itr = head;//先頭ポインタから始める
	while (itr != NULL) {//末尾までポインタを進める
		itr->data.draw(count);//描画
		itr = itr->next;//進める
	}
}
void EnemyList::update(int count) {
	ENode* itr = head;//先頭ポインタから始める
	while (itr != NULL) {//末尾までポインタを進める
		itr->data.update();//更新
		itr = itr->next;//進める
	}
}
void EnemyList::add(Enemy b) {
	ENode* pnode = (ENode*)malloc(sizeof(ENode));//新しいnodeデータ
	pnode->data = b;//新たなノードにデータを入れる
	pushBack(&pnode, &head, &tail);//末尾に追加
}
ENode* EnemyList::del(ENode* target) {
	return deleteNode(&target, &head, &tail);
}
void EnemyList::delAll() {
	deleteAllNode(&head, &tail);
}

void EnemyList::collision_with_PlayerShot(BulletList* b) {
	ENode* itrE = head;//敵リストの先頭ポインタ
	Bullet* itrB = b->getHead();//弾リストの先頭ポインタ
	while (itrE != NULL) {//末尾までポインタを進める
		while (itrB != NULL) {//末尾までポインタを進める
			if ((Btwn(itrE->data.x - itrE->data.graph->size_x, itrB->x + itrB->r, itrE->data.x + itrE->data.graph->size_x) ||
				Btwn(itrE->data.x - itrE->data.graph->size_x, itrB->x - itrB->r, itrE->data.x + itrE->data.graph->size_x)) &&
				(Btwn(itrE->data.y - itrE->data.graph->size_y, itrB->y + itrB->r, itrE->data.y + itrE->data.graph->size_y) ||
				Btwn(itrE->data.y - itrE->data.graph->size_y, itrB->y - itrB->r, itrE->data.y + itrE->data.graph->size_y))) {
				itrB = b->del(itrB);//当たっていたら弾を消す
				itrE->data.HP -= 1;
			}
			else {
				itrB = itrB->next;//進める
			}
		}
		if (itrE->data.HP <= 0) {
			itrE = del(itrE);//HPが0以下になっていたら敵を消す
		}
		else {
			itrE = itrE->next;//それ以外ならそのまま進める
		}
		itrB = b->getHead();
	}
}