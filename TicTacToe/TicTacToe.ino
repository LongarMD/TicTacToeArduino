#include <JC_Button.h>

const int PRIMARY_C = 1;

int selectPins[3] = {10, 9, 8};
int inputPins[2] = {6, 7};
int extraPins[2] = {11, 12};

int gameState[3][3] = {{1, -1, 0}, {1, 0, 0}, {0, 0, 0}};
int ledConfig[3][3][3] = {
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}},
    {{0, 1, 1}, {1, 0, 0}, {1, 0, 1}},
    {{1, 1, 0}, {1, 1, 1}, {-1, -1, -1}}
};

int blinkPin[2] = {-1, -1};
int currentPlayer = 1;
int currentPin[2] = {0, 0};

Button confirmButton(5);
Button selectButton(4);

void setup(){
    for (int i = 0; i < 3; i++)
    {
        pinMode(selectPins[i], OUTPUT);
        if(i == 2) break;
        pinMode(inputPins[i], OUTPUT);
        pinMode(extraPins[i], OUTPUT);
    }

    confirmButton.begin();
    selectButton.begin();

    SetMuxInput(0);
    Serial.begin(9600);
}

void SetMuxInput(int color)
{
    int a = LOW, b = LOW;
    if(color != 0)
    {
        a = PRIMARY_C == color ? HIGH : LOW;
        b = a == HIGH ? LOW : HIGH;
    }

    digitalWrite(inputPins[0], a);
    digitalWrite(inputPins[1], b);
}

void SetMuxSelect(int pinConfig[])
{
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(selectPins[i], pinConfig[i] == 1 ? HIGH : LOW);
    }
}

/*
// TODO: PWM?
if(ledConfig[i][j][0] == -1){
    SetMuxInput(0);
    digitalWrite(extraPins[0], PRIMARY_C != color ? HIGH : LOW);
    digitalWrite(extraPins[1], PRIMARY_C != color ? LOW : HIGH);
}
*/

void DisplayGrid()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            int color = gameState[i][j];
            if(color == 0 or ledConfig[i][j][0] == -1) continue;

            SetMuxInput(0);
            SetMuxSelect(ledConfig[i][j]);
            SetMuxInput(color);
            
        }
    
}

void loop()
{
    selectButton.read();
    confirmButton.read();
    
    DisplayGrid();

    if (confirmButton.releasedFor(500))
    {
        //Serial.println("Long press");
    }
    if(selectButton.wasPressed())
    {
        //Serial.println("Short press");
    }

}
