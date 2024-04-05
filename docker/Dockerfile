### Build/test container ###
# Define builder stage
FROM teami-net:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make -j 2

# Copy static file folders so they are accessible to test
RUN mkdir folder1
RUN mkdir folder2
RUN cp -r ../folder1 ./
RUN cp -r ../folder2 ./

# Install floating point arithmetic tool for multi-threaded integration test
# Only need to install in builder, not deploy because only used in tests
RUN apt-get install bc

RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

RUN mkdir folder1
RUN mkdir folder2
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/src/server_config .
COPY --from=builder /usr/src/project/folder1/ ./folder1/
COPY --from=builder /usr/src/project/folder2/ ./folder2/


EXPOSE 8080

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
CMD ["server_config"]