本实验将实现如下功能：本实验利用ALIENTEK MiniSTM32开发板和ATK-VS1053 MP3模块，实现一个简单的MP3播放
器功能.支持上下曲切换和暂停等功能.KEY0,下一首;KEY1,上一首.WK_UP暂停.

另外，本例程可以用USMART来调节音量音效。

关于本例程的介绍，请看《ATK-VS1053 MP3模块使用说明_AN1401.pdf》


MP3模块与开发板的连接关系：
--------------------------------------------------------
ALIENTEK MiniSTM32开发板|PA11|PA12|PA6|PA7|PA5|PA4 |PA8
-------------------------------------------------------- 
ATK-VS1053 MP3模块      |RST |DREQ|SO |SI |SCK|XDCS|XCS
--------------------------------------------------------

注意：
1，需要从开发板的VOUT2引出电源给MP3模块！接5V即可,3.3V不用接!
2，SD卡根目录一定要存放MUSIC文件夹，并将歌曲放到该文件夹下面。 
3，如果要更新字库，请拷贝开发板光盘-->5，SD卡根目录文件 文件夹里面的SYSTEM文件夹到SD卡根目录。

               	正点原子@ALIENTEK
               	2014-3-25
		广州市星翼电子科技有限公司
                电话：020-38271790
                传真：020-36773971
	       	购买：http://shop62103354.taobao.com
                      http://shop62057469.taobao.com
                公司网站：www.alientek.com
         	技术论坛：www.openedv.com