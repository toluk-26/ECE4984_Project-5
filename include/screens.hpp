#ifndef SCREENS_HPP
#define SCREENS_HPP

#include "TFT_eSPI.h" // TFT LCD header file

// Define shared measure for screen layout. This is the height of the bars at
// the top and bottom of the screen for data source and device state,
// respectively.
#define STATUS_HEIGHT 30

// Instantiate the TFT for display.
extern TFT_eSPI tft;

// Draw the screen area for displaying temperature.
void drawValues();

// Display the provided temperature.
void displayTempText(char *temp);

// Display the provided temperature.
void displayHumidityText(char *humidity);

// Display the data source at the top of the screen by displaying the provided
// string.
void displaySource(String sourceMessage);

// Display the server state at the bottom of the screen by displaying the
// provided string.
void displayState(String stateMessage);

// Display the data source at the top of the screen by displaying the provided
// string.
void displaySource(String sourceMessage);

#endif // SCREENS_HPP