TARGET := deriv.exe
EXTERN := cpp
COMPILER := g++

COMPILE_OPTION := -Wall -O2
# to generate dependent files #
COMPILE_OPTION_DES := -MMD -MP 

# store .o and .d files #
TMPDIR := tmp
# store the target file #
DEBUGDIR := debug

# sources, objects and dependencies #
SRCS := $(wildcard *.$(EXTERN))
OBJS := $(patsubst %.$(EXTERN), $(TMPDIR)/%.o, $(SRCS))
DEPS := $(patsubst %.$(EXTERN), $(TMPDIR)/%.d, $(SRCS))

# link #
$(DEBUGDIR)/$(TARGET) : $(OBJS) | $(DEBUGDIR)
	@ echo linking...
	$(COMPILER) $(OBJS) -o $(DEBUGDIR)/$(TARGET)
	@ echo completed!

# compile #
$(TMPDIR)/%.o : %.$(EXTERN) | $(TMPDIR)
	@ echo compiling $<...
	$(COMPILER) $< -o $@ -c $(COMPILE_OPTION) $(COMPILE_OPTION_DES)

# create TMPDIR when it does not exist #
$(TMPDIR) :
	@ mkdir $(patsubst ./%, %, $(TMPDIR))

$(DEBUGDIR) :
	@ mkdir $(DEBUGDIR)

# files dependecies #
-include $(DEPS)

VARIABLE 	:= x
FORMULA1 	:= mul x ^ e x
FORMULA2	:= - ln sin x ^ x 3
FORMULA3	:= ^ x sin mul 2 x
FORMULA4 	:= + ^ x sin mul 2 x ^ x / 1 x

# run command #
.PHONY : run
run:
	./$(DEBUGDIR)/$(TARGET) $(VARIABLE) $(FORMULA1)
	./$(DEBUGDIR)/$(TARGET) $(VARIABLE) $(FORMULA2)
	./$(DEBUGDIR)/$(TARGET) $(VARIABLE) $(FORMULA3)
	./$(DEBUGDIR)/$(TARGET) $(VARIABLE) $(FORMULA4)

# clean command #
.PHONY : clean
clean:
	@ echo try to clean...
	rm -r $(DEBUGDIR)/$(TARGET) $(OBJS) $(DEPS)
	@ echo completed!