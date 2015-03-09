This directory contains an empty simulator and the main() code to run
this simulator using Nactural Actor-Critic or Least squares policy
iteration. To create your optimisation problem, edit MySimulator.cc
and fill in the blanks indicated. Compile, and run
TemplateRLApp. Watch the long term average reward increase as the RL
code learns to control your pronlem. Inevitably you will also have to
edit TemplateRLApp.cc to change the #define constants at the top in
order to get things to work well (or at all).

I've tried as hard as possible to make this easy. As a consequence
I've made decisions about what options to turn on and off in the
library. To get the best performance you'll have to explore more
aspects of the library.

Don't forget to 
$ cp Makefile.default Makefile

The code will compile without changes, but it will crash when you run it
without completing the template.

-Doug Aberdeen 20/7/2007.
