docker build -t h2_compiler .
docker run -it --entrypoint /bin/bash h2_compiler