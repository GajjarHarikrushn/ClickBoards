#include "sam.h"
#include "displayDriver.c"

int main() {
    Display *display = initDisplay();
    drawScreen(display);
}