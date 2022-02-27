#include <JC_Button.h>

const int PRIMARY_C = 1;

int MUX_SELECT[3] = {10, 9, 8};
int MUX_INPUT[2] = {6, 7};
int MUX_CONFIG[3][3][3] = {
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}},
    {{0, 1, 1}, {1, 0, 0}, {1, 0, 1}},
    {{1, 1, 0}, {1, 1, 1}, {-1, -1, -1}}
};

int EXTRA_LED[2] = {3, 5};

int gameState[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

const int BLINK_LENGHT = 500;
unsigned long blinkTime = 0;
int blinkPin[2] = {0, 0};
int blinkColor = 0;

bool gameEnded = false;
unsigned const long END_TIME_OFFSET = 2000;
unsigned long endTime = 0;

int currentPlayer = 1;
int currentPos[2] = {0, 0};

Button confirmButton(11);
Button selectButton(12);

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

void ColorToPinValue(int * a, int * b, int color)
{
    if(color != 0)
    {
        *a = PRIMARY_C == color ? HIGH : LOW;
        *b = *a == HIGH ? LOW : HIGH;
    }
}

void SetMuxInput(int color)
{
    int a = LOW, b = LOW;
    ColorToPinValue(&a, &b, color);

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
    if(millis() >= blinkTime + BLINK_LENGHT)
    {
        blinkColor = abs(blinkColor) == 1 ? 0 : currentPlayer;
        blinkTime = millis();
    }
}

void DisplayExtra(int color)
{
    int a = LOW, b = LOW;
    ColorToPinValue(&a, &b, color);

    analogWrite(EXTRA_LED[0], (255 * a) / 4); //
    analogWrite(EXTRA_LED[1], (255 * b) / 4);
    // digitalWrite(EXTRA_LED[0], a); //255 * (float)a / 3
    // digitalWrite(EXTRA_LED[1], b);
}

void DisplayGrid()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            if(MUX_CONFIG[i][j][0] == -1) continue;
            int color = gameState[i][j];

            if(blinkPin[0] == i && blinkPin[1] == j)
            {
                Blink();
                color = blinkColor;
            }
            else if(color == 0) continue;

            SetMuxInput(0);
            SetMuxSelect(MUX_CONFIG[i][j]);
            SetMuxInput(color);
            
        }
    if(blinkPin[0] == 2 && blinkPin[1] == 2)
    {
        int prevColor = blinkColor;
        Blink();
        if(prevColor != blinkColor)
            DisplayExtra(blinkColor);
    }
}

void MoveSelect()
{
    int n = 0;
    // Move
    do {
        int row = (currentPos[0] + (currentPos[1] + 1) / 3) % 3;
        int column = (currentPos[1] + 1) % 3;

        currentPos[0] = row;
        currentPos[1] = column;

        blinkTime = millis();
        blinkColor = 0;
        blinkPin[0] = row;
        blinkPin[1] = column;

        n++;
    }
    while(gameState[currentPos[0]][currentPos[1]] != 0 && n != 9);
    
    // Debug
    // Serial.print(currentPos[0]);
    // Serial.print(", ");
    // Serial.println(currentPos[1]);
}

bool FieldFull()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if(gameState[i][j] == 0) return false;
    
    return true;
}

int CheckWin()
{
    int row_sum[3] = {0, 0, 0};
    int column_sum[3] = {0, 0, 0};
    int diag_sum[2] = {0, 0};

    for (int i = 0; i < 3; i++)
    {
        diag_sum[0] += gameState[i][i];
        diag_sum[1] += gameState[i][2-i];
        for (int j = 0; j < 3; j++)
        {
            row_sum[i] += gameState[i][j];
            column_sum[j] += gameState[i][j];
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if(abs(row_sum[i]) == 3)
            return row_sum[i] / abs(row_sum[i]);
        if(abs(column_sum[i]) == 3)
            return column_sum[i] / abs(column_sum[i]);
        if(i != 2 && abs(diag_sum[i]) == 3)
            return diag_sum[i] / abs(diag_sum[i]);
    }

    return 0;
}

void Reset(){
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            gameState[i][j] = 0;

    DisplayGrid();
    DisplayExtra(0);

    currentPos[0] = 2;
    currentPos[1] = 2;
    MoveSelect();
}

void loop()
{
    selectButton.read();
    confirmButton.read();
    
    DisplayGrid();

    if(gameEnded)
    {
        if(millis() >= endTime + END_TIME_OFFSET) // TODO: Nicer end game state
        {
            Reset();
            gameEnded = false;
        }

        return;
    }

    if (confirmButton.wasPressed())
    {
        //Serial.println("Long press");

        gameState[currentPos[0]][currentPos[1]] = currentPlayer;
        if(currentPos[0] == 2 && currentPos[1] == 2)
            DisplayExtra(currentPlayer);

        currentPlayer = -currentPlayer;
        currentPos[0] = 2;
        currentPos[1] = 2;
        MoveSelect();

        int winner = CheckWin();
        if(winner != 0 || FieldFull())
        {
            blinkPin[0] = -1;
            blinkPin[1] = -1;

            gameEnded = true;
            endTime = millis();
        }
    }

    if(selectButton.wasPressed())
    {
        //Serial.println("Short press");
        MoveSelect();

        if(currentPos[0] == 2 && currentPos[1] == 2)
            DisplayGrid();
        if(gameState[2][2] == 0)
            DisplayExtra(0);
    }

}
