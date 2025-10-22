#include "screens.hpp"
#include "Free_Fonts.h" // Free fonts header

TFT_eSPI tft;

// home screen
void screenHome() {
    tft.setFreeFont(FSS12);
    tft.setTextColor(TFT_WHITE);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    tft.drawString("Project 5", 160, 15);
    tft.drawString("Tolu Kolade", 160, 78);
    tft.drawString("toluk@vt.edu", 160, 102);
    tft.drawString("Alex Halloran", 160, 150);
    tft.drawString("alexanderha22@vt.edu", 160, 175);
    tft.drawString("ECE 4984 Fall 2025", 160, 210);
    
}

// Display the server state at the bottom of the screen by displaying the
// provided string.
void displayState(String stateMessage) {
    tft.setFreeFont(FSS9);
    tft.setTextColor(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.fillRect(0, TFT_WIDTH - STATUS_HEIGHT, TFT_HEIGHT, STATUS_HEIGHT,
                 TFT_ORANGE);
    tft.drawString(stateMessage, TFT_HEIGHT / 2,
                   TFT_WIDTH - STATUS_HEIGHT + 12);
}

// Draw the screen area for displaying temperature.
void drawValues() {
    tft.setTextDatum(MC_DATUM);

    tft.fillRect(0, STATUS_HEIGHT, TFT_HEIGHT, TFT_WIDTH - (2 * STATUS_HEIGHT),
                 TFT_LIGHTGREY);
    tft.fillRect(5, STATUS_HEIGHT + 5, (TFT_HEIGHT / 2) - 8,
                 TFT_WIDTH - (2 * STATUS_HEIGHT) - 10, TFT_DARKGREEN);
    tft.fillRect((TFT_HEIGHT / 2) + 3, STATUS_HEIGHT + 5, (TFT_HEIGHT / 2) - 8,
                 TFT_WIDTH - (2 * STATUS_HEIGHT) - 10, TFT_DARKGREEN);

    tft.setFreeFont(FSS9);
    tft.setTextColor(TFT_LIGHTGREY);
    tft.drawString("Temperature", TFT_HEIGHT / 4 + 1, STATUS_HEIGHT + 18);
    tft.drawString("Fahrenheit", TFT_HEIGHT / 4,
                   TFT_WIDTH - STATUS_HEIGHT - 20);
    tft.drawString("Humidity", 3 * TFT_HEIGHT / 4, STATUS_HEIGHT + 18);
    tft.drawString("Percent", 3 * TFT_HEIGHT / 4,
                   TFT_WIDTH - STATUS_HEIGHT - 20);
}

// Display the provided temperature.
void displayTempText(char *temp) {
    // Fill in the display area for the temperature to erase old value.
    tft.fillRect(6, (TFT_WIDTH / 2) - 25, (TFT_HEIGHT / 2) - 10, 42,
                 TFT_DARKGREEN);

    // Display temperature value.
    tft.setFreeFont(FSSB24);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(temp, (TFT_HEIGHT / 4) - 2, (TFT_WIDTH / 2) - 7);
}

void displayHumidityText(char *humidity) {
    // Fill in the display area for the humidity to erase old value.
    tft.fillRect((TFT_HEIGHT / 2) + 4, (TFT_WIDTH / 2) - 25,
                 (TFT_HEIGHT / 2) - 10, 42, TFT_DARKGREEN);

    // Display temperature value.
    tft.setFreeFont(FSSB24);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(humidity, (3 * TFT_HEIGHT / 4) - 2, (TFT_WIDTH / 2) - 7);
}

// Display the data source at the top of the screen by displaying the provided
// string.
void displaySource(String sourceMessage) {
    tft.setFreeFont(FSSB9);
    tft.setTextColor(TFT_DARKGREEN);
    tft.setTextDatum(MC_DATUM);
    tft.fillRect(0, 0, TFT_HEIGHT, STATUS_HEIGHT, TFT_LIGHTGREY);
    tft.drawString(sourceMessage, TFT_HEIGHT / 2, STATUS_HEIGHT - 14);
}