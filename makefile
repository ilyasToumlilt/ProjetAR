# Projet AR - Content-Adressable Network
# 
# @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
# @author Paul  Mabillot <paul.mabilot@etu.upmc.fr>
#
# @version 1.0

CC=mpicc -Wall -ansi
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

$(LIB)/libCAN.a: $(OBJ)/point.o $(OBJ)/space.o $(OBJ)/node.o $(OBJ)/list_node.o $(OBJ)/CAN.o
	ar -rcs $@ $^

can: $(LIB)/libCAN.a

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

