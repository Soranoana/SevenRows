/* 定数宣言 */
//scanf用
#define _CRT_SECURE_NO_WARNINGS
//プレイヤー数
#define PLAYER_NUM 4
//カードの総枚数
#define DECK_NUM 52
//スート種類
#define SUIT 4
//数字の種類
#define NUMBERS 13
//スートを保存する文字列の大きさ
#define SUIT_SIZE 20
//数字を保存する文字列の大きさ
#define NUMBERS_SIZE 3
//スタートの数
#define SEVEN 7
//手札の終わりを示す
#define LIST_END "listEnd"
//手札の空きを示す
#define LIST_EMPTY "listEmpty"
//出力先ディレクトリ
#define OUTPUT "C:\\testLog\\"
//パスできる回数
#define PASS 3

/* 外部参照 */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>
#include<stdarg.h>

/* 関数宣言 */
void shuffle();
void initializePlayer();
void dispField();
bool isOverSeven(int num);
int ArrayLength(char *obj);
void sortCards(int num);
int countHand(int num);
bool needSwapHand(char *hand1, char *hand2);
bool canUseCard(char *card);
void printTC(char *string, int len, ...);
void handSub(int player, int choice);
int backCardSuit(char *card);
int backCardNum(char *card);
void useCard(char *card);
void judge();
void playerPlay();
void printT(char *string);

/* グローバル変数 */
char suit[SUIT][SUIT_SIZE] = { "ハート","スペード","ダイヤ","クラブ" };
//カード種類一覧
char numbers[NUMBERS][NUMBERS_SIZE] = { "A","2","3","4","5","6","7","8","9","10","J","Q","K" };
//カードの置き場所
char fieldCard[4][NUMBERS][NUMBERS_SIZE];
//デッキの用意
char cards[DECK_NUM][SUIT_SIZE + NUMBERS_SIZE];
/* 色コード */
char color[22][8] = { "\x1b[40m",
					  "\x1b[41m",
					  "\x1b[42m",
					  "\x1b[43m",
					  "\x1b[44m",
					  "\x1b[45m",
					  "\x1b[46m",
					  "\x1b[47m",
					  "\x1b[49m",
					  "\x1b[30m",
					  "\x1b[31m",
					  "\x1b[32m",
					  "\x1b[33m",
					  "\x1b[34m",
					  "\x1b[35m",
					  "\x1b[36m",
					  "\x1b[37m",
					  "\x1b[39m",
					  "\x1b[4m" ,
					  "\x1b[1m" ,
					  "\x1b[7m" ,
					  "\x1b[0m" , };
FILE *fp;
//勝者保存　兼　ゲーム終了フラグ
int winner = -1;
//printT用結合先
char tmpG[100];
//ターン
int turn;

/* 関数 */
//色コードcolor用のindex
enum Colors{
	backBlk,	//背景　黒
	backRed,	//背景　赤
	backGrn,	//背景　緑
	backYel,	//背景　黄
	backBlu,	//背景　青
	backMaz,	//背景　マゼンタ
	backCia,	//背景　シアン
	backGly,	//背景　灰
	backDef,	//背景　デフォルト
	charBlk,	//文字　黒
	charRed,	//文字　赤
	charGrn,	//文字　緑
	charYel,	//文字　黄
	charBlu,	//文字　青
	charMaz,	//文字　マゼンタ
	charCia,	//文字　シアン
	charGly,	//文字　灰
	charDef,	//文字　デフォルト
	othrUnd,	//装飾　下線
	othrBol,	//装飾　太字
	othrRvs,	//装飾　反転（背景と文字色）
	othrDef,	//装飾　デフォルト
}colors;

//プレイヤーが保持するデータ
struct Player{
	//プレイヤー名
	char name[100];
	//手札
	char hand[DECK_NUM][SUIT_SIZE + NUMBERS_SIZE];
	//手札の枚数
	int handNum;
	//パス
	int passNum;
	bool state;//1負けてる 0負けてない

}players[PLAYER_NUM];

//playerの初期化
void initializePlayer(){
	//残りのデッキのカード数(スタート数字を引く)
	int pool = DECK_NUM - 4;
	//引く数
	int cardDrow;

	//player初期化
	for(int i = 0; i < PLAYER_NUM; i++){
		//プレイヤー名入力
		sprintf(tmpG, "プレイヤー%d 名:", i + 1);
		printT(tmpG);
		scanf("%s", players[i].name);
		fprintf(fp, "%s\n", players[i].name);

		//カード数を人数で割って切り上げた数をドロー数にする
		cardDrow = (int)ceil(pool / (PLAYER_NUM - i));
		for(int j = 0; j < cardDrow; j++){
			sprintf(players[i].hand[j], "%s", cards[pool - 1 - j]);
		}

		//残りをすべてスペースで初期化
		for(int j = cardDrow; j < DECK_NUM; j++){
			sprintf(players[i].hand[j], "%s", LIST_END);
		}

		//カードの枚数を保存
		players[i].handNum = cardDrow;

		//残りのデッキのカード数を減らす
		pool -= cardDrow;

		//手札をスート、数字順にソート
		sortCards(i);

		//パスの回数初期化
		players[i].passNum = 0;
		//状態初期化
		players[i].state = 0;
	}
}

//num番目の人の手札をスート、数字順にソートする
void sortCards(int num){
	//手札の枚数を確認する
	int handNum = countHand(num);

	//バブルソート風に手札をソート
	for(int i = 0; i < handNum; i++){
		for(int j = i + 1; j < handNum; j++){
			if(needSwapHand(players[num].hand[i], players[num].hand[j])){
				//swap
				char tmp[SUIT_SIZE + NUMBERS_SIZE];
				sprintf(tmp, "%s", players[num].hand[i]);
				sprintf(players[num].hand[i], "%s", players[num].hand[j]);
				sprintf(players[num].hand[j], "%s", &tmp);
			}
		}
	}
}

//hand1がhand2よりソート的に奥であるか
bool needSwapHand(char *hand1, char *hand2){
	//検索結果保存先
	int num1suit = 0, num2suit = 0, num1num = 0, num2num = 0;

	//hand1のスートがどれであるか（suit配列の何番目であるか）
	for(int i = 0; i < SUIT; i++){
		if(strstr(hand1, suit[i]) != NULL){
			num1suit = i;
			break;
		}
		//最後まで回っている
		if(i + 1 == SUIT){
			//空である
			if(strcmp(hand1, LIST_EMPTY) == 0){
				num1suit = i + 1;
				break;
			}
		}
	}
	//hand1の数字がどれであるか（numbers配列の何番目であるか）
	for(int i = 0; i < NUMBERS; i++){
		if(strstr(hand1, numbers[i]) != NULL){
			num1num = i;
			break;
		}
		//最後まで回っている
		if(i + 1 == SUIT){
			//空である
			if(strcmp(hand1, LIST_EMPTY) == 0){
				num1num = i + 1;
				break;
			}
		}
	}
	//hand2のスートがどれであるか（suit配列の何番目であるか）
	for(int i = 0; i < SUIT; i++){
		if(strstr(hand2, suit[i]) != NULL){
			num2suit = i;
			break;
		}
		//最後まで回っている
		if(i + 1 == SUIT){
			//空である
			if(strcmp(hand2, LIST_EMPTY) == 0){
				num2suit = i + 1;
				break;
			}
		}
	}
	//hand2の数字がどれであるか（numbers配列の何番目であるか）
	for(int i = 0; i < NUMBERS; i++){
		if(strstr(hand2, numbers[i]) != NULL){
			num2num = i;
			break;
		}
		//最後まで回っている
		if(i + 1 == SUIT){
			//空である
			if(strcmp(hand2, LIST_EMPTY) == 0){
				num2num = i + 1;
				break;
			}
		}
	}

	//スートが奥なら、または、同じで数字が奥なら
	if(num1suit > num2suit){
		return true;
	} else if(num1suit == num2suit){
		if(num1num > num2num){
			return true;
		}
	}
	//どちらでもなければスワップの必要なし
	return false;

}

//手札の枚数を確認する
int countHand(int num){
	int i;
	for(i = 0; i < DECK_NUM; i++){
		if(strcmp(players[num].hand[i], LIST_END) == 0){
			break;
		}
	}
	return i;
}

//52-4枚のデッキを作る
void shuffle(){

	//まず仮のデッキを作る
	char tmpDeck[DECK_NUM - 4][SUIT_SIZE + NUMBERS_SIZE];
	//仮のデッキに順繰りにカードを入れる
	for(int i = 0, index = 0; i < SUIT; i++){
		for(int j = 0; j < NUMBERS; j++){
			//スタートの数字は飛ばす
			if(j + 1 != SEVEN){
				//スート+数字の文字列を代入
				sprintf(tmpDeck[index], "%s%s", suit[i], numbers[j]);
				index++;
			}
		}
	}

	//本命デッキを一枚ずつ吟味して入れていく
	for(int i = 0; i < DECK_NUM - 4; i++){
		//乱数生成（残ってるカード枚数までの数字）
		int randI = rand() % (DECK_NUM - i - 4);
		//仮デッキの乱数番目のカードを入れる
		sprintf(cards[i], "%s", tmpDeck[randI]);

		//取り出したカードの配列後ろをひとつづつ詰める
		for(int j = randI; j < DECK_NUM - 4; j++){
			//まず当該配列の中身を消す
			sprintf(tmpDeck[j], LIST_END);
			//一番後ろでない限り
			if(j + 1 < DECK_NUM - 4){
				//一つ後ろの配列の値を代入する
				sprintf(tmpDeck[j], "%s", tmpDeck[j + 1]);
			}
		}
	}
	return;
}

//フィールドを表示する
void dispField(){
	//一人ずつステータスを表示する
	for(int i = 0; i < PLAYER_NUM; i++){
		//現在のプレイヤーを指す矢印
		if(turn %PLAYER_NUM == i){
			printT(">");
		} else{
			printT(" ");
		}
		//名前
		printT(players[i].name);

		//パスの状態
		sprintf(tmpG, "\tpass : %d\n", players[i].passNum);
		printT(tmpG);

		//カードの枚数を視覚的に表示
		printT("\tcard : ");
		int count = countHand(i);
		for(int k = 0; k < count; k++){
			printT("■");
		}
		printT("\n");

		//生きているか？
		if(players[i].state == 0){
			//生きている
			printT("\tAlive\n");
		} else{
			//死んでいる
			printT("\tDead\n");
		}

	}
	printT("------------------------------------\n");
	//フィールドを表示
	for(int i = 0; i < SUIT; i++){
		//スートを表示
		sprintf(tmpG, "\n%s  \t", suit[i]);
		printT(tmpG);
		//カードを一枚ずつ表示
		for(int j = 0; j < NUMBERS; j++){
			sprintf(tmpG, " %s", fieldCard[i][j]);
			printT(tmpG);
		}
		printT("\n");
	}
	printT("------------------------------------\n");

	return;
}

//NPCの挙動
/* 基本的にランダム
 * 手札の中から出せるカードをピックアップする
 * その中からランダムにカードを選び、場に出す
 * 手札に出せるものがなければパスを行う
 * 以上を繰り返す
 */
void npcPlay(int playerNum){

	//手札の数を確認
	int myHandNum = countHand(playerNum);

	//手札の中の使用可能なカードのインデックスを保存
	int usableIndex[DECK_NUM];
	//usableIndexの最新の長さ
	int usableHandNum = 0;
	//手札をすべて一枚ずつ見ていく
	for(int i = 0; i < players[playerNum].handNum; i++){
		//このカードは使えるか？
		if(canUseCard(players[playerNum].hand[i]) == true){
			//使えるならインデックスを保存
			usableIndex[usableHandNum] = i;
			//長さを増やす
			usableHandNum++;
		}
	}

	//出せるカードがないなら
	if(usableHandNum == 0){
		//パスを一回使用しターン終了
		players[playerNum].passNum++;
		return;
	}

	//仕様可能なカードの中からランダムに選ぶ
	int handRand = rand() % usableHandNum;
	//カードをフィールドに置く
	int index = usableIndex[handRand];
	useCard(players[playerNum].hand[index]);
	//使ったカードを手札から減らす
	handSub(playerNum, index);

	return;
}

//プレイヤーの行動
void playerPlay(){
	//手札を表示
	printT("手札\n");
	//手札の枚数を確認
	int count = countHand(0);
	//手札を一枚ずつ表示する
	for(int i = 0; i < count; i++){
		sprintf(tmpG, "%d.%s\t", i, players[0].hand[i]);
		printT(tmpG);
		if(i % 5 == 5 - 1){
			printT("\n");
		}
	}
	//最後にパスの選択肢を用意する
	printT("\n100.パス\n");
	int choice;
	while(1){
		//入力
		printT("\n出したいカード名の");
		printTC("前の数字", 3, backGly, charRed, othrUnd);
		printT("を入力してください...(例  3.ハート5 → 3と入力 )  ( ,,`･ω･´)ﾝﾝﾝ？\n");
		scanf("%d", &choice);
		fprintf(fp, "%d\n", choice);
		//入力された数字は手札の範囲か
		if(0 <= choice && choice < count){
			//手札の範囲なら終了
			if(canUseCard(players[0].hand[choice]) == true){
				//場に出すことができるか
				break;
			} else{
				//出せなければループ
				printT("そのカードは出せません");
			}
		} else if(choice == 100){
			//100を入力されたらパスを消費し終了
			players[0].passNum++;
			return;
		} else{
			//範囲外
			printT("その番号は範囲外です");
		}
	}
	//カードをフィールドに置く
	useCard(players[0].hand[choice]);
	//カードを手札から消す
	handSub(0, choice);

	return;
}

//与えられたカードが場に出せるカードか
bool canUseCard(char *card){
	//スートの確認
	int l_suit = backCardSuit(card);

	//数字の確認
	int l_numbers = backCardNum(card);

	//7より大きいか？
	if(isOverSeven(l_numbers) == true){
		//大きいなら
		int i;
		//7-1から始めて与えられた数まで確認する
		for(i = SEVEN - 1; i < NUMBERS && i < l_numbers - 1; i++){
			// *を含んでいたら(間に隙間があったら)
			if(strcmp(fieldCard[l_suit][i + 1], "*") == 0){
				//置けない判定
				//return false;
				break;
			}
		}
		//最後まで検索しきっていた＝すべて埋まっている
		if(i == l_numbers - 1){
			//置ける判定
			return true;
		}
	} else{
		//小さいなら
		int i;
		//7-2から始めて与えられた数字まで確認する
		for(i = SEVEN - 2; 0 < i && l_numbers < i; i--){
			// *を含んでいたら(間に隙間があったら)
			if(strcmp(fieldCard[l_suit][i], "*") == 0){
				//return false;
				break;
			}
		}
		//最後まで検索しきっていた＝すべて埋まっている
		if(i == l_numbers){
			//置ける判定
			return true;
		}
	}
	//エラー対策
	return false;
}

//スートを確認し、スート番号を返す
int backCardSuit(char *card){
	//スートの確認
	int l_suit;
	for(l_suit = 0; l_suit < SUIT; l_suit++){
		//スート一覧と比較
		//スート情報を含むならブレイク
		//その添え字を保存する
		if(strstr(card, suit[l_suit]) != '\0'){
			break;
		}
	}
	//すべてスルーしたらエラー
	if(l_suit == SUIT){
		sprintf(tmpG, "error %d行目\n不明なスートがあります。\nカード名:%s\n", __LINE__, card);
		printT(tmpG);
		exit(-1);
	}
	return l_suit;
}

//カード数字を確認し、数字番号を返す
int backCardNum(char *card){
	//カード数字の確認
	int l_numbers;
	for(l_numbers = 0; l_numbers < NUMBERS; l_numbers++){
		//カード数字一覧と比較
		//カード数字情報を含むならブレイク
		//その添え字を保存する
		if(strstr(card, numbers[l_numbers]) != '\0'){
			break;
		}
	}
	//すべてスルーしたらエラー
	if(l_numbers == NUMBERS){
		sprintf(tmpG, "error %d行目\n不明な数字があります。\nカード名:%s\n", __LINE__, card);
		printT(tmpG);
		exit(-1);
	}
	return l_numbers;
}

//7より大きいか返す
bool isOverSeven(int num){
	if(SEVEN - 1 < num){
		return true;
	} else{
		return false;
	}
}

//カードを場に出す
void useCard(char *card){
	//手札のスートインデックスを抽出
	int cardSuit = backCardSuit(card);
	//手札のナンバーインデックスを抽出
	int cardNum = backCardNum(card);
	//手札をフィールドにコピー
	sprintf(fieldCard[cardSuit][cardNum], "%s", numbers[cardNum]);

	return;
}

//与えられたプレイヤー番号と手札番号から
//手札を減らす
void handSub(int player, int choice){
	//使ったカードのところに空表示を入れる
	sprintf(players[player].hand[choice], "%s", LIST_EMPTY);
	//ソートする
	sortCards(player);
	//リストの最後に来た空表示をリスト終了表示に置き換える
	sprintf(players[player].hand[countHand(player) - 1], "%s", LIST_END);
	//手札枚数を減らす
	players[player].handNum -= 1;

	return;
}

//配列の大きさを求める
int ArrayLength(char *obj){
	return sizeof(*obj) / sizeof(*(obj + 0));
}

//色情報を文字列に加える
//第3引数以降に整数　推奨：enum.colors
void printTC(char *string, int len, ...){
	//可変引数用のおまじないポインタ
	va_list ptr;

	//文字色をいじったか
	bool useTextColor = false;
	//背景をいじったか
	bool useBackColor = false;
	//装飾をいじったか
	bool useOther = false;

	//可変引数読み取り開始
	va_start(ptr, len);
	for(int i = 0; i < len; i++){
		//引数（整数）から色エスケープシーケンス取り出し
		int index = va_arg(ptr, int);

		//終端用に何を使ったか記録
		if(0 <= index && index <= 8){
			//背景をいじった
			useBackColor = true;
		} else if(9 <= index && index <= 17){
			//文字をいじった
			useTextColor = true;
		} else if(18 <= index && index <= 21){
			//装飾をいじった
			useOther = true;
		}

		//出力
		printf("%s", color[index]);
	}
	//読み取り終了
	va_end(ptr);

	//本文出力
	printT(string);

	//使ったものに対応してデフォルトに戻す
	if(useBackColor == true){
		printf("%s", color[backDef]);
	}
	if(useTextColor == true){
		printf("%s", color[charDef]);
	}
	if(useOther == true){
		printf("%s", color[othrDef]);
	}

	//終了
	return;
}

//現在の時刻を名前に持つテキストを作る
char *nowTimeFileName(char *path){

	int year, month, day, hour, minute, second;

	//初期化ついでに出力先ディレクトリ
	char output[1000] = OUTPUT;

	//時間をもらう
	time_t timer = time(NULL);
	struct tm *local = localtime(&timer);

	//年
	year = local->tm_year + 1900;
	sprintf(output, "%s%d", output, year);

	//月
	month = local->tm_mon + 1;
	sprintf(output, "%s-%d", output, month);

	//日
	day = local->tm_mday;
	sprintf(output, "%s-%d", output, day);

	//時
	hour = local->tm_hour;
	sprintf(output, "%s-%d", output, hour);

	//分
	minute = local->tm_min;
	sprintf(output, "%s-%d", output, minute);

	//秒
	second = local->tm_sec;
	sprintf(output, "%s-%d", output, second);

	//最後にテキスト
	sprintf(path, "%s.txt", output);
	return path;
}

//ゲームのジャッジ
void judge(){
	//パスを使い果たした人はいるか？
	for(int i = 0; i < PLAYER_NUM; i++){
		if(players[i].passNum >= PASS){
			//パスを使い果たした人はdeadにする
			players[i].state = 1;
			//手持ちのカードをすべて場に出す
			for(int j = 0; j < players[i].handNum; j++){
				useCard(players[i].hand[j]);
			}
		}
	}

	//初めにカードをゼロにした人が現れたか
	//勝者が決まっていない
	if(winner == -1){
		//プレイヤーを一人ずつ見ていく
		for(int i = 0; i < PLAYER_NUM; i++){
			//カード枚数がゼロ
			if(players[i].handNum == 0){
				//勝者確定
				winner = i;
				break;
			}
		}
	}
	return;
}

//標準+ファイル出力
//printfの引数には未対応
void printT(char *string){
	//標準出力
	printf("%s", string);
	//ファイル出力
	fprintf(fp, "%s", string);

	return;
}

//メイン
int main(void){

	//ファイル出力準備
	char path[1000];
	fp = fopen(nowTimeFileName(&path[0]), "w+");

	//乱数初期値設定
	srand((unsigned int)time(NULL));

	//デッキ構築＆シャッフル
	shuffle();

	//プレイヤー情報の初期化
	initializePlayer();

	//場の初期化
	for(int i = 0; i < SUIT; i++){
		for(int j = 0; j < NUMBERS; j++){
			if(j == SEVEN - 1){
				//7は代入
				sprintf(fieldCard[i][j], "%d", SEVEN);
			} else{
				//それ以外はマスク
				sprintf(fieldCard[i][j], "*");
			}
		}
	}

	//誰のターンか
	turn = rand() % PLAYER_NUM;
	//フィールド表示
	while(winner == -1){
		//コンソールクリア
		system("cls");
		//ゲームマスタージャッジ
		judge();
		//フィールドを表示
		dispField();
		//プレイヤーのターンか？
		if(turn % PLAYER_NUM == 0){
			if(players[0].state == 0){
				//プレイヤーの手番
				playerPlay();
			}
		} else{
			if(players[turn % PLAYER_NUM].state == 0){
				//他プレイヤーの手番
				npcPlay(turn % PLAYER_NUM);
			}
		}
		turn++;
	}


	//プレイヤーの勝利条件
	/*
	パスを使い切らずに一番に手札をゼロにする
	（他プレイヤーがすべてDeadしても手札を使いきるまで続ける）
	*/

	//最初に手札をゼロにした人が勝者
	//勝者が自分なら勝ち
	sprintf(tmpG, "\n勝者 %s\n", players[winner].name);
	printT(tmpG);
	if(winner == 0){
		//あなたの勝ち
		printT("\n\ncongratulation!\n");
	} else{
		//あなたの負け
		printT("\n\nGAME OVER\n");
	}

	if(fclose(fp) == 0){
		printf("\nファイルクローズ成功\n");
	} else{
		printf("\nファイルクローズ失敗\n");
		return -1;
	}
	//終了
	return 0;
}
