# windows-RTC-texture-test
This is the complete minimal reproducible code sample of flutter issue [The UI fails to response after the screen is locked for a while #103945](https://github.com/flutter/flutter/issues/103945).

The source code is forked from [jnschulze/flutter-playground](https://github.com/jnschulze/flutter-playground) and I make a little change. The code can mock the situation of RTC.

The problem in  issue [The UI fails to response after the screen is locked for a while #103945](https://github.com/flutter/flutter/issues/103945) is easy to reproduce: 

1、Run the example, press the button and then the number it show will change.

2、Lock your screen for some time, for example 30 mins.

3、Reopen your screen and press the button and now it will not response to your action.

English is not my mother tongue, so if my description is not accurate enough, contact me directly by email 2238897886@11.com.
