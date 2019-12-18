#!/bin/bash
# Installing dependencies needed to build thermofun on (k)ubuntu linux 16.04 or 18.04

sudo apt-get install -y libcurl4-openssl-dev libboost-all-dev

# Uncomment what is necessary to reinstall by force 
#sudo rm -f /usr/local/lib/libvelocypack.a
#sudo rm -f /usr/local/lib/libjsonarango.a
#sudo rm -f /usr/local/include/nlohmann/json.hpp

threads=3
BRANCH_JSON=master
BRANCH_TFUN=master
git status

# nlohmann/json
test -f /usr/local/include/nlohmann/json.hpp || {

	# Building yaml-cpp library
	mkdir -p ~/code && \
		cd ~/code && \
		git clone https://github.com/nlohmann/json.git && \
		cd json && \
		mkdir -p build && \
		cd build && \
		cmake .. -DJSON_BuildTests=OFF && \
		make && \
		sudo make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}


# Velocypack from ArangoDB (added for installing JSONIO database client)
# if no VPack installed in /usr/local/lib/libvelocypack.a (/usr/local/include/velocypack)
test -f /usr/local/lib/libvelocypack.a || {

	# Building velocypack library
	mkdir -p ~/code && \
		cd ~/code && \
		git clone https://github.com/arangodb/velocypack.git && \
		cd velocypack && \
		mkdir -p build && \
		cd build && \
		cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF && \
		make -j $threads && \
		sudo make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

# JSONIO database client (added for building ThermoMatch code)
# if no JSONIO installed in /usr/local/lib/libjsonio.a (/usr/local/include/jsonio)
test -f /usr/local/lib/libjsonarango.a || {

	# Building jsonio library
	mkdir -p ~/code && \
		cd ~/code && \
		git clone https://bitbucket.org/gems4/jsonarango.git -b $BRANCH_JSON && \
		cd jsonarango && \
		mkdir -p build && \
		cd build && \
		cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release && \
		make -j $threads && \
		sudo make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

sudo ldconfig 

