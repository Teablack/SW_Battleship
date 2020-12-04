int ship_count[] = {4, 3, 2, 1}; //1os-4 2os-3 3os-2 4os-1
int field[10][10];

int read_sign()
{
    int sign = Serial.parseInt();
    return sign;
}

void swap(int &a, int &b)
{
    int t = a;
    a = b;
    b = t;
}

int read_col_or_row()
{
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
            Serial.print("błędne dane! Wpisz jeszcze raz\n"); //!przerobic na czytaj z klawiatury!
            is_good = false;
        }
    }
    return n;
}

void insertion_row_col(int &row,int &col){

        Serial.print("wpisz wiersz:\n");
        row = read_col_or_row();
        Serial.print("wpisz kolumne:\n");
        col = read_col_or_row();

}

void check_and_insert(bool is_goodpos,int row,int col){
    if (is_goodpos)
        {
            field[row][col] = 1;
        }
        else Serial.print("zla pozycja !Wpisz jeszcze raz\n");
}

void show_field()
{ //wyświetlam pole gry
    Serial.print("\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
            Serial.print(field[i][j]);
        Serial.print("\n");
    }
}

bool is_neighbor(int row, int column, int old_row, int old_column)
{
    if (row == old_row && ((column - 1 == old_column && column != 0) || (column + 1 == old_column && column != 9)))
        return true;
    if (column == old_column && ((row - 1 == old_row && row != 0) || (row + 1 == old_row) && row != 9))
        return true;
    return false;
}

bool check_pos(int row, int column, int old_row, int old_column) 
{
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
            if (field[i][j] && i != old_row && j != old_column)
                return false;
        }
    }
    return true;
}
bool check_pos_for_2(int row, int column)
{
    if (check_pos(row, column, 11, 11))
    {
        if (row!=0 && check_pos(row - 1, column, 11, 11))
            return true;
        if (row != 9 && check_pos(row + 1, column, 11, 11))
            return true;
        if (column!=0 && check_pos(row, column - 1, 11, 11))
            return true;
        if (column != 9 && check_pos(row, column + 1, 11, 11))
            return true;
    }
    return false;
}
void read_1ship_pos()
{ //czytam statek pojedynczy 
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insertion_row_col(row,column);
        is_goodpos = check_pos(row, column, 11, 11);
        check_and_insert(is_goodpos,row,column);
    }
}

void read_2ship_pos() 
{
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insertion_row_col(row,column);
        is_goodpos = check_pos_for_2(row, column); 
        check_and_insert(is_goodpos,row,column);
    }
    int old_row = row, old_column = column;

    Serial.print("wpisz drugi klocek\n");
    show_field();

    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row,column);
        is_goodpos=is_neighbor(row, column, old_row, old_column);
        is_goodpos = is_goodpos &&(check_pos(row, column, old_row, old_column));
        check_and_insert(is_goodpos,row,column);
    }
}

bool check_pos_for3(int row, int column)
{ //sprawdza czy dla danego pola mozna dostawic jeszcze dwa klocki
    if (check_pos(row, column, 11, 11))
    {
        if (column >= 2 && check_pos(row, column - 1, 11, 11) && check_pos(row, column - 2, 11, 11))
            return true;
        if (column <= 7 && check_pos(row, column + 1, 11, 11) && check_pos(row, column + 2, 11, 11))
            return true;
        if (row >= 2 && check_pos(row - 1, column, 11, 11) && check_pos(row - 2, column, 11, 11))
            return true;
        if (row <= 7 && check_pos(row + 1, column, 11, 11) && check_pos(row + 2, column, 11, 11))
            return true;
        if (row >= 1 && row >= 8 && check_pos(row - 1, column, 11, 11) && check_pos(row + 1, column, 11, 11))
            return true;
        if (column >= 1 && column >= 8 && check_pos(row, column + 1, 11, 11) && check_pos(row, column - 1, 11, 11))
            return true;
    }
    return false;
}

bool check_pos_for3_2(int row, int col, int old_row1, int old_col1)
{ //czy moge wstawić 2 klocek do statku z 3 kloc
    if (is_neighbor(row, col, old_row1, old_col1))
    {
        if (check_pos(row, col, old_row1, old_col1))
        {
            if (row == old_row1)
            {
                int left = col,
                    right = old_col1;
                if (col > old_col1)
                    swap(left, right);
                if (left && (check_pos(row, left - 1, old_row1, old_col1)) || (right != 9 && check_pos(row, right + 1, old_row1, old_col1)))
                    return true;
            }
            else
            {
                int top = row,
                    bottom = old_row1;
                if (row > old_row1)
                    swap(top, bottom);
                if ((top && check_pos(top - 1, col, old_row1, old_col1)) || (bottom != 9 && check_pos(bottom + 1, col, old_row1, old_col1)))
                    return true;
            }
        }
    }
    return false;
}

bool check_pos_for3_3(int row, int col, int old_row1, int old_col1, int old_row2, int old_col2)
{
    if ((row == old_row1 && old_row1 == old_row2) || (col == old_col1 && old_col1 == old_col2))
    {
        if (is_neighbor(row, col, old_row1, old_col1) && check_pos(row, col, old_row1, old_col1))
            return true;
        if (is_neighbor(row, col, old_row2, old_col2) && check_pos(row, col, old_row2, old_col2))
            return true;
    }
    return false;
}

void read_3ship_pos()
{ 
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insertion_row_col(row,column);

        is_goodpos = check_pos_for3(row, column);
        check_and_insert(is_goodpos,row,column);
    }
    int old_row1 = row, old_col1 = column;

    show_field();

    Serial.print("wpisz 2 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row,column);
        is_goodpos = check_pos_for3_2(row, column, old_row1, old_col1);
        check_and_insert(is_goodpos,row,column);
    }
    int old_row2 = row, old_col2 = column;
    show_field();
    Serial.print("wpisz 3 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row,column);
        is_goodpos = check_pos_for3_3(row, column, old_row1, old_col1, old_row2, old_col2);
        check_and_insert(is_goodpos,row,column);
    }
    Serial.print("statek3 zapisany\n");
}

bool check_pos_for4(int row,int col){
    if (check_pos(row, col, 11, 11))
    {
        if(row>=0 && check_pos(row+1,col,11,11)&& check_pos(row+2,col,11,11)&& check_pos(row+3,col,11,11)) return true;
        if(row<=9 && check_pos(row-1,col,11,11)&& check_pos(row-2,col,11,11)&& check_pos(row-3,col,11,11)) return true;
        if(row>=1 && check_pos(row-1,col,11,11)&& check_pos(row+1,col,11,11)&& check_pos(row+2,col,11,11)) return true;
        if(row<=8 && check_pos(row+1,col,11,11)&& check_pos(row-1,col,11,11)&& check_pos(row-2,col,11,11)) return true;
        if(row>=2 && check_pos(row-1,col,11,11)&& check_pos(row-2,col,11,11)&& check_pos(row+1,col,11,11)) return true;
        if(row<=7 && check_pos(row+1,col,11,11)&& check_pos(row+2,col,11,11)&& check_pos(row-1,col,11,11)) return true;
        if(row>=3 && check_pos(row-1,col,11,11)&& check_pos(row-2,col,11,11)&& check_pos(row-3,col,11,11)) return true;
        if(row<=6 && check_pos(row+1,col,11,11)&& check_pos(row+2,col,11,11)&& check_pos(row+3,col,11,11)) return true;

        if(col>=0 && check_pos(row,col+1,11,11)&& check_pos(row,col+2,11,11)&& check_pos(row,col+3,11,11)) return true;
        if(col<=9 && check_pos(row,col-1,11,11)&& check_pos(row,col-2,11,11)&& check_pos(row,col-3,11,11)) return true;
        if(col>=1 && check_pos(row,col-1,11,11)&& check_pos(row,col+1,11,11)&& check_pos(row,col+2,11,11)) return true;
        if(col<=8 && check_pos(row,col+1,11,11)&& check_pos(row,col-1,11,11)&& check_pos(row,col-2,11,11)) return true;
        if(col>=2 && check_pos(row,col-1,11,11)&& check_pos(row,col-2,11,11)&& check_pos(row,col+1,11,11)) return true;
        if(col<=7 && check_pos(row,col+1,11,11)&& check_pos(row,col+2,11,11)&& check_pos(row,col-1,11,11)) return true;
        if(col>=3 && check_pos(row,col-1,11,11)&& check_pos(row,col-2,11,11)&& check_pos(row,col-3,11,11)) return true;
        if(col<=6 && check_pos(row,col+1,11,11)&& check_pos(row,col+2,11,11)&& check_pos(row,col+3,11,11)) return true;

    }

    return false;
}

void read_4ship_pos(){
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insertion_row_col(row,column);
        is_goodpos = check_pos_for4(row,column);
        check_and_insert(is_goodpos,row,column);
    }
    int old_row1 = row, old_col1 = column;
    show_field();
}
void read_ships(int count)
{
    switch (count)
    {
    case 4:
        Serial.print("Wpisz statki pojedyncze\n");
        for (int i = 0; i < count; i++)
        {
            read_1ship_pos();
            show_field();
        }
        break;
    case 3:
        Serial.print("Wpisz statki podwojne\n");
        for (int i = 0; i < count; i++)
        {
            read_2ship_pos();
            show_field();
        }
        break;
    case 2:
        Serial.print("Wpisz statki podtrojne\n");
        read_3ship_pos();
        show_field();
        read_3ship_pos();
        show_field();
        break; 
      case 1: 
        Serial.print("Wpisz statek czwarty\n");
      	read_4ship_pos();
        show_field();
      	break; 
    }
    Serial.print("/n Koniec wstawiania\n");
}

void init_void()
{ //procedura init; ustawiam wsz wartosci pola na puste
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
            field[i][j] = 0;
    }
    for (int i = 0; i < 4; i++)
        read_ships(ship_count[i]);
}

void setup()
{
    Serial.begin(9600);
    Serial.print("Start!\n");
    init_void();
}

void loop()
{
}
