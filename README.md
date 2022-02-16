# μCShell

μCShell (micro-C-shell) or (micro-Controller-Shell) aims to be a hardware agnostic shell/console library
for embedded systems. Originally developed as a debugging utility, μCShell can become so much more.

At the time of this writing μCShell uses `strtok` and additional parsing to run commands commonly passed
through a com-port/terminal viewer, such as [putty](https://www.putty.org/) or [MobaTerm](https://mobaxterm.mobatek.net/), via a microcontroller's UART peripheral. Though the source of input and manner output are ultimately up to the designer and application at hand.

μCShell simply provides the command & callback registering mechanisms to implemnt an embeded console
experience. 

## Use Cases
- Debugging utility where realtime operation of the microcontroller is preferred over a debug state where pausing
  of code execution can yield unnatural behavior such as timeouts or misrepresentation of system behavior. 
  - For example tuning PID values and observing the changes take effect in realtime as oppose to pausing code or 
    having to rebuild and reflash firmware to observe a minor tweak of a value.
- Streaming of data:
  - Variables
  - Registers
## How to use
.... 


## History

