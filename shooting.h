#pragma once
/*以下に定数の定義*/
#define MAX_X 640//ゲーム画面の最大x
#define MAX_Y 480//ゲーム画面の最大y
#define MIN_X 0//ゲーム画面の最小x
#define MIN_Y 0//ゲーム画面の最小y
#define CX (MAX_X + MIN_X)/2 //中心のx
#define CY (MAX_Y + MIN_Y)/2 //中心のy

#define PUSH 1//キーを押した瞬間
#define PULL -1//キーを離した瞬間
#define NO_CHANGE 0//それ以外の状態（キー変化なし)

namespace COLOR {
	static const int white = GetColor(255, 255, 255);
	static const int red = GetColor(255, 0, 0);
	static const int green = GetColor(0, 255, 0);
	static const int blue = GetColor(0, 0, 255);
	static const int gray = GetColor(128, 128, 128);
}

/*以下にクラスの定義*/
class KeyInput {//キーボード入力
private:
	char preKeys[256];
public:
	char nowKeys[256];

	KeyInput() {//コンストラクタ
		GetHitKeyStateAll(nowKeys);//現在のキーの押下状態を取得
	}
	void update() {
		memcpy(preKeys, nowKeys, sizeof(nowKeys));//前のキーの押下状態を更新
		GetHitKeyStateAll(nowKeys);//現在のキーの押下状態を取得
	}
	int getState(int where) {
		return nowKeys[where] - preKeys[where];//パルスの検出→押した瞬間なら1,離した瞬間なら-1が返る
	}
};

//画像
class Graphic {
private:
	TCHAR* filepath;//画像の保存場所
	int numSliceX, numSliceY;//画像の分割数
	int width, height;	//画像の縦横サイズ
	int* handle;//画像ハンドル
public:
	int size_x, size_y;//分割後の画像サイズ(の1/2)
	double exRate;//画像の拡大率

	Graphic(const char* FileName, int Width, int Height, int numX, int numY, double ExRate = 1);
	void draw(int x, int y, int index = 0, double angle = 0);
	void setExRate(double exRate = 1);
};

//弾のデータ
typedef struct BulletData {
	int x, y;//座標
	double angle;//現在進行角度
	int r; //半径
	int color;//色
	int ptn;//動きパターンID
	struct BulletData* before;//前のデータへのポインタ
	struct BulletData* next;//次のデータへのポインタ
}Bullet;

//弾の生成・消去
class BulletList {
private:
	Bullet* head = NULL; //リストの先頭を指すポインタ
	Bullet* tail = NULL; //リストの末尾を指すポインタ

public:
	void draw();//描画
	void calc(int count);//計算 (動きパターンで場合分け)
	void add(Bullet b);//弾の追加(リストの末尾に追加)
	Bullet* del(Bullet* target);//弾の削除 (戻値：次を指すポインタ)
	void delAll();//リスト全体を消去

	Bullet* getHead() { return head; }
	Bullet* getTail() { return tail; }
};


//キャラクター
class Character {
public:
	int x, y;//座標
	double v;//移動速度
	Graphic* graph;//画像データ
	int index;	//画像ハンドルのインデックス
	double angle = 0; //画像の回転角度(rad)
	int HP, maxHP;//maxHPはHPの最大値

	//コンストラクタ
	Character(int X, int Y, int V, Graphic* Image, int firstIndex = 0, int hp = 1, int maxhp = 1);
	//描画
	void draw();
};

//プレイヤー
class Player : public Character {
public:
	using Character::Character;//Characterクラスのコンストラクタを呼ぶ

	//更新
	void update(KeyInput keys);
	//描画
	void draw(KeyInput keys, int count);
	// 自弾の追加
	void createShot(KeyInput keys, BulletList* BList_p, int count, int interval = 5);
};

//敵
class Enemy : public Character {
private:
	int index_min; //画像のインデックス範囲の最小値
	int frameSum;  //アニメーションの総コマ数
public:
	int moveID;//移動パターンID
	//ここに攻撃パターン属性（未実装）

	Enemy(int X, int Y, int V, Graphic* Image, int Index_min = 0, int FrameSum = 1, int hp = 1, int maxhp = 1, int moveId = 0) :Character(X, Y, V, Image, Index_min, hp, maxhp) {
		index_min = Index_min;
		frameSum = FrameSum;
		moveID = moveId;
	}
	void update();//更新
	void draw(int count);//描画
};

//敵リストのノード
typedef struct EnemyNode{
	Enemy data;
	struct EnemyNode* before;
	struct EnemyNode* next;
}ENode;
//敵リスト
class EnemyList {
private:
	ENode* head = NULL; //リストの先頭を指すポインタ
	ENode* tail = NULL; //リストの末尾を指すポインタ
public:
	void draw(int count);//描画
	void update(int count);//更新
	void add(Enemy b);//リストの末尾に追加
	ENode* del(ENode* target);//削除 (戻値：次を指すポインタ)
	void delAll();//リスト全体を消去
	void collision_with_PlayerShot(BulletList* b);//プレイヤーの弾との衝突処理
};


/*以下に関数のプロトタイプ宣言を書く*/
int Btwn(int p1, int p, int p2);// p1からp2の範囲にpがあるとき1を返す

template<typename T>
void pushBack(T** node, T** head, T** tail);//head->...->tailの末尾にnodeを追加

template<typename T>
T* deleteNode(T** node, T** head, T** tail);//nodeをhead-tailのリストから消す(nodeの次のノードを返す)

template<typename T>
void deleteAllNode(T** head, T** tail);//head-tailのリストのノードをすべて消去

template<typename T>
int isInSquare(T chara, int min_x, int min_y, int max_x, int max_y);//キャラクターが四角形の範囲の中にいるかチェック