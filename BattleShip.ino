#define player 1
#include <LiquidCrystal.h>
#include <Wire.h>
#include <string.h>

// int field[10][10];  //moje pole
//                     //0-pusty
//                     //1-statek nieznany , w ktory nie trafiono
//                     //2-trafiony
//                     //3-zabity
// int field2[10][10]; //obce pole
//                     //0-znany pusty
//                     //1-pole nieznane
//                     //2-trafiony
//                     //3-zabity
int lose = 20, win = 20;
int data[4]; //1- pionowo,col,min_row,max_row
             //0- poziomo,row,min_col,max_col
class Display{
    LiquidCrystal *lcd1, *lcd2, *lcd3, *lcd4, *lcd5, *lcd6;
public:
    LiquidCrystal *lcds[6];
    int field[10][10];
    int field2[10][10];
    String cLook[4];

    Display(int e1, int e2, int e3, int e4, int e5, int e6){
        cLook[0] = " ";
        cLook[1] = "@";
        cLook[2] = "/";
        cLook[3] = "X";

        lcd1 = new LiquidCrystal(12, e1, 5, 4, 3, 2);
        lcd2 = new LiquidCrystal(12, e2, 5, 4, 3, 2);
        lcd3 = new LiquidCrystal(12, e3, 5, 4, 3, 2);
        lcd4 = new LiquidCrystal(12, e4, 5, 4, 3, 2);
        lcd5 = new LiquidCrystal(12, e5, 5, 4, 3, 2);
        lcd6 = new LiquidCrystal(12, e6, 5, 4, 3, 2);
        lcds[0] = lcd1;
        lcds[1] = lcd2;
        lcds[2] = lcd3;
        lcds[3] = lcd4;
        lcds[4] = lcd5;
        lcds[5] = lcd6;

        lcd1->begin(16, 2);
        lcd2->begin(16, 2);
        lcd3->begin(16, 2);
        lcd4->begin(16, 2);
        lcd5->begin(16, 2);
        lcd6->begin(16, 2);
    };
};
Display *disp;
int key = 0;
/*----------------------KOMUNIKACJA-----------------------------------------*/

void waitSerial(){
    while (!Serial.available()){};
};

void send_answer(int val){
    Serial.write(val / 256);
    Serial.write(val % 256);
}

void receive_answer(int *a){
    byte b1, b2;
    waitSerial();
    b1 = Serial.read();
    waitSerial();
    b2 = Serial.read();
    *a = b2 + b1 * 256;
}

void send_location(int row, int col){

    send_answer(row);
    send_answer(col);
}

void receive_location(int *row, int *col){

    receive_answer(row);
    receive_answer(col);
}

void send_data(){
    for (int i = 0; i < 4; ++i)
        send_answer(data[i]);
}

void receive_data(){
    for (int i = 0; i < 4; ++i)
        receive_answer(&data[i]);
}

int gotKey;
char sgotKey;
bool wantKey = false;

void onKeyPressed(int numBytes){
    if (!wantKey)
        return;
    sgotKey = Wire.read();
    show_message((String)sgotKey);
    gotKey = sgotKey - '0';
    wantKey = false;
}

int getKey(){
    do{
        wantKey = true;
        while (wantKey){};
    } while (gotKey < 0 || gotKey > 9);
    return gotKey;
}

int getDir(){
    do{
        wantKey = true;
        while (wantKey){};
    } while (gotKey < 1 || gotKey > 4);
    return gotKey;
}

void insert_key(int *a, int *b){
    show_message("wpisz wiersz");
    *a = getKey();
    show_message("wpisz kolumne");
    *b = getKey();
}

void insert_key_with_dir(int *a, int *b, int *dir){
    show_message("wpisz wiersz");
    *a = getKey();
    show_message("wpisz kolumne");
    *b = getKey();
    show_message("wpisz kierunek");
    *dir = getDir();
}

/*----------------------KOMUNIKACJA-----------------------------------------*/
/*----------------------WYswietlanie PAwel---------------------------------------*/

void show_message(String k){

    disp->lcds[0]->print("                   ");
    disp->lcds[0]->setCursor(0, 0);
    disp->lcds[0]->print(k);
    disp->lcds[0]->setCursor(0, 0);
}

void show_field(int (&f)[10][10]){

    char c;
    int j = 1;
    for (int i = 0; i < 10; i++){
        c = i + '0';
        disp->lcds[j]->setCursor(0, i % 2);
        disp->lcds[j]->print(c);
        i % 2 ? j++ : j;
    }
    int k = 1;
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            disp->lcds[k]->setCursor(j + 3, i % 2);
            disp->lcds[k]->print(disp->cLook[f[i][j]]);
        }
        i % 2 ? k++ : k;
    }
}

/*----------------------WYswietlanie PAwel---------------------------------------*/
/*----------------------KOMUNIKACJA-----------------------------------------*/

//czy mozna wstawic pojedynczy klocek
bool check_pos(int row, int column, int old_row, int old_column){
    int left = column - 1,
        right = column + 1,
        top = row - 1,
        bottom = row + 1;
    //sprawdzam czy są ścianki
    if (!column)
        left = column;
    else if (column == 9)
        right = column;
    if (!row)
        top = row;
    else if (row == 9)
        bottom = row;

    for (int i = top; i <= bottom; i++){
        for (int j = left; j <= right; j++){

            if ((i == old_row) && (j == old_column)){}
            else if (disp->field[i][j])
                return false;
        }
    }
    return true;
}

//pamietaj ze wstawiasz NIE STATEK a ilosc klockow ktorych brakuje !
// Ile(ale <=count) nad tym !pustym! polem można wstawic
int top_count(int row, int col, int count, int b1, int b2){
    int top = 0;
    for (int i = 1; i <= count; i++){
        if (((row - i) >= 0) && (check_pos(row - i, col, b1, b2)))
            top++;
        else
            return top;
    }
    return top;
}

int left_count(int row, int col, int count, int b1, int b2){
    int left = 0;
    for (int i = 1; i <= count; i++){
        if (((col - i) >= 0) && (check_pos(row, col - i, b1, b2)))
            left++;
        else
            return left;
    }
    return left;
}

int bottom_count(int row, int col, int count, int b1, int b2){
    int bottom = 0;
    for (int i = 1; i <= count; i++){
        if (((row + i) <= 9) && (check_pos(row + i, col, b1, b2)))
            bottom++;
        else
            return bottom;
    }
    return bottom;
}

int right_count(int row, int col, int count, int b1, int b2){
    int right = 0;
    for (int i = 1; i <= count; i++){
        if (((col + i) <= 9) && (check_pos(row, col + i, b1, b2)))
            right++;

        else
            return right;
    }
    return right;
}

bool is_good_and_insert(int row, int col, int dir, int size){
    int i;
    switch (dir){
    case 1:
        if (top_count(row, col, size - 1, 11, 11) == (size - 1)){

            for (i = row; i > row - size; i--)
                disp->field[i][col] = 1;
            return true;
        }
        break;
    case 2:
        if (bottom_count(row, col, size - 1, 11, 11) == (size - 1)){

            for (i = row; i < row + size; i++)
                disp->field[i][col] = 1;
            return true;
        }
        break;
    case 3:
        if (left_count(row, col, size - 1, 11, 11) == (size - 1)){

            for (i = col; i > col - size; i--)
                disp->field[row][i] = 1;
            return true;
        }
        break;
    case 4:
        if (right_count(row, col, size - 1, 11, 11) == (size - 1)){

            for (i = col; i < col + size; i++)
                disp->field[row][i] = 1;
            return true;
        }
        break;
    }
    return false;
}

//procedury do wczytywania roznych typow statkow
//statek pojedynczy
void read_ship(int size){
    bool is_goodpos = false, is_good_ship = false;
    int row, col, dir;
    while (!is_goodpos){
        if (size != 1){
            insert_key_with_dir(&row, &col, &dir);
            is_goodpos = check_pos(row, col, 11, 11);
            if (is_goodpos)
                is_good_ship = is_good_and_insert(row, col, dir, size);
        }
        else{

            insert_key(&row, &col);
            is_goodpos = check_pos(row, col, 11, 11);
            if (is_goodpos)
                is_good_ship = is_good_and_insert(row, col, 1, size);
        }

        if (is_good_ship)
            show_message("wstawiono");
        else{
            show_message("bledna lokalizacja");
            is_goodpos = false;
        }
    }
}

//wczytywanie wszystkich statkow
void read_ships(){
    int i;

    show_message("statki typu 1");
    for (i = 0; i < 4; i++){
        read_ship(1);
        show_field(disp->field);
    }
    show_message("statki typu 2");
    for (i = 0; i < 3; i++){
        read_ship(2);
        show_field(disp->field);
    }
    show_message("statki typu 3");
    for (i = 0; i < 2; i++){
        read_ship(3);
        show_field(disp->field);
    }
    show_message("statki typu 4");
    read_ship(4);
    show_field(disp->field);
    show_message("Koniec wstawiania");
}

/*----------------------PROCES GRY----------------------------------------*/
void insert_in_data(int dir, int k, int k1, int k2){
    data[0] = dir;
    data[1] = k;
    data[2] = k1;
    data[3] = k2;
}

bool is_killed(int row, int col){

    bool a = check_pos(row, col, row, col);
    int maxim = 0, minim = 0, i;
    if ((disp->field[row - 1][col] == 2) || (disp->field[row + 1][col] == 2)){
        //jesli w pionie jest trafiony

        i = row;
        maxim = i;
        minim = i;
        while ((disp->field[i][col])){ //poki nie dojdziesz do pustego idz do gory  (poki jest 1 albo 2)
            if (i < 0)
                break;
            if ((disp->field[i][col]) == 1)
                return false; //jesli znajdziesz statek w ktory nie trafiono wyjdz
            minim = i;
            i--;
        }
        i = row;
        while ((disp->field[i][col])){ //poki nie dojdziesz do pustego idz do dolu (poki jest 1 albo 2)

            if (i > 9)
                break;
            if ((disp->field[i][col]) == 1)
                return false; //jesli znajdziesz statek w ktory nie trafiono wyjdz
            maxim = i;
            i++;
        }
        insert_in_data(1, col, minim, maxim);
        return true;
    }
    else if ((disp->field[row][col - 1] == 2) || (disp->field[row][col + 1] == 2)){ //jesli w poziomie jest trafiony
        i = col;
        maxim = i;
        minim = i;
        while ((disp->field[row][i])){ //poki nie dojdziesz do pustego idz zlewa
            if (i < 0)
                break;
            if ((disp->field[row][i]) == 1)
                return false; //jesli znajdziesz statek w ktory nie trafiono wyjdz
            minim = i;
            i--;
        }
        i = col;
        while ((disp->field[row][i])){ //poki nie dojdziesz do pustego idz z prawa

            if (i > 9)
                break;
            if ((disp->field[row][i]) == 1)
                return false; //jesli znajdziesz statek w ktory nie trafiono wyjdz
            maxim = i;
            i++;
        }

        insert_in_data(0, row, minim, maxim);
        return true;
    }
    else if (a){
        insert_in_data(0, row, col, col);
        return true;
    };
    return false;
}
//ustawia statki na zabite
void set_killed(int (&f)[10][10], int my){
    int i, j;
    if (data[0]){
        j = data[1];
        for (i = data[2]; i <= data[3]; i++){
            f[i][j] = 3;
            if (my)
                lose--;
            else
                win--;
        };
    }
    else{
        j = data[1];
        for (i = data[2]; i <= data[3]; i++){
            f[j][i] = 3;
            if (my)
                lose--;
            else
                win--;
        }
    }
}

void hit(int row, int col){
    disp->field[row][col] = 2; //ustaw jako hit
    bool a = is_killed(row, col);
    if (a){ //jesli zabity to wyslij 3 i maciez data[] i ustaw swoje statki jako zabite

        set_killed(disp->field, 1);
        send_answer(3);
        send_data();
        show_field(disp->field);
    }
    else{ //jesli nie wyslij 2 i ustaw aktualny statek na 2
        send_answer(2);
    };
}

void sender(){
    bool again = true;
    while (again){
        bool is_goodpos = false;
        int row, col;
        again = false;
        while (!is_goodpos){
            show_message("twoja kolej");
            insert_key(&row, &col);
            if (disp->field2[row][col] == 1){ //jesli pole nieznane (=1)
                send_location(row, col);
                is_goodpos = true;
                int a;
                receive_answer(&a);
                if (!a){
                    show_message("nie trafiles");
                    disp->field2[row][col] = 0;

                } //pusty
                else if (a == 2){
                    show_message("trafiles");
                    disp->field2[row][col] = 2;
                    again = true;
                    show_field(disp->field2);
                }
                else{
                    show_message("zabiles");
                    receive_data();
                    set_killed(disp->field2, 0);
                    again = true;
                    show_field(disp->field2);
                } //killed odbior macierzy i wypewnienie na killed !napisac fukcje uniw z przekaz argumentow
            }
            else
                show_message("nie powtarzaj sie!");
        }
        show_field(disp->field2);
    }
}

void receiver(){
    bool again = true;
    while (again){
        int row, col;
        again = false;
        show_message("czekaj");
        receive_location(&row, &col);
        if (disp->field[row][col] == 0)
            send_answer(0); //jesli pusty
        else{
            hit(row, col);
            again = true;
        }
        show_field(disp->field);
    }
}

/*----------------------PROCES GRY----------------------------------------*/

void init_void(){
    int i, j;

    for (i = 0; i < 10; i++){
        for (j = 0; j < 10; j++){
            disp->field[i][j] = 0;  //moje puste
            disp->field2[i][j] = 1; //obce nieznane
        }
    }
    show_message("wpisz statki");
    read_ships();
}

bool game_over(){
    if ((lose != 0) && (win != 0))
        return false;
    else
        return true;
}

void run_game(){
    bool a;
    if (player){ //gdy jestes pierwszy

        sender();
        show_field(disp->field2);
        a = game_over();
        if (!a)
            receiver();
        show_field(disp->field);
    }
    else{
        receiver();
        show_field(disp->field);
        a = game_over();
        if (!a)
            sender();
        show_field(disp->field2);
    }
}
void test1(){
    //jednoos
    disp->field[0][3] = 1;
    disp->field[0][7] = 1;
    disp->field[9][5] = 1;
    disp->field[9][9] = 1;
    //dwuos
    disp->field[2][5] = 1;
    disp->field[3][5] = 1;
    disp->field[2][9] = 1;
    disp->field[3][9] = 1;
    disp->field[4][2] = 1;
    disp->field[4][3] = 1;
    //trzyos
    for (int i = 7; i < 10; i++)
        disp->field[i][2] = 1;
    for (int i = 5; i < 8; i++)
        disp->field[i][7] = 1;
    for (int i = 1; i < 5; i++)
        disp->field[i][0] = 1;
    show_field(disp->field);
}
void setup(){

    disp = new Display(6, 7, 8, 9, 10, 11);
    disp->lcds[0]->setCursor(0, 1);
    disp->lcds[0]->print("   0123456789");
    disp->lcds[0]->setCursor(0, 0);
    Wire.begin(9);
    Wire.onReceive(onKeyPressed);

    Serial.begin(9600);

    init_void();
    //test1();
    bool a = game_over();
    show_field(disp->field2);
    while (!a){
        run_game();
    }
    if (!lose)
        show_message("przegrales");
    else
        show_message("wygrales");
}

void loop(){}
