FROM continuumio/miniconda3

COPY Content/lib/ /root/lib/
COPY Content/libext/ /usr/lib/x86_64-linux-gnu/
COPY ubuntu_prod/env.sh /root/
COPY Content/docker-server/ /root/srv/
COPY Content/libfdf/ /root/srv/fdf/
RUN conda create -n env python=3.6 \
    && conda clean -afy \
    && find /opt/conda/ -follow -type f -name '*.a' -delete \
    && find /opt/conda/ -follow -type f -name '*.pyc' -delete \
    && find /opt/conda/ -follow -type f -name '*.js.map' -delete \
    && echo "source activate env" > ~/.bashrc \
    && /bin/bash -c "source activate env" \
    && /opt/conda/envs/env/bin/pip install waitress falcon falcon_cors \
    && ldconfig /root/lib && ldconfig /opt/conda/envs/env/lib && mkdir /root/srv/Data
ENV PATH /opt/conda/envs/env/bin:$PATH

WORKDIR /root/srv
EXPOSE 5001/tcp
CMD ["/bin/bash", "/root/env.sh"]
