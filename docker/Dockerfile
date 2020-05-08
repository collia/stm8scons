FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive
RUN useradd -ms /bin/bash  user

RUN apt-get update && \
    apt-get upgrade -y -q && \
    apt-get install -y ccache wget make git libusb-dev libusb-1.0 \
      libtool autotools-dev automake pkg-config scons sdcc && \
    mkdir /home/user/project && \
    mkdir /home/user/stm8StdPeriph


RUN git clone https://github.com/vdudouyt/stm8flash &&\
	cd stm8flash && \
	make && \
	make install

#USER user

WORKDIR /home/user/project

ENTRYPOINT ["/bin/bash", "-c"]
