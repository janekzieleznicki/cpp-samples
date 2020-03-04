#include "Library.hpp"
int main(int argc, char **argv) {
  HelloConsumer{}(HelloProvider{});
}