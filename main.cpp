/**
 * Feedback by Lydia Tse:
 * - Hilarious Snoop Dog reference "dropItLikeItsHot" :D
 * - Good job! 
 * */
//------------------------------------
//  Prog5Vexed.cpp
//     Move pieces to be adjacent.  Once adjacent pieces match, they are cleared off the board.  Once the entire
//     board is clear, play advances to the next level.
//
//  Prog 5: Vexed with Graphics
//  Author: Deep Diganvker
//  Using Codio, for UIC CS 141, Fall 2019
// 
// Instructions:
//    To run this program, first select the "Build and Run" option at the top of the window. You
//    can ignore the following error messages that will appear:
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to initialize inotify, joystick connections and disconnections won't be notified
//    To see the graphical output then select the "Viewer" option at the top of the window.
//    
// For more information about SFML graphics, see: https://www.sfml-dev.org/tutorials
// Be sure to close the old window each time you rebuild and rerun, to ensure you
// are seeing the latest output. 
//    
#include <SFML/Graphics.hpp> // Needed to access all the SFML graphics libraries
#include <iostream>          // Since we are using multiple libraries, now use std::
                             //     in front of every cin, cout, endl, and string 
#include <fstream>           // For file input
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen() 
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)
#include <iomanip>           // Used for setw()

// Global constants
const int SquareSize = 40;
const int BoardRows = 8;
const int BoardColumns = 10;
const int SpaceSize = 3;

// Compute the window size based on the size of each square and the spaces in between them
const int WindowXSize = (SquareSize + SpaceSize) * BoardColumns + SpaceSize;
const int WindowYSize = (SquareSize + SpaceSize) * BoardRows + SpaceSize + 50;
const int MaxNumberOfBoards = 118;
const int NumberOfPresetBoardParValues = 60; // After the first 60 par values, the default par value is 15
const char FileName[] = "boards.txt";
const int SleepAmount = 10;             // Length of time to pause, in milliseconds
const int WallValue = 9;                // Data file stores wall values as 9
const int BlankRGBValue = 250;    // Value for each of RGB for a blank Square
const int TextRGBvalue = 125;           // Value for each of RGB for text numbers displayed in a Square
const int WallRGBvalue = 75;            // Value for each of RGB for a wall Square
const int BackgroundRGBvalue = 128;     // Value for each of RGB for the background Rectangle

// Color names
const sf::Color BlankColor     = sf::Color( BlankRGBValue, BlankRGBValue, BlankRGBValue); // Almost White
const sf::Color RedColor       = sf::Color(255,102,102);   // Red
const sf::Color OrangeColor    = sf::Color(255,153, 51);   // Orange
const sf::Color LimeColor      = sf::Color(153,255,153);   // Lime
const sf::Color LightBlueColor = sf::Color(102,255,255);   // Light Blue
const sf::Color PurpleColor    = sf::Color(178,102,255);   // Purple
const sf::Color GreenColor     = sf::Color(  0,204,  0);   // Green
const sf::Color BlueColor      = sf::Color( 51,153,255);   // Blue
const sf::Color YellowColor    = sf::Color(255,255,102);   // Yellow
const sf::Color WallColor      = sf::Color( WallRGBvalue, WallRGBvalue, WallRGBvalue); // Dark Gray


//---------------------------------------------------------------------------------------
class Square {
	public:
		// Constructors 
		Square();                                                  // Default constructors
		Square( int theSize, int theXPosition, int theYPosition,   // Fully-qualified constructor
			     const sf::Color &theColor, bool theVisibility, bool isAWallValue, std::string theText);
		Square( const Square &otherSquare);                        // Copy constructor
			
		// Get (accessor) functions
		sf::RectangleShape getTheSquare() { return theSquare; }
		int getSize()         { return size;       }
		int getXPosition()    { return xPosition;  }
		int getYPosition()    { return yPosition;  }
		sf::Color& getColor() { return color;      }
		bool getIsVisible()   { return isVisible;  }
		bool getIsCaptured()  { return isCaptured; }
	    bool getIsAWall()     { return isAWall;    }
		std::string getText() { return text;       }
        
	
		// Set (mutator) functions
		void setSize( int theSize) { 
			size = theSize; 
			theSquare.setSize( sf::Vector2f(theSize, theSize));
		}
		void setXPosition( int theXPosition) { 
			xPosition = theXPosition; 
			theSquare.setPosition( theXPosition, yPosition);   // Set the position of the square
		}
		void setYPosition( int theYPosition) { 
			yPosition = theYPosition; 
			theSquare.setPosition( xPosition, theYPosition);   // Set the position of the square
		}
		void setColor( sf::Color & theColor) { 
			color = theColor; 
			theSquare.setFillColor( theColor);    // Also update the color on the square itself
		}
		void setColor( int R, int G, int B) {
			sf::Color theNewColor( R, G, B);
			color = theNewColor;
			theSquare.setFillColor( theNewColor);
		}
		void setVisibility( bool theVisibility) { isVisible = theVisibility;     }
		void setIsCaptured( bool isCaptured)    { this->isCaptured = isCaptured; }
		void setIsAWall( bool isAWall)          { this->isAWall = isAWall;       }
		void setText( std::string theText)      { text = theText;                }

		// Utility functions
		void displayText( sf::RenderWindow *pWindow, sf::Font theFont, sf::Color theColor, int textSize);
	
	private:
		int size;
		int xPosition;
		int yPosition;
		sf::Color color;
		bool isVisible;
		bool isCaptured;   // Indicates whether or not it is part of the captured area
	    bool isAWall;      // Set to true when piece is a wall
		std::string text;
		sf::RectangleShape theSquare;

}; //end class Square


//---------------------------------------------------------------------------------------
// Square class: Default Constructor 
Square::Square()
{
	size = 0;
	xPosition = 0;
	yPosition = 0;
	color = sf::Color::Black;
	isVisible = false;
	isCaptured = false;
	isAWall = false;
	text = "";		
}


//---------------------------------------------------------------------------------------
// Square class: Fully-qualified constructor, used to set all fields
Square::Square( int theSize, int theXPosition, int theYPosition, 
			    const sf::Color &theColor, bool theVisibility, bool isAWallValue, std::string theText)
{
	// Set the class fields
	size = theSize;
	xPosition = theXPosition;
	yPosition = theYPosition;
	color = theColor;
	isVisible = theVisibility;
	isCaptured = false;      // By default squares have not been captured
	isAWall = isAWallValue;  // Set to true for Squares that are walls
	text = theText;
	// Use the values to set the display characteristics of theSquare
	theSquare.setSize( sf::Vector2f(theSize, theSize));
	theSquare.setPosition( theXPosition, theYPosition);   // Set the position of the square
	theSquare.setFillColor( theColor);
}


//---------------------------------------------------------------------------------------
// Square class: Copy constructor
Square::Square( const Square &otherSquare)
{
	this->size = otherSquare.size;
	this->xPosition = otherSquare.xPosition;
	this->yPosition = otherSquare.yPosition;
	this->color = otherSquare.color;
	this->isVisible = otherSquare.isVisible;
	this->isCaptured = otherSquare.isCaptured;
	this->isAWall = otherSquare.isAWall;
	this->text = otherSquare.text;
	// Use the values to set the display characteristics of theSquare
	theSquare.setSize( sf::Vector2f(size, size) );
	theSquare.setPosition( xPosition, yPosition);   // Set the position of the square
	theSquare.setFillColor( color);
}


//---------------------------------------------------------------------------------------
// Square class utility function to create a sf::Text object to store and display text 
// associated with this Square.
// 
// Assuming we display output in sf::RenderWindow window(...), then call this function using: 
//    aSquare.displayTest( &window);
// or when using an array of Square pointers declared as:  Square *squaresArray[ 4];
// then call it using:  squaresArray[i]->displayText( &window);
void Square::displayText( 
		sf::RenderWindow *pWindow,   // The window into which we draw everything
		sf::Font theFont,            // Font to be used in displaying text   
		sf::Color theColor,          // Color of the font
		int textSize)                // Size of the text to be displayed
{	
	// Create a sf::Text object to draw the text, using a sf::Text constructor
	sf::Text theText( text,        // text is a class data member
					  theFont,     // font from a font file, passed as a parameter
					  textSize);   // this is the size of text to be displayed

	// Set the Text color 
	theText.setColor( theColor);

	// Place text in the corresponding square, centered in both x (horizontally) and y (vertically)
	// For horizontal center, find the center of the square and subtract half the width of the text 
	int theXPosition = xPosition + (SquareSize / 2) - ((strlen(text.c_str()) * theText.getCharacterSize()) / 2);
	// For the vertical center, from the top of the square go down the amount: (square size - text size) / 2
	int theYPosition = yPosition + (SquareSize - theText.getCharacterSize()) / 2;
	// Use additional offsets to get numbers centered
	theText.setPosition( theXPosition + 7, theYPosition - 1);

	// Finally draw the Text object in the RenderWindow
	pWindow->draw( theText);
}


//------------------------------------------------------------------------------------------------------------------
// Color utility function.
// Translate the board number to a color to be displayed.
// Here are some color codes that are useful:
// 
// 		Black 0,0,0
// 		White 255,255,255
// 		
// 		Red 255,102,102
// 		Orange 255,153,51
// 		Lime 153,255,153
// 		LightBlue 102,255,255
// 		Purple 178,102,255
// 		Green 0,204,0
// 		Blue 51,153,255
// 		Yellow 255,255,102
// 		DarkBlue 0,0,255
// 		Magenta 255,0,255


sf::Color getColorFromTextBoardValue( int n) {
    sf::Color color;
    switch( n) {
		case 0: return BlankColor;     break;  // Space, is default light gray
		case 1: return RedColor;       break;  // Red
        case 2: return OrangeColor;    break;  // Orange
        case 3: return LimeColor;      break;  // Lime
        case 4: return LightBlueColor; break;  // Light Blue
        case 5: return PurpleColor;    break;  // Purple
        case 6: return GreenColor;     break;  // Green
        case 7: return BlueColor;      break;  // Blue
        case 8: return YellowColor;    break;  // Yellow
        case 9: return WallColor;      break;  // Wall
        default: std::cout << "Invalid value " << n << " sent to translate().  Exiting...";
                 exit( -1);
                 break;
     } // end switch( n)
     return color;
} // end Board::getColorFromTextBoardValue()


//------------------------------------------------------------------------------------------------------------------
// Class used to read in from file and store all game boards
class AllBoards
{
    public:
        //------------------------------------------------------------------------------------------------------------------
        // Constructor that reads in data from the data file.
        AllBoards();    // Declaration.  See the definition outside the class, below.
    
        // Get and Set member functions.
    
        //------------------------------------------------------------------------------------------------------------------
        // Find and return the designated board array
        void setBoardValues( int boardIndex, int boardValues[])
        {
            // Validate board index number
            if( boardIndex < 0 || boardIndex > MaxNumberOfBoards) {
                std::cout << "In setBoardValues inside AllBoards class, boardIndex "
                          << boardIndex << " is out of range. Exiting..." << std::endl;
                exit( -1);
            }
            
            // Retrieve the desired board, from among the array of all the boards
            for( int i = 0; i < BoardRows * BoardColumns; i++) {
				boardValues[ i] = allBoards[ boardIndex][ i];
			}
        }
    
        //------------------------------------------------------------------------------------------------------------------
        // Retrieve the par value
        int getParValue( int boardIndex) {
			// Sanity check
            if( boardIndex > MaxNumberOfBoards) {
				std::cout << "Invalid board index when trying to retrieve par value. Exiting." << std::endl;
				exit( -1);
			}
            return levelParValues[ boardIndex];
        } // end getParValue()
    
    private:
        int par = 0;             // The number of moves it should take to solve this level
        int currentLevel = -1;   // Which board we're on
        int allBoards[ 118][ BoardRows * BoardColumns];   // Array to store all 118 8x10 boards
    
        // Par values for levels 0 through 59.  Default is 15 after that.
        const int levelParValues[ MaxNumberOfBoards] =
           // 0   1   2   3   4   5   6   7   8   9
            { 4,  3, 14,  5,  6,  4,  4, 14,  5, 25,  //   0 -   9
             12, 17, 17,  8,  4, 12,  9,  8, 12,  8,  //  10 -  19
             10, 10, 16, 13, 20, 14, 10,  9, 12, 14,  //  20 -  29
             15, 13, 20,  8, 15, 10, 10, 11,  7, 23,  //  30 -  39
              8, 11, 16, 14, 12, 13, 13,  3, 35, 18,  //  40 -  49
             26, 10, 13, 18, 26, 12, 15,  5, 22, 15,  //  50 -  59
			 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  //  60 -  69
			 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  //  70 -  79
			 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  //  80 -  89
			 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  //  90 -  99
			 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  // 100 - 109
			 15, 15, 15, 15, 15, 15, 15, 15};         // 110 - 117
}; //end class allBoards


//------------------------------------------------------------------------------------------------------------------
// AllBoards constructor
AllBoards::AllBoards()
 {
    // Read in all the boards from the data file
    std::ifstream inputFileStream;  // declare the input file stream
 
    // Open input file and verify file open worked.
    inputFileStream.open( FileName);
    if( !inputFileStream.is_open()) {
        std::cout << "Could not find input file. " << FileName << ".  Exiting..." << std::endl;
        exit( -1);
    }
 
    // Read the five lines of comments at the top of the datafile.  Datafile structure is:
    //    // Originally from Vexed v2.0 - globals.c "Global variable declarations"
    //    // Copyright (C) 1999 James McCombe (cybertube@earthling.net)
    //    // September 1,2001 - Version 2.0 changes by Mark Ingebretson (ingebret@yahoo.com) and others.
    //    // Oct 13, 2019  Format changed by Dale Reed (reed @ uic.edu)
    //    // Covered by the GNU General Public License https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html
    //    // Level 000
    //    9 9 9 9 9 9 9 9 9 9
    //    9 9 9 9 9 9 9 9 9 9
    //    9 9 9 2 1 0 0 9 9 9
    //    9 9 9 9 9 0 0 9 9 9
    //    9 9 9 0 0 0 0 9 9 9
    //    9 9 9 1 0 0 2 9 9 9
    //    9 9 9 9 1 2 9 9 9 9
    //    9 9 9 9 9 9 9 9 9 9
    //    // Level 001
    //    9 9 9 9 9 9 9 9 9 9
    //    9 9 9 9 9 9 9 9 9 9
    // ...
     
    // Read and discard the five lines of comments at the beginning of the file.
    std::string inputLine;
    for( int i=0; i<5; i++) {
        getline(inputFileStream, inputLine);
    }
     
    // Now read each of the 118 (MaxNumberOfBoards) boards.  Each one starts with a comment that has the board number.
    for( int boardIndex = 0; boardIndex < MaxNumberOfBoards; boardIndex++) {
        // Read the comment with the board number
        getline(inputFileStream, inputLine);
        
        // For each board now read the BoardRows * BoardColumns number of board values, as integers
        for( int row=0; row<BoardRows; row++) {
            for( int col=0; col<BoardColumns; col++) {
                inputFileStream >> allBoards[ boardIndex][ row * BoardColumns + col];
            }
            // Get rid of end-of-line character at the end of each board, so we can get a clean start on reading the
            //   first line of the next board, on the line with the comment and board number.
            getline(inputFileStream, inputLine);
        }
    }//end for( int boardIndex=0...
 
    // Close the input file.
    inputFileStream.close();
}//end AllBoards() constructor


//--------------------------------------------------------------------
// Display Instructions
void displayTextInstructions()
{
    std::cout << "Welcome to the game of Vexed.  \n"
		      << "Click on the \"Viewer\" option in the menu to show graphics.      \n"
			  << "  \n"
			  << "When prompted for a move, enter -1 to select among the following:  \n"
			  << "   X to exit the program                 \n"
			  << "   R to reset the current board          \n"
			  << "   S to set the board to some new level  \n"
		      << std::endl;
}//end displayInstructions()


//---------------------------------------------------------------------------------------
void initializeDisplayWindow(
		sf::RenderWindow &window,                  // Drawing window
		sf::RectangleShape &backgroundRectangle,   // Light colored background rectangle
		int boardValues[ BoardRows * BoardColumns],// Array of board integer values, from data file
		Square squaresArray[ ],                    // Array of Square objects used for graphics 
		sf::Text &messagesLabel,                   // Text used to display messages on graphics window
		sf::Font &theFont,                         // Font object used to display text
		std::string message)                       // Message to be displayed
        
{
	// Initialize the light gray background rectangle the size of the board.
	// Each size is the size of all the squares, plus the size of all square leading edge spaces, 
	// plus one more space at the trailing edge of that row or column.
	int backgroundWidth = BoardColumns * SquareSize + BoardColumns*SpaceSize + SpaceSize;
	int backgroundHeight = BoardRows * SquareSize + BoardRows*SpaceSize + SpaceSize;
	backgroundRectangle = sf::RectangleShape( sf::Vector2f(backgroundWidth, backgroundHeight));
	backgroundRectangle.setFillColor( sf::Color( BackgroundRGBvalue, BackgroundRGBvalue, BackgroundRGBvalue) );	
// 	int moveNumber = 1;
	// For the messages label make a text object from the font
	messagesLabel.setColor(sf::Color::Green);
	// Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
	messagesLabel.setPosition( 0, WindowYSize - messagesLabel.getCharacterSize() - 10);                  
	
	// Initialize the array of graphical Square objects
	// For each board row
	for( int row=0; row<BoardRows; row++) {
		// For each board column
		for( int col=0; col<BoardColumns; col++) {
			
			// Calculate the one-dimensional array index value from the current row and col
			int oneD_Index = row * BoardColumns + col;
			
			// Allocate each square individually.
			// Store a string in each square which contains its number, in order of creation
			char name[ 81];
			// "print" the square creation order number into a string to be stored in the square
			sprintf( name ,"%d", oneD_Index);   

			// Set each graphical squaresArray element's values, using a Square constructor.
			// Add SpaceSize for the offset between squares in each row and col
			int xPosition = col * SquareSize + col*SpaceSize + SpaceSize;  
			int yPosition = row * SquareSize + row*SpaceSize + SpaceSize;  
			
			// Find the color that corresponds with the current board position integer value
			int currentBoardPositionValue = boardValues[ oneD_Index];
			sf::Color theColor = getColorFromTextBoardValue( currentBoardPositionValue);  
			bool visibility = true;
			std::string textToDisplay = name;
			
			// Find whether or not it is a wall
			int isAWallValue = false;
			if( currentBoardPositionValue == WallValue) {
				isAWallValue = true;
			}
			squaresArray[ oneD_Index] = Square( SquareSize, xPosition, yPosition, theColor, 
											    visibility, isAWallValue, textToDisplay);
		} //end for( int col...)
	} // end for( int row...)	
	
} //end initializeBoard()


//---------------------------------------------------------------------------------------
// Redraw the background, board squares, text and messages label.
void redraw( sf::RenderWindow &window,                  // Drawing window
			 sf::RectangleShape backgroundRectangle,    // Light colored background rectangle
			 Square squaresArray[ ],                    // Array of Square objects used for graphics 
			 sf::Text messagesLabel,                    // Text used to display messages on graphics window
		     sf::Font font,                             // Pre-created font object used to display text
		     std::string message)                       // Message to be displayed
{		
	// Clear the graphics window, erasing what is displayed
	window.clear();

	// Redraw all screen components to the background frame buffer
	window.draw( backgroundRectangle);
	for( int i=0; i<BoardRows * BoardColumns; i++) {
		// Draw the square if its visibility is set to true.
		if( squaresArray[ i].getIsVisible()) { 
			window.draw( squaresArray[ i].getTheSquare());
			// If square is not a wall, display its index number
			if( ! squaresArray[ i].getIsAWall() ) {
				squaresArray[i].displayText( &window, font, sf::Color( TextRGBvalue, TextRGBvalue, TextRGBvalue), 17);
			}
		}
	}

	// Construct string to be displayed at bottom of screen
	messagesLabel.setString( message);            // Store the string into the messagesLabel
	window.draw( messagesLabel);                  // Display the messagesLabel

	// Display the background frame buffer, replacing the previous RenderWindow frame contents.
	// This is known as "double-buffering", where you first draw into a background frame, and then
	// replace the currently displayed frame with this background frame.
	window.display();
} // end redraw()


//---------------------------------------------------------------------------------------
// Swap colors.  This is used to make a move, where pieces don't actually move, just 
//    the adjacent colors change.
void swapColors( Square & source, 
				 Square & destination )
{    
	sf::Color colorCopy = source.getColor();
	source.setColor( destination.getColor() );
	destination.setColor( colorCopy);
}


//---------------------------------------------------------------------------------------
void getAndValidateUserInput( 
			int &squareIndex,       // Index of source square
			int &destinationIndex,  // Index of destination square.  Is set and returned
			Square squaresArray[],  // Array of all board SquaresmoveAction
			int moveNumber,         // Move number
			char &moveAction)       // Used to return a move action to retry, reset, or exit. 
{
	// Infinite loop used for move validation
	while( true) {
		// Prompt for the piece to move and the direction to move
		squareIndex = -1;
		std::cout << moveNumber << ". Enter the square index and direction to move (l=left, r=right): ";
		std::cin >> squareIndex;
		
		// Handle userInput
		if( squareIndex == -1) {
			std::cout << "Enter 'X' to exit, 'R' to reset, or 'S' to select a new board: ";
			std::cin >> moveAction;
			moveAction = toupper( moveAction);
			return;
		}
		
		// Index of a piece was selected as the piece to "move." 			
		// Read in the move direction.
		char direction;
		std::cin >> direction;
		
		// Handle moving left or right
		if( direction == 'l') {
			destinationIndex = squareIndex - 1; 
		}
		else if( direction == 'r' ) {
			destinationIndex = squareIndex + 1;
		}
		else {
			std::cout << "   *** Move must be 'l' for left or 'r' for right. Retry." << std::endl;
			continue;
		}

		// Verify that destination square is valid
		sf::Color destinationColor = squaresArray[ destinationIndex].getColor();
		if( destinationColor != BlankColor ) {
			std::cout << "   *** Invalid move.  Destination is not blank. Retry." << std::endl;
			continue;
		}

		// Verify that source square is not a blank or a wall
		sf::Color sourceColor = squaresArray[ squareIndex].getColor();
		if( sourceColor == BlankColor || sourceColor == WallColor ) {
			std::cout << "   *** Invalid source square. Retry." << std::endl;
			continue;
		}

		// All input validation tests passed, so break out of validation loop.
        break;
	}
    //moveSquares(squareArray[squareIndex], destinationIndex);
} //end getAndValidateUserInput()


//Gravity function that checks the square underneath the current square and sees if its color is clear
//if it is then it goes down 10. It also checks if its not a wall or blank.
//Also if you liked this fuction name please do let me know, itll make me happy that you got back to me.
void dropItLikeItsHot(Square squaresArray[]){
    for(int i = 0; i < 80; i++){
        if(squaresArray[i].getColor() != BlankColor && squaresArray[i].getColor() != WallColor){
            while(squaresArray[i+10].getColor() == BlankColor){
                swapColors(squaresArray[i], squaresArray[i+10]);
            }
        }
    }
}

//The destroy function first checks to see if the square index isnt a blank or a wall
//if its not then it checks its neighboring squares and checks if the current square and neighbor have the same color 
//it also checks if the boolean getIsCaptured is false, what this does is it makes sure it doesnt look at the same square more than once
//it then sets the square at the position to true using the setIsCaptured.
//and lastly it calls itself, alas recursion
void destroy(sf::Color color, Square squaresArray[],int i){
    
        if(squaresArray[i].getColor() != BlankColor && squaresArray[i].getColor() != WallColor){
            
            if(squaresArray[i+1].getColor() == squaresArray[i].getColor() && squaresArray[i+1].getIsCaptured() == false){
                squaresArray[i+1].setIsCaptured(true);
                destroy(color, squaresArray,i+1);
            }

            if(squaresArray[i-1].getColor() == squaresArray[i].getColor() && squaresArray[i-1].getIsCaptured() == false){
                squaresArray[i-1].setIsCaptured(true);
                destroy(color, squaresArray,i-1);
            }

            if(squaresArray[i-10].getColor() == squaresArray[i].getColor() && squaresArray[i-10].getIsCaptured() == false){
                squaresArray[i-10].setIsCaptured(true);
                destroy(color, squaresArray,i-10);
            }

            if(squaresArray[i+10].getColor() == squaresArray[i].getColor() && squaresArray[i+10].getIsCaptured() == false){
                squaresArray[i+10].setIsCaptured(true);
                destroy(color, squaresArray,i+10);
            }
        }
    }
//This function runs through the whole board and checks to see if any square index at i has getIsCaptured
//set to true, if it does, then itll change the color to blank and reset the Iscaptured bool to false
void checkCaptured(Square squaresArray[], sf::Color color){
    for(int i = 0; i < 80; i++){
        if(squaresArray[i].getIsCaptured() == true){
            sf::Color blank = BlankColor;
            squaresArray[i].setColor(blank);
            squaresArray[i].setIsCaptured(false);
        }
    }
}

//This function here runs through the whole board and checks if i does not equal blankcolor and wallcolor
//if it doesnt then it knows that it has cleared the entire board and must go to the next board if it returns true
//but that last part is implemented in line 744 not actually here.
bool isNotTheSameAs( Square squaresArray[])
{
    for(int i = 0; i < 80; i++){
            if( squaresArray[i].getColor() != BlankColor && squaresArray[i].getColor() != WallColor){
                return false;
            }
        }
    return true;
}
    
//---------------------------------------------------------------------------------------
int main()
{	 
	// Create the font object from the font file, to be used in displaying text.
	sf::Font font;              
	if (!font.loadFromFile("arial.ttf"))
	{
		std::cout << "Unable to load font. " << std::endl;
		exit( -1);
	}
    
	// Create other graphics-oriented objects
	sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Prog 5: Vexed");  // Drawing window
	sf::RectangleShape backgroundRectangle;  // Background gray rectangle for the board
	sf::Text messagesLabel( "Welcome to Vexed", font, 24);   // Messages label at screen bottom, for debugging
	Square squaresArray[ BoardRows * BoardColumns];	         // Array of graphical Square objects
	char aString[ 81];             // String used for printing messages
	int moveNumber = 1;            // Displays move number as play progresses
	
	AllBoards allTheBoards;     // Reads in and stores all the board number values from a data file
    Square allTheSquares;
    
	int boardValues[ BoardRows * BoardColumns];  // Board int values array, from data file
    int currentBoardIndex = 0;  // Starting board index
    int score = 0;              // Score accumulates par points for each level finished
    char moveAction = ' ';      // This is set to 'X' for exit, 'R' for reset, and 'S' to set the level
    int i = 0;
    sf::Color color;
	

	// Retrieve the board integer values from the data file, to be stored in boardIntValuesArray
	allTheBoards.setBoardValues( currentBoardIndex, boardValues);
	
	// Initialize the displayWindow with the components that have been created above
	initializeDisplayWindow( window, backgroundRectangle, boardValues, squaresArray, messagesLabel, font, "Welcome");
	// (Re)draw the graphics
	redraw( window, backgroundRectangle, squaresArray, messagesLabel, font, "Welcome");
	
	system("clear");   // Clear the text screen, to get rid of graphics initialization error messages. 
	displayTextInstructions();
	
	// Run the program as long as the window is open.  This is known as the "Event loop".
	while (window.isOpen())
	{
		moveAction = ' ';   // Will get reset below for user selections to exit, reset, or select level.
		int squareIndex = -1;
		int destinationIndex = -1;  
		getAndValidateUserInput( squareIndex, destinationIndex, squaresArray, moveNumber, moveAction);
		
		if( moveAction != ' ') {
			// Handle the special moveAction cases for exit, reset, and selecting the level
			if( moveAction == 'X') {
				// User input was 'X' to exit the game
				window.close();
				exit( 0);
			}
			else {
				if( moveAction == 'S') {
					// User input was 'S' to select the level for play.  This should only be used for testing,
					// since score is no longer meaningful once you jump ahead a level.
					std::cout << "Select the new board level: ";
					std::cin >> currentBoardIndex;
				}
				if( moveAction == 'R' || moveAction == 'S' || moveAction == 'r' || moveAction == 's') {
					// Reset the board
					allTheBoards.setBoardValues( currentBoardIndex, boardValues);
					moveNumber = 1;
					// Reinitialize the displayWindow, using new board values to reset squaresArray values
					initializeDisplayWindow( window, backgroundRectangle, boardValues, squaresArray, 
											 messagesLabel, font, "Welcome");
					// (Re)draw the graphics using the new squaresArray values
					redraw( window, backgroundRectangle, squaresArray, messagesLabel, font, "Welcome");
					continue;
				}
			} // end else {}
		} // end if( moveAction...)
        
        
		
		// A valid move was chosen. Move the piece, by swapping the source and destination squares' colors.
		swapColors(squaresArray[squareIndex], squaresArray[destinationIndex]);
        
        //Here we implent the gravity, destroy, and checkCaptured functions
        //what I did was I ran through the whole board, after that i call the gravity function
        //then in the conditional statement i check if the color isnt wall color and blankcolor
        //if it passes then itll run the destroy function and outside the if statement i call the 
        //checkCaptured to change any adjacent squares 
        for (i = 0; i < 80; i++){
          dropItLikeItsHot(squaresArray);
          if (color != WallColor && color != BlankColor){
               destroy(color, squaresArray,i);
           }
            checkCaptured(squaresArray, color);
        }	
            
        for (i = 0; i < 80; i++){
          dropItLikeItsHot(squaresArray);
           if (color != WallColor && color != BlankColor){
                destroy(color, squaresArray,i);
           }
          checkCaptured(squaresArray, color);
        }
        
        //This body of code is to figure out if the player has passed the current board, it they do, then
        //I print out the proper statements if the player wins, or if they lose
        //I also change the level and print out statements stating that the player took too many attempts
        if(isNotTheSameAs(squaresArray) == true){
            if(moveNumber <= allTheBoards.getParValue(currentBoardIndex)){
                score = score  + allTheBoards.getParValue(currentBoardIndex);
                std::cout << "\nCongratulations! onto level " << currentBoardIndex + 1 << " \nScore: " << score << std::endl<< std::endl;
                currentBoardIndex++;
                allTheBoards.setBoardValues(currentBoardIndex, boardValues);
                moveNumber = 0;
                initializeDisplayWindow(window, backgroundRectangle, boardValues, squaresArray, messagesLabel, font, "Welcome");                    
                redraw(window, backgroundRectangle, squaresArray, messagesLabel, font, "Welcome");              
            }

        
            else{
                std::cout << "\nSorry you took " << moveNumber << " moves and you must finish within " 
                          << allTheBoards.getParValue(currentBoardIndex)
                          << " moves before moving on. \n" << std::endl;

                allTheBoards.setBoardValues(currentBoardIndex, boardValues);
                moveNumber = 0;
                initializeDisplayWindow(window, backgroundRectangle, boardValues, squaresArray, messagesLabel, font, "Welcome");
                redraw(window, backgroundRectangle, squaresArray, messagesLabel, font, "Welcome");  


            }
        }
		// Print the moved piece number into a string, which will then be displayed graphically.
		sprintf( aString, "Moved piece %d" ,squareIndex );
		moveNumber++;
		
		// Redraw the graphics
		redraw( window, backgroundRectangle, squaresArray, messagesLabel, font, aString);

		// Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
		std::this_thread::sleep_for(std::chrono::milliseconds( SleepAmount));
	}//end while( window.isOpen())
	
	return 0;
}//end main()
