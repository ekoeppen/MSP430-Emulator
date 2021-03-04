FROM alpine:3.13 as dev
RUN apk add --no-cache gcc make libgcc readline-dev linux-headers musl-dev
COPY . .
RUN make && make install

FROM alpine:3.13
RUN apk add --no-cache libgcc readline
COPY --from=dev /usr/local/bin/msp430-emu /usr/local/bin/msp430-emu
