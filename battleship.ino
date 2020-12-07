#define  player 1

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

int read_dir(){
    bool is_good = false;
    int n;
    while (!is_good)
    {
        is_good = true;
        while (!(Serial.available() > 0))
        {
        }
        n = read_sign();
        if (n > 4 || n < 1)
        {
            Serial.print("bledne dane! Wpisz jeszcze raz\n"); //!przerobic na czytaj z klawiatury!
            is_good = false;
        }
    }
    return n;
}
//wczytywanie kroku
void insert_key_with_dir(int &row, int &col, int &dir){

    Serial.print("wpisz wiersz: ");
    row = read_col_or_row();
    Serial.print(row);
    Serial.print("\nwpisz kolumne: ");
    col = read_col_or_row();
    Serial.print(col);;
    Serial.print("\nwpisz kierunek: ");
    dir = read_dir();
    Serial.print(dir);
    Serial.print("\n");
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
    if(a==3)Serial.print("wysłano ze zabite : ");
    else if(a==2)Serial.print("wysłano ze trafiono : ");
    Serial.print(a);
    Serial.print("\n");
}

void receive_answer(int &a){
    Serial.print("wprowadz odpowiedz\n");
    a=read_col_or_row();
}
//wyslij 2 do 2 gracza i data[] 
void send_data(){
    Serial.print("Wyslano datagram\n");
}

void receive_data(){
    Serial.print("odebrales datagram");
    for(int i=0;i<4;i++){
        Serial.print("znak:");
        data[i]=read_col_or_row();
    }
}

void insert_in_data(int dir,int k,int k1,int k2){
    data[0]=dir;
    data[1]=k;
    data[2]=k1;
    data[3]=k2;
}
//dla testow
void print_data(){
    for(int i =0;i<4;i++){
        Serial.print(data[i]);
    }
}
/*----------------------KOMUNIKACJA-----------------------------------------*/

//wyświetlanie pola gry //przerobic na taka z przekazywana tabela
void show_field(int (&f)[10][10]){
    Serial.print("  0 1 2 3 4 5 6 7 8 9 ");
    Serial.print("\n");
    for (int i = 0; i < 10; i++)
    {
        Serial.print(i);
        Serial.print(" ");
        for (int j = 0; j < 10; j++)
        {
            Serial.print(f[i][j]);
            Serial.print(" ");
        }
        Serial.print("\n");
    }
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

//procedury do wczytywania roznych typow statkow
//statek pojedynczy
void read_ship(int size){
    bool is_goodpos=false,is_good_ship = false;
    int row, col,dir;
    while (!is_goodpos)
    {
        insert_key_with_dir(row,col,dir);
        is_goodpos = check_pos(row, col, 11, 11);
        is_good_ship=is_good_and_insert(row,col,dir,size);
        if(is_goodpos&&is_good_ship) Serial.print("wstawiono\n");
        else is_goodpos=false;
    }
}

bool is_good_and_insert(int row, int col, int dir, int size)
{
    int i;
    switch (dir){
    case 1:
        if (top_count(row, col, size - 1, 11, 11) == (size - 1)){
            
            for (i = col; i > col - size; i--)
                field[i][row] = 1;
            return true;    
        }   
        break;
    case 2:
        if (bottom_count(row, col, size - 1, 11, 11) == (size - 1)){
            
            for (i = col; i < col + size; i++)
                field[i][row] = 1;
            return true;
        }
        break;
    case 3:
        if (left_count(row, col, size - 1, 11, 11) == (size - 1)){
            
            for (i = row; i > row - size; i--)
                field[i][col] = 1;
            return true;
        }
        break;
    case 4:
        if (right_count(row, col, size - 1, 11, 11) == (size - 1)){
            
            for (i = row; i < row + size; i++)
                field[i][col] = 1;
            return true;    
        }
        break;
    }
    return false;
}
//wczytywanie wszystkich statkow
void read_ships(){
    int i;
    
        Serial.print("Wpisz statki pojedyncze\n");
        for(i=0;i<4;i++) {
            read_ship(1);
            show_field(field);
        }
        Serial.print("Wpisz statki podwojne\n");
        for(i=0;i<3;i++) {
            read_ship(2);
            show_field(field);
        }
        Serial.print("Wpisz statki podtrojne\n");
        for(i=0;i<2;i++) {
            read_ship(3);
            show_field(field);
        }
        Serial.print("Wpisz statek czworke\n");
        read_ship(4);
        show_field(field);
        Serial.print("Koniec wstawiania\n");
}

/*----------------------PROCES GRY----------------------------------------*/

bool is_killed(int row,int col){

    int maxim=0,minim=0,i;
        if((field[row-1][col]==2) || (field[row+1][col]==2)){ 
        //jesli w pionie jest trafiony
    
        i=row;
        while((field[i][col])){  //poki nie dojdziesz do pustego idz do gory  (poki jest 1 albo 2)
            if(i<0) break;
            if((field[i][col])==1) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
            minim=i; i--;
        }
        i=row;
        while((field[i][col])){  //poki nie dojdziesz do pustego idz do dolu (poki jest 1 albo 2)
            
            if(i>9) break;
            if((field[i][col])==1) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
            maxim=i;i++;
        }
         insert_in_data(1,col,minim,maxim);
    }
    else if((field[row][col-1]==2) || (field[row][col+1]==2)) {  //jesli w poziomie jest trafiony
        i=col;
        while((field[row][i])){  //poki nie dojdziesz do pustego idz zlewa
            if(i<0) break;
            if((field[i][col])==1) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
            minim=i;  i--;                 
        }
        i=col;
        while((field[row][i])){  //poki nie dojdziesz do pustego idz z prawa
            
            if(i>9) break;
            if((field[i][col])==1) return false;     //jesli znajdziesz statek w ktory nie trafiono wyjdz
            maxim=i; i++;
            
        }
        insert_in_data(0,row,minim,maxim);
    }
    else return false;
    Serial.print("\n");
    print_data();
    return true;
}
//ustawia statki na zabite 
void set_killed(int (&f)[10][10], int my){
    int i,j;
    if(data[0]){
        j=data[1];
        for(i=data[2];i<=data[3];i++) {
            f[i][j]=3;
            if(my) lose--;
            else win--;
        };
        
    }
    else{
        j=data[1];
        for(i=data[2];i<=data[3];i++) {
            f[j][i]=3;
            if(my) lose--;
            else win--;
        }
    }
}

void hit(int row,int col){
    field[row][col]=2; //ustaw jako hit
    bool a = is_killed(row,col);
    if(a){     //jesli zabity to wyslij 3 i maciez data[] i ustaw swoje statki jako zabite
        
        set_killed(field,1);
        send_answer(3);
        send_data();
        show_field(field);

    }
    else{               //jesli nie wyslij 2 i ustaw aktualny statek na 2
        send_answer(2);
    };              
    
}

void sender(){
    bool again=true;
    while(again){
        bool is_goodpos = false;
        int row, col;
        again=false;
        while (!is_goodpos)
        {
            Serial.print("wpisz pole innego gracza\n");
            insert_key(row, col); 
            if(field2[row][col]==1) { //jesli pole nieznane (=1)
                send_location(row,col);
                is_goodpos=true;
                int a;
                receive_answer(a);
                if(!a) {
                    Serial.print("nie trafiles\n");
                    field2[row][col]=0;
                
                }  //pusty
                else if (a==2) { 
                    Serial.print("Trafiles\n");
                    field2[row][col]=2; 
                    again=true;
                }
                else {
                    Serial.print("zabiles\n");
                    receive_data();
                    set_killed(field2,0); 
                    again=true;} //killed odbior macierzy i wypewnienie na killed !napisac fukcje uniw z przekaz argumentow
            }   
            else Serial.print("Pole juz bylo wybierane!\n");
        }
        show_field(field2);
    }
}

void receiver(){
    bool again=true;
    while(again){
        int row, col;
        again=false;
        Serial.print("inny gracz wysyla do ciebie \n");
        receive_location(row,col);
        if(field[row][col]==0) send_answer(0);  //jesli pusty
        else {
            hit(row,col);
            again=true;
        }
        show_field(field2);
    }
}

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
    Serial.println("Wpisz statki");
    read_ships();
}

bool game_over(){
    if((lose!=0) && (win!=0)) return false;
    else return true;
}

void run_game(){
    bool a;
    if(player){ //gdy jestes pierwszy
            show_field(field2);
            sender();
            a=game_over();
            if(!a)receiver(); 
            show_field(field);
    }
    else {
            receiver();
            show_field(field2);
            a=game_over();
            if(!a)sender();
            show_field(field);
    }
}
void test1(){
    //jednoos
    field[0][3]=1;
    field[0][7]=1;
    field[9][5]=1;
    field[9][9]=1;
    //dwuos
    field[2][5]=1;
    field[3][5]=1;
    field[2][9]=1;
    field[3][9]=1;
    field[4][2]=1;
    field[4][3]=1;
    //trzyos
    for(int i=7;i<10;i++)  field[i][2]=1;
    for(int i=5;i<8;i++)  field[i][7]=1;
    for(int i=1;i<5;i++)  field[i][0]=1;
    show_field(field);
    Serial.print("\n");
}
void setup(){
    Serial.begin(9600);
    
    init_void();
    // //test1();
    // bool a=game_over();
    // while(!a){
    //     run_game();
    // }
    // if(!lose) 
    //     Serial.print("przegrales");
    // else
    //     Serial.print("wygrales");

}

void loop(){
}
