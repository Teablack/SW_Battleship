int ship_count[] = {4, 3, 2, 1}; //1os-4 2os-3 3os-2 4os-1
int field[10][10];

//wczytywanie 1 liczbe
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
//wczytywanie columne albo wiersz
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
            Serial.print("bledne dane! Wpisz jeszcze raz\n"); //!przerobic na czytaj z klawiatury!
            is_good = false;
        }
    }
    return n;
}
//wczytywanie kroku
void insertion_row_col(int &row, int &col)
{

    Serial.print("wpisz wiersz: ");
    row = read_col_or_row();
    Serial.print(row);
    Serial.print("\nwpisz kolumne: ");
    col = read_col_or_row();
    Serial.print(col);
    Serial.print("\n");
}
//wstawianie elementu statku
void check_and_insert(bool is_goodpos, int row, int col)
{
    if (is_goodpos)
    {
        field[row][col] = 1;
    }
    else
        Serial.print("zla pozycja !Wpisz jeszcze raz\n");
}
//wyświetlanie pola gry
void show_field()
{
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
//czy sa to sasiednie komorki
bool is_neighbor(int row, int column, int old_row, int old_column)
{
    if (row == old_row && ((column - 1 == old_column && column != 0) || (column + 1 == old_column && column != 9)))
        return true;
    if (column == old_column && ((row - 1 == old_row && row != 0) || (row + 1 == old_row) && row != 9))
        return true;
    return false;
}
//czy mozna wstawic pojedynczy klocek
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
int top_count(int row, int col, int count, int b1, int b2)
{
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
int left_count(int row, int col, int count, int b1, int b2)
{
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
int bottom_count(int row, int col, int count, int b1, int b2)
{
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
int right_count(int row, int col, int count, int b1, int b2)
{
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
bool is_good_first(int row, int col, int count)
{
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
bool is_good_second(int row, int col, int old_row, int old_col, int count)
{
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

//NIE TESTOWANA!!
//czy trzeci klocek jest dobry do tego statku i wg dobry - zle dziala dla 3 klocku w 4os (wstawia tylko po jednej stronie) po lewej ?
bool is_good_third(int row, int col, int old_row1, int old_col1, int old_row2, int old_col2, int count)
{

    int top = 0, bottom = 0, left = 0, right = 0;

    if (

        (
            (check_pos(row, col, old_row1, old_col1)) && (is_neighbor(row, col, old_row1, old_col1))) ||
        ((is_neighbor(row, col, old_row2, old_col2)) && (check_pos(row, col, old_row2, old_col2))))
    {
        if ((col == old_col1) && (old_col1 == old_col2))
        { //jesli pionowo

            int top_row = min(row, min(old_row1, old_row2)),
                bottom_row = max(row, max(old_row1, old_row2));

            top = top_count(top_row, col, count, top_row, col); //    bottom_row, col
            if (top == count)
                return true;
            bottom = bottom_count(bottom_row, col, count - top, bottom_row, col); // top_row, col
            if (top + bottom == count)
                return true;
        }
        else if ((row == old_row1) && (old_row1 == old_row2))
        {

            int left_col = min(col, min(old_col1, old_col2)),
                right_col = max(col, max(old_col1, old_col2));

            left = left_count(row, left_col, count, row, left_col); //row right_col
            if (left == count)
                return true;

            right = right_count(row, right_col, count - left, row, right_col); // row left_col
            if (left + right == count)
                return true;
        }
    }
    return false;
}
//NIE TESTOWANA!!
//zle dziala - wstawia tylko po jednej stronie
bool is_good_forth(int row, int col, int old_row1, int old_col1, int old_row2, int old_col2, int old_row3, int old_col3)
{ //tylko sprawdzic czy wg mozna czy jest sasiad i czy row col row col
    if ((col == old_col1) && (old_col1 == old_col2))
    { //czy pionowo
        int top = min(old_row1, min(old_row2, old_row3)),
            bottom = max(old_row1, max(old_row2, old_row3));
        if ((top_count(top, col, 1, bottom, 11)) && (is_neighbor(row, col, top, col)))
            return true;
        if ((bottom_count(bottom, col, 1, top, 11)) && (is_neighbor(row, col, bottom, col)))
            return true;
    }
    else if ((row == old_row1) && (old_row1 == old_row2))
    { //czy poziomo
        int left = min(old_col1, min(old_col2, old_col3)),
            right = max(old_col1, max(old_col2, old_col3));
        if ((left_count(row, left, 1, right, 11)) && (is_neighbor(row, col, row, left)))
            return true;
        if ((right_count(row, right, 1, left, 11)) && (is_neighbor(row, col, row, right)))
            return true;
    }
    return false;
}

//procedyry do wczytywania roznych typow statkow

//statek pojedynczy
void read_1ship()
{
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = check_pos(row, column, 11, 11);
        check_and_insert(is_goodpos, row, column);
    }
    Serial.print("statek 1 zapisany\n");
}

void read_2ship()
{
    Serial.print("wpisz pierwszy klocek\n");
    bool is_goodpos = false;
    int row, column;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_first(row, column, 1);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row = row, old_column = column;
    show_field();
    Serial.print("wpisz drugi klocek\n");

    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_second(row, column, old_row, old_column, 0);
        check_and_insert(is_goodpos, row, column);
    }
    Serial.print("statek 2 zapisany\n");
}

void read_3ship()
{
    bool is_goodpos = false;
    int row, column;
    Serial.print("wpisz 1 klocek\n");
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_first(row, column, 2);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row1 = row, old_col1 = column;
    show_field();

    Serial.print("wpisz 2 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_second(row, column, old_row1, old_col1, 1);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row2 = row, old_col2 = column;
    show_field();

    Serial.print("wpisz 3 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_third(row, column, old_row1, old_col1, old_row2, old_col2, 0);
        check_and_insert(is_goodpos, row, column);
    }
    Serial.print("statek 3 zapisany\n");
}

void read_4ship()
{
    bool is_goodpos = false;
    int row, column;
    Serial.print("wpisz 1 klocek\n");
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_first(row, column, 3);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row1 = row, old_col1 = column;
    show_field();

    Serial.print("wpisz 2 klocek\n");
    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_second(row, column, old_row1, old_col1, 2);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row2 = row, old_col2 = column;
    show_field();
    Serial.print("wpisz 3 klocek\n");

    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_third(row, column, old_row1, old_col1, old_row2, old_col2, 1);
        check_and_insert(is_goodpos, row, column);
    }

    int old_row3 = row, old_col3 = column;
    show_field();
    Serial.print("wpisz 4 klocek\n");

    is_goodpos = false;
    while (!is_goodpos)
    {
        insertion_row_col(row, column);
        is_goodpos = is_good_forth(row, column, old_row1, old_col1, old_row2, old_col2, old_row3, old_col3);
        check_and_insert(is_goodpos, row, column);
    }

    Serial.print("statek 4 zapisany\n");
}
//wczytywanie wszystkich statkow
void read_ships(int count)
{
    switch (count)
    { 
    case 4:
        Serial.print("Wpisz statki pojedyncze\n");
        for (int i = 0; i < count; i++)
        {
            read_1ship();
            show_field();
        }
        break;
    case 3:
        Serial.print("Wpisz statki podwojne\n");
        for (int i = 0; i < count; i++)
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
