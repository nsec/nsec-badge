FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

RUN echo "deb [trusted=yes] http://ppa.launchpad.net/team-gcc-arm-embedded/ppa/ubuntu bionic main" >> /etc/apt/sources.list && \
echo "deb-src [trusted=yes] http://ppa.launchpad.net/team-gcc-arm-embedded/ppa/ubuntu bionic main" >> /etc/apt/sources.list

RUN apt-get update && apt-get install -y --no-install-recommends \
	build-essential \
	ca-certificates \
	gcc-arm-embedded \
	libnewlib-arm-none-eabi \
	python3 \
	python3-pil \
	python3-pycparser \
	python3-serial
