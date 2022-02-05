# μCShell

μCShell (micro-C-shell) or (micro-Controller-Shell) aims to be a hardware agnostic shell/console library
for embedded systems. 

At the time of this writing μCShell uses `strtok` and additional parsing to run commands commonly passed
through a com-port/terminal viewer, such as [putty](https://www.putty.org/) or [MobaTerm](https://mobaxterm.mobatek.net/), via a microcontroller's UART peripheral. Though the 
source of input and manner output are
ultimately up to the designer and application at hand.

μCShell simply provides the command & callback registering mechanisms to implemnt an embeded console
experience. 

## How to use
.... 

