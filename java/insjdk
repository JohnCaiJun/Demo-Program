#!/bin/bash

# $1 为jdk下载地址

JDK_URL=$1
JDK_NAME=jdk8

removeOldJdk(){
#rpm -qa | grep java
#rpm -e --nodeps java-1.7.0-oepnjdk*
}

downloadJdk(){
	cd ~
	# 1. downLoad jdk 
	wget -O ${JDK_NAME}.tar.gz ${JDK_URL}

	# 2. tar
	mkdir /usr/local/${JDK_NAME} && tar --strip-components=1 -zxf ${JDK_NAME}.tar.gz -C /usr/local/${JDK_NAME}
}


setting(){
at << EOF >> /etc/profile

#add jdk path
export JAVA_HOME=/usr/local/jdk8
export JRE_HOME=${JAVA_HOME}/jre
export CLASSPATH=.:${JAVA_HOME}/lib/dt.jar:${JAVA_HOME}/lib/tools.jar:${JRE_HOME}/lib
export PATH=${JAVA_HOME}/bin:${PATH}
EOF
}

downloadJdk
setting


