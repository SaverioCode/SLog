FROM debian:trixie

ARG VENV=/home/dev/venv
ARG CONAN_VERSION=2.24.0
ARG SLOG_DIR=/home/dev/slog

ENV DEBIAN_FRONTEND=noninteractive
ENV PATH="/home/dev/venv/bin:$PATH"

RUN apt-get update && apt-get install -y
RUN apt install -y \
    sudo \
    build-essential \
    cmake \
    clang \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash dev \
    && echo "dev:dev" | chpasswd \
    && usermod -aG sudo dev

USER dev
WORKDIR /home/dev
RUN mkdir ${SLOG_DIR}

RUN python3 -m venv ${VENV}
RUN ${VENV}/bin/pip install conan==${CONAN_VERSION}
RUN conan profile detect
RUN --mount=type=bind,source=tests/conanfile.py,target=${SLOG_DIR}/conanfile.py \
    conan install ${SLOG_DIR} --output-folder=${SLOG_DIR}/conan --build=missing

WORKDIR ${SLOG_DIR}

CMD ["/bin/bash"]
