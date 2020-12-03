int ship_count[] ={4,3,2,1}; //1os-4 2os-3 3os-2 4os-1
int field[10][10];

int read_sign(){
  int sign=Serial.parseInt();
  return sign;
}

int read_col_or_row(){
	bool is_good=false;
  	int n;
	while(!is_good){
		is_good=true;
  		while(!(Serial.available()>0)){}
  		n=read_sign();
      	if(n>9||n<0) {
     		Serial.print("błędne dane!");	//!przerobic na czytaj z klawiatury!	
     		is_good=false;
        }
    }
return  n;     
}  

void show_field(){//wyświetlam pole gry
  	Serial.print("\n");
	for(int i=0;i<10;i++){	
    	for(int j=0;j<10;j++)Serial.print(field[i][j]);
    	Serial.print("\n");
  	}
}

bool check_pos(int row,int column){
  int 	left=	column-1,		
  		right=	column+1,
  		top=	row-1,
        bottom=	row+1;
  //sprawdzam czy są ścianki
  if(!column)		left=column;
  else if(column==9)right=column;
  if(!row)			top=column;
  else if(row==9)	bottom=column;
    
  for(int i=left;i<=right;i++){
  	for(int j=top;j<=bottom;j++){
      if(field[i][j]) return false;
  	}
  }
	return true;	
}
void read_1ship_pos(){	//czytam pojedynczy statek
  	bool is_goodpos=true;
  	int row,column;
  	
      	Serial.print("wpisz wiersz\n");
  		while(!(Serial.available()>0)){}
  		row=read_sign();
   
      	Serial.print("wpisz kolumne\n");
  		while(!(Serial.available()>0)){}
      	column=read_sign();
    
    is_goodpos=check_pos(row,column); 
  	if(is_goodpos){
    	field[row][column]=1;
    }
  	else Serial.print("błędny krok!\n");
}

void read_ships(int count){
	switch (count){
      case 4:
      	Serial.print("Wpisz pozycje statkow1\n");
      	for(int i=0;i<count;i++)read_1ship_pos();
      	show_field();
        break;	/*	
      case 3:
        for(int i=0;i<count;i++)read_2ship_pos();
        break;
      case 2:
      	read_3ship_pos();
      	read_3ship_pos();
      	break;
      case 1:
      	read_4ship_pos();
      	break; */
    }
}

void init_void(){ 	//procedura init; ustawiam wsz wartosci pola na puste
  for(int i=0;i<10;i++){	
    for(int j=0;j<10;j++) field[i][j]=0;
  }
  for(int i=0;i<4;i++) read_ships(ship_count[i]);
}

void setup()
{
  Serial.begin(9600);
  Serial.print("Start!");
  init_void();
}

void loop()
{
  
 
}
