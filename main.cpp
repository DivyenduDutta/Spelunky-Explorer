#include "interface/cli/CLI.h"

int main(int argc, char **argv) {
	CLI cli = CLI(argc, argv);
	cli.run("Spelunky explorer", "1.0");
}