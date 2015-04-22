# TP02 - Implantation sous MPI d'horloges de Lamport
#
# @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
# @copyright (c) 2015, toumlilt
#
# @version 1.0
# @package toumlilt/M1/AR

CC=gcc -Wall -ansi
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src

all: directories can


directories: ${OBJ} ${BIN} ${LIB}

${OBJ}:
	mkdir ${OBJ}
${BIN}:
	mkdir ${BIN}
${LIB}:
	mkdir ${LIB}


# regles generales :
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $< -I$(INC)

$(BIN)/% : $(OBJ)/%.o
	$(CC) -o $@ $< ${LPTHREAD}
#fin regles generales

# without server
can: $(BIN)/CAN

# with server
# comming soon

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

