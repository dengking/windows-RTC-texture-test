# windows-RTC-texture-test
This is the complete minimal reproducible code sample of flutter issue [The UI fails to response after the screen is locked for a while #103945](https://github.com/flutter/flutter/issues/103945).

The source code is forked from [jnschulze/flutter-playground](https://github.com/jnschulze/flutter-playground) and I make a little change. The code can mock the situation of RTC.

The problem in  issue [The UI fails to response after the screen is locked for a while #103945](https://github.com/flutter/flutter/issues/103945) is easy to reproduce.

For easy to reproduce, change the settings as follow  

![image](https://user-images.githubusercontent.com/14926769/170446131-3c874a19-3a5c-4887-92fd-61b971ce429f.png)

Then do as follow:

1、Run the example, press the button and then the number it show will change.

2、Do not touch your computer and it will go to sleep after some time. If you change the sleeping time, it will sooner.

3、Reopen your screen and press the button and now it will not response to your action.

English is not my mother tongue, so if my description is not accurate enough, contact me directly by email 2238897886@qq.com.
