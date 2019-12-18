#!/bin/bash

# Uncomment what is necessary to reinstall by force 
#rm -f ${CONDA_PREFIX}/lib/libvelocypack.a
#rm -f ${CONDA_PREFIX}/lib/libjsonarango.a

threads=3
BRANCH_JSON=master
BRANCH_TFUN=master


# Velocypack from ArangoDB (added for installing JSONIO database client)
# if no VPack installed in ${CONDA_PREFIX}/lib/libvelocypack.a
test -f ${CONDA_PREFIX}/lib/libvelocypack.a || {

	# Building velocypack library
	mkdir -p ~/code && \
		cd ~/code && \
		git clone https://github.com/arangodb/velocypack.git && \
		cd velocypack && \
		mkdir -p build && \
		cd build && \
		cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF -DCMAKE_INSTALL_PREFIX:PATH=${CONDA_PREFIX} && \
		make -j $threads && \
		make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

# jsonarango database client (added for building ThermoMatch code)
# if no jsonarango installed in ${CONDA_PREFIX}/lib/libjsonarango.a (${CONDA_PREFIX}/include/libjsonarango)
test -f ${CONDA_PREFIX}/lib/libjsonarango.a || {

	# Building jsonarango library
	mkdir -p ~/code && \
		cd ~/code && \
		git clone https://bitbucket.org/gems4/jsonarango.git -b $BRANCH_JSON && \
		cd jsonarango && \
		mkdir -p build && \
		cd build && \
		cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DJSONARANGO_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release && \
		make -j $threads && \
		make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

