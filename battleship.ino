int  player =  1;
int ship_count[] = {4, 3, 2, 1}; //1os-4 2os-3 3os-2 4os-1
int field[10][10];  //moje pole 
                    //0-pusty 
                    //1-statek nieznany , w ktory nie trafiono
                    //2-trafiony
                    //3-zabity
int field2[10][10]; //obce pole
                    //0-znany pusty
                    //1-pole nieznane
                    //2-trafiony
                    //3-zabity
int lose=20,win=20;
int data[4];//1- pionowo,col,min_row,max_row
            //0- poziomo,row,min_col,max_col


/*----------------------KOMUNIKACJA-----------------------------------------*/
//wczytywanie 1 liczbe
int read_sign(){
    int sign = Serial.parseInt();
    return sign;
}
//wczytywanie columne albo wiersz
int read_col_or_row(){
    bool is_good = false;
    int n;
    while (!is_good)
    {
        is_good = true;
        while (!(Serial.available() > 0))
        {
        }
        n = read_sign();
        if (n > 9 || n < 0)
        {
            Serial.print("bledne dane! Wpisz jeszcze raz\n"); //!przerobic na czytaj z klawiatury!
            is_good = false;
        }
    }
    return n;
}
//wczytywanie kroku
void insert_key(int &row, int &col){

    Serial.print("wpisz wiersz: ");
    row = read_col_or_row();
    Serial.print(row);
    Serial.print("\nwpisz kolumne: ");
    col = read_col_or_row();
    Serial.print(col);
    Serial.print("\n");
}
//wstawianie elementu statku
void check_and_insert(bool is_goodpos, int row, int col){
    if (is_goodpos)
    {
        field[row][col] = 1;
    }
    else
        Serial.print("zla pozycja !Wpisz jeszcze raz\n");
}
//TUTAJ WYSYLA SIE TO DO INNEGO ARDU ALE JA WYSWIETLAM W KONSOLI HA-HA  
void send_location(int row,int col){
    Serial.write("wyslano: ");
    Serial.write(row);
    Serial.write(" ");
    Serial.write(col);
    Serial.write("\n");
}

void receive_location(int &row,int &col){
    Serial.print("wyslij do gracza :");
    Serial.print("wpisz wiersz: ");
    row = read_col_or_row();
    Serial.print(row);
    Serial.print("\nwpisz kolumne: ");
    col = read_col_or_row();
    Serial.print(col);
    Serial.print("\n");
}

//wyslij odpowiedz do drugiego gracza
void send_answer(int a){
    Serial.print("wysłano ze zabite : ");
    Serial.print(a);
    Serial.print("\n");
}

void receive_answer(int &a){
    Serial.print("wprowadz odpowiedz: ");
    a=read_col_or_row();
}
//wyslij 2 do 2 gracza i data[] 
void send_data(){
    Serial.print("Wyslano datagram");
}

void receive_data(){
    for(int i=0;i<4;i++){
        data[i]=read_col_or_row();
    }
}

/*----------------------KOMUNIKACJA-----------------------------------------*/

//wyświetlanie pola gry //przerobic na taka z przekazywana tabela
void show_field(){
    Serial.print("  0 1 2 3 4 5 6 7 8 9 ");
    Serial.print("\n");
    for (int i = 0; i < 10; i++)
    {
        Serial.print(i);
        Serial.print(" ");
        for (int j = 0; j < 10; j++)
        {
            Serial.print(field[i][j]);
            Serial.print(" ");
        }
        Serial.print("\n");
    }
}

void swap(int &a, int &b){
    int t = a;
    a = b;
    b = t;
}

//czy sa to sasiednie komorki
bool is_neighbor(int row, int column, int old_row, int old_column){
    if (row == old_row && ((column - 1 == old_column && column != 0) || (column + 1 == old_column && column != 9)))
        return true;
    if (column == old_column && ((row - 1 == old_row && row != 0) || (row + 1 == old_row) && row != 9))
        return true;
    return false;
}
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

    for (int i = top; i <= bottom; i++)
    {
        for (int j = left; j <= right; j++)
        {

            if ((i == old_row) && (j == old_column))
            {
            }
            else if (field[i][j])
                return false;
        }
    }
    return true;
}

//pamietaj ze wstawiasz NIE STATEK a ilosc klockow ktorych brakuje !
// Ile(ale <=count) nad tym !pustym! polem można wstawic
int top_count(int row, int col, int count, int b1, int b2){
    int top = 0;
    for (int i = 1; i <= count; i++)
    {
        if (((row - i) >= 0) && (check_pos(row - i, col, b1, b2)))
            top++;
        else
            return top;
    }
    return top;
}

// Ile(ale <=count) zlewa od tego !pustego! pola można wstawic
int left_count(int row, int col, int count, int b1, int b2){
    int left = 0;
    for (int i = 1; i <= count; i++)
    {
        if (((col - i) >= 0) && (check_pos(row, col - i, b1, b2)))
            left++;
        else
            return left;
    }
    return left;
}

// Ile(ale <=count ) na dole od tego !pustego! pola można wstawic
int bottom_count(int row, int col, int count, int b1, int b2){
    int bottom = 0;
    for (int i = 1; i <= count; i++)
    {
        if (((row + i) <= 9) && (check_pos(row + i, col, b1, b2)))
            bottom++;
        else
            return bottom;
    }
    return bottom;
}

// Ile(ale <=count) z prawa od tego !pustego! pola można wstawic
int right_count(int row, int col, int count, int b1, int b2){
    int right = 0;
    for (int i = 1; i <= count; i++)
    {
        if (((col + i) <= 9) && (check_pos(row, col + i, b1, b2)))
            right++;

        else
            return right;
    }
    return right;
}

//czy pierwszy klocek jest dobry dla tego statku i wg czy dobry
bool is_good_first(int row, int col, int count){
    int top = 0, bottom = 0, left = 0, right = 0;
    if (check_pos(row, col, 11, 11))
    {
        top = top_count(row, col, count, 11, 11);
        if (top == count)
            return true;
        bottom = bottom_count(row, col, count - top, 11, 11);
        if (top + bottom == count)
            return true;
        left = left_count(row, col, count, 11, 11);
        if (left == count)
            return true;
        right = right_count(row, col, count - left, 11, 11);
        if (left + right == count)
            return true;
    }
    return false;
}

//czy drugi klocek jest dobry dla tego statku i wg czy dobry
bool is_good_second(int row, int col, int old_row, int old_col, int count){
    int top = 0, bottom = 0, left = 0, right = 0;
    if ((check_pos(row, col, old_row, old_col)) && (is_neighbor(row, col, old_row, old_col))) //dodatkowo sprawdzenie na sasiedzi
    {
        if (col == old_col)
        { //jesli pionowo
            int top_row = min(row, old_row),
                bottom_row = max(row, old_row);

            top = top_count(top_row, col, count, 11, 11);
            if (top == count)
                return true;
            bottom = bottom_count(bottom_row, col, count - top, 11, 11);
            if (top + bottom == count)
                return true;
        }
        else //jesli poziomo
        {
            int left_col = min(col, old_col),
                right_col = max(col, old_col);
            left = left_count(row, left_col, count, 11, 11);
            if (left == count)
                return true;
            right = right_count(row, right_col, count - left, 11, 11);
            if (left + right == count)
                return true;
        }
    }
    return false;
}

//czy trzeci klocek jest dobry do tego statku i wg dobry
bool is_good_third(int row, int col, int old_row1, int old_col1, int old_row2, int old_col2, int count){

    int top = 0, bottom = 0, left = 0, right = 0;

    if (((check_pos(row, col, old_row1, old_col1)) && (is_neighbor(row, col, old_row1, old_col1))) ||
        ((is_neighbor(row, col, old_row2, old_col2)) && (check_pos(row, col, old_row2, old_col2)))){
        if ((col == old_col1) && (old_col1 == old_col2)){  //jesli pionowo
            int top_row = min(row, min(old_row1, old_row2)),
                bottom_row = max(row, max(old_row1, old_row2));

            top = top_count(top_row, col, count, top_row, col); 
            if (top == count) return true;
            bottom = bottom_count(bottom_row, col, count - top, bottom_row, col); 
            if (top + bottom == count) return true;
        }
        else if ((row == old_row1) && (old_row1 == old_row2)){
            int left_col = min(col, min(old_col1, old_col2)),
                right_col = max(col, max(old_col1, old_col2));

            left = left_count(row, left_col, count, row, left_col);
            if (left == count) return true;

            right = right_count(row, right_col, count - left, row, right_col);
            if (left + right == count) return true;
        }
    }
    return false;
}
//NIE TESTOWANA!!
//zle dziala - wstawia tylko po jednej stronie 
bool is_good_forth(int row, int col, int old_row1, int old_col1, int old_row2, int old_col2, int old_row3, int old_col3){ //tylko sprawdzic czy wg mozna czy jest sasiad i czy row col row col
    if ((col == old_col1) && (old_col1 == old_col2))
    { //czy pionowo
        int top = min(old_row1, min(old_row2, old_row3)),
            bottom = max(old_row1, max(old_row2, old_row3));
        if ((top_count(top, col, 1, bottom, col)) && (is_neighbor(row, col, top, col)))
            return true;
        if ((bottom_count(bottom, col, 1, top, col)) && (is_neighbor(row, col, bottom, col)))
            return true;
    }
    else if ((row == old_row1) && (old_row1 == old_row2))
    { //czy poziomo
        int left = min(old_col1, min(old_col2, old_col3)),
            right = max(old_col1, max(old_col2, old_col3));
        if ((left_count(row, left, 1,row,left)) && (is_neighbor(row, col, row, left)))
            return true;
        if ((right_count(row, right, 1, row, right)) && (is_neighbor(row, col, row, right)))
            return true;
    }
    return false;
}

//procedury do wczytywania roznych typow statkow
//statek pojedynczy
void read_1ship(){
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = check_pos(row, column, 11, 11);
        check_and_insert(is_goodpos, row, column);
    }
    Serial.print("statek 1 zapisany\n");
}

void read_2ship(){
    Serial.print("wpisz pierwszy klocek\n");
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_first(row, column, 1);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row = row, old_column = column;
    show_field();
    Serial.print("wpisz drugi klocek\n");

    is_goodpos = false;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_second(row, column, old_row, old_column, 0);
        check_and_insert(is_goodpos, row, column);
    }
    Serial.print("statek 2 zapisany\n");
}

void read_3ship(){
    bool is_goodpos = false;
    int row, column;
    Serial.print("wpisz 1 klocek\n");
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_first(row, column, 2);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row1 = row, old_col1 = column;
    show_field();

    Serial.print("wpisz 2 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_second(row, column, old_row1, old_col1, 1);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row2 = row, old_col2 = column;
    show_field();

    Serial.print("wpisz 3 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_third(row, column, old_row1, old_col1, old_row2, old_col2, 0);
        check_and_insert(is_goodpos, row, column);
    }
    Serial.print("statek 3 zapisany\n");
}

void read_4ship(){
    bool is_goodpos = false;
    int row, column;
    Serial.print("wpisz 1 klocek\n");
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_first(row, column, 3);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row1 = row, old_col1 = column;
    show_field();

    Serial.print("wpisz 2 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_second(row, column, old_row1, old_col1, 2);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row2 = row, old_col2 = column;
    show_field();
    Serial.print("wpisz 3 klocek\n");

    is_goodpos = false;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_third(row, column, old_row1, old_col1, old_row2, old_col2, 1);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row3 = row, old_col3 = column;
    show_field();
    Serial.print("wpisz 4 klocek\n");

    is_goodpos = false;
    while (!is_goodpos)
    {
        insert_key(row, column);
        is_goodpos = is_good_forth(row, column, old_row1, old_col1, old_row2, old_col2, old_row3, old_col3);
        check_and_insert(is_goodpos, row, column);
    }

    Serial.print("statek 4 zapisany\n");
}
//wczytywanie wszystkich statkow
void read_ships(int count){
    int i;
    switch (count)
    { 
    case 4:
        Serial.print("Wpisz statki pojedyncze\n");
        for (i = 0; i < count; i++)
        {
            read_1ship();
            show_field();
        }
        break;
    case 3:
        Serial.print("Wpisz statki podwojne\n");
        for (i = 0; i < count; i++)
        {
            read_2ship();
            show_field();
        }
        break; 
    case 2:
        Serial.print("Wpisz statki podtrojne\n");
        read_3ship();
        show_field();
        read_3ship();
        show_field();
        break; 
    case 1:
        Serial.print("Wpisz statek czwarty\n");
        read_4ship();
        show_field();
        break;
    }
    Serial.print("Koniec wstawiania\n");
}

/*----------------------PROCES GRY----------------------------------------*/


// bool is_killed(int row,int col){
//     int maxim,minim,i;
//     if(check_pos(row,col,row,col)){

//         insert_send_data(0,row,col,col);
//         return true;
//     }
//     else if((field[row-1][col]==2) || (field[row+1][col]==2)){ 
//         //jesli w pionie jest trafiony
//         i=row;
//         while(!(field[i][col])){  //poki nie dojdziesz do pustego idz do gory
//             if(field[i][col]) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
//             minim=i; i--;
//         }
//         i=row;
//         while(!(field[i][col])){  //poki nie dojdziesz do pustego idz do dolu
//             if(field[i][col]) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
//             maxim=i;i++;
//         }
//          insert_send_data(1,col,minim,maxim);
//     }
//     else {  //jesli w poziomie jest trafiony
//         i=row;
//         while(!(field[row][i])){  //poki nie dojdziesz do pustego idz zlewa
//             if(field[row][i]) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
//             minim=i; i--;
//         }
//         i=row;
//         while(!(field[row][i])){  //poki nie dojdziesz do pustego idz z prawa
//             if(field[row][i]) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
//             maxim=i;i++;
//         }
//          insert_send_data(0,row,minim,maxim);
//     }
//     return false;
// }

//ustawia swoje statki na zabite - przerobic na uniwerslne, przekaz tablice
// void set_killed(){
//     int i,j;
//     if(data[0]){
//         j=data[1];
//         for(i=data[2];i<=data[3];i++) field[j][i];
//     }
//     else{
//         j=data[1];
//         for(i=data[2];i<=data[3];i++) field[i][j];
//     }
// }
// //zrobic uniwersalne ?
// void trafiony(int row,int col){
//     if(is_killed){     //jesli zabity to wyslij 3 i maciez data[] i ustaw swoje statki jako zabite
//         set_killed();
//         send_killed();
//     }
//     else{               //jesli nie wyslij 2 i ustaw aktualny statek na 2
//         send_hit();
//         field[row][col]=2; //ustaw jako trafiony
//     } ;              
    
// }

// void sender(){
//     bool is_goodpos = false;
//     int row, col;
//     while (!is_goodpos)
//     {
//         Serial.print("wpisz pole innego gracza");
//         insert_key(row, col); 
//         if(field2[row][col]) { //jesli pole nieznane 
//             send_to_player(row,col);
//             is_goodpos=true;
//         } 
//         int a;
//         get_answer(a);
//         if(!a) field2[row][col]=0;  //pusty
//         else if (a==2)  field2[row][col]=2; //trafiony
//         //else //killed odbior macierzy i wypewnienie przekazywanie field

//     }
// }

// void receiver(){

// }
/*----------------------PROCES GRY----------------------------------------*/

void init_void(){ 
    int i,j;
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++){
            field[i][j] =0; //moje puste
            field2[i][j]=1;//obce nieznane
        }
    }
    for (i = 0; i < 4; i++)
        read_ships(ship_count[i]);
}

bool game_over(){
    if(!lose || !win) return true;
    else return false;
}

void setup(){
    Serial.begin(9600);
    Serial.print("Start!\n");
    //init_void();
    while(!game_over){
        if(player){ //gdy jestes pierwszy
            //sender();
            //receiver(); 
        }
        else {
            //receiver();
            //sender();
        }
    }
    if(!lose) 
        Serial.print("przegrales");
    else 
        Serial.print("wygrales");
}

void loop(){
}
