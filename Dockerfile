FROM ubuntu:latest AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
build-essential \
make \
cmake \
#vcpkg
pkg-config git zip unzip curl tar 

#vcplg installation
RUN git clone https://github.com/Microsoft/vcpkg.git /opt/vcpkg && \
    cd /opt/vcpkg && \
    ./bootstrap-vcpkg.sh

RUN ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg

RUN vcpkg integrate install && \
    vcpkg update

WORKDIR /app
COPY . .

ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="${VCPKG_ROOT}:${PATH}"

RUN chmod +x ./scripts/build.sh 
RUN sh ./scripts/build.sh

FROM ubuntu:latest

COPY --from=build /app/build/mesrouter /usr/bin/mesrouter
COPY --from=build /app/configs/baseline.json /usr/bin/conf.json

ENV CONFIG_PATH=/usr/bin/conf.json

ENTRYPOINT mesrouter