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
	int score = 0;
	KeyInput keys;//キー入力結果を格納する場所

	Graphic* G_player = new Graphic("player.png", 69, 120, 3, 3);
	Graphic* G_fairy = new Graphic("fairy.png", 168, 150, 2, 2);
	Player p(CX, CY + 100, 4, G_player, 3, 5, 5);
	Enemy fairy1(CX - 200, 0, 4, G_fairy, 0, 2, 1, 1, 4, 1, 100);
	Enemy fairy2(MIN_X + GetRand(MAX_X - MIN_X), 0, 300, G_fairy, 2, 2, 20, 20, 5, 3, 500);
	EnemyList Enemys;//敵リスト

	BulletList BList_p;//プレイヤーの弾リスト
	BulletList BList_e;//敵の弾リスト

	int scene = Title;//現在のシーン
	int old_scene;//１つ前のシーン
	int stage = 1;//現在のステージ

	const char* str;

	Graphic G_title_back("title_background.png", 640, 624);
	Graphic G_title_logo("title_logo.png", 463, 94);
	int ty = MAX_Y - G_title_back.size_y; //背景画像の初期座標

    /*メインループ*/
	while (1) {
		if (ProcessMessage() != 0) {//メッセージ処理
			break;//×ボタンでループ終了
		}
		ClearDrawScreen();//画面を消す
		clsDx();//printfDxで出力した文字列をループごとに消す
		keys.update();//キーの押下状態を取得

		switch (scene) {
		case Initialize:
			Enemys.delAll(); //敵をすべて消去
			BList_p.delAll();//プレイヤーの弾をすべて消す
			BList_e.delAll();//敵の弾をすべて消す

			p.x = CX;
			p.y = CY + 100; // プレイヤーを初期位置に戻す
			p.HP = p.maxHP; // HP全回復

			count = 0;//経過時間を0に戻す
			score = 0;//スコアを0に戻す
			stage = 1;//ステージ1から再開
			sceneChange(Game, &scene, &old_scene);
			break;
		case Title:
			//描画
			G_title_back.draw(G_title_back.size_x, ty);
			G_title_logo.draw(CX, CY - 100);
			//更新
			if (ty != G_title_back.size_y) { //背景画像を画像上端につくまで下に移動
				ty++;
			}
			else {
				str = "－ 何かキーを押してください －";
				DrawFormatString(CX - GetDrawFormatStringWidth(str) / 2, CY + 48, COLOR::white, str);
			}
			//シーン切り替え
			if (CheckHitKeyAll()) //なんでもいいからキーを押されたら
				sceneChange(Initialize, &scene, &old_scene); //初期化シーンに移行
			break;
		case Game:
			//ここにゲームシーンの描画処理を書く
			p.draw(keys, count);
			BList_p.draw();
			BList_e.draw();
			Enemys.draw(count);
			DrawFormatString(CX - GetDrawFormatStringWidth("SCORE：%d", score) / 2, MAX_Y - 24, COLOR::white, "SCORE：%d", score);
			//ここにゲームシーンの更新処理を書く
			p.update(keys);
			p.createShot(keys, &BList_p, count);
			BList_p.calc(count);
			BList_e.calc(count);
			Enemys.collision_with_PlayerShot(&BList_p, &score);
			Enemys.update(&BList_e, p, count);
			p.collision_with_EnemyShot(&BList_e);

			switch (stage) {
			case 1:
				//ステージ1の敵生成処理
				if (count % 30 == 0)
					Enemys.add(fairy1);
				break;
			case 2:
				//ステージ2の敵生成処理
				if (count % 60 == 0)
					Enemys.add(fairy2);
				break;
			}
			//ここでステージ切り替え判定
			if (count == 300)
				stage = 2;
			//ここでシーン切り替え判定
			if (p.HP <= 0)
				sceneChange(GameOver, &scene, &old_scene);
			if (count == 600)
				sceneChange(GameClear, &scene, &old_scene);
			break;
		case GameOver:
			str = "GAME OVER...";
			DrawFormatString(CX - GetDrawFormatStringWidth(str) / 2, CY, COLOR::white, str);
			str = "Rキーでリトライ";
			DrawFormatString(CX - GetDrawFormatStringWidth(str) / 2, CY + 30, COLOR::white, str);
			str = "スペースキーを押すと終了します";
			DrawFormatString(CX - GetDrawFormatStringWidth(str) / 2, CY + 48, COLOR::white, str);

			//ここでシーン切り替え判定
			if (keys.getState(KEY_INPUT_R) == PUSH)
				sceneChange(Initialize, &scene, &old_scene);
			if (keys.getState(KEY_INPUT_SPACE) == PUSH)
				sceneChange(Ending, &scene, &old_scene);
			break;
		case GameClear:
			str = "GAME CLEAR!";
			DrawFormatString(CX - GetDrawFormatStringWidth(str) / 2, CY, COLOR::white, str);
			str = "SCORE：%d";
			DrawFormatString(CX - GetDrawFormatStringWidth(str, score) / 2, CY + 18, COLOR::white, str, score);
			str = "スペースキーを押すと終了します";
			DrawFormatString(CX - GetDrawFormatStringWidth(str) / 2, CY + 48, COLOR::white, str);

			//ここでシーン切り替え判定
			if (keys.getState(KEY_INPUT_SPACE) == PUSH)
				sceneChange(Ending, &scene, &old_scene);
			break;
		}
		if (scene == Ending) {
			break;//Mainループ脱出
		}

		ScreenFlip();//裏画面を表画面に反映
		count++;
	}

	Enemys.delAll(); //敵をすべて消去
	BList_p.delAll();//プレイヤーの弾をすべて消す
	BList_e.delAll();//敵の弾をすべて消す
	DxLib_End(); // DXライブラリ終了処理
	return 0;
}

/*以下に関数の定義を書く*/
void sceneChange(int toScene, int* nowScene, int* oldScene) {
	*oldScene = *nowScene;//現在のシーンを1つ前のシーンに代入
	*nowScene = toScene;//現在のシーンを新しいシーンに変更
}

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

	if (HP > maxHP)
		HP = maxHP;//現在HPが最大HPを超えないようにする
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
	Bullet b1 = { x - 10, y, 270, 3 ,COLOR::white, 0, 5 };//弾データを用意
	Bullet b2 = { x + 10, y, 270, 3 ,COLOR::white, 0, 5 };//弾データを用意
	BList_p->add(b1);//弾を追加
	BList_p->add(b2);//弾を追加
}

void Enemy::update() {
	switch (moveID) {
	case 0:
		break;
	case 1:// ランダムな4方向移動
		int direction;//移動方向
		direction = GetRand(4);//乱数を取得
		switch (direction) {
		case 0:
			x += v; //右へ移動
			break;
		case 1:
			x -= v; //左へ移動
			break;
		case 2:
			y += v; //下へ移動
			break;
		case 3:
			y -= v; //上へ移動
			break;
		}
		break;
	case 2://右へ移動
		x += v;
		break;
	case 3://左へ移動
		x -= v;
		break;
	case 4:{//円周上を動く
		double theta = PI * (180 - t) / 180; //角度シータ
		x = 200.0 * cos(theta) + CX;
		y = 100.0 * sin(theta);
		break;
		}
	case 5:{//下向きジャンプ
		double ts = (double)t / 60; // 経過フレーム数tを秒に変換
		y = (double)v * ts - 0.5 * v / 1.5 * pow(ts, 2);
		break;
		}
	}

	if (HP > maxHP) {//現在HPが最大HPを超えていたら直す
		HP = maxHP;
	}
	t++;//時間経過
}
void Enemy::draw(int count) {
	int frame = (count / 10) % frameSum; // 10countごとにコマが変化

	index = index_min + frame;
	graph->draw(x, y, index, angle);
}
void Enemy::createShot(BulletList* BList_e, Player p, int count) {
	switch (shotID) {
	case 1: //プレイヤーの方向にまっすぐ向かう弾(青)
		if (count % 30 == 0) {
			double rad = atan2(p.y - y, p.x - x);
			rad = rad * 180 / PI;//弾からプレイヤーへの角度を度数法で計算
			Bullet b = { x, y, rad, 3 ,COLOR::blue, shotID, 3 };//弾データを用意
			BList_e->add(b);//弾を追加
		}
		break;
	case 2: //プレイヤーの方向にまっすぐ向かう弾(赤)
		if (count % 60 == 0) {
			double rad = atan2(p.y - y, p.x - x);
			rad = rad * 180 / PI;//弾からプレイヤーへの角度を度数法で計算
			Bullet b = { x, y, rad, 3 ,COLOR::red, shotID, 10 };//弾データを用意
			BList_e->add(b);//弾を追加
		}
		break;
	case 3: // 1秒後に放射線状に放たれる
		if (t == 60) {
			for (int i = 0; i < 360; i += 10) {
				Bullet b = { x, y, i, 3 ,COLOR::red, shotID, 5 };//弾データを用意
				BList_e->add(b);//弾を追加
			}
		}
		break;
	}
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
template<typename T>
int isInSquare(T chara, int min_x, int min_y, int max_x, int max_y) {
	if ((Btwn(min_x, chara.x + chara.graph->size_x, max_x) || Btwn(min_x, chara.x - chara.graph->size_x, max_x)) &&
		(Btwn(min_y, chara.y + chara.graph->size_y, max_y) || Btwn(min_y, chara.y - chara.graph->size_y, max_y))) {
		return 1;//範囲内
	}
	else {
		return 0;//範囲外
	}
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
		case 1://プレイヤーに向かう弾（遅い）
		case 2://プレイヤーに向かう弾（速い）
			itr->x += itr->v * cos(rad);//x方向の移動
			itr->y += itr->v * sin(rad);//y方向の移動
			break;
		case 3://放射線状に放たれる弾
			itr->x += itr->v * (cos(rad) - sin(rad));//x方向の移動
			itr->y += itr->v * (sin(rad) + cos(rad));//y方向の移動
			break;
		}
		itr->t++;//時間経過

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
void EnemyList::update(BulletList* BList_e, Player p, int count) {
	ENode* itr = head;//先頭ポインタから始める
	while (itr != NULL) {//末尾までポインタを進める
		itr->data.update();//更新
		itr->data.createShot(BList_e, p, count);//弾生成
		if (isInSquare(itr->data, MIN_X, MIN_Y, MAX_X, MAX_Y)) {//画面内にいる場合
			itr = itr->next;//進める
		}
		else {//画面の範囲外にいるとき
			itr = del(itr);//該当の敵をリストから消す
		}
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

void EnemyList::collision_with_PlayerShot(BulletList* b, int* score) {
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
			*score += itrE->data.score;
			itrE = del(itrE);//HPが0以下になっていたら敵を消す
		}
		else {
			itrE = itrE->next;//それ以外ならそのまま進める
		}
		itrB = b->getHead();
	}
}

void Player::collision_with_EnemyShot(BulletList* b) {
	Bullet* itrB = b->getHead();//弾リストの先頭ポインタ
	while (itrB != NULL) {//末尾までポインタを進める
		if ((Btwn(x - graph->size_x, itrB->x + itrB->r, x + graph->size_x) ||
			Btwn(x - graph->size_x, itrB->x - itrB->r, x + graph->size_x)) &&
			(Btwn(y - graph->size_y, itrB->y + itrB->r, y + graph->size_y) ||
				Btwn(y - graph->size_y, itrB->y - itrB->r, y + graph->size_y))) {
			itrB = b->del(itrB);//当たっていたら弾を消す
			HP -= 1;
		}
		else {
			itrB = itrB->next;//進める
		}
	}
}