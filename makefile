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

P2P=P2P_Viewer

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
	$(CC) -o $@ $< -L$(LIB) -lCAN
#fin regles generales

$(LIB)/libCAN.a: $(OBJ)/point.o $(OBJ)/space.o $(OBJ)/node.o $(OBJ)/data.o $(OBJ)/list_node.o $(OBJ)/CAN.o $(OBJ)/CANinsertData.o $(OBJ)/CANsearchData.o $(OBJ)/display.o 
	ar -rcs $@ $^

can: $(LIB)/libCAN.a

main: can $(BIN)/CANmain2
	mpirun -np 10 $(BIN)/CANmain2

view:
	$(P2P)/$(BIN)/p2p_viewer $(P2P)/trace/test-1.txt $(P2P)/Web/index.html
	#P2P_Viewer/$(BIN)/p2p_viewer trace/test-2.txt Web/index-2.html
	chromium $(P2P)/Web/index.html #Web/index-2.html


clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

