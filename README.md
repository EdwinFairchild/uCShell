# uCShell

First and foremost uCShell is a work in progress.

# How to use
-Include uCShell.h and uCShell_Config.h in your project
making sure that your environment knows where to find uCShell.c and H files
```
#include "uCShell.h"
#include "uCShell_Config.h"
```
# History

I wrote uCShell to be a hardware agnostic debugging solution where conventional debugging methods are non-existent. That is not to say it cannot be used as a general purpose console like tool.
The first personal version of uCShell (at the time called BFP “brute force parser”) came to rise from my need to tweak motor speed control variables real time for a school project in 2020.
Having to tweak a value and then compile, reflash then test and repeat was awful so I wrote what eventually turned out to be uCShell. 
At the time it only accepted single character commands and arguments. It was heavily hardware dependent on STM32 microcontrollers.
Around late 2021 when I started my first firmware engineering job I was working on a device where I did not have easy access to the debug port and I needed to trigger some faults in order to test logic, a lot of faults mind you.  I saw the correlation between my previous motor control problem and this new problem when I found myself , pseudo triggering faults , compiling, flashing test and repeat. I reached for my prewritten uCShell and started to add features, of which the first was to make it hardware agnostic to the best of my abilities. 

And here we are a work in progress , becoming more useful with each iteration. 
