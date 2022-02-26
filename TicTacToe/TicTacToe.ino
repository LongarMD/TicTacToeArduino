#include <JC_Button.h>

const int PRIMARY_C = 1;

int MUX_SELECT[3] = {10, 9, 8};
int MUX_INPUT[2] = {6, 7};
int MUX_CONFIG[3][3][3] = {
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}},
    {{0, 1, 1}, {1, 0, 0}, {1, 0, 1}},
    {{1, 1, 0}, {1, 1, 1}, {-1, -1, -1}}
};

int EXTRA_LED[2] = {11, 12};

int gameState[3][3] = {{-1, 0, -1}, {0, 0, 0}, {0, 0, 0}};

const int BLINK_LENGHT = 2500;
int blink_count = 0;
int blinkPin[2] = {-1, -1};
int blinkColor = 0;

int currentPlayer = 1;
int currentPos[2] = {0, 0};

Button confirmButton(5);
Button selectButton(4);

void setup(){
    for (int i = 0; i < 3; i++)
    {
        pinMode(MUX_SELECT[i], OUTPUT);
        if(i == 2) break;
        pinMode(MUX_INPUT[i], OUTPUT);
        pinMode(EXTRA_LED[i], OUTPUT);
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

    digitalWrite(MUX_INPUT[0], a);
    digitalWrite(MUX_INPUT[1], b);
}

void SetMuxSelect(int pinConfig[])
{
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(MUX_SELECT[i], pinConfig[i] == 1 ? HIGH : LOW);
    }
}

void Blink()
{
    blink_count++;

    if(blink_count >= BLINK_LENGHT)
    {
        blinkColor = abs(blinkColor) == 1 ? 0 : currentPlayer;
        blink_count = 0;
    }
}

void DisplayGrid()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            int color = gameState[i][j];

            if(blinkPin[0] == i && blinkPin[1] == j)
            {
                Blink();
                color = blinkColor;
            }
            if(color == 0 or MUX_CONFIG[i][j][0] == -1) continue;

            SetMuxInput(0);
            SetMuxSelect(MUX_CONFIG[i][j]);
            SetMuxInput(color);
            
        }
}

void MoveSelect()
{
    // Move
    do {
        int row = (currentPos[0] + (currentPos[1] + 1) / 3) % 3;
        int column = (currentPos[1] + 1) % 3;

        currentPos[0] = row;
        currentPos[1] = column;

        blinkPin[0] = row;
        blinkPin[1] = column;
    }
    while(gameState[currentPos[0]][currentPos[1]] != 0);
    
    // Debug
    Serial.print(currentPos[0]);
    Serial.print(", ");
    Serial.println(currentPos[1]);
}

void CheckWin()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            int color = gameState[i][j];
            if(color == 0) return;
        }
}

void Reset(){
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            gameState[i][j] = 0;
}

void loop()
{
    selectButton.read();
    confirmButton.read();
    
    DisplayGrid();

    if (confirmButton.wasPressed())
    {
        //Serial.println("Long press");

        gameState[currentPos[0]][currentPos[1]] = currentPlayer;

        currentPlayer = -currentPlayer;
        currentPos[0] = 2;
        currentPos[1] = 2;
        MoveSelect();

        //CheckWin();
    }

    if(selectButton.wasPressed())
    {
        //Serial.println("Short press");
        MoveSelect();
    }

}
