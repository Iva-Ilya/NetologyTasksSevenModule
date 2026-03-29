FROM gcc:latest
RUN apt update && apt install -y clang
WORKDIR /usr/src/Docker
RUN git clone https://github.com/Ckoll123/Netologiya.git /usr/src/Docker
RUN g++ -o main_gpp Module_7/3/2/main.cpp
RUN clang++ -o main_clang Module_7/3/2/main.cpp
CMD ["sh", "-c", "ls main_gpp main_clang; echo '--- Запуск через g++ ---'; ./main_gpp; echo '\\n--- Запуск через clang++ ---'; ./main_clang"]