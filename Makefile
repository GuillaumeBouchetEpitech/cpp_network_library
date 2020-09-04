
build_platform=native_pthread

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native_pthread)

$(info build_platform is valid, value=$(build_platform))

else

$(error unsupported value for "build_platform", value=$(build_platform))

endif

#

DIR_TARGET=		./bin

NAME_NATIVE=	$(DIR_TARGET)/exec

#### DIRS

DIR_SRC=	src

#### /DIRS

DIR_OBJ=	./obj/native

#### SRC

SRC=	$(wildcard \
			$(DIR_SRC)/*.cpp \
			$(DIR_SRC)/network/*.cpp)

#

OBJ=	$(patsubst %.cpp, \
			$(DIR_OBJ)/%.o, \
			$(SRC))

#

#

#### /BULLET_SRC



#######


ifeq ($(build_mode),release)

	# optimisation flag
	BUILD_FLAG=		-O3

else

	# debug flag
	BUILD_FLAG=		-g3

endif


CXX=g++
AR=ar

CXXFLAGS=	$(BUILD_FLAG) \
			-std=c++17 \
			-Wall -W -Wextra -Wunused \
			-I$(DIR_SRC)

LDFLAGS=	$(BUILD_FLAG) \
			-pthread

RM=			rm -rf


#######

#
## RULE(S)

all:	native_pthread

ensurefolders:
				@mkdir -p $(DIR_TARGET)

native_pthread:	ensurefolders $(OBJ)
				@mkdir -p `dirname $(NAME_NATIVE)`
				$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME_NATIVE) $(LDFLAGS)

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $< -c -o $@

#

clean:
		$(RM) $(DIR_OBJ)

fclean:	clean
		$(RM) $(NAME_NATIVE)

re:			fclean all

.PHONY:		all \
			main \
			clean \
			fclean \
			re

## RULE(S)
#
