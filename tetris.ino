
#include "LedControl.h"
#include <TM1637Display.h>

#define CLK 7
#define DIO 8

TM1637Display display = TM1637Display(CLK, DIO);

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin A1 is connected to the DataIn 
 pin A0 is connected to the CLK 
 pin 12 is connected to LOAD 
 We have only a single MAX72XX.
 */
//LedControl lc = LedControl(A1,A0,12,1);


LedControl lc = LedControl(9,11,10,1);

int row = 0;

int val = 0;

int column = 4;

int temp_count = 0;

int rotation_index = 0;

int count_gameover = 0 ;

unsigned int time_interval = 1000;



unsigned int global_random_no ;



unsigned int ground_flag = 0;

unsigned int bottom_obstacle_flag = 0;

unsigned int block_gen_flag = 1;

unsigned int moving_map_element[8][8];

unsigned int static_map_element[9][9];

unsigned int dec_score;  

unsigned int _1st_digit , _2nd_digit ;

/* we always wait a bit between updates of the display */
unsigned long delaytime=100;


unsigned int button_pressed_D2 = 0 ;
unsigned long time_button_pressed_D2 ;
unsigned long time_button_bounce_D2 ;

unsigned int button_pressed_D3 = 0 ;
unsigned long time_button_pressed_D3 ;
unsigned long time_button_bounce_D3 ;

unsigned int button_pressed_D4 = 0 ;
unsigned long time_button_pressed_D4 ;
unsigned long time_button_bounce_D4 ;

unsigned int button_pressed_D5 = 0 ;
unsigned long time_button_pressed_D5 ;
unsigned long time_button_bounce_D5 ;

unsigned long t1, t2;

unsigned int time_lock_flag = 0 ;







void setup() {

  Serial.begin(9600);

  randomSeed(analogRead(0));

  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);

  pinMode(A0, OUTPUT);


  
  
  //analogPinInit();
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,0);
  /* and clear the display */
  lc.clearDisplay(0);

  initial_set_underground();

  
} //end of setup()


void block_gen()
{
    unsigned int _random_no ;
  //unsigned int _random_no = random(4);

  if(static_map_element[1][column] == 1)
  gameover();

  if(block_gen_flag == 1) // If block_gen_flag = 1, a random number for the case selection will be picked
  {
  _random_no = random(4);
  global_random_no = _random_no;
  }
  else
  {
    _random_no = global_random_no;
  }
  

  //global_random_no = _random_no ;
  
  //Serial.print("Random no. is : ");
  //Serial.println(_random_no); 
 
  switch(_random_no){
 
    case 0 : // block 田
    moving_map_element[row][column-1] = 1;
    moving_map_element[row][column] = 1;
    moving_map_element[row + 1][column - 1] = 1;
    moving_map_element[row + 1][column] = 1;


    break;

    case 1 : // block "small L"
    if(rotation_index == 0)
    {
    moving_map_element[row][column] = 1;
    moving_map_element[row + 1][column] = 1;
    moving_map_element[row + 1][column + 1] = 1; 
    }   

    if(rotation_index == 1)
    {
    moving_map_element[row][column] = 1;
    moving_map_element[row][column + 1] = 1;
    moving_map_element[row + 1][column] = 1; 
    }


    if(rotation_index == 2)
    {
    moving_map_element[row][column + 1] = 1;
    moving_map_element[row + 1][column] = 1;
    moving_map_element[row + 1][column + 1] = 1; 
    }

    if(rotation_index > 2)
    rotation_index = 0 ;
  
    break;

    case 2 : // block |
    
    if(rotation_index == 0)
    {
    moving_map_element[row][column] = 1;
    moving_map_element[row + 1][column] = 1;
    }

    if(rotation_index == 1)
    {
    moving_map_element[row][column] = 1;
    moving_map_element[row][column + 1] = 1;
    }

    if(rotation_index > 1)
    rotation_index = 0;
    break;

    case 3 : // block .
    moving_map_element[row][column] = 1;
    break;   
  }

  block_gen_flag = 0;
 
  } // end of block_gen()
  

void bottom_detect()
{
  if(global_random_no == 0 && (static_map_element[row + 2][column] == 1 || static_map_element[row + 2][column - 1] == 1)) 
  {
  bottom_obstacle_flag = 1;
  
  //Serial.println("THERE IS STATIC BLOCK ON BOTTOM !!!");
  
  ground_flag = 1;
  set_static_led();
  bottom_obstacle_flag = 0 ;  
  }
 
  if(global_random_no == 1 && rotation_index == 0 &&((static_map_element[row + 2][column] == 1) || (static_map_element[row + 2][column + 1] == 1)))
  {
    //Serial.println("WTF");
    //Serial.print("row in WTF: ");
    //Serial.println(row);
    //Serial.print("column in WTF: ");
    //Serial.println(column);
  bottom_obstacle_flag = 1;
  //Serial.println("THERE IS STATIC BLOCK ON BOTTOM !!!");
  ground_flag = 1;
  set_static_led();
  bottom_obstacle_flag = 0 ;   
  }

  if(global_random_no == 1 && rotation_index == 1 &&((static_map_element[row + 2][column] == 1) || (static_map_element[row + 1][column + 1] == 1)))
  {
   
  bottom_obstacle_flag = 1;
  //Serial.println("THERE IS STATIC BLOCK ON BOTTOM !!!");
  ground_flag = 1;
  set_static_led();
  bottom_obstacle_flag = 0 ;   
  }

  if(global_random_no == 1 && rotation_index == 2 &&((static_map_element[row + 2][column] == 1) || (static_map_element[row + 2][column + 1] == 1)))
  {
  bottom_obstacle_flag = 1;
  //Serial.println("THERE IS STATIC BLOCK ON BOTTOM !!!");
  ground_flag = 1;
  set_static_led();
  bottom_obstacle_flag = 0 ;   
  }

  if(global_random_no == 2 && rotation_index == 0 && static_map_element[row + 2][column] == 1)
  {
  bottom_obstacle_flag = 1;
  //Serial.println("THERE IS STATIC BLOCK ON BOTTOM !!!");
  ground_flag = 1;
  set_static_led();
  bottom_obstacle_flag = 0 ;   
  }
  
  if(global_random_no == 2 && rotation_index == 1 && (static_map_element[row + 1][column] == 1 || static_map_element[row + 1][column + 1] == 1))
  {
  bottom_obstacle_flag = 1;
  //Serial.println("THERE IS STATIC BLOCK ON BOTTOM !!!");
  ground_flag = 1;
  set_static_led();
  bottom_obstacle_flag = 0 ;   
  }

  //if(global_random_no == 2 && rotation_index == 1 && row == 0 && (static_map_element[1][column] == 1 || static_map_element[1][column + 1] == 1))
  //gameover();


  if(global_random_no == 3  && static_map_element[row + 1][column] == 1)
  {
  bottom_obstacle_flag = 1;
  //Serial.println("THERE IS STATIC BLOCK ON BOTTOM !!!");
  ground_flag = 1;
  set_static_led();
  bottom_obstacle_flag = 0 ;   
  }

  //if(global_random_no == 3  && row == 0 && static_map_element[1][column] == 1)
  //gameover();

  
} // end of bottom_detect()


void block_fall()
{
   if(time_lock_flag == 0)
   {
   t1 = millis();
   time_lock_flag = 1;
   }

   t2 = millis();

 if(t2 - t1 >= time_interval)
 {

 Serial.println("BLOCK FALL !!!");

 time_lock_flag = 0;
 
 if((row < 6) && ((global_random_no == 0 ) || (global_random_no == 1) || (global_random_no == 2 && rotation_index == 0)) && ((bottom_obstacle_flag == 0) || ground_flag == 0))
 {
  row++;
  //Serial.println("row has been deduced by 1 , size = 2");
 }


if(row < 7 && global_random_no == 3 && (bottom_obstacle_flag == 0 || ground_flag == 0))
{
row++;
//Serial.println("row has been deduced by 1, size = 1");
}

if(row < 7 && global_random_no == 2 && rotation_index == 1 && (bottom_obstacle_flag == 0 || ground_flag == 0))
{
row++;
//Serial.println("row has been deduced by 1, size = 1");
}
 }

} //end of block_fall()


void old_block_fall_clear() //used to clear the previous block during falling
{
  //Serial.println("old_block_fall_clear() has been executed");
   switch(global_random_no){

    case 0 : // block 田
   
    moving_map_element[row][column-1] = 0;
    moving_map_element[row][column] = 0;
    moving_map_element[row + 1][column - 1] = 0;
    moving_map_element[row + 1][column] = 0;
    
    break;

    case 1 : // block L
   if(rotation_index == 0)
    {
    moving_map_element[row][column] = 0;
    moving_map_element[row + 1][column] = 0;
    moving_map_element[row + 1][column + 1] = 0; 
    }   

    if(rotation_index == 1)
    {
    moving_map_element[row][column] = 0;
    moving_map_element[row][column + 1] = 0;
    moving_map_element[row + 1][column] = 0; 
    }

    /*if(rotation_index == 2)
    {
    moving_map_element[row][column] = 01;
    moving_map_element[row][column + 1] = 0;
    moving_map_element[row + 1][column + 1] = 0; 
    }*/

    if(rotation_index == 2)
    {
    moving_map_element[row][column + 1] = 0;
    moving_map_element[row + 1][column] = 0;
    moving_map_element[row + 1][column + 1] = 0; 
    }

    if(rotation_index > 1)
    rotation_index = 0 ;
    
    break;

    case 2 : // block |

    if(rotation_index == 0)
    {
    moving_map_element[row][column] = 0;
    moving_map_element[row + 1][column] = 0;
    }

     if(rotation_index == 1)
    {
    moving_map_element[row][column] = 0;
    moving_map_element[row][column + 1] = 0;
    }

    if(rotation_index > 1)
    rotation_index = 0;
    
    break;
    

    case 3 : // block .
    moving_map_element[row][column] = 0;
    break;   
  }
} //End of old_block_fall_clear()


void block_control()
{ 

  int reading2 = digitalRead(3);
  int reading3 = digitalRead(2);
  int reading4 = digitalRead(4);
  
  //if(reading2 == 0)
  //{

    if(reading2 == 0 && button_pressed_D2 == 0)
    {
      button_pressed_D2 = 1;
      time_button_pressed_D2 = millis();
    }

    if(reading2 == 1 && button_pressed_D2 == 1)
  { 
  time_button_bounce_D2 = millis();

  if(time_button_bounce_D2 - time_button_pressed_D2 > 50)
  {
  Serial.println("Button D2 has been pressed");

  if((global_random_no == 0) && (static_map_element[row][column + 1] != 1) && (static_map_element[row + 1][column + 1] != 1) && column < 7)
    column++ ;


    if(global_random_no == 1 && rotation_index == 0 && static_map_element[row][column + 1] != 1 && static_map_element[row][column + 2] != 1 && column < 6)
    column++ ;

    if(global_random_no == 1 && rotation_index == 1 && static_map_element[row][column + 2] != 1 && static_map_element[row + 1][column + 1] != 1 && column < 6)
    column++ ;

    if(global_random_no == 1 && rotation_index == 2 && static_map_element[row][column + 2] != 1 && static_map_element[row + 1][column + 2] != 1 && column < 6)
    column++ ;


    if(global_random_no == 2 && rotation_index == 0 && static_map_element[row][column + 1] != 1 && static_map_element[row + 1][column + 1] != 1 && column < 7)
    column++ ;

    if(global_random_no == 2 && rotation_index == 1 && static_map_element[row][column + 2] != 1 && column < 6)
    column++ ;


    if(global_random_no == 3 && static_map_element[row][column + 1] != 1 && column < 7)
    column++ ; 
  }
  button_pressed_D2 = 0;
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    

   if(reading3 == 0 && button_pressed_D3 == 0)
    {
      button_pressed_D3 = 1;
      time_button_pressed_D3 = millis();
    }

    if(reading3 == 1 && button_pressed_D3 == 1)
  { 
  time_button_bounce_D3 = millis();

  if(time_button_bounce_D3 - time_button_pressed_D3 > 50)
  {
  Serial.println("Button D3 has been pressed");
  if((global_random_no == 0) && (static_map_element[row][column - 2] != 1) && (static_map_element[row + 1][column - 2] != 1) && column > 1)
    column--;

    if(global_random_no == 1 && rotation_index == 0 && static_map_element[row][column - 1] != 1 && static_map_element[row + 1][column - 1] != 1 && column > 0)
    column-- ;

    if(global_random_no == 1 && rotation_index == 1 && static_map_element[row][column - 1] != 1 && static_map_element[row + 1][column - 1] != 1 && column > 0)
    column-- ;

    if(global_random_no == 1 && rotation_index == 2 && static_map_element[row + 1][column - 1] != 1 && static_map_element[row][column] != 1 && column > 0)
    column-- ;


    if(global_random_no == 2 && rotation_index == 0 && static_map_element[row][column - 1] != 1 && static_map_element[row - 1][column + 1] != 1 && column > 0)
    column-- ;

    if(global_random_no == 2 && rotation_index == 1 && static_map_element[row][column - 1] != 1 && column > 0)
    column-- ;


    if(global_random_no == 3 && static_map_element[row][column - 1] != 1 && column > 0)
    column-- ;

  
  }
  button_pressed_D3 = 0;
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

if(reading4 == 0 && button_pressed_D4 == 0)
    {
      button_pressed_D4 = 1;
      time_button_pressed_D4 = millis();
    }

    if(reading4 == 1 && button_pressed_D4 == 1)
  { 
  time_button_bounce_D4 = millis();

  if(time_button_bounce_D4 - time_button_pressed_D4 > 50)
  {
  Serial.println("Button D4 has been pressed");
  rotation_index++;
  }
  button_pressed_D4 = 0;
  }
  
} //end of block_control()



void row_bingo()
{
  int i, j; 
  int bingo_count = 0 ;

 for(i = 0 ; i < 8 ; i++)
 {
  for(j = 0 ; j < 8 ; j++)
  {
    if(static_map_element[i][j] == 1)
    bingo_count++ ;
  }
    if(bingo_count == 8) //cancel the row full of 8 ON LEDs
    {
      for(j = 0 ; j < 8 ; j++)
      static_map_element[i][j] = 0;
      led_move_down(i);  
      dec_score++;
      time_interval -= 7 ;
      
      beep(800, 300); // BEEP SOUND FUNCTION
    }
  bingo_count = 0;
  
 } //end of for-loop i
 //Serial.print("SCORE = ");
 //Serial.println(dec_score);
} // end of row_bingo()



void led_move_down(int cancelled_row)
{
   int i,j ;
   //for(i = 0 ; i < 8 ; i++)
   for(i = 7 ; i >= 0 ; i--)
   {
    for(j = 0 ; j < 8 ; j++)
    {
      if((static_map_element[i][j] == 1) && (i < cancelled_row)){
      static_map_element[i][j] = 0 ;
      static_map_element[i + 1][j] = 1;
      }
    }
   }
} // end of led_move_down()




void left_right_detect()
{
  
}



void set_led()
{
  int i,j;

  for(i=0 ; i<8 ; i++)
  {
    for(j=0 ; j<8 ; j++)
    {
      if((moving_map_element[i][j]==1) || (static_map_element[i][j]==1))
      lc.setLed(0, i, j, true);
      else
      lc.setLed(0, i, j, false);
    }
  }
}


void set_static_led()
{
  int i,j ;
  if(ground_flag == 1 || bottom_obstacle_flag == 1)
  {
    for(i=0 ; i<8 ; i++)
    {
      for(j=0 ; j<8 ; j++)
      {
        if(static_map_element[i][j] == 0)
        static_map_element[i][j] = moving_map_element[i][j];
      }
     
    }

    
    for(i=0 ; i<8 ; i++)
    {
      for(j=0 ; j<8 ; j++)
      moving_map_element[i][j] = 0;    
    }
    //Serial.println("Function set_static_led() is executed");
    //row_col_reset();
    
    temp_count++;
    
  } //end of if(ground_flag == 1)
} //end of set_static_led()


void print_map()
{
  int i,j;

  for(i=0 ; i<8 ; i++)
  {
    for(j=0 ; j<8 ; j++)
    {
      if(moving_map_element[i][j] == 1){
      Serial.print("moving_map_element[");
      Serial.print(i);
      Serial.print("]");
      Serial.print("[");
      Serial.print(j);
      Serial.print("]");
      Serial.println("= 1");
      }
    }
  }

  for(i=0 ; i<8 ; i++)
  {
    for(j=0 ; j<8 ; j++)
    {
      if(static_map_element[i][j]==1){
      Serial.print("static_map_element[");
      Serial.print(i);
      Serial.print("]");
      Serial.print("[");
      Serial.print(j);
      Serial.print("]");
      Serial.println("= 1");
      }
    }
  }

  Serial.print("ground_flag =");
  Serial.println(ground_flag);
  Serial.println();
  Serial.print("row = ");
  Serial.println(row);
  Serial.print("column = ");
  Serial.println(column);
  Serial.println();
  Serial.print("temp_count : ");
  Serial.println(temp_count);
  Serial.print("GLOBAL RANDOM NO : ");
  Serial.println(global_random_no);

  Serial.print("bottom_obstacble_flag : ");
  Serial.println(bottom_obstacle_flag);

  Serial.print("ROTATION INDEX = ");
  Serial.println(rotation_index);
  
  
} //end of print_map()


void row_col_reset()
{ 
 row = 0; column = 4;
    block_gen_flag = 1;
    ground_flag = 0;
    bottom_obstacle_flag = 0;   //TRY TRY 
    //Serial.println("row_col_reset() has been executed");
}

void initial_set_underground()
{
  int i;
  for(i = 0 ; i < 8 ; i++)
  static_map_element[8][i] = 1;
}

void dec_bin()
{
  display.setBrightness(1);
  display.showNumberDec(dec_score);
} //end of dec_bin()




void beep(unsigned int freq, unsigned int duration)
{
  tone(A0, freq, duration);
}



void gameover()
{
  int k ;

  beep(200, 800);
    while(1)
      {
        for(k = 0 ; k < 8 ; k++)
        {
        lc.setRow(0,k,B11111111);
        delay(100);
      }
      }
} // end of gameover()


void pinoutStateRead()
{
  int A0, A1, A2, A3, A4 , A5, A6;


  A0 = digitalRead(A0);
  A1 = digitalRead(A1);
  A2 = digitalRead(A2);
  A3 = digitalRead(A3);
  A4 = digitalRead(A4);
  A5 = digitalRead(A5);
  A6 = digitalRead(A6);


  Serial.print("A0 = ");
  Serial.println(A0);
  Serial.print("A1 = ");
  Serial.println(A1);
  Serial.print("A2 = ");
  Serial.println(A2);
  Serial.print("A3 = ");
  Serial.println(A3);
  Serial.print("A4 = ");
  Serial.println(A4);
  Serial.print("A5 = ");
  Serial.println(A5);
  Serial.print("A6 = ");
  Serial.println(A6);
}

void analogPinInit()
{
  digitalWrite(A0, 0);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(A3, 0);
  digitalWrite(A4, 0);
  digitalWrite(A5, 0);
  digitalWrite(A6, 0);
}



void loop() { 

  if(temp_count >= 2)
  {
  row_col_reset();
  temp_count = 0;
  }
  
  block_gen();
  
  row_bingo();
 
  set_led();

  if(ground_flag == 0)
  old_block_fall_clear();

  block_fall();

  if(ground_flag == 0)
  block_control();

  dec_bin();

  bottom_detect();
  
} //end of main loop()