#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hclib.h"

char *fen( exboard_t *board, char *active, char *castling, char *enpassant, int half, int full){
	
    char *rStr = (char *) malloc(sizeof(char)*100);
    int space;	
    int count = 0;
    char piece;

    if (rStr == NULL) {
        return NULL;
    }

    for (int i = 7; i >= 0; i--) { 
        space = 0;
        for (int j = 0; j < 8; j++) { 
            piece = board->board[i][j];

            if ((piece >= 'a' && piece <= 'z') || (piece >= 'A' && piece <= 'Z')) {
                rStr[count++] = piece;
            } else {
                // Empty square
                while(j <= 7 && ' ' == board->board[i][j]){
                    space++;
                    j++;    
                }
                rStr[count++] = space + '0';
                j--;
				space = 0;
            }
        }
        rStr[count++] = '/';
    }
	rStr[count-1] = ' '; 
    // Add active player
    //rStr[count++] = ' ';
    rStr[count++] = *active;

    // Add castling availability
    rStr[count++] = ' ';
    strncpy(rStr + count, castling, strlen(castling));
    count += strlen(castling);

    // Add en passant target square
    rStr[count++] = ' ';
    strncpy(rStr + count, enpassant, strlen(enpassant));
    count += strlen(enpassant);

    // Add half-move clock
    count += sprintf(rStr + count, " %d", half);

    // Add full-move number
    count += sprintf(rStr + count, " %d", full);

    // Null-terminate the FEN string
    rStr[count] = '\0';
    count++;

    rStr = (char *) realloc(rStr, count * sizeof(char));

    return rStr;

}

exboard_t *boardstring( char *string ){
    exboard_t * board = (exboard_t *) malloc(sizeof(exboard_t));
    
    if (board == NULL) {
        return NULL;
    }    
    int j=0,i=0,count=0,x=0;
   	
		while (i < 2 ) {		
			if (string[count] == ' ') {
				board->bprison[x] = '\0';
				count = 18;
				break; 
			} else if (string[count] == '\n') {
				i++;
			} else {
				board->bprison[x++] = string[count];
			}
			count++;
		}

		// Reset x
		x = 0;
		i = 0;
		// Black airfield
		while (i < 2) {
			if (string[count] == ' ') {
				board->bairfield[x] = '\0';
				count = 45;
				break; 
			} else if (string[count] == '\n') {
				i++;
			} else {
				board->bairfield[x++] = string[count];
			}
			count++;
		}

		count = 45;
		i = 0;
		x = 0;
		// Now handling the board		
		for (j = 7; x!=116;) {
			if (string[115-x] == '\n') {
				i++;
				j = 7;  // Reset j for the new row
				x++; 
				count ++; 
			} else {
				board->board[i][j--] = string[115-x];
				x++;
				count ++; 
			}

		}

		x = 0;
		count= 126;
		i = 0;
		while (i<2) {
			if (string[count] == ' ') {
				board->wairfield[x] = '\0';
				i++; 
				count = 144;
				break;
			} else if (string[count] == '\n') {
				i++;
			} else {
				board->wairfield[x++] = string[count];
			}
			count++;
		}

		// White prison
		i=0;
		x = 0;
		while (i<2) {
			if (string[count] == ' ') {
				board->wprison[x] = '\0';
				break; 
			} else if (string[count] == '\n') {
				i++;
			} else {
				board->wprison[x++] = string[count];
			}
			count++;
		}    
		
    return board;
}


exboard_t *newboard(){
    int j = 0, i =0;
    exboard_t * Brd = (exboard_t *) malloc(sizeof(exboard_t));
    char whtpiecs[] = {'P','R','N','B','Q','K','B','N','R'};
    char blkpiecs[] = {'p','r','n','b','q','k','b','n','r'};

    if(Brd == NULL){
        fprintf(stderr, "Malloc failed\n");
        return NULL; 
    }

    // White and Black Front Row
    for(i=0; i<8; i++){
        Brd->board[1][i] = whtpiecs[0];
        Brd->board[6][i] = blkpiecs[0];
    }

    //White and Black Back row 
    for(i= 0; i<8; i++){
        //white piece
        Brd->board[0][i] = whtpiecs[i+1];
        //black piece
        Brd->board[7][i] = blkpiecs[i+1];
    }
    
    //Blank Spaces
    for(i=0; i<4;i++){
        for(j =0; j<8; j++){
            Brd->board[i+2][j] = ' ';
        }

    }
    //Black Prison and Airfield
    Brd->bprison[0]='\0';
    Brd->bairfield[0]='\0';

    //White Prirson and Airfield
    Brd->wprison[0]='\0';
    Brd->wairfield[0]='\0'; 

    return Brd;  
}

exboard_t *copyboard( exboard_t *board ){

    exboard_t * cpyboard = (exboard_t *) malloc(sizeof(exboard_t));
    //if board is null exit and return null 
    if(board == NULL){
        fprintf(stderr, "Malloc failed\n");
        return NULL; 
    }

    memcpy( cpyboard, board, sizeof(exboard_t));
    return cpyboard; 

}

char *stringboard( exboard_t *board ){
    int j = 0, i =0, count =0;
    char *strboard = (char *) malloc(162*sizeof(char));
    
    for(i = 0; i<=161;i++)
		strboard[i] = ' '; 
	
	for(i=8; i<161;i++)
		if((i+1)%9==0)
			strboard[i] = '\n';
			
		
    //Characters for black prison
    for(i = 0, count=0; i<=17; ){     
        
        if( board->bprison[count] == '\0'){
			break; 
		}
        if(i != 8 && i != 17 ){
			strboard[i] = board->bprison[count];
			count++;
			i++;
		}
		else{
			i++;
		}	
    }


    //Characters for black airfield
    for(i = 0, count=0; i<=17; ){        
        if( board->bairfield[count] == '\0'){
			break; 
		}
        if(i != 8 && i != 17 ){
			strboard[i+18] = board->bairfield[count];
			count++;
			i++;
		}
		else{
			i++;
		}	
    }
    
    // seperation characters 
    for(i = 36; i < 44; i++){              
        strboard[i] = '-';
    }
    
    //Characters for the board
    for(j = 7, i = 44, count = 44; i < 116; i++){
	    if(i==44 || i == 53 || i == 62 || i == 71 || i == 80  || i == 89 || i == 98 || i ==107){
            strboard[i] = '\n';
            j--;
            count = 44; 
        }
        else{
	        strboard[i] = board->board[j+1][count-44];
            count++;
        }
    }
    

    //Characters for Seperation
    for(i = 117; i < 125; i++){
	 strboard[i] = '-';
    }

    for(i = 0, count=0; i<=17; ){     
        
        if( board->wairfield[count] == '\0'){
			break; 
		}
        if(i != 8 && i != 17 ){
			strboard[i+126] = board->wairfield[count];
			count++;
			i++;
		}
		else{
			i++;
		}	
    }


	
    for(i = 0, count=0; i<=17; ){     
        
        if( board->wprison[count] == '\0'){
			break; 
		}
        if(i != 8 && i != 17 ){
			strboard[i] = board->wprison[count];
			count++;
			i++;
		}
		else{
			i++;
		}	
    }
    strboard[161] = '\0';
    return strboard;
}

exboard_t *apply_move( exboard_t *board, move_t *move){

    char piece;
    exboard_t * Brd = copyboard(board);

    //castling
    
    if((move->from_i == 0 ||move->from_i == 7 )&& move->from_j == 4){
        piece = Brd->board[move->from_i][move->from_j];
        if(piece == 'k'|| piece =='K'){
            if(move->from_i-move->to_i==0){
                if(move->from_j-move->to_j== 2){
                    Brd->board[move->from_i][move->from_j] = ' ';
                    Brd->board[move->to_i][move->to_j] = piece;
                    
                    piece =  Brd->board[move->to_i][7];
                    Brd->board[move->to_i][7]= ' ';
                    Brd->board[move->to_i][5]= piece;


                }
                else if(move->from_j-move->to_j == -2){
                    Brd->board[move->from_i][move->from_j] = ' ';
                    Brd->board[move->to_i][move->to_j] = piece;
                    
                    piece =  Brd->board[move->to_i][0];
                    Brd->board[move->to_i][0]= ' ';
                    Brd->board[move->to_i][3]= piece;                    

                }
            }
        }
    }

    //black airfiled to board 
    if(move->from_i == 8){
        piece = move->promotion;
        Brd->board[move->to_i][move->to_j] = piece;
        for(int i =0;Brd->bairfield[i] != '\0';i++){
            if(Brd->bairfield[i] == piece){
                while(Brd->bairfield[i] != '\0'){
                    Brd->bairfield[i] = Brd->bairfield[i+1];
                    i++;
                } 
            }
        }
 	return Brd;	
    }
    //white airfield to board
    if(move->from_i == -1){
        piece = move->promotion;
        Brd->board[move->to_i][move->to_j] = piece;
        for(int i=0;Brd->wairfield[i] != '\0';i++){
            if(Brd->wairfield[i] == piece){
                while(Brd->wairfield[i] != '\0'){
                    Brd->wairfield[i] = Brd->wairfield[i+1];
                    i++;
                } 
            }
        }
 	return Brd;	
    }

    //black prison to board 
    if(move->from_i == 9){
        //moves promotion piece to board 
        piece = move->promotion;
        Brd->board[move->to_i][move->to_j] = piece;
        for(int i=0;Brd->bprison[i] != '\0';i++){
            if(Brd->bprison[i] == piece){
                while(Brd->bprison[i] != '\0'){
                    Brd->bprison[i] = Brd->bprison[i+1];
                    i++;
                } 
            }
        } 
        piece = move->hostage;
        //moving hostage to blacks airfield
        for(int i=0;i<16;i++){
            if(Brd->bairfield[i]=='\0'){
                Brd->bairfield[i] = piece;
                Brd->bairfield[i+1] = '\0';
                break;
            }
        }
        //removes hostage from whites prison 
        for(int i=0;Brd->wprison[i] != '\0';i++){
	        if(Brd->wprison[i] == piece){
                while(Brd->wprison[i] != '\0'){
                    Brd->wprison[i] = Brd->wprison[i+1];
                    i++;
                } 
            }
        }
	return Brd;
    }
    //white prison to board
    if( move->from_i == -2){
        
        //moves promotion piece to board 
        piece = move->promotion;
        Brd->board[move->to_i][move->to_j] = piece;

        //removes promotion piece from white prison 
        for(int i=0;Brd->wprison[i] != '\0';i++){
            if(Brd->wprison[i] == piece){
                while(Brd->wprison[i] != '\0'){
                    Brd->wprison[i] = Brd->wprison[i+1];
                    i++;
                } 
            }
        } 
        piece = move->hostage;
        //moving hostage to white airfield
        for(int i=0;i<16;i++){
            if(Brd->wairfield[i]=='\0'){
                Brd->wairfield[i] = piece;
                Brd->wairfield[i+1] = '\0';
                break;
            }
        }
        //removes hostage from blacks prison 
        for(int i=0;Brd->bprison[i] != '\0';i++){
	        if(Brd->bprison[i] == piece){
                while(Brd->bprison[i] != '\0'){
                    Brd->bprison[i] = Brd->bprison[i+1];
                    i++;
                } 
            }
        }
	return Brd;
    }
    
    //if promotion has a value
    if(move->promotion != ' ') {
        piece = move->promotion;   
        Brd->board[move->from_i][move->from_j] = ' ';
        Brd->board[move->to_i][move->to_j] = piece;
 	return Brd; 	
    }

    // en passant for black
    if(Brd->board[move->from_i][move->from_j]=='p' && move->from_i-move->to_i ==1){
	  if((move->from_j-move->to_j == 1||move->from_j-move->to_j == -1)){
            if(Brd->board[move->from_i][move->to_j] != ' ' && Brd->board[move->to_i][move->to_j] == ' '){
                piece = Brd->board[move->from_i][move->to_j]; 
		if(piece >= 'B' && piece<='R'){
		for(int i=0;i<16;i++){
                    if(Brd->bprison[i]=='\0'){
                        Brd->bprison[i]= piece;
                        Brd->bprison[i+1] = '\0';
                        break;
                    }
                }
                piece = Brd->board[move->from_i][move->from_j];   
                Brd->board[move->from_i][move->from_j] = ' ';
		Brd->board[move->from_i][move->to_j] = ' ';
                Brd->board[move->to_i][move->to_j] = piece;                   
		return Brd;
            }
	    }	
        }

    }
  
    // en passant for white
    if(Brd->board[move->from_i][move->from_j]=='P' && move->to_i-move->from_i ==1){
	  if((move->from_j-move->to_j == 1||move->from_j-move->to_j == -1)){
            if(Brd->board[move->from_i][move->to_j] != ' ' && Brd->board[move->to_i][move->to_j] == ' '){
                piece = Brd->board[move->from_i][move->to_j]; 
                if(piece>='b' && piece<='r'){
		for(int i=0;i<16;i++){
                    if(Brd->wprison[i]=='\0'){
                        Brd->wprison[i]= piece;
                        Brd->wprison[i+1] = '\0';
                        break;
                    }
                }
                piece = Brd->board[move->from_i][move->from_j];   
                Brd->board[move->from_i][move->from_j] = ' ';
		Brd->board[move->from_i][move->to_j] = ' ';
                Brd->board[move->to_i][move->to_j] = piece;                   
		return Brd;
            }
	    }	
        }

    }

    piece = Brd->board[move->to_i][move->to_j];
    //checks to see if the piece being taken is black
    if(piece >= 'b' && piece <= 'r'){
        //adding taken piece to white prison
        for(int i=0;i<16;i++){
            if(Brd->wprison[i]=='\0'){
                Brd->wprison[i]= piece;
                Brd->wprison[i+1] = '\0';
                break;
            }
        }
    }
    //checks to see if the piece being taken is white
    else if (piece >= 'B'&& piece <= 'R'){
        //adding taken piece to black prison
        for(int i=0;i<16;i++){
            if(Brd->bprison[i]=='\0'){
                Brd->bprison[i]= piece;
                Brd->bprison[i+1] = '\0';
                break;
            }
        }
    }
        //moving piece to a spot
        piece = Brd->board[move->from_i][move->from_j];   
        Brd->board[move->from_i][move->from_j] = ' ';
        Brd->board[move->to_i][move->to_j] = piece;

return Brd;

}

move_t **moves( board_t *board, int from_i, int from_j ){


    move_t **moves;

    char piece = (*board)[from_i][from_j];
    int colour;
    
    if(piece >= 'b'&& piece <= 'r'){
        colour = 1;
    }
    else if(piece >= 'B'&& piece <= 'R'){
        colour = 0;
    }

    //calling knights moves
    if(piece == 'n' || piece == 'N'){
	moves = knightmoves( board, from_i, from_j, colour );
    }
    //calling bishop moves
    if(piece == 'b' || piece == 'B'){
        moves = bishopmoves( board, from_i, from_j, colour );
    }
    //calling rook moves
    if(piece == 'r' || piece == 'R'){
        moves = rookmoves( board, from_i, from_j, colour );
    }
    //calling queen moves
    if(piece == 'q' || piece == 'Q'){
	moves = queenmoves( board, from_i, from_j, colour );
    }
    //calling king moves
    if(piece == 'k' || piece == 'K'){
        moves = kingmoves( board, from_i, from_j, colour );
    }
    //calling pawn moves
    if(piece == 'p' || piece == 'P'){
        moves = pawnmoves( board, from_i, from_j, colour );
    }
    return moves; 
}

move_t **knightmoves( board_t *board, int from_i, int from_j, int colour ){

    int count = 0;
    move_t ** moves = (move_t **) malloc(9*sizeof(move_t *));
    int to_j;
    int to_i;
    char piece;

    //move 1
    if(from_i+1 <=7 && from_j-2 >= 0){
        to_j= from_j-2;
        to_i = from_i+1;
        char piece = (*board)[to_i][to_j];
        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R') ){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move 2
    if(from_i+1 <=7 && from_j+2 <= 7){
        to_j= from_j+2;
        to_i = from_i+1;
        piece =(*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move3
    if(from_i+2 <=7 && from_j-1 >= 0){
        to_j= from_j-1;
        to_i = from_i+2;
        piece =(*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move4
    if(from_i+2 <=7 && from_j+1 >= 0){
        to_j= from_j+1;
        to_i = from_i+2;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move5
    if(from_i-1 >= 0 && from_j-2 >= 0){
        to_j= from_j-2;
        to_i = from_i-1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move6
    if(from_i-1 >=0 && from_j+2 <= 7){
        to_j= from_j+2;
        to_i = from_i-1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move7
    if(from_i-2 >=0 && from_j-1 >= 0){
        to_j= from_j-1;
        to_i = from_i-2;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move8
    if(from_i-2 >=0 && from_j+1 <= 7){
        to_j= from_j+1;
        to_i = from_i-2;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    moves = (move_t **) realloc(moves, count * sizeof(move_t *));
    moves[count]= NULL; 
    return moves; 

}

move_t **bishopmoves( board_t *board, int from_i, int from_j, int colour ){

    int count = 0;
    move_t ** moves = (move_t **) malloc(16*sizeof(move_t *));
    int to_j = from_i;
    int to_i = from_j;
    char piece;
 
    // up and right
    for(int i=1; ; i++){
        //check to see if piece is on the board
        if(from_j+i <= 7 && from_i+i <=7 ){     
            piece = (*board)[from_i+i][from_j+i];

            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j+i;
                    to_i = from_i+i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }
    to_j = from_j;
    to_i = from_i;
    // up and left 
    for(int i=1; ; i++){
        //check to see if piece is on the board 
        if(from_j-i >= 0 && from_i+i <=7){
             
            piece = (*board)[from_i+i][from_j-i];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j-i;
                    to_i = from_i+i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }
    to_j = from_j;
    to_i = from_i;
    // down and right
    for(int i=1; ; i++){
        //checks to see if peice is on board
        if(from_j+i <= 7 && from_i-i >= 0){
            piece = (*board)[from_i-i][from_j+i];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j+i;
                    to_i = from_i-i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }
    to_j = from_j;
    to_i = from_i;
    // down and left
    for(int i=1; ; i++){
        if(from_j-i >= 0 && from_i-i >=0){
            piece = (*board)[from_i-i][from_j-i];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j-i;
                    to_i = from_i-i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }

    moves = (move_t **) realloc(moves, count * sizeof(move_t *));
    moves[count]= NULL; 
    return moves; 

}

move_t **rookmoves( board_t *board, int from_i, int from_j, int colour ){

    int count = 0;
    move_t ** moves = (move_t **) malloc(16*sizeof(move_t *));
    int to_j=from_j;
    int to_i=from_i;
    char piece;
   // up
    for(int i=1;i<=8 ; i++){
        //check to see if piece is on the board
        if(from_i+i <=7 ){
            piece = (*board)[from_i+i][from_j];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j;
                    to_i = from_i+i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;

            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more up
            if(piece!= ' '){
	        break;
	    }	
        }
        //when the spot is off the board can't go more up
        else 
	    break;
    
    }
    to_i=from_i;
    to_j=from_j;
    // down 
    for(int i=1; ; i++){
        //check to see if piece is on the board 
        if(from_i-i >=0){
            piece = (*board)[from_i-i][from_j];
            //checks to see if piece/spot is empty or oppents piece
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j;
                    to_i = from_i-i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more down
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more down
        else 
            break;
    
    }
    to_i=from_i;
    to_j=from_j;
    // right
    for(int i=1; ; i++){
        //checks to see if peice is on board
        if(from_j+i <= 7){
            piece = (*board)[from_i][from_j+i];
            //checks to see if piece/spot is empty or oppents piece
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j+i;
                    to_i = from_i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more right
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more right
        else 
            break;
    
    }
    to_i=from_i;
    to_j=from_j;
    // left
    for(int i=1; ; i++){
        if(from_j-i >= 0){
            
            
            piece = (*board)[from_i][from_j-i];
            //checks to see if piece/spot is empty or oppents piece
            //checks to see if piece/spot is empty or oppents piece
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j-i;
                    to_i = from_i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more left
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more left
        else 
            break;
    
    }
    moves = (move_t **) realloc(moves, count * sizeof(move_t *));
    moves[count]= NULL; 
    return moves; 

}

move_t **queenmoves( board_t *board, int from_i, int from_j, int colour ){  
   
    int count = 0;
    move_t ** moves = (move_t **) malloc(32*sizeof(move_t *));
    int to_i = from_i;
    int to_j = from_j;
    char piece;
   // up
    for(int i=1;i<=8 ; i++){
        //check to see if piece is on the board
        if(from_i+i <=7 ){
            piece = (*board)[from_i+i][from_j];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j;
                    to_i = from_i+i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;

            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more up
            if(piece!= ' '){
	        break;
	    }	
        }
        //when the spot is off the board can't go more up
        else 
	    break;
    
    }

    to_i=from_i;
    to_j=from_j;
    // down 
    for(int i=1; ; i++){
        //check to see if piece is on the board 
        if(from_i-i >=0){
            piece = (*board)[from_i-i][from_j];
            //checks to see if piece/spot is empty or oppents piece
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j;
                    to_i = from_i-i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more down
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more down
        else 
            break;
    
    }
    to_i=from_i;
    to_j=from_j;
    // right
    for(int i=1; ; i++){
        //checks to see if peice is on board
        if(from_j+i <= 7){
            piece = (*board)[from_i][from_j+i];
            //checks to see if piece/spot is empty or oppents piece
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j+i;
                    to_i = from_i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more right
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more right
        else 
            break;
    
    }
    to_i=from_i;
    to_j=from_j;
    // left
    for(int i=1; ; i++){
        if(from_j-i >= 0){
            
            
            piece = (*board)[from_i][from_j-i];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j-i;
                    to_i = from_i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more left
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more left
        else 
            break;
    
    }
    to_j = from_i;
    to_i = from_j;
    // up and right
    for(int i=1; ; i++){
        //check to see if piece is on the board
        if(from_j+i <= 7 && from_i+i <=7 ){     
            piece = (*board)[from_i+i][from_j+i];

            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j+i;
                    to_i = from_i+i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }
    
    to_j = from_j;
    to_i = from_i;
    // up and left 
    for(int i=1; ; i++){
        //check to see if piece is on the board 
        if(from_j-i >= 0 && from_i+i <=7){
             
            piece = (*board)[from_i+i][from_j-i];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j-i;
                    to_i = from_i+i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }
    to_j = from_j;
    to_i = from_i;
    // down and right
    for(int i=1; ; i++){
        //checks to see if peice is on board
        if(from_j+i <= 7 && from_i-i >= 0){
            piece = (*board)[from_i-i][from_j+i];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j+i;
                    to_i = from_i-i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }
    to_j = from_j;
    to_i = from_i;
    // down and left
    for(int i=1; ; i++){
        if(from_j-i >= 0 && from_i-i >=0){
            piece = (*board)[from_i-i][from_j-i];
            //checks to see if piece/spot is empty or oppents piece
            if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                    to_j = from_j-i;
                    to_i = from_i-i;
                    moves[count] = (move_t *) malloc(sizeof(move_t));
                    moves[count]->from_j = from_j;
                    moves[count]->from_i = from_i;
                    moves[count]->to_j = to_j;
                    moves[count]->to_i = to_i;
                    moves[count]->promotion = ' '; 
                    moves[count]->hostage = ' '; 
                    count++;
            }
            //when piece is you're own piece exit loop
            else{
                break;
            }
            //when piece is enemy peice exit lop can't go more diagonal
            if(piece!= ' ')
                break;
        }
        //when the spot is off the board can't go more diagonal
        else 
            break;
    
    }
    moves = (move_t **) realloc(moves, count * sizeof(move_t *));
    moves[count]= NULL; 
    return moves;

}

move_t **kingmoves( board_t *board, int from_i, int from_j, int colour ){

    int count = 0;
    move_t ** moves = (move_t **) malloc(9*sizeof(move_t *));
    int to_j=from_j;
    int to_i=from_i;
    char piece;

    //move 1 - up and left 
    if(from_i+1 <=7 && from_j-1 >= 0){
        to_j= from_j-1;
        to_i = from_i+1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R') ){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move 2 - up 
    if(from_i+1 <=7){
        to_j= from_j;
        to_i = from_i+1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move 3 - up and right
    if(from_i+1 <=7 && from_j+1 <= 7){
        to_j= from_j+1;
        to_i = from_i+1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move 4 - left 
    if(from_j-1 >= 0){
        to_j= from_j-1;
        to_i = from_i;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move5 - right
    if(from_j+1 <= 7){
        to_j= from_j+1;
        to_i = from_i;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move6 - down and left
    if(from_i-1 >=0 && from_j-1 >= 0){
        to_j= from_j-1;
        to_i = from_i-1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move7 - down
    if(from_i-1 >=0){
        to_j= from_j;
        to_i = from_i-1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move8 - down and right
    if(from_i-1 >=0 && from_j+1 <= 7){
        to_j= from_j+1;
        to_i = from_i-1;
        piece = (*board)[to_i][to_j];

        if( piece == ' ' || (colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_j = from_j;
                moves[count]->from_i = from_i;
                moves[count]->to_j = to_j;
                moves[count]->to_i = to_i;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }

    moves = (move_t **) realloc(moves, count * sizeof(move_t *));
    moves[count]= NULL; 
    return moves; 

}

move_t **pawnmoves(board_t *board, int from_i, int from_j, int colour ){

    int count = 0;
    move_t ** moves = (move_t **) malloc(5*sizeof(move_t *));
    int to_i=from_i;
    int to_j=from_j;
    char piece,temp;

    //move 1 - forward two spots 
    if((from_i==1 && colour == 0) || (from_i==6 && colour == 1)){
        if(colour == 0){
            to_j= from_j;
            to_i = from_i+2;
            temp = (*board)[to_i-1][to_j]; 
        }
        if (colour == 1){
            to_j= from_j;
            to_i = from_i-2;
            temp = (*board)[to_i+1][to_j];
        }


        piece = (*board)[to_i][to_j];
        if( piece == ' ' && temp == ' ' ){
            moves[count] = (move_t *) malloc(sizeof(move_t));
            moves[count]->from_i = from_i;
            moves[count]->from_j = from_j;
            moves[count]->to_i = to_i;
            moves[count]->to_j = to_j;
            moves[count]->promotion = ' '; 
            moves[count]->hostage = ' '; 
            count++;
        }

    }
    //move 2 - moving forward one space
    if((from_i+1 <=7 && colour == 0) || (from_i-1 >=0 && colour == 1)){
        
        if(colour == 0){
            to_j= from_j;
            to_i = from_i+1;
        }
        if (colour == 1){
            to_j= from_j;
            to_i = from_i-1;
        }
  
        piece = (*board)[to_i][to_j];

        if( piece == ' ' ){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_i = from_i;
                moves[count]->from_j = from_j;
                moves[count]->to_i = to_i;
                moves[count]->to_j = to_j;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move 3 - taking left piece
    if((from_i+1 <=7 && from_j-1 >= 0 && colour == 0) || (from_i-1 >=0 && from_j-1 >=0 && colour == 1)){
        
        if(colour == 0){
            to_j= from_j-1;
            to_i = from_i+1;
        }
        else if (colour == 1){
            to_j= from_j-1;
            to_i = from_i-1;
        }
        piece = (*board)[to_i][to_j];

        if((colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_i = from_i;
                moves[count]->from_j = from_j;
                moves[count]->to_i = to_i;
                moves[count]->to_j = to_j;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    //move 4 - taking right piece
    if((from_i+1 <=7 && from_j+1 <= 7 && colour == 0) || (from_i-1 >=0 && from_j+1 <=7 && colour == 1)){
        
        if(colour == 0){
            to_j= from_j+1;
            to_i = from_i+1;
        }
        if (colour == 1){
            to_j= from_j+1;
            to_i = from_i-1;
        }
        piece = (*board)[to_i][to_j];

        if((colour == 0 && piece >= 'b' && piece <= 'r') || (colour == 1 && piece >= 'B' && piece <= 'R')){
                moves[count] = (move_t *) malloc(sizeof(move_t));
                moves[count]->from_i = from_i;
                moves[count]->from_j = from_j;
                moves[count]->to_i = to_i;
                moves[count]->to_j = to_j;
                moves[count]->promotion = ' '; 
                moves[count]->hostage = ' '; 
                count++;
            }
    }
    moves = (move_t **) realloc(moves, count * sizeof(move_t *));
    moves[count]= NULL; 
    return moves; 

}
