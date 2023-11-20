BUILD=build
TEST=test
SRC=src

SOURCES=$(wildcard $(SRC)/*.c)
TEST_SOURCES=$(wildcard $(TEST)/*.c)

build_prod:
	gcc -o ./$(BUILD)/bmi \
		$(SOURCES) main.c

build_test:
	gcc -o ./$(BUILD)/test \
		$(SOURCES) $(TEST_SOURCES)

test: build_test
	./$(BUILD)/test

exec:
	./$(BUILD)/bmi example.bm

run: build_prod exec

