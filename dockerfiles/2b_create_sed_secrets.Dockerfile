# 2021 Collegiate eCTF
# Generate SED secrets Dockerfile
# Ben Janis
#
# (c) 2021 The MITRE Corporation

# load current SSS container to modify
ARG DEPLOYMENT
FROM ${DEPLOYMENT}/sss

ARG SCEWL_ID

# NOTE: only sss/ and its subdirectories in the repo are accessible to this Dockerfile as .
# NOTE: to maximize the useage of container cache, use ADD to map in only the files/directories you need
#       (e.g. only mapping in the SED directory rather than the entire repo)

# do here whatever you need here to create secrets for the new SED that the SSS needs access to

# RUN mkdir ${SCEWL_ID}.secrets

ADD ./randGenSED.py .

# RUN mv ./randGenSED.py ./${SCEWL_ID}.secrets/randGenSED.py



#WORKDIR /${SCEWL_ID}.secrets

RUN python3 randGenSED.py


RUN mv data1 ${SCEWL_ID}.data1

RUN mv data2 ${SCEWL_ID}.data2

RUN mv data3 ${SCEWL_ID}.data3

RUN mv data4 ${SCEWL_ID}.data4



