FROM ubuntu:18.04

COPY Content/fdf/fdf-cpp/ /root/cpp/
COPY Content/lib/ /root/lib/
COPY Content/Anaconda3-5.0.1-Linux-x86_64.sh /root/
COPY ubuntu_build/env.sh /root/
COPY Content/docker-server/ /root/srv/

WORKDIR /root/cpp

RUN         apt-get update \
                && apt-get install -y software-properties-common \
                && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
                && apt-get update \
                && apt-get install -y make gcc-7 g++-7 gcc-7 \ 
                && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 100 \
                && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 100 \
                && bash /root/Anaconda3-5.0.1-Linux-x86_64.sh -b \
                && rm /root/Anaconda3-5.0.1-Linux-x86_64.sh \
                && apt-get install -y libboost-all-dev freeglut3-dev libwebp-dev \
                && /root/anaconda3/bin/pip install --upgrade pip \
                && /root/anaconda3/bin/pip install waitress falcon falcon_cors \
		&& bash /root/cpp/setup_ubuntu_docker.sh \
                && bash /root/cpp/_build_all.sh \
                && cp /root/cpp/fdf.so /root/srv/fdf/ \
                && rm -rf /root/cpp/ \
		&& mkdir /root/srv/Data/ \
                && apt-get autoremove \
                && apt-get clean

WORKDIR /root/srv
EXPOSE 8080/tcp
CMD ["/bin/bash", "/root/env.sh"]
