#
#  Makefile
#

# 最終目的のファイル
TARGET = gojou.exe

# ソースファイル(*.c)の一覧
SRCS = gojou.c

#オブジェクトファイル(*.o)の一覧
OBJS = ${SRCS:.c=.o}

# ヘッダファイルの一覧（今は何もなし）
HEADERS =

# コンパイラ・リンカの指定
CC = i686-pc-cygwin-gcc
CCFLAGS = -Wall -I/usr/include/opengl
LD = i686-pc-cygwin-gcc
LDFLAGS =
LIBS = -lglpng -lglut32 -lglu32 -lopengl32 -lwinmm #オプションの設定

# OBJSからTARGETを作る方法
$(TARGET) : $(OBJS)
		$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET) $(LIBS)

# *.c から *.o を作る方法
.c.o :
		$(CC) $(CCFLAGS) -c $<

# *.o は HEADERS と Makefile に依存（これらが書き換わった時にも*.oを再構築）
$(OBJS) : $(HEADERS) Makefile

# make cleanとしたときに実行されるコマンド
clean :
		rm -f $(TARGET) $(OBJS) core *~