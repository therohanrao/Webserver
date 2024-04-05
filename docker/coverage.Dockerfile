### Build/test container ###
# Define builder stage
FROM teami-net:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Copy static file folders so they are accessible to test
RUN mkdir folder1
RUN mkdir folder2
RUN cp -r ../folder1 ./
RUN cp -r ../folder2 ./

# Install floating point arithmetic tool for multi-threaded integration test
# Only need to install in builder, not deploy because only used in tests
RUN apt-get install bc

# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage -j 2