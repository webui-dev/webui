#include "webui.h"

bool   gameActive    = true ;
char * currentPlayer = "X"  ;
int gameState[9] = {0,0,0,0,0,0,0,0,0};
char*  winningMessage ;
int winningConditions[8][3] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {0, 3, 6},
    {1, 4, 7},
    {2, 5, 8},
    {0, 4, 8},
    {2, 4, 6}
};

void close_the_application(webui_event_t* e){
    // Close all opened windows
    webui_exit();
}

void winningMessageFunction(webui_event_t * e , int currentIndex)
{
    asprintf(&winningMessage, "%s%s%s", "Player " , currentPlayer , " has won!" );
    webui_script_t js = {
        .script = "document.querySelector('.game--status').innerHTML ='" ,
        .timeout = 3
    };
    asprintf(&js.script, "%s%s%s" , js.script , winningMessage, "'");
    webui_script(e->window, &js);
}

void handleCellPlayed(int clickedCellIndex)
{
    gameState[clickedCellIndex] = currentPlayer == "X" ? 1 : 2 ;
}

void handlePlayerChange(webui_event_t* e)
{
    char* currentPlayerTurn ;
    currentPlayer = currentPlayer == "X" ? "O" : "X";
    asprintf(&currentPlayerTurn ,"%s%s%s", "It's " , currentPlayer , "'s turn");
    webui_script_t js = {
        .script = "document.querySelector('.game--status').innerHTML =\"" ,
        .timeout = 3
    };
    asprintf(&js.script, "%s%s%s" , js.script , currentPlayerTurn, "\"");
    webui_script(e->window, &js);
}

void handleResultValidation(webui_event_t * e, int index) {
    bool roundWon = false;

    for (int i = 0; i <= 7; i++) {
        int winCondition[3] ;
        for(int j = 0; j<3 ; j++) winCondition[j]= winningConditions[i][j];
        int a = gameState[winCondition[0]];
        int b = gameState[winCondition[1]];
        int c = gameState[winCondition[2]];
        if (a == 0 || b == 0 || c == 0) {
            continue;
        }
        if (a == b && b == c) {
            roundWon = true;
            break;
        }
    }

    if (roundWon) {
        winningMessageFunction(e, index);
        gameActive = false;
        return;
    }

    int roundDraw = true;
    for (int i = 0; i <= 8; i++)
        if( gameState[i]==0 )
            roundDraw = false;
    if (roundDraw) {
        webui_script_t js = {
            .script = "document.querySelector('.game--status').innerHTML =\"Game ended in a draw\" " ,
            .timeout = 3
        };
        webui_script(e->window, &js);
        gameActive = false;
        return;
    }

    handlePlayerChange(e);
}

void handleCellClick(webui_event_t * e, int index)
{
    int clickedCellIndex = index ;

    if (gameState[clickedCellIndex] != 0 || !gameActive) {
        return;
    }

    webui_script_t js = {
        .script = "document.querySelector('[data-cell-index=\"" ,
        .timeout = 3
    };

    asprintf(&js.script , "%s%d%s%s%s", js.script , clickedCellIndex , "\"]').innerHTML = \"", currentPlayer , "\" ;" );
    webui_script(e->window, &js);
    handleCellPlayed(clickedCellIndex);
    handleResultValidation(e,clickedCellIndex);
}

void cell_0(webui_event_t *e )
{
    int index = 0;
    handleCellClick( e, index);
}

void cell_1(webui_event_t *e )
{
    int index = 1;
    handleCellClick( e, index);
}
void cell_2(webui_event_t *e )
{
    int index = 2;
    handleCellClick( e, index);
}
void cell_3(webui_event_t *e )
{
    int index = 3;
    handleCellClick( e, index);
}
void cell_4(webui_event_t *e )
{
    int index = 4;
    handleCellClick( e, index);
}
void cell_5(webui_event_t *e )
{
    int index = 5;
    handleCellClick( e, index);
}
void cell_6(webui_event_t *e )
{
    int index = 6;
    handleCellClick( e, index);
}
void cell_7(webui_event_t *e )
{
    int index = 7;
    handleCellClick( e, index);
}
void cell_8(webui_event_t *e )
{
    int index = 8;
    handleCellClick( e, index);
}

void RestartGame(webui_event_t* e){
    for( int i = 0; i< 9 ; i++)
    {
        webui_script_t js = {
            .script = "document.querySelector('[data-cell-index=\"" ,
            .timeout = 3
        };
        asprintf(&js.script, "%s%d%s" , js.script , i, "\"]').innerHTML = \"\" " );
        webui_script(e->window, &js);
    }
    webui_script_t js = {
            .script = "document.querySelector('.game--status').innerHTML = \"It's X 's turn\"" ,
            .timeout = 3
        };
    webui_script(e->window, &js);
    gameActive = true;
    currentPlayer = "X";
    for( int i = 0; i< 9 ; i++)
    {
        gameState[i] = 0;
    }
}

int main()
{
    webui_set_timeout(10);
    webui_window_t* my_window = webui_new_window();

    // Bind HTML elements with functions
    webui_bind( my_window , "clear" , RestartGame          );
    webui_bind( my_window , "Exit"  , close_the_application);
    webui_bind( my_window , "0"     , cell_0               );
    webui_bind( my_window , "1"     , cell_1               );
    webui_bind( my_window , "2"     , cell_2               );
    webui_bind( my_window , "3"     , cell_3               );
    webui_bind( my_window , "4"     , cell_4               );
    webui_bind( my_window , "5"     , cell_5               );
    webui_bind( my_window , "6"     , cell_6               );
    webui_bind( my_window , "7"     , cell_7               );
    webui_bind( my_window , "8"     , cell_8               );

    // HTML server
    const char* url = webui_new_server(my_window, "TicTacToa");

    // Show the window
    if(!webui_show(my_window, url, webui.browser.chrome))   // Run the window on Chrome
        webui_show(my_window, url, webui.browser.any);      // If not, run on any other installed web browser

    // Wait until all windows get closed
    webui_wait();

    return 0;
}
